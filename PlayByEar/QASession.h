#ifndef _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_
#define _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_

#include "EventHandler.h"

class CPlayByEarDlg;

class CQASession : OIL::CEventSource, public OIL::CEventReceiver
{
public:     
    enum LEVEL: long
    {
        SINGLE_NOTE_SINGLE_OCTAVE = 0,
        SINGLE_NOTE_MULTI_OCTAVE,
        MULTINOTE_SINGLE_OCTAVE,
        MULTINOTE_MULTI_OCTAVE,
        CARNATIC_RAGA,
        WESTERN_SCALE
    };

    enum STATE : long
    {
        TO_BE_STARTED = 0, // Starts Here and Stops Here.
        PREPARING_QUESTIONNAIRE, // Comes From: {TO_BE_STARTED} Goes To: {PREPARE_QUESTIONNAIRE}; PREPARING_QUESTIONNAIRE is just for UI updates; PREPARE_QUESTIONNAIRE does the actual job
        PREPARE_QUESTIONNAIRE, // Comes From: {PREPARING_QUESTIONNAIRE} Goes To: {PREPARING_QUESTION}
        PREPARING_QUESTION, // Comes From: {TO_BE_STARTED, RIGHT_ANSWER} or External Event: GoToNextQuestion(), Goes To: {POSING_QUESTION}
        POSING_QUESTION, // Comes From: {PREPARING_QUESTION}, Goes To: {COMPLETED_QUESTION} when Question is Complete
        COMPLETED_QUESTION, // Triggered when Play is Complete; Goes To: {AWAITING_ANSWER, RECEIVING_ANSWER}
        AWAITING_ANSWER, // Comes From: {COMPLETED_QUESTION}, Loops in...
        RECEIVING_ANSWER, // Comes From: {COMPLETED_QUESTION} or External Event: AnswerEntered(), Halts Processing; Goes to: {EVALUATING_ANSWER} on SubmitAnswer()
        EVALUATING_ANSWER, // Comes From: External Event: SubmitAnswer(), Goes to: {WRONG_ANSWER, RIGHT_ANSWER}
        WRONG_ANSWER, // Comes From: {EVALUATING_ANSWER}, Halts Processing; Goes to: {RETRY_AGAIN} on TryAgain();
        RETRY_AGAIN,  // Comes From: {WRONG_ANSWER}, Goes to: {POSING_QUESTION, PREPARING_QUESTION}
        RIGHT_ANSWER, // Comes From: {EVALUATING_ANSWER}, Goes to: {PREPARING_QUESTION}
    } ;  

    typedef std::vector<unsigned char> NOTES;

protected:
    STATE m_nCurState; // Keeps track of the current state
    LEVEL m_nCurLevel; // Question Difficulty Level

    void ProcessState_ToBeStarted();
    void ProcessState_PreparingQuestionnaire();
    void ProcessState_PrepareQuestionnaire();
    void ProcessState_PreparingQuestion();
    void ProcessState_PosingQuestion();
    void ProcessState_CompletedQuestion();
    void ProcessState_AwaitingAnswer();
    void ProcessState_ReceivingAnswer();
    void ProcessState_EvaluatingAnswer();
    void ProcessState_WrongAnswer();
    void ProcessState_TryAgain();
    void ProcessState_RightAnswer();

    void OnQuestionPlayComplete(const OIL::CEventSource* pSender, OIL::CEventHandlerArgs* pArgs);

    void PrepareQuestionnaire_Level1();
    void PrepareQuestionnaire_Level2();
    void PrepareQuestionnaire_Level3();
    void PrepareQuestionnaire_Level4();
    void PrepareQuestionnaire_Level5();

    CString m_strCurStatus;
    CString m_strInfo;

