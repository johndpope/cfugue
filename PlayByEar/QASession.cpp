#include "StdAfx.h"
#include "PlayByEarDlg.h"
#include "QASession.h"

CQASession::CQASession(CPlayByEarDlg* pDlg) :
    m_pDlg(pDlg),
    m_bFirstQuestion(true), 
    m_nQuestionWaitRound(0),
    m_nAnswerWaitRound(0),
    m_nRetryCount(0),
    m_nReplayCount(0),
    m_nResultAnouncementRound(0),
    m_nCurState(TO_BE_STARTED),
    m_nCurLevel(SINGLE_NOTE_SINGLE_OCTAVE),
    m_nQuestionCount(0),
    m_bHaltProcessing(false),
    m_bWaitBeforeNewQuestion(true),
    m_bWaitBeforeRetry(true),
    m_nAnswerNotesEntered(0),
    m_nCorrectAnswerCount(0),
    m_dAccuracy(1.0f),
    m_dEfficiency(1.0f)
{
    // Subscribe to the 'Play Complete' event
    m_pDlg->m_evPlayComplete.Subscribe(this, &CQASession::OnQuestionPlayComplete);

    // Initialize the randomizer
    srand( (unsigned)time( NULL ) );
}

CQASession::~CQASession(void)
{
}

void CQASession::Start()
{
    _ASSERTE(m_nCurState == TO_BE_STARTED);

    m_nCurState = PREPARING_QUESTIONNAIRE;
    m_bHaltProcessing = false;
}

void CQASession::Stop()
{
    m_nCurState = TO_BE_STARTED;
    m_bHaltProcessing = false;
}

void CQASession::SubmitAnswer()
{
    _ASSERTE(m_nCurState == RECEIVING_ANSWER);
    m_nCurState = EVALUATING_ANSWER;
    m_bHaltProcessing = false;
}

// Answer was wrong. Try again afresh for a new answer (for the same question)
void CQASession::TryAgain()
{
    _ASSERTE(m_nCurState == WRONG_ANSWER);
    m_nCurState = RETRY_AGAIN;
    m_bHaltProcessing = false;
}

void CQASession::ReplayQuestion()
{
    _ASSERTE(m_nCurState == RECEIVING_ANSWER || m_nCurState == AWAITING_ANSWER);

    m_nCurState = POSING_QUESTION;
    m_nQuestionWaitRound = 0;
    m_bHaltProcessing = false;

    m_nReplayCount++; // Increment the Question Replay Count
    ComputeScore(); // Update the Efficiency
}

// Player thread indicated the completion of Play.
// Raised for both answer notes and questions notes.
// We check if this is 'posing_question' state to 
// ensure this is a question that we played.
void CQASession::OnQuestionPlayComplete(const OIL::CEventSource* pSender, OIL::CEventHandlerArgs* pArgs)
{
    if(IsSessionActive() && m_nCurState == POSING_QUESTION)
    {
        m_nCurState = COMPLETED_QUESTION;
        m_bHaltProcessing = false;
    }
}

void CQASession::GoToNextQuestion()
{
    // Make sure we have a valid session going on
    _ASSERTE(m_nCurState != TO_BE_STARTED);

    if(m_nCurState == RIGHT_ANSWER)
    {
        // Nothing to do. User is ready to goto next question. Let him go.

        //TODO: Increment the stats in Process_RightAnswer() itself so he gets a chance to see them.
    }
    else // User is trying to skip the present question
    {
        // TODO: User is skipping the question - Increment the Wrong Answer Count..
    }

    m_nCurState = PREPARING_QUESTION;
    m_bHaltProcessing = false;
}

// Gets the current state in string form
CString CQASession::GetStatusString()
{
    return m_strCurStatus;
}

// Gets more detailed information about the current state
CString CQASession::GetInfoString()
{
    return m_strInfo;
}

