#ifndef _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_
#define _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_

class CQASession
{
public:     
    enum LEVEL: long
    {
        SINGLE_NOTE_SINGLE_OCTAVE = 0,
        SINGLE_NOTE_MULTI_OCTAVE,
        MULTINOTE,
        CARNATIC_RAGA,
        WESTERN_SCALE
    };

    enum STATE : long
    {
        TO_BE_STARTED = 0, // Starts Here and Stops Here.
        PREPARING_QUESTIONNAIRE, // Comes From: {TO_BE_STARTED} Goes To: {PREPARING_QUESTION}
        PREPARING_QUESTION, // Comes From: {TO_BE_STARTED, RIGHT_ANSWER} or External Event: GoToNextQuestion(), Goes To: {POSING_QUESTION}
        POSING_QUESTION, // Comes From: {PREPARING_QUESTION}, Goes To: {COMPLETED_QUESTION}
        COMPLETED_QUESTION, // Comes From: {POSING_QUESTION}, Goes To: {AWAITING_ANSWER, RECEIVING_ANSWER}
        AWAITING_ANSWER, // Comes From: {COMPLETED_QUESTION}, Loops in...
        RECEIVING_ANSWER, // Comes From: {COMPLETED_QUESTION} or External Event: AnswerEntered(), Halts Processing; Goes to: {EVALUATING_ANSWER} on SubmitAnswer()
        EVALUATING_ANSWER, // Comes From: External Event: SubmitAnswer(), Goes to: {WRONG_ANSWER, RIGHT_ANSWER}
        WRONG_ANSWER, // Comes From: {EVALUATING_ANSWER}, Halts Processing; Goes to: {RETRY_AGAIN} on TryAgain();
        RETRY_AGAIN,  // Comes From: {WRONG_ANSWER}, Goes to: {POSING_QUESTION, PREPARING_QUESTION}
        RIGHT_ANSWER, // Comes From: {EVALUATING_ANSWER}, Goes to: {PREPARING_QUESTION}
    } ;  

protected:
    STATE m_nCurState; // Keeps track of the current state
    LEVEL m_nCurLevel; // Question Difficulty Level

    void ProcessState_ToBeStarted();
    void ProcessState_PreparingQuestionnnaire();
    void ProcessState_PreparingQuestion();
    void ProcessState_PosingQuestion();
    void ProcessState_CompletedQuestion();
    void ProcessState_AwaitingAnswer();
    void ProcessState_ReceivingAnswer();
    void ProcessState_EvaluatingAnswer();
    void ProcessState_WrongAnswer();
    void ProcessState_TryAgain();
    void ProcessState_RightAnswer();

    CString m_strCurStatus;
    CString m_strInfo;

    bool m_bFirstQuestion;
    UINT m_nQuestionWaitRound; // Used to 'hold' the question play duration
    UINT m_nAnswerWaitRound; // Used for blinking the answer.
    UINT m_nRetryCount; // Used to keep track of the Retry options
    bool m_bHaltProcessing; // Should external module stop calling the ProcessCurrentState() ?

    UINT m_nQuestionCount; // Total Number of questions
    int  m_nCurQuestion; // Keeps track of the current question index

    std::vector<unsigned char> m_AnswerNotes;   // Sequence of MidiNotes supplied as Answer

public:
    CQASession(void);
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
    typedef std::vector<unsigned char> ANSWERNOTES;
    const ANSWERNOTES& GetAnswerNotes() { return m_AnswerNotes; }

    // Sets the current question difficulty level
    inline void SetCurrentLevel(LEVEL level) { m_nCurLevel = level; }

    // Gets the current question difficulty level
    inline LEVEL GetCurrentLevel() const { return m_nCurLevel;}

    // Gets the Current State of the QASession
    inline STATE GetCurrentState() const { return m_nCurState; }

    // Should external module stop calling the ProcessCurrentState() ?
    inline bool ShouldHaltProcessing() const { return m_bHaltProcessing; }

};

#endif // _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_