    bool m_bFirstQuestion;
    UINT m_nQuestionWaitRound; // Used to 'hold' the question play duration
    UINT m_nAnswerWaitRound; // Used for blinking the answer.
    UINT m_nRetryCount; // Used to keep track of the Retry options
    UINT m_nReplayCount; // Used to keep track of number of times questions was replayed
    UINT m_nResultAnouncementRound; // Used to 'hold' the Results
    bool m_bHaltProcessing; // Should external module stop calling the ProcessCurrentState() ?
    int m_nResultsInfoIndex; // Index of the string to be used to display Results
    UINT m_nCorrectAnswerCount; // Number of questions answered correctly (even after retries)
    double m_dAccuracy; // Accuracy of Answers (number of retries decrements this)
    double m_dEfficiency; // Efficiency of Answers (number of replays decrements this)
    UINT m_nAnswerNotesEntered; // Keeps track of number of keys pressed as part of answer (Affects Efficiency)

    std::vector<double> vecEfficiencies;
    std::vector<double> vecAccuracies;

    bool m_bWaitBeforeNewQuestion; // Should Wait before posing next question (after correct answer)?
    bool m_bWaitBeforeRetry; // Should Wait before Reposing the same question (for wrong answer) ?

    typedef std::vector<NOTES> QUESTIONNAIRE;
    QUESTIONNAIRE m_Questionnaire; // The questions prepeared

    int  m_nQuestionCount; // Total Number of questions
    int  m_nCurQuestion; // Keeps track of the current question index

    std::vector<unsigned char> m_AnswerNotes;   // Sequence of MidiNotes supplied as Answer

    CPlayByEarDlg* m_pDlg; // The Window associated with this QASession

public:
    OIL::CEvent evQASessionComplete; // Raised when a QASession is Complete

    CQASession(CPlayByEarDlg* pDlg);
    ~CQASession(void);

    // Starts (or Resumes a previously stopped) QA Session
    void Start();

    // Stops the current QA Session. Does nothing if no Active Session
    void Stop();

    // Answer Notes entered
    void AnswerEntered(unsigned char MidiNoteNumber);

    // Answer Submitted
    void SubmitAnswer();

    // Answer was wrong. Try again afresh for a new answer (for the same question)
    void TryAgain();

    // Request for Question Replay
    void ReplayQuestion();

    // Request to Skip to N 
    void GoToNextQuestion();

    // Gets the current state in string form
    CString GetStatusString();

    // Gets more detailed information about the current state
    CString GetInfoString();

    // Processes the current state based on the events encountered till now.
    // To be called periodically from the OnTimer() if ShouldHaltProcessing()==false
    void ProcessCurrentState();

    // Returns if there is an active QASession in progress.
    // True if Start has been called (and not yet stopped)
    inline bool IsSessionActive() const { return m_nCurState != TO_BE_STARTED; } 

    // Retrieves the Current Answer Notes
    const NOTES& GetAnswerNotes() { return m_AnswerNotes; }

    // Sets the current question difficulty level
    inline void SetCurrentLevel(LEVEL level) { m_nCurLevel = level; }

    // Gets the current question difficulty level
    inline LEVEL GetCurrentLevel() const { return m_nCurLevel;}

    // Gets the Current State of the QASession
    inline STATE GetCurrentState() const { return m_nCurState; }

    // Gets the Current Question Number. Valid only if IsSessionActive() is true.
    inline int GetCurrentQuestionNumber() const { return m_nCurQuestion; }

    // Returns the total number of questions for the present QASession, if any
    inline int GetQuestionCount() const { return m_nQuestionCount; }

    // Should external module stop calling the ProcessCurrentState() ?
    inline bool ShouldHaltProcessing() const { return m_bHaltProcessing; }

    inline bool GetWaitBeforeRetry() const { return m_bWaitBeforeRetry; }

    inline bool GetWaitBeforeNewQuestion() const { return m_bWaitBeforeNewQuestion; }

    // Should Wait before a retry (after a wrong answer)?
    inline void SetWaitBeforeRetry(bool bWait=true)  { m_bWaitBeforeRetry = bWait; }

    // Should Wait before posing a new question (after a right answer)?
    inline void SetWaitBeforeNewQuestion(bool bWait=true)  { m_bWaitBeforeNewQuestion = bWait; }   

    inline double GetAccuracy() const { return m_dAccuracy; }
    inline double GetEfficiency() const { return m_dEfficiency; }
    inline UINT GetCorrectAnswers() const { return m_nCorrectAnswerCount; }

    void ComputeScore();
};

#endif // _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_