// Processes the current state based on the events encountered till now.
// To be called periodically from the OnTimer.
void CQASession::ProcessCurrentState()
{
    switch(m_nCurState)
    {
    case TO_BE_STARTED: ProcessState_ToBeStarted(); break;
    case PREPARING_QUESTIONNAIRE: ProcessState_PreparingQuestionnaire(); break;
    case PREPARE_QUESTIONNAIRE: ProcessState_PrepareQuestionnaire(); break;
    case PREPARING_QUESTION: ProcessState_PreparingQuestion(); break;
    case POSING_QUESTION: ProcessState_PosingQuestion(); break;
    case COMPLETED_QUESTION: ProcessState_CompletedQuestion(); break;
    case AWAITING_ANSWER: ProcessState_AwaitingAnswer(); break;
    case RECEIVING_ANSWER: ProcessState_ReceivingAnswer(); break;
    case EVALUATING_ANSWER: ProcessState_EvaluatingAnswer(); break;
    case WRONG_ANSWER: ProcessState_WrongAnswer(); break;
    case RETRY_AGAIN: ProcessState_TryAgain(); break;
    case RIGHT_ANSWER: ProcessState_RightAnswer(); break;
    }
}

void CQASession::ProcessState_ToBeStarted()
{
    m_strInfo = _T("Use the Piano below to practice the notes or the <i>Test | Start</i> menu option to start a QA Session");
    m_strCurStatus = _T(" - No active QA Session in progress - ");

    m_bHaltProcessing = true; // Wait till 'Start' is Requested
}

// This method is just for UI purpose. PREPARE_QUESTIONNAIRE does the actual work.
void CQASession::ProcessState_PreparingQuestionnaire()
{
    m_strCurStatus = _T("Preparing Questionnaire ... ");
    m_strInfo = _T(" ");

    m_nCurState = PREPARE_QUESTIONNAIRE;
}

void CQASession::ProcessState_PrepareQuestionnaire()
{
    m_nQuestionCount = 0;

    switch(m_nCurLevel)
    {
    case SINGLE_NOTE_SINGLE_OCTAVE: PrepareQuestionnaire_Level1(); break;
    case SINGLE_NOTE_MULTI_OCTAVE: PrepareQuestionnaire_Level2(); break;
    case MULTINOTE: PrepareQuestionnaire_Level3(); break;
    case CARNATIC_RAGA:
    case WESTERN_SCALE: break;
    }

    // When done go to Preparing the first question
    m_nCurState = PREPARING_QUESTION;    
    m_bFirstQuestion = true;
    m_nCurQuestion = -1;
    m_nCorrectAnswerCount = 0; // Number of answers correctly answered eventually
    m_dAccuracy = 1.0f; // Accuracy of Answers (number of retries decrements this)
    m_dEfficiency = 1.0f; // Efficiency of Answers (number of replays decrements this)

    vecEfficiencies.clear();
    vecAccuracies.clear();
}

void CQASession::ProcessState_PreparingQuestion()
{
    m_strCurStatus = m_bFirstQuestion ? _T("Preparing the Question ...") : _T("Preparing Next Question ...");
    m_strInfo = _T("");

    // Clear answers from any previous questions
    m_AnswerNotes.clear();
    m_nRetryCount = 0;
    m_nReplayCount = 0;
    m_nAnswerNotesEntered = 0;

    if(++m_nCurQuestion >= m_nQuestionCount)
    {
        m_strCurStatus = _T("Test Complete");
        m_strInfo = _T("No more questions");
        Stop();        
        RaiseEvent(&evQASessionComplete, &OIL::CEventHandlerArgs()); // Inform any Listeners
        return; // No more questions. Lets halt here and return.
    }

    m_nCurState = POSING_QUESTION;
    m_nQuestionWaitRound = 0;

    m_bFirstQuestion = false;
}

void CQASession::ProcessState_PosingQuestion()
{
    m_strCurStatus = _T("Playing the Notes ... Listen carefully !!");
    m_strInfo.Format(_T("%sQuestion: %d/%d"), 
        ((m_nRetryCount > 0 || m_nReplayCount > 0)? _T("Replaying ") : _T("")), 
        m_nCurQuestion+1, m_nQuestionCount);

    m_pDlg->m_NotesToPlay = m_Questionnaire.at(m_nCurQuestion);
    m_pDlg->m_evPlayNotes.SetEvent();

    // Wait till we receive 'Play Complete Event'
    m_bHaltProcessing = true; 
}

void CQASession::ProcessState_CompletedQuestion()
{
    if(m_AnswerNotes.size()==0)
    {
        m_nCurState = AWAITING_ANSWER;
        m_nAnswerWaitRound = 0;
        ProcessState_AwaitingAnswer();
    }
    else // if user has already entered few notes earlier for this question
    {
        m_nCurState = RECEIVING_ANSWER;
        ProcessState_ReceivingAnswer();
    }
}

