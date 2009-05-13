#ifndef _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_
#define _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_

class CQASession
{
protected:
    enum STATE : long
    {
        TO_BE_STARTED = 0,        
        PREPARING_QUESTION,
        POSING_QUESTION,
        AWAITING_ANSWER,
        EVALUATING_ANSWER,
        WRONG_ANSWER,
        RIGHT_ANSWER,
    } m_nCurState;  // Keeps track of the current state

    enum LEVEL: long
    {
        SINGLE_NOTE_SINGLE_OCTAVE = 0,
        SINGLE_NOTE_MULTI_OCTAVE,
        MULTINOTE,
        CARNATIC_RAGA,
        WESTERN_SCALE
    } m_nCurLevel; // Question Difficulty Level

    void ProcessState_ToBeStarted();
    void ProcessState_PreparingQuestion();
    void ProcessState_PosingQuestion();
    void ProcessState_AwaitingAnswer();
    void ProcessState_EvaluatingAnswer();
    void ProcessState_WrongAnswer();
    void ProcessState_RightAnswer();

    CString m_strCurStatus;
    CString m_strInfo;

    bool m_bFirstQuestion;
    UINT m_nAnswerWaitRound;
    UINT m_nResultAnouncementRound;

public:
    CQASession(void);
    ~CQASession(void);

    // Starts (or Resumes a previously stopped) QA Session
    void Start();

    // Stops the current QA Session. Does nothing if no Active Session
    void Stop();

    // Answer Submitted
    void SubmitAnswer();

    // Gets the current state in string form
    CString GetStatusString();

    // Gets more detailed information about the current state
    CString GetInfoString();

    // Processes the current state based on the events encountered till now.
    // To be called periodically from the OnTimer.
    void ProcessCurrentState();
};

#endif // _QASESSION_H__10BEE106_416D_403e_860A_4FD80B33D9C2_