#include "StdAfx.h"
#include "PlayByEarDlg.h"
#include "QASession.h"

CQASession::CQASession(CPlayByEarDlg* pDlg) :
    m_pDlg(pDlg),
    m_bFirstQuestion(true), 
    m_nQuestionWaitRound(0),
    m_nAnswerWaitRound(0),
    m_nRetryCount(0),
    m_nResultAnouncementRound(0),
    m_nCurState(TO_BE_STARTED),
    m_nCurLevel(SINGLE_NOTE_SINGLE_OCTAVE),
    m_nQuestionCount(0),
    m_bHaltProcessing(false),
    m_bWaitBeforeNewQuestion(true),
    m_bWaitBeforeRetry(true)
{
    // Subscribe to the 'Play Complete' event
    m_pDlg->m_evPlayComplete.Subscribe(this, &CQASession::OnQuestionPlayComplete);
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
    case PREPARING_QUESTIONNAIRE: ProcessState_PreparingQuestionnnaire(); break;
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

void CQASession::ProcessState_PreparingQuestionnnaire()
{
    m_strCurStatus = _T("Preparing Questionnnaire ... ");
    m_strInfo = _T(" ");

    m_nQuestionCount = 5;

    // When done go to Preparing the first question
    m_nCurState = PREPARING_QUESTION;    
    m_bFirstQuestion = true;
    m_nCurQuestion = -1;
}

void CQASession::ProcessState_PreparingQuestion()
{
    m_strCurStatus = m_bFirstQuestion ? _T("Preparing the Question ...") : _T("Preparing Next Question ...");
    m_strInfo = _T("");

    // Clear answers from any previous questions
    m_AnswerNotes.clear();
    m_nRetryCount = 0;

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
    m_strInfo.Format(_T("Question: %d/%d"), m_nCurQuestion+1, m_nQuestionCount);

    ANSWERNOTES ans;
    ans.push_back(60);
    ans.push_back(70);
    ans.push_back(80);
    ans.push_back(60);
    ans.push_back(80);
    ans.push_back(60);

    // Set the event to play the notes
    m_pDlg->m_NotesToPlay = ans;
    m_pDlg->m_evPlayNotes.SetEvent();

    // Wait till we receive 'Play Complete Event'
    m_bHaltProcessing = true; 

    //else // playing is false - completed 
    //{
    //    if(m_nQuestionWaitRound++ > 1)
    //        m_nCurState = COMPLETED_QUESTION;
    //}

                //if(m_nQuestionWaitRound++ > 3)
                //    m_nCurState = COMPLETED_QUESTION;

    //do
    //{
    //    _ASSERTE(m_nCurState == POSING_QUESTION);

    //    switch(MsgWaitForMultipleObjects(0, NULL, false, 500, QS_INPUT))
    //    {
    //    case WAIT_TIMEOUT:
    //        {
    //            if(m_nQuestionWaitRound++ > 3)
    //            {
    //                m_nCurState = COMPLETED_QUESTION;
    //                return; // return from this
    //            }
    //            break;
    //        }
    //    default: // Some Window Message arrived
    //        {
    //            MSG msg;
    //            while(PeekMessage(&msg, m_hWnd, 0, 0, PM_NOREMOVE))
    //            {
    //                //if(msg.message == WM_TIMER) continue; //ignore the timer messages
    //                //TranslateMessage(&msg);
    //                //DispatchMessage(&msg);
    //            }
    //            break;
    //        }
    //    }
    //}while(true);
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
    static bool nEvenRound = 0;
    nEvenRound = !nEvenRound;

    m_strCurStatus = _T("Verifying the Answer...");
    m_strInfo = _T("");

    m_nCurState = nEvenRound ? WRONG_ANSWER : RIGHT_ANSWER;
    m_nResultAnouncementRound = 0;
}

void CQASession::ProcessState_WrongAnswer()
{
    m_strCurStatus = _T("Wrong Answer...");
    m_strInfo = _T("Ooops...");

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

    if(m_nRetryCount++ >= 2) // If we have run out of tries
    {
        m_strCurStatus = _T("Run out of Retries");
        m_strInfo = _T("No more retries possible...<br/><p>Press <i>Enter</i> when ready to goto next question..</p>");
        m_bHaltProcessing = true; // Stop till user presses 'Enter'
        m_nCurState = RIGHT_ANSWER; // So that when pressed enter, user can be redirected to goto next question
        // usually this should not happen. Because with exhaused tries, user 'should'
        // step on the right answer somehow.
        // However, when we restrict the retries - we add the answer ourselves with a penalty 
        // and go to next question
    }
    else
        m_nCurState = POSING_QUESTION; // Pose the same question again.
}

void CQASession::ProcessState_RightAnswer()
{
    m_strCurStatus = _T("Right Answer...");
    m_strInfo = _T("Good work...");

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
            //TODO: Make sure the length is correct as per question
            m_AnswerNotes.push_back(MidiNoteNumber); break;
        }
    case CARNATIC_RAGA:
    case WESTERN_SCALE:
        {
            //TODO: Make sure this is taken care of
            m_AnswerNotes.push_back(MidiNoteNumber); break;
        }        
    }
    m_nCurState = RECEIVING_ANSWER;
}