void CQASession::ProcessState_AwaitingAnswer()
{
    m_strCurStatus = (m_nAnswerWaitRound%2) ? _T("Awaiting Answer...") : _T("");

    if(m_nCurLevel <= CQASession::CARNATIC_RAGA)
        m_strInfo = _T("Use the Piano below to answer the question. When done, press Enter to Submit");
    else
        m_strInfo = _T("Use the above ComboBox to answer the question. When done, press Enter to Submit");

    m_nAnswerWaitRound++;
}

void CQASession::ProcessState_ReceivingAnswer()
{
    m_strCurStatus = _T("  Submit the Answer..");

    if(m_nCurLevel <= CQASession::CARNATIC_RAGA)
        m_strInfo = _T("Use the Piano below to alter the Answer. When satisfied, press Enter to Submit");
    else
        m_strInfo = _T("Alter the Answer using the above ComboBox. When satisfied, press Enter to Submit");

    m_bHaltProcessing = true;
}

void CQASession::ProcessState_EvaluatingAnswer()
{
    //static bool nEvenRound = 0;
    //nEvenRound = !nEvenRound;

    m_strCurStatus = _T("Verifying the Answer...");
    m_strInfo = _T("");

    m_nCurState = (m_AnswerNotes == m_Questionnaire.at(m_nCurQuestion)) ? RIGHT_ANSWER : WRONG_ANSWER;
    m_nResultAnouncementRound = 0;

    m_nResultsInfoIndex = (int)((double)rand() * 10 / (double)(RAND_MAX + 1) ); // 10 entries in the szWrong and szCorrect entries below

    if(m_nCurState == RIGHT_ANSWER) // in case of wrong answers, user gets retry chance
    {
        m_nCorrectAnswerCount++; // number of times RIGHT_ANSWER state is entered
    }        
    //ComputeScore(); // for wrong answer - we compute in ProcessState_TryAgain()
}

const TCHAR* szWrong[] = {_T("<font color=\"#cc3300\">Need to do better...Try again!!</font>"), 
                    _T("<font color=\"#cc3300\">Wrong Guess...Try again!!</font>"), 
                    _T("<font color=\"#cc3300\">Wrong Entry...Try again!!</font>"), 
                    _T("<font color=\"#cc3300\">Wrong Answer...Try again!!</font>"), 
                    _T("<font color=\"#cc3300\">Oooops...Try again!!</font>"), 
                    _T("<font color=\"#cc3300\">Thats not Correct...Try again!!</font>"),
                    _T("<font color=\"#cc3300\">Thats a near miss...Try again!!</font>"),
                    _T("<font color=\"#cc3300\">Listen carefully and answer correctly ...!!</font>"),
                    _T("<font color=\"#cc3300\">Concentrate and Try again...!!</font>"),
                    _T("<font color=\"#cc3300\">Try again...!!</font>"),};

void CQASession::ProcessState_WrongAnswer()
{
    m_strCurStatus = _T("Wrong Answer...");
    m_strInfo = szWrong[m_nResultsInfoIndex];

    if(m_bWaitBeforeRetry)
    {
        m_bHaltProcessing = true; // Stop till user presses 'Enter'
        m_strInfo += _T("<br/><p>Press <i>Enter</i> to Retry a new answer..</p>");
    }
    else
        if(m_nResultAnouncementRound++ >= 2) // Wait sometime before we go to next question
            TryAgain();
}

void CQASession::ProcessState_TryAgain()
{
    m_strCurStatus = _T("Verifiying Retry options...");
    m_strInfo = _T(" ");

    if(m_nRetryCount++ >= 9) // If we have run out of tries
    {
        m_strCurStatus = _T("Run out of Retries");
        m_strInfo = _T("No more retries possible...<br/><p>Press <i>Enter</i> to go to next question..</p>");
        m_bHaltProcessing = true; // Stop till user presses 'Enter'
        m_nCurState = RIGHT_ANSWER; // So that when pressed enter, user can be redirected to goto next question
        // usually this should not happen. Because with exhaused tries, user 'should'
        // step on the right answer somehow.
        // However, when we restrict the retries - we add the answer ourselves with a penalty 
        // and go to next question
    }
    else
        m_nCurState = POSING_QUESTION; // Pose the same question again.

//    m_nAnswerNotesEntered -= m_Questionnaire[m_nCurQuestion].size(); // Give few keys discount for efficiency calculation
}

