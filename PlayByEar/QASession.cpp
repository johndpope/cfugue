#include "StdAfx.h"
#include "QASession.h"

CQASession::CQASession(void) 
    : m_nCurState(TO_BE_STARTED), 
    m_bFirstQuestion(true), 
    m_nAnswerWaitRound(0),
    m_nResultAnouncementRound(0),
    m_nCurLevel(SINGLE_NOTE_SINGLE_OCTAVE)
{
}

CQASession::~CQASession(void)
{
}

void CQASession::Start()
{
    _ASSERTE(m_nCurState == TO_BE_STARTED);

    m_nCurState = PREPARING_QUESTION;
}

void CQASession::Stop()
{
    m_nCurState = TO_BE_STARTED;
}

void CQASession::SubmitAnswer()
{
    _ASSERTE(m_nCurState == AWAITING_ANSWER);
    m_nCurState = EVALUATING_ANSWER;
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
    case PREPARING_QUESTION: ProcessState_PreparingQuestion(); break;
    case POSING_QUESTION: ProcessState_PosingQuestion(); break;
    case AWAITING_ANSWER: ProcessState_AwaitingAnswer(); break;
    case EVALUATING_ANSWER: ProcessState_EvaluatingAnswer(); break;
    case WRONG_ANSWER: ProcessState_WrongAnswer(); break;
    case RIGHT_ANSWER: ProcessState_RightAnswer(); break;
    }
}

void CQASession::ProcessState_ToBeStarted()
{
    m_strInfo = _T("Use the Piano below to practice the notes or the <i>Test | Start</i> menu option to start a QA Session");
    m_strCurStatus = _T(" - No active QA Session in progress - ");
}

void CQASession::ProcessState_PreparingQuestion()
{
    m_strCurStatus = m_bFirstQuestion ? _T("Preparing the Question ...") : _T("Preparing Next Question ...");
    m_strInfo = _T("");

    m_nCurState = POSING_QUESTION;

    m_bFirstQuestion = false;
}

void CQASession::ProcessState_PosingQuestion()
{
    m_strCurStatus = _T("Playing the Notes ... Listen carefully !!");
    m_strInfo = _T("");
    m_nCurState = AWAITING_ANSWER;

    m_nAnswerWaitRound = 0;
}

void CQASession::ProcessState_AwaitingAnswer()
{
    m_strCurStatus = (m_nAnswerWaitRound%2) ? _T("Awaiting Answer...") : _T("");
    m_strInfo = _T("Use the Piano below to answer the question. When done, press Enter to Submit");

    m_nAnswerWaitRound++;
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

    if(m_nResultAnouncementRound++ >= 2) // Wait sometime before we go to next question
    {
        m_nCurState = PREPARING_QUESTION;
        m_nResultAnouncementRound = 0;
    }
}

void CQASession::ProcessState_RightAnswer()
{
    m_strCurStatus = _T("Right Answer...");
    m_strInfo = _T("Good work...");

    if(m_nResultAnouncementRound++ >= 2) // Wait sometime before we go to next question
    {
        m_nCurState = PREPARING_QUESTION;
        m_nResultAnouncementRound = 0;
    }
}