const TCHAR* szCorrect[] = {_T("<font color=green>Good Work...!!</font>"), 
                    _T("<font color=green>Good Guess...!!</font>"), 
                    _T("<font color=green>Fantastic Work...!!</font>"), 
                    _T("<font color=green>Fantastic Job...!!</font>"), 
                    _T("<font color=green>Thats Wonderful...!!</font>"),
                    _T("<font color=green>Thats Awesome...!!</font>"),
                    _T("<font color=green>Thats Correct...!!</font>"),
                    _T("<font color=green>Excellent Guess...!!</font>"),
                    _T("<font color=green>Excellent Work...!!</font>"),
                    _T("<font color=green>Excellent Job...!!</font>")};

void CQASession::ProcessState_RightAnswer()
{      
    m_strCurStatus = _T("Right Answer...");
    m_strInfo = szCorrect[m_nResultsInfoIndex];

    if(m_bWaitBeforeNewQuestion)
    {
        m_bHaltProcessing = true; // Stop till user presses 'Enter'
        m_strInfo += _T("<br/><p>Press <i>Enter</i> when ready to goto next question..</p>");
    }
    else
        if(m_nResultAnouncementRound++ >= 2) // Wait sometime before we go to next question
            GoToNextQuestion();
}

// Answer Notes entered
void CQASession::AnswerEntered(unsigned char MidiNoteNumber)
{
    switch(m_nCurLevel)
    {
    case SINGLE_NOTE_SINGLE_OCTAVE:
    case SINGLE_NOTE_MULTI_OCTAVE:
        {
            // We maintain only single element. Either replace it or add it
            if(m_AnswerNotes.size() == 0) m_AnswerNotes.push_back(MidiNoteNumber);
            m_AnswerNotes.at(0) = MidiNoteNumber; break;
        }
    case MULTINOTE: 
        {            
            UINT QuestionLength = m_Questionnaire.at(m_nCurQuestion).size();
            if(m_AnswerNotes.size() >= QuestionLength)
                m_AnswerNotes.erase(m_AnswerNotes.begin()); // Remove the front element                         
            m_AnswerNotes.push_back(MidiNoteNumber); // Add the new one at the end
            break;
        }
    case CARNATIC_RAGA:
    case WESTERN_SCALE:
        {
            //TODO: Make sure this is taken care of
            _ASSERTE(_T("Not Supported") == NULL); break;
        }        
    }
    m_nCurState = RECEIVING_ANSWER;

    // Update the Efficiency Score
    m_nAnswerNotesEntered++;
}

// Single Note - Middle Octave Level
void CQASession::PrepareQuestionnaire_Level1()
{
    const int MID_C = 60; // Midi Number for Middle C

    // Clear the memory
    m_Questionnaire.clear();

    // Add three sets of the octave
    for(int i=0; i < 12; ++i)
    {
        NOTES question;
        question.push_back(MID_C + i);
        m_Questionnaire.push_back(question);
    }
    for(int i=0; i < 12; ++i)
    {
        NOTES question;
        question.push_back(MID_C + i);
        m_Questionnaire.push_back(question);
    }
    for(int i=0; i < 12; ++i)
    {
        NOTES question;
        question.push_back(MID_C + i);
        m_Questionnaire.push_back(question);
    }    
    // Now shuffle them
    std::random_shuffle(m_Questionnaire.begin(), m_Questionnaire.end());

    this->m_nQuestionCount = m_Questionnaire.size();
    this->m_nCurQuestion = -1;
}
 
// Single Note - All octaves [1-5] Level
void CQASession::PrepareQuestionnaire_Level2()
{
    struct { int LowKey; int HighKey; } KeyLimits[] = 
    {
        {36, 55}, // C1 to G2
        {43, 67}, // G1 to G3
        {55, 79}, // G2 to G4
        {67, 91}, // G3 to G5
        {79, 100} // G5 to E7
    };

    // Fill and Shuffle the octave numbers. This makes sure
    // We get different octave order each time.
    std::vector<int> Octaves;
    Octaves.push_back(0);   
    Octaves.push_back(1);    
    Octaves.push_back(2);
    Octaves.push_back(3);   
    Octaves.push_back(4);       
    Octaves.push_back(1);    
    Octaves.push_back(2);
    Octaves.push_back(3);   
    std::random_shuffle(Octaves.begin(), Octaves.end());

    // Clear the memory
    m_Questionnaire.clear();

    // We create questions seperately for each octave.
    // This makes sure that keys are reletively close 
    // to each other and all notes in an octave
    // are covered before proceeding with other octaves
    QUESTIONNAIRE Questionnaire;
    for(int nOctaveNum=0, nOctaveCount = Octaves.size(); nOctaveNum < nOctaveCount; ++nOctaveNum)
    {
        Questionnaire.clear();
        for(int noteId=KeyLimits[Octaves[nOctaveNum]].LowKey, 
                nMax = KeyLimits[Octaves[nOctaveNum]].HighKey; 
                noteId <= nMax; noteId++)
        {
            NOTES question;
            question.push_back(noteId);
            Questionnaire.push_back(question);
        }
        std::random_shuffle(Questionnaire.begin(), Questionnaire.end()); // Randomize
        m_Questionnaire.insert(m_Questionnaire.end(), Questionnaire.begin(), Questionnaire.end());
    }  
    
    this->m_nQuestionCount = m_Questionnaire.size();
    this->m_nCurQuestion = -1;
}

// Mutiple Notes Level
void CQASession::PrepareQuestionnaire_Level3()
{
    std::vector<int> AllNotes;
    for(int i= 36; i <= 100; ++i) // Insert all notes
    {
        AllNotes.push_back(i);
    }
    std::random_shuffle(AllNotes.begin(), AllNotes.end());
    
    // Clear the memory
    m_Questionnaire.clear();

    // Randomly compute the length of each question
    // and pick up the notes of the question from randomized All notes sequentially
    for(int questionId = 0, nMax = AllNotes.size(); questionId < nMax; ++questionId)
    {
        int nLen = (AllNotes[questionId] % 5) + 1;
           
        NOTES question;
        for(int i=0; i < nLen; ++i)        
            question.push_back(AllNotes[(i + questionId) < nMax ? (i + questionId) : questionId]);
        
        std::random_shuffle(question.begin(), question.end());
        m_Questionnaire.push_back(question);
    }    

    this->m_nQuestionCount = m_Questionnaire.size();
    this->m_nCurQuestion = -1;
}

//Scale-Raga Level
void CQASession::PrepareQuestionnaire_Level4()
{
    // Clear the memory
    m_Questionnaire.clear();

    _ASSERTE(_T("Currently Not Supported") == NULL);

    this->m_nQuestionCount = m_Questionnaire.size();
    this->m_nCurQuestion = -1;
}


void CQASession::ComputeScore()
{
    if(m_nCurQuestion < 0 || this->IsSessionActive() == false) return;

    if(vecAccuracies.size() <= m_nCurQuestion) vecAccuracies.push_back(0);
    if(vecEfficiencies.size() <= m_nCurQuestion) vecEfficiencies.push_back(0);

    // Compute Accuracy based on Retry Count
    double dAccu=1.0f, dNum=10, dDen=12;
    for(UINT i=0; i < m_nRetryCount; ++i)
    {
        dAccu *= (dNum / dDen);
        dNum--; dDen--;
    }
    vecAccuracies.at(m_nCurQuestion) = dAccu;

    // Compute Efficiency based on Replay Count and Keyboard number of inputs
    double dEff=1.0f; dNum=10, dDen=12;
    for(UINT i=0; i < m_nReplayCount; ++i)
    {
        dEff *= (dNum / dDen);
        dNum--; dDen--;
    }

    double dKeyboardEff=1.0f;
    UINT nKeysReqd = m_Questionnaire[m_nCurQuestion].size() * 2; // *2 to give some free extra keys
    if(m_nAnswerNotesEntered > nKeysReqd) // if extra keys pressed
    {
        dKeyboardEff = (double)nKeysReqd/(double)m_nAnswerNotesEntered;
    }

    vecEfficiencies.at(m_nCurQuestion) =  (dEff + dKeyboardEff)/2.0f;

    dAccu = 0; dEff =0;
    for(UINT i=0; i <= m_nCurQuestion; ++i)
    {
        dAccu += vecAccuracies[i];
        dEff += vecEfficiencies[i];
    }

    m_dAccuracy = dAccu/vecAccuracies.size();
    m_dEfficiency = dEff/vecEfficiencies.size();
}
