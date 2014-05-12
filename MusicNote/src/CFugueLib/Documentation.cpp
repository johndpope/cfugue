/*
    This is part of CFugue, a C++ Runtime environment for MIDI Score Programming
    Copyright (C) 2009 Gopalakrishna Palem

    For links to further information, or to contact the author,
    see <http://cfugue.sourceforge.net/>.
*/

/////////////////////////////////////////////////////////////////
// Main Page
/////////////////////////////////////////////////////////////////
/*! \mainpage CFugue, The C++ Music Programming Library

\section mainPageContents Contents
    - \ref overview
    - \ref features
    - \ref pageExamples "Examples"
    - \ref pageMusicString "CFugue MusicString"
    - \ref download "Download CFugue"
    - \ref copyright "Contact us"

\section overview Overview

CFugue, meaning Carnatic Fugue or the C/C++ replacement of JFugue, is a high level Music Programming Library.

CFugue makes it possible to play music notes directly from C/C++ programs,
without ever having to deal with the low-level MIDI complexities. This library 
provides a beautiful abstraction that lets you concetrate on programming the <i>Music</i> rather
than worry about the MIDI nuances.

CFugue has numerous features that make it possible to use it directly from many 
platforms, including but not limited to ASP pages, .Net applications and even non-Windows
based systems.

\section usage Usage

Using CFugue to play music is as easy as writing plain music notes. Just create
a Player object and call the Play method on it, supplying the Music notes to be played. Simple.
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {
        CFugue::Player player; <span class="comment">// Create the Player Object</span>
        player.Play("C D E F G A B"); <span class="comment">// Play the Music Notes on the default MIDI output port</span>
    }
</pre>
And the music notes are not restricted to be of just Western sytle either. CFugue fully 
supports both Western and Carnatic Music notations - with a simple hint from the \ref subKeySignatures "KeySignature"
directive, one should be able to switch between them seemlessly.

For more usage demonstrations, please refer to \ref pageExamples "CFugue examples".

\section features Features
CFugue is a C++ runtime environment for music note programming with below features:
    - Highlevel music programming library with direct support for C++ bindings and .Net P/Invoke calls
    - Provides COM interfaces to enable easy integration with ASP clients and COM compatible clients, such as Delphi, VB etc.
    - Easy to use notation. Writing music for CFugue is as easy as writing plain music notes.
    - Event subscription model allows easy extension of the library to allow advanced features such as customized music renderers and parsers.
    - Platform compatible code to make the library accessible from a veriety of platforms such as Windows and multiple variants of Linux: OpenSuse, Ubuntu etc.
    - Fully compatible with both Western style of music and Carnatic music. Offers below capabilities:
        - Specifying simples notes (such as C, D, E .. for Western and S, R, G .. for Carnatic)
        - Specifying note durations
        - Specifying ties, chords, sequential notes and parallel notes
        - Specifying Raga, Tala and Speed directives
        - Specifying MIDI Instruments and tracks
        - and more... 
        
For further details on CFugue music note specifications, please refer to \ref pageMusicString "CFugue MusicString".

\section download Download

CFugue is ready to be used in your applications. It can be found at the downloads section of http://sourceforge.net/projects/cfugue/

\section limitations Limitations

CFugue is under active development, with much of the features, such as rendering to sheet music, etc. 
pending. By the time the development completes, we hope to have this Limitations section to be empty.

\section copyright Copyright

This is a product of CineFx Research Labs, made available free of charge for personal and research use. 
For commercial usage, please contact the author. 

CFugue is distributed with the hope that it will be useful.
No warranty of what-so-ever is implied, including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 Author: <a href="http://gopalakrishna.palem.in/">Gopalakrishna Palem</a>

 Project: http://cfugue.sourceforge.net

 Copyright (C) 2009 <a href="http://cenacle.co.in">Cenacle Research India Private Limited</a>

*/

/////////////////////////////////////////////////////////////////
// Examples page
/////////////////////////////////////////////////////////////////

/*! \page pageExamples CFugue usage examples

\section secExampleContents Contents
    - \ref secCExamples
        - \ref ancCExamplePlay "Playing the Notes"
        - \ref ancCExampleSaveMIDI "Saving to MIDI file"
        - \ref ancCExamplePlayOptions "Configuring Play Options"
        - \ref ancCExampleParserEvents "Accessing the Parser events"
    - \ref secCplusCplusExamples
        - \ref ancPlay "Playing the Notes"
        - \ref ancSaveMIDI "Saving to MIDI file"
        - \ref ancAsynchPlay "Playing Asynchronously"
        - \ref ancParserEvents "Accessing the Parser events"
    - \ref secPInvokeUsage
        - \ref subsecPInvokePlay "Playing the Notes"
        - \ref subsecPInvokeSave "Saving to MIDI file"
        - \ref ancPInvokeExampleParserEvents "Accessing the Parser events"
    - \ref index "Back to main page"

\section secCExamples C Examples
\subsection ancCExamplePlay Playing the Notes
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {		        
        CFugue::PlayMusicString("C D E F G A B"); <span class="comment">// Play the Music Notes on the default MIDI output port</span>
    }
</pre>
Playing them Carnatic style:
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {      
        CFugue::PlayMusicString("K[MELA_DEFAULT] S R G M P D N"); <span class="comment">// Play the Music Notes on the default MIDI output port</span>
    }
</pre>

\subsection ancCExampleSaveMIDI Saving Music Notes to MIDI output file
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {           
        CFugue::SaveAsMidiFile("Cq Dw Ex", "MidiOutput.midi"); <span class="comment">// Save the Music Notes to Midi file directly, without playing</span>
    }
</pre>

\subsection ancCExamplePlayOptions Configuring the Play Options

The <tt>PlayMusicString()</tt> method plays the music notes on default MIDI output port with default MIDI timer resolution. However, if you would like to configure these options, use the <tt>PlayMusicStringWithOpts()</tt> method. It works similar to the <tt>PlayMusicString()</tt> method, but directs the output based on the input options.

<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {
        CFugue::PlayMusicStringWithOpts(_T("Cq Dw Ex"), <span class="comment">// CFugue MusicString to be played</span>
                                                MIDI_MAPPER, <span class="comment">// MIDI Output port</span>
                                                48); <span class="comment">// MIDI Timer Resolution in MilliSeconds</span>
     }
</pre>

\subsection ancCExampleParserEvents Accessing the Parser events

A MusicString need be parsed before it can be played or rendered as sheet output. While performing
the parsing operation, parser generates Trace events and Error events to let the caller
know of the status. Callers can handle these events and output the Trace and Error information
to a log file or appropriate UI.

Use the <tt>PlayMusicStringCB()</tt> method to add listeners to the parser trace and error events.
Once subscribed, the attached listeners will be invoked during the parse phase. A sample demonstrating the procedure is below.
<pre class="fragment">
    \#include "CFugueLib.h"
    void __stdcall OnParseTrace(void* pUserData, const TCHAR* szTraceMsg)
    {
        OutputDebugString(_T("\n"));
        OutputDebugString(szTraceMsg);
    }
    void __stdcall OnParseError(void* pUserData, long lErrCode, const TCHAR* szErrorMsg, const TCHAR* szToken);
    {
        OutputDebugString(_T("\nError --> "));
        OutputDebugString(szErrMsg);
        if(szToken != NULL)
        {
            OutputDebugString(_T("\t Token: "));	 
            OutputDebugString(szToken);
        }
    }
    void main()
    {
        CFugue::PlayMusicStringCB(_T("Cq Dw Ex"), <span class="comment">// CFugue MusicString to be played</span>
                                        OnParseTrace, <span class="comment">// Parse Trace Event Handler</span>
                                        OnParseError, <span class="comment">// Prase Error Event Handler</span>
                                        NULL);
     }
</pre>
Use <tt>PlayMusicStringWithOptsCB()</tt> method to subscribe handlers and also configure the Play options. Modified snippet looks like below.
<pre class="fragment">
    void main()
    {
        CFugue::PlayMusicStringWithOptsCB(_T("Cq Dw Ex"), <span class="comment">// CFugue MusicString to be played</span>
                                                MIDI_MAPPER, <span class="comment">// MIDI Output port</span>
                                                48, <span class="comment">// MIDI Timer Resolution in MilliSeconds</span>
                                                OnParseTrace, <span class="comment">// Parse Trace Event Handler</span>
                                                OnParseError, <span class="comment">// Prase Error Event Handler</span>
                                                NULL);
     }
</pre>

\section secCplusCplusExamples C++ Examples
\subsection ancPlay Playing the Notes
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {		
        <span class="comment">// Create the Player Object</span>
        CFugue::Player player; 
        <span class="comment">// Play the Music Notes on the default MIDI output port</span>
        player.Play("C D E F G A B"); 
    }
</pre>
Playing them Carnatic style:
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {
        <span class="comment">// Create the Player Object</span>
        CFugue::Player player; 
        <span class="comment">// Play the Music Notes on the default MIDI output port</span>
        player.Play("K[MELA_DEFAULT] S R G M P D N"); 
    }
</pre>

\subsection ancSaveMIDI Saving Music Notes to MIDI output file
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {    
        <span class="comment">// Create the Player Object </span>
        CFugue::Player player;
        <span class="comment">// Save the Music Notes to Midi file directly, without playing</span>
        player.SaveAsMidiFile("Cq Dw Ex", "MidiOutput.midi"); 
    }
</pre>
Sometimes you might want to play the Notes first on the MIDI output and then save the played 
content to an output file. You can achieve this using the Player::SaveToMidiFile() method.
You need to call this method <i>after</i> the call to Player::Play() or Player::PlayAsync().
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {   
        <span class="comment">// Create the Player Object </span> 
        CFugue::Player player; 
        <span class="comment">// Play few Music Notes on MIDI output port</span>
        player.Play("ci di K[MELA_22] Pa M G R"); 
        <span class="comment">// Now, save that played content to a MIDI output file</span>
        player.SaveToMidiFile("Output.mid"); 
    }
</pre>

\subsection ancAsynchPlay Asynchronous Play

The Player::Play() method plays the music notes synchronously, i.e. the method 
will not return till all the music notes are played completely. However, if you would like to play
the MusicString content asynchronously, you can use the Player::PlayAsync() method
instead. It works similar to the Play() method, except that it returns immediately.

<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {
        <span class="comment">// Create the Player object</span>
        CFugue::Player player; 
        <span class="comment">// Start Playing Asynchronously </span>
        if(player.PlayAsync("Cx Dh Ah.")) 
            while(player.IsPlaying()) <span class="comment">// Wait while the play is in progress</span>
                Sleep(1000);
        <span class="comment">// Match every PlayAsync with a StopPlay</span>
        player.StopPlay(); 
    }
</pre>
At any time after the call to PlayAsync, the Player::IsPlaying() method can be used
to know if the play is still in progress. Player::StopPlay() allows you
to stop the play if it is still in progress. Calling StopPlay() after every PlayAsync()
call is a good practice (no matter if play is in progress or not). A typical usage of these methods
is demonstrated in the above code snippet.

\subsection ancParserEvents Accessing the Parser events

A MusicString need be parsed before it can be played or rendered as sheet output. While performing
the parsing operation, parser generates Trace events and Error events to let the caller
know of the status. Callers can handle these events and output the Trace and Error information
to a log file or appropriate UI.

Use the Player::Parser() method to access the parser object associated with the player
and subscribe to its evTrace and evError events.
Once subscribed, the attached handlers will be invoked during the parse phase whenever any of the
Play or Save methods is invoked on the player. A sample demonstrating the procedure is below.
<pre class="fragment">
    \#include "CFugueLib.h"
    void OnParseTrace(const CFugue::CParser*, 
                CFugue::CParser::TraceEventHandlerArgs* pEvArgs)
    {
        OutputDebugString(_T("\n"));
        OutputDebugString(pEvArgs->szTraceMsg);
    }
    void OnParseError(const CFugue::CParser*,
                CFugue::CParser::ErrorEventHandlerArgs* pEvArgs)
    {
        OutputDebugString(_T("\nError --> "));
        OutputDebugString(pEvArgs->szErrMsg);
        if(pEvArgs->szToken)
        {
            OutputDebugString(_T("\t Token: "));	 
            OutputDebugString(pEvArgs->szToken);
        }
    }
    void main()
    {
        <span class="comment">// Create the Player Object</span>
        CFugue::Player player; 
        <span class="comment">// Access the Player's Parser and Subscribe to its Trace and Error Events</span>
        player.Parser().evTrace.Subscribe(&OnParseTrace); 
        player.Parser().evError.Subscribe(&OnParseError);
        <span class="comment">// Parse the Notes and Save the content to a MIDI file</span>
        player.SaveAsMidiFile(_T("Cq Dw Ex"), "MidiOutput.midi"); 
     }
</pre>

\section secPInvokeUsage P/Invoke usage
\subsection subsecPInvokePlay Playing the Notes
<pre class="fragment">
public static class CFugueLib
{
    [DllImport("CFugueDll.Dll")]
    public static extern bool PlayMusicString([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes);
}
public static void Main()
{    
    CFugue::PlayMusicString("Cq Dw Ex");<span class="comment">// Play the Music Notes on default MIDI Output Port</span>
}
</pre>
\subsection subsecPInvokeSave Saving Music Notes to MIDI output file
<pre class="fragment">
public static class CFugueLib
{
    [DllImport("CFugueDll.Dll")]
    public static extern bool SaveAsMidiFile([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, [MarshalAs(UnmanagedType.LPStr)] String szOutputFilePath);
}
public static void Main()
{    
    CFugue::SaveAsMidiFile("Cq Dw Ex", "MidiOutput.mid");<span class="comment">// Save the Music Notes to a MIDI file</span>
}
</pre>
\subsection ancPInvokeExampleParserEvents Accessing the Parser events

A MusicString need be parsed before it can be played or rendered as sheet output. While performing
the parsing operation, parser generates Trace events and Error events to let the caller
know of the status. Callers can handle these events and output the Trace and Error information
to a log file or appropriate UI.

Use the <tt>PlayMusicStringCB()</tt> method to add listeners to the parser trace and error events.
Once subscribed, the attached listeners will be invoked during the parse phase. A sample demonstrating the procedure is below.
<pre class="fragment">
    public static class CFugueLib
    {
        public delegate void ParserTraceDelegate(IntPtr userData, [MarshalAs(UnmanagedType.LPStr)] String szTraceMsg);
        public delegate void ParserErrorDelegate(IntPtr userData, int errCode,
                                                [MarshalAs(UnmanagedType.LPStr)] String szErrorMsg,
                                                [MarshalAs(UnmanagedType.LPStr)] String szToken);
        [DllImport("CFugueDll.Dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlayMusicStringCB([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, 
                                                [MarshalAs(UnmanagedType.FunctionPtr)] ParserTraceDelegate td,
                                                [MarshalAs(UnmanagedType.FunctionPtr)] ParserErrorDelegate ed,
                                                IntPtr userData);
    }
    public void OnParseTrace(IntPtr userData, [MarshalAs(UnmanagedType.LPStr)] String szTraceMsg)
    {
        <span class="comment">// Do something with szTraceMsg ... </span>
    }
    public void OnParseError(IntPtr userData, int errCode, [MarshalAs(UnmanagedType.LPStr)] String szErrorMsg, [MarshalAs(UnmanagedType.LPStr)] String szToken)
    {
        <span class="comment">// Do something with szErrorMsg and szToken ... </span>
    }
void main()
{
    CFugueLib.PlayMusicStringWithOptsCB("C D E", <span class="comment">// CFugue MusicString to be played</span>
                                        new CFugueLib.ParserTraceDelegate(OnParseTrace), <span class="comment">// Parse Trace Event Handler</span>
                                        new CFugueLib.ParserErrorDelegate(OnParseError), <span class="comment">// Prase Error Event Handler</span>
                                        IntPtr.Zero);
 }
</pre>
Use <tt>PlayMusicStringWithOptsCB()</tt> method to subscribe handlers and also configure the Play options. Modified snippet looks like below.
<pre class="fragment">
void main()
{
    CFugueLib.PlayMusicStringWithOptsCB("C D E", <span class="comment">// CFugue MusicString to be played</span>
                                            MIDI_MAPPER, <span class="comment">// MIDI Output port</span>
                                            48, <span class="comment">// MIDI Timer Resolution in MilliSeconds</span>
                                            new CFugueLib.ParserTraceDelegate(OnParseTrace), <span class="comment">// Parse Trace Event Handler</span>
                                            new CFugueLib.ParserErrorDelegate(OnParseError), <span class="comment">// Prase Error Event Handler</span>
                                            IntPtr.Zero);
 }
</pre>

*/


/////////////////////////////////////////////////////////////////
// CFugue MusicString page
/////////////////////////////////////////////////////////////////
/*! \page pageMusicString CFugue MusicString

\section contents Contents
    - \ref introduction
    - \ref secMusicString
        - \ref subsecNotes
            - \ref subsubRoot
            - \ref subsubOctave
            - \ref subsubChord
            - \ref subsubDuration
            - \ref subsubVelocity
            - \ref subsubConnectors
		- \ref subRestNotes
        - \ref subInstruments
        - \ref subKeySignatures
		- \ref subVoices
    - \ref secInteroperation
    - \ref index "Back to main page"

\section introduction Introduction
One of the capabilities of CFugue that makes it so elegant and easy to use is the concept of Music Note Strings 
(or simply <i>MusicString</i>s, as referred to by JFugue). MusicStrings provide the ability to specify 
Music Notes and MIDI instructions as a sequence of characters (and tokens) in human readable form,
as opposed to the usual complex binary data byte form. This makes it easy to learn and master. Below is an example of a MusicString supplied to the <i>Play()</i> method to play a Mid-C note and a C-Major chord. (Please refer \ref pageExamples "CFugue API examples" for more such examples.)
<pre class="fragment">
    \#include "CFugueLib.h"
    void main()
    {		
        CFugue::Player player; <span class="comment">// Create the Player Object</span>        
        player.Play("C CMaj"); <span class="comment">// Play a Mid-C followed by C-Major Chord </span>
    }
</pre>
Few more examples of MusicStrings:
<pre class="fragment">
    CFugue::Player player; <span class="comment">// Create the Player Object</span>    
    player.Play("I[FLUTE] FMaj"); <span class="comment">// Play the F-Major chord with Flute </span>
    player.Play("C D E F G A B"); <span class="comment">// Play all the mid octave notes </span>
    player.Play("K[MELA] S R G M P D N"); <span class="comment">// Play all the mid octave (madhya sthayi) notes in Carnatic Music </span>
    player.Play("C+D+E+F+G+A+B"); <span class="comment">// Play all the mid octave notes in parallel</span>    
    player.Play("CI CI CI GI FI GI D#I. FI. G#I GI. RI"); <span class="comment">// Notes with durations</span>
    player.Play("K[HAMSADHWANI]T8S2 ( S' GA' R'  S' R' S' N   PA N S'  RI' RI' )"); <span class="comment">// A piece of Vatapi Ganapathim in double speed </span>
</pre>
In the following we discuss at length the details of what constitues a MusicString and how it can be used for creating music with CFugue.

\section secMusicString Components of MusicString
In CFugue, a valid MusicString is formed out of a series of music tokens. Tokens are nothing but sequences of alpha numeric charaters. Seperated with whitespace, each of them represents either a music note related data or a CFugue related command. The first character of a token usually determines if the token should be considered as a note token or a command token. 

Typical examples of note tokens are notes, chords, ties and so on, while MIDI instructions, such as instrument change, voice command etc. are few examples of CFugue related commands.

When CFugue parses a MusicString, it uses the first character of the tokens to identify the notes and commands. If no decision can be made about a token, CFugue will simply ignore the token and moves on (observe that this is unlike any typical parser behavior, where an unrecognized token will halt the parsing).

Below are the first characters and their associated token classifications for CFugue MusicStrings:
\htmlonly
<table align="center">    
    <tr><td class="indexkey">Character</td> <td class="indexkey">API Constant</td>  <td class="indexkey">Significance</td>  </tr>
    <tr><td class="indexkey">V </td> <td class="indexvalue">TOKEN_START_VOICE  </td> <td class="indexvalue">Specifies a Voice token.</td></tr> 
    <tr><td class="indexkey">T </td> <td class="indexvalue">TOKEN_START_TEMPO  </td> <td class="indexvalue">Specifies a Tempo token.</td></tr>
    <tr><td class="indexkey">I </td> <td class="indexvalue">TOKEN_START_INSTRUMENT  </td> <td class="indexvalue">Specifies a Instrument token.</td></tr> 
    <tr><td class="indexkey">L </td> <td class="indexvalue">TOKEN_START_LAYER  </td> <td class="indexvalue">Specifies a Layer token.</td></tr> 
    <tr><td class="indexkey">K </td> <td class="indexvalue">TOKEN_START_KEYSIGNATURE  </td> <td class="indexvalue">Specifies a KeySignature token.</td></tr> 
    <tr><td class="indexkey">X </td> <td class="indexvalue">TOKEN_START_CONTROLLER  </td> <td class="indexvalue">Specifies a Controller token.</td></tr> 
    <tr><td class="indexkey">@ </td> <td class="indexvalue">TOKEN_START_TIME  </td> <td class="indexvalue">Specifies a Time token.</td></tr> 
    <tr><td class="indexkey">* </td> <td class="indexvalue">TOKEN_START_KEYPRESSURE  </td> <td class="indexvalue">Specifies a Keypressure token.</td></tr>  
    <tr><td class="indexkey">+ </td> <td class="indexvalue">TOKEN_START_CHANNELPRESSURE  </td> <td class="indexvalue">Specifies a Channel pressure token.</td></tr> 
    <tr><td class="indexkey">& </td> <td class="indexvalue">TOKEN_START_PITCHBEND  </td> <td class="indexvalue">Specifies a Pitchbend token. </td></tr> 
    <tr><td class="indexkey">| </td> <td class="indexvalue">TOKEN_START_MEASURE  </td> <td class="indexvalue">Specifies a Measure token.</td></tr> 
    <tr><td class="indexkey">$ </td> <td class="indexvalue">TOKEN_START_DICTIONARY  </td> <td class="indexvalue">Specifies a Dictionary token.</td></tr><!--     <tr><td>[ </td> <td>TOKEN_START_NOTE  </td> <td>Specifies a numeric Note token.</td></tr>  -->
</table>
\endhtmlonly

Each of these will be explained in detail in later parts of this discussion. For now, when a token starts with a character that is different from all of these shown above, CFugue tries to read that token as a note token. If it succeeds in that, it will generate note events for further processing. And if it fails, it will ignore the token and moves on.

CFugue MusicStrings are case-insensitive. That is, the characters can be used in any case, either upper or lower, while creating the MusicStrings. CFugue will internally convert all the supplied MusicStrings to upper case before processing them. Use this fact to your advantage by mixing the case for your MusicString tokens and making them more readable and easy to understand. Examples of few valid MusicStrings that showcase this advantage are:
<pre  class="fragment">
    player.Play("C");
    player.Play("FMaj");
    player.Play("Cq+Eh+Gq");
    player.Play("C6q+E4h+G8q D5Maj7w");
    player.Play("T[120] V0 I[Flute] Cs B4s A4i B4i C");
    player.Play("I[XyloPhone] K[Mela_29] S R G M P D N S'");
</pre>

\subsection subsecNotes Specifying Notes
Specifying music notes in CFugue MusicStrings is simple and straight forward. Just indicate the root of the note followed by its optional attributes, such as octave, duration, any additional connectors etc. and you are done. Thus the structure of a valid note token is:
\htmlonly
<table align="center">
<tr> <td class="indexvalue">Root[Octave][Chord][Duration][Velocity][Connectors]</td> </tr>
</table>
\endhtmlonly

Except for the Root, all others in the above are optional. And when they are present, they have to be in the order specified above. Changing the order will cause incorrect results.

\subsubsection subsubRoot Root
The root is specified either by a note name or by its MIDI numeric value. Alphabets such as C, D, E .. indicate the names for the Western style and S, R, G .. indicate the names for the Carnatic style. The complete list is as shown below for the two systems:
\htmlonly
<table align="center">
 <tr> <td class="indexkey">Halfstep index</td>	<td class="indexkey">Western Note</td>	<td class="indexkey">Carnatic Swara</td> </tr>
 <tr> <td class="indexvalue">0</td>				<td class="indexvalue">C</td>				<td class="indexvalue">S</td>				</tr>
 <tr> <td class="indexvalue">1</td>				<td class="indexvalue">C#</td>				<td class="indexvalue">R1</td>				</tr>
 <tr> <td class="indexvalue">2</td>				<td class="indexvalue">D</td>				<td class="indexvalue">R2</td>				</tr>
 <tr> <td class="indexvalue">3</td>				<td class="indexvalue">E<i>b</i></td>				<td class="indexvalue">G2</td>				</tr>
 <tr> <td class="indexvalue">4</td>				<td class="indexvalue">E</td>				<td class="indexvalue">G3</td>				</tr>
 <tr> <td class="indexvalue">5</td>				<td class="indexvalue">F</td>				<td class="indexvalue">M1</td>				</tr>
 <tr> <td class="indexvalue">6</td>				<td class="indexvalue">F#</td>				<td class="indexvalue">M2</td>				</tr>
 <tr> <td class="indexvalue">7</td>				<td class="indexvalue">G</td>				<td class="indexvalue">P</td>				</tr>
 <tr> <td class="indexvalue">8</td>				<td class="indexvalue">G#</td>				<td class="indexvalue">D1</td>				</tr>
 <tr> <td class="indexvalue">9</td>				<td class="indexvalue">A</td>				<td class="indexvalue">D2</td>				</tr>
 <tr> <td class="indexvalue">10</td>			<td class="indexvalue">B<i>b</i></td>				<td class="indexvalue">N2</td>				</tr>
 <tr> <td class="indexvalue">11</td>			<td class="indexvalue">B</td>				<td class="indexvalue">N3</td>				</tr>
</table>
\endhtmlonly

In addition, CFugue allows <i>#</i> and <i>b</i> modifiers for the Western style of notes to access one halfstep up or down the given note. Thus, one can use <i>D#</i> to indicate the note <i>Eb</i>, <i>Fb</i> to indicate the note <i>E</i> and so on. Repeating a modifier more than once is also supported. For example, <i>C##</i> signifies <i>D</i> and <i>Bbb</i> signifies <i>A</i>. However, it is advised to refrain from mixing <i>#</i> and <i>b</i> in the same token. For example, do not try something like <i>C\#b</i> to get back to <i>C</i>. Though CFugue understands such notation correctly, it is not always guaranteed to work.

Similarily, for Carnatic music, CFugue allows <i>R3</i>, <i>G1</i>, <i>D3</i> and <i>N1</i> Swaras, which essentially are the same as <i>G2</i>, <i>R2</i>, <i>N2</i> and <i>D2</i> from the above, respectively.

\note Before we can pass any Carnatic music notes to CFugue for parsing, it need to be informed that we are indeed passing Carnatic style of notes and not Western style of notes. That is, we need to tell CFugue to use Carnatic note parser and not the default Western note parser for the notes we are about to supply. We do this by using the \ref subKeySignatures "Key Signature" directive as shown below:
<pre class="fragment">
    <span class="comment">// Switch to Carnatic mode and play some notes</span>
    player.Play("K[MELA] S R G M P D N"); 
</pre> The K[MELA] directive informs the parser to switch to Carnatic mode of parsing and to interpret the subsequent notes in that mode. For further discussion on this, please refer \ref secInteroperation.

Music notes can also be specified using their MIDI numeric value directly. Usually, when a note is specified using its name, such as C, D, E .. or S, R, G.., it is treated as a relative note. Its absolute position will be internally re-computed based on the Octave specifiers and the Scale/Raga settings. On the otherhand, when a note is specified with its MIDI numeric value directly, it is treated as an absolute note and will not be affected by the current Scale or Octave settings. An example of such numeric note is:
<pre class="fragment">
    <span class="comment">// Play a Mid-C </span>
    player.Play("[60]");
</pre>
Observe that we need to enclose the numeric value in square brackets []. Failing to do so might result in CFugue ignoring the token. Below is the complete listing of MIDI numeric values for all the notes.

\htmlonly
<table align="center">
<tr><td align="center" class="indexkey">Octave</td><td align="center" class="indexkey">C<br>S</td> <td align="center" class="indexkey">C#/D<i>b</i><br>R1</td> <td align="center" class="indexkey">D<br>R2/G1</td> <td align="center" class="indexkey">D#/E<i>b</i><br>R3/G2</td><td align="center" class="indexkey">E<br>G3</td><td align="center" class="indexkey">F<br>M1</td><td align="center" class="indexkey">F#/G<i>b</i><br>M2</td><td align="center" class="indexkey">G<br>P</td><td align="center" class="indexkey">G#/A<i>b</i><br>D1</td><td align="center" class="indexkey">A<br>D2/N1</td><td align="center" class="indexkey">A#/B<i>b</i><br>N2</td><td align="center" class="indexkey">B<br>N3</td>  </tr>
<tr> <td align="center" class="indexkey">0</td>
        <td align="center" class="indexvalue">0</td>
        <td align="center" class="indexvalue">1</td>
        <td align="center" class="indexvalue">2</td>
        <td align="center" class="indexvalue">3</td>
        <td align="center" class="indexvalue">4</td>
        <td align="center" class="indexvalue">5</td>
        <td align="center" class="indexvalue">6</td>
        <td align="center" class="indexvalue">7</td>
        <td align="center" class="indexvalue">8</td>
        <td align="center" class="indexvalue">9</td>
        <td align="center" class="indexvalue">10</td>
        <td align="center" class="indexvalue">11</td>
</tr>
<tr> <td align="center" class="indexkey">1</td>
        <td align="center" class="indexvalue">12</td>
        <td align="center" class="indexvalue">13</td>
        <td align="center" class="indexvalue">14</td>
        <td align="center" class="indexvalue">15</td>
        <td align="center" class="indexvalue">16</td>
        <td align="center" class="indexvalue">17</td>
        <td align="center" class="indexvalue">18</td>
        <td align="center" class="indexvalue">19</td>
        <td align="center" class="indexvalue">20</td>
        <td align="center" class="indexvalue">21</td>
        <td align="center" class="indexvalue">22</td>
        <td align="center" class="indexvalue">23</td>
</tr>
<tr> <td align="center" class="indexkey">2</td>
        <td align="center" class="indexvalue">24</td>
        <td align="center" class="indexvalue">25</td>
        <td align="center" class="indexvalue">26</td>
        <td align="center" class="indexvalue">27</td>
        <td align="center" class="indexvalue">28</td>
        <td align="center" class="indexvalue">29</td>
        <td align="center" class="indexvalue">30</td>
        <td align="center" class="indexvalue">31</td>
        <td align="center" class="indexvalue">32</td>
        <td align="center" class="indexvalue">33</td>
        <td align="center" class="indexvalue">34</td>
        <td align="center" class="indexvalue">35</td>
</tr>
<tr> <td align="center" class="indexkey">3</td>
        <td align="center" class="indexvalue">36</td>
        <td align="center" class="indexvalue">37</td>
        <td align="center" class="indexvalue">38</td>
        <td align="center" class="indexvalue">39</td>
        <td align="center" class="indexvalue">40</td>
        <td align="center" class="indexvalue">41</td>
        <td align="center" class="indexvalue">42</td>
        <td align="center" class="indexvalue">43</td>
        <td align="center" class="indexvalue">44</td>
        <td align="center" class="indexvalue">45</td>
        <td align="center" class="indexvalue">46</td>
        <td align="center" class="indexvalue">47</td>
</tr>
<tr> <td align="center" class="indexkey">4</td>
        <td align="center" class="indexvalue">48</td>
        <td align="center" class="indexvalue">49</td>
        <td align="center" class="indexvalue">50</td>
        <td align="center" class="indexvalue">51</td>
        <td align="center" class="indexvalue">52</td>
        <td align="center" class="indexvalue">53</td>
        <td align="center" class="indexvalue">54</td>
        <td align="center" class="indexvalue">55</td>
        <td align="center" class="indexvalue">56</td>
        <td align="center" class="indexvalue">57</td>
        <td align="center" class="indexvalue">58</td>
        <td align="center" class="indexvalue">59</td>
</tr>
<tr> <td align="center" class="indexkey">5</td>
        <td align="center" class="indexvalue">60</td>
        <td align="center" class="indexvalue">61</td>
        <td align="center" class="indexvalue">62</td>
        <td align="center" class="indexvalue">63</td>
        <td align="center" class="indexvalue">64</td>
        <td align="center" class="indexvalue">65</td>
        <td align="center" class="indexvalue">66</td>
        <td align="center" class="indexvalue">67</td>
        <td align="center" class="indexvalue">68</td>
        <td align="center" class="indexvalue">69</td>
        <td align="center" class="indexvalue">70</td>
        <td align="center" class="indexvalue">71</td>
</tr>
<tr> <td align="center" class="indexkey">6</td>
        <td align="center" class="indexvalue">72</td>
        <td align="center" class="indexvalue">73</td>
        <td align="center" class="indexvalue">74</td>
        <td align="center" class="indexvalue">75</td>
        <td align="center" class="indexvalue">76</td>
        <td align="center" class="indexvalue">77</td>
        <td align="center" class="indexvalue">78</td>
        <td align="center" class="indexvalue">79</td>
        <td align="center" class="indexvalue">80</td>
        <td align="center" class="indexvalue">81</td>
        <td align="center" class="indexvalue">82</td>
        <td align="center" class="indexvalue">83</td>
</tr>
<tr> <td align="center" class="indexkey">7</td>
        <td align="center" class="indexvalue">84</td>
        <td align="center" class="indexvalue">85</td>
        <td align="center" class="indexvalue">86</td>
        <td align="center" class="indexvalue">87</td>
        <td align="center" class="indexvalue">88</td>
        <td align="center" class="indexvalue">89</td>
        <td align="center" class="indexvalue">90</td>
        <td align="center" class="indexvalue">91</td>
        <td align="center" class="indexvalue">92</td>
        <td align="center" class="indexvalue">93</td>
        <td align="center" class="indexvalue">94</td>
        <td align="center" class="indexvalue">95</td>
</tr>
<tr> <td align="center" class="indexkey">8</td>
        <td align="center" class="indexvalue">96</td>
        <td align="center" class="indexvalue">97</td>
        <td align="center" class="indexvalue">98</td>
        <td align="center" class="indexvalue">99</td>
        <td align="center" class="indexvalue">100</td>
        <td align="center" class="indexvalue">101</td>
        <td align="center" class="indexvalue">102</td>
        <td align="center" class="indexvalue">103</td>
        <td align="center" class="indexvalue">104</td>
        <td align="center" class="indexvalue">105</td>
        <td align="center" class="indexvalue">106</td>
        <td align="center" class="indexvalue">107</td>
</tr>
<tr> <td align="center" class="indexkey">9</td>
        <td align="center" class="indexvalue">108</td>
        <td align="center" class="indexvalue">109</td>
        <td align="center" class="indexvalue">110</td>
        <td align="center" class="indexvalue">111</td>
        <td align="center" class="indexvalue">112</td>
        <td align="center" class="indexvalue">113</td>
        <td align="center" class="indexvalue">114</td>
        <td align="center" class="indexvalue">115</td>
        <td align="center" class="indexvalue">116</td>
        <td align="center" class="indexvalue">117</td>
        <td align="center" class="indexvalue">118</td>
        <td align="center" class="indexvalue">119</td>
</tr>
<tr> <td align="center" class="indexkey">10</td>
        <td align="center" class="indexvalue">120</td>
        <td align="center" class="indexvalue">121</td>
        <td align="center" class="indexvalue">122</td>
        <td align="center" class="indexvalue">123</td>
        <td align="center" class="indexvalue">124</td>
        <td align="center" class="indexvalue">125</td>
        <td align="center" class="indexvalue">126</td>
        <td align="center" class="indexvalue">127</td>
</tr>
</table>\endhtmlonly

Be informed that when you specify a note using its MIDI numeric value, you cannot use the Octave field in the token anymore. Octave fields are only applicable for notes specified using their names.

\subsubsection subsubOctave Octave
For Western music, CFugue supports octaves in the range [0, 10]. You can specify an octave for a non-numeric note by appending the octave number to it, such as <i>C2</i> to signify C note in 2nd octave or <i>Bb8</i> to signify B-flat note in 8th octave. Observe that CFugue starts the octave numbering at 0 (instead of -1 used by some other representations). Middle-C in CFugue is C5, which is MIDI note value 60.

Octaves are optional. If you do not specify any octave value for a note, a default value of Octave 5 will be used for that note (unless it is a chord note, in which case a default value of 3 will be used). 

For Carnatic music, there is no concept of absolute octaves. Instead it supports the concept of <i>Sthayis</i>, which is a relative value that changes from instrument to instrument and from person to person. Typically each singer will be having his/her own base <i>Sthayi</i>, which is usually referred to as medium sthayi, and varies above and below it, leading to upper sthayi notes and lower sthayi notes.

In CFugue's Carnatic music notation, a plain note indicates a medium sthayi note. One can append the character <i>'</i> to get an upper sthayi note of it, and the character <i>.</i> to get a lower sthayi note of it. For example, <i>S'</i> is the upper sthayi note of the medium note <i>S</i>, with <i>S.</i> being its lower sthayi correspondent.

Multiple <i>'</i> and <i>.</i> can be appended to a note to get more upper and lower sthayis, such as <i>P'''</i> or <i>D...</i>. However, do not mix <i>'</i> and <i>.</i> in a single token. For example, usage such as <i>S'.</i> is not valid. <i>S'.</i> is not same as <i>S</i> in CFugue.

Below are few valid examples of notes with Octaves and Sthayis. 
<pre class="fragment">
    player.Play(" C4 D4 E4 F4 G4 A4 B4    C D E F G A B    C6 C#6 E6 F#6 G6 Ab6 B6 "); 
    player.Play(" S. R. G. M. P. D. N.    S R G M P D N    S' R1' G' M2' P' D1' N' "); 
</pre>
For compatibility between Western and Carnatic music, CFugue defines the default medium sthayi as Octave 5. Thus, both of the above two lines produce the same results. For further details on how to interoperate between Western and Carnatic style of music, refer \ref secInteroperation.

\subsubsection subsubChord Chord
CFugue supports the below specified chords:
\htmlonly
<table align="center">
        <tr><td align></td><td class="indexkey">Chord Name</td><td class="indexkey">Haflstep intervals</td></tr>

        <tr><td align="right">Major</td><td class="indexvalue">MAJ</td><td class="indexvalue">         {0,4,7}</td></tr>
        <tr><td align="right">Minor</td><td class="indexvalue">MIN</td><td class="indexvalue">         {0,3,7}</td></tr>
        <tr><td align="right">Augmented</td><td class="indexvalue">AUG</td><td class="indexvalue">         {0,4,8}</td></tr>
        <tr><td align="right">Diminished</td><td class="indexvalue">DIM</td><td class="indexvalue">         {0,3,6}</td></tr>
        <tr><td align="right">7th dominant</td><td class="indexvalue">DOM7</td><td class="indexvalue">        {0,4,7,1}</td></tr>
        <tr><td align="right">Major 7th</td><td class="indexvalue">MAJ7</td><td class="indexvalue">        {0,4,7,11}</td></tr>
        <tr><td align="right">Minor 7th</td><td class="indexvalue">MIN7</td><td class="indexvalue">        {0,3,7,1}</td></tr>
        <tr><td align="right">Suspended 4th</td><td class="indexvalue">SUS4</td><td class="indexvalue">        {0,5,7}</td></tr>
        <tr><td align="right">Suspended 2nd</td><td class="indexvalue">SUS2</td><td class="indexvalue">        {0,2,7}</td></tr>
        <tr><td align="right">Major 6th</td><td class="indexvalue">MAJ6</td><td class="indexvalue">        {0,4,7,9}</td></tr>
        <tr><td align="right">Minor 6th</td><td class="indexvalue">MIN6</td><td class="indexvalue">        {0,3,7,9}</td></tr>
        <tr><td align="right">9th dominant</td><td class="indexvalue">DOM9</td><td class="indexvalue">        {0,4,7,10,14}</td></tr>
        <tr><td align="right">9th Major</td><td class="indexvalue">MAJ9</td><td class="indexvalue">        {0,4,7,11,14}</td></tr>
        <tr><td align="right">9th Minor</td><td class="indexvalue">MIN9</td><td class="indexvalue">        {0,3,7,10,14}</td></tr>
        <tr><td align="right">Diminished 7th</td><td class="indexvalue">DIM7</td><td class="indexvalue">        {0,3,6,9}</td></tr>
        <tr><td align="right">Add 9</td><td class="indexvalue">ADD9</td><td class="indexvalue">        {0,4,7,14}</td></tr>
        <tr><td align="right">Minor 11th</td><td class="indexvalue">MIN11</td><td class="indexvalue">       {0,7,10,14,15,17}</td></tr>
        <tr><td align="right">11th dominant</td><td class="indexvalue">DOM11</td><td class="indexvalue">       {0,7,10,14,17}</td></tr>
        <tr><td align="right">13th dominant</td><td class="indexvalue">DOM13</td><td class="indexvalue">       {0,7,10,14,16,21}</td></tr>
        <tr><td align="right">Major 13th</td><td class="indexvalue">MAJ13</td><td class="indexvalue">       {0,7,11,14,16,21}</td></tr>
        <tr><td align="right">Minor 13th</td><td class="indexvalue">MIN13</td><td class="indexvalue">       {0,7,10,14,15,21}</td></tr>
        <tr><td align="right">7-5 dominant</td><td class="indexvalue">DOM7_5</td><td class="indexvalue">      {0,4,6,1}</td></tr>
        <tr><td align="right">7-5 dominant</td><td class="indexvalue">DOM7<5</td><td class="indexvalue">      {0,4,6,1}</td></tr>
        <tr><td align="right">7+5 dominant</td><td class="indexvalue">DOM75</td><td class="indexvalue">       {0,4,8,1}</td></tr>
        <tr><td align="right">7+5 dominant</td><td class="indexvalue">DOM7>5</td><td class="indexvalue">      {0,4,8,1}</td></tr>
        <tr><td align="right">Major 7-5</td><td class="indexvalue">MAJ7_5</td><td class="indexvalue">      {0,4,6,11}</td></tr>
        <tr><td align="right">Major 7-5</td><td class="indexvalue">MAJ7<5</td><td class="indexvalue">      {0,4,6,11}</td></tr>
        <tr><td align="right">Major 7+5</td><td class="indexvalue">MAJ75</td><td class="indexvalue">       {0,4,8,11}</td></tr>
        <tr><td align="right">Major 7+5</td><td class="indexvalue">MAJ7>5</td><td class="indexvalue">      {0,4,8,11}</td></tr>
        <tr><td align="right">Minor Major 7</td><td class="indexvalue">MINMAJ7</td><td class="indexvalue">     {0,3,7,11}</td></tr>
        <tr><td align="right">7-5-9 dominant</td><td class="indexvalue">DOM7_5_9</td><td class="indexvalue">    {0,4,6,10,13}</td></tr>
        <tr><td align="right">7-5-9 dominant</td><td class="indexvalue">DOM7<5<9</td><td class="indexvalue">    {0,4,6,10,13}</td></tr>
        <tr><td align="right">7-5+9 dominant</td><td class="indexvalue">DOM7_59</td><td class="indexvalue">     {0,4,6,10,15}</td></tr>
        <tr><td align="right">7-5+9 dominant</td><td class="indexvalue">DOM7<5>9</td><td class="indexvalue">    {0,4,6,10,15}</td></tr>
        <tr><td align="right">7+5-9 dominant</td><td class="indexvalue">DOM75_9</td><td class="indexvalue">     {0,4,8,10,13}</td></tr>
        <tr><td align="right">7+5-9 dominant</td><td class="indexvalue">DOM7>5<9</td><td class="indexvalue">    {0,4,8,10,13}</td></tr>
        <tr><td align="right">7+5+9 dominant</td><td class="indexvalue">DOM759</td><td class="indexvalue">      {0,4,8,10,15}</td></tr>
        <tr><td align="right">7+5+9 dominant</td><td class="indexvalue">DOM7>5>9</td><td class="indexvalue">    {0,4,8,10,15}</td></tr>
</table>
\endhtmlonly

In the above haflstep intervals, zero indicates the root note. Other values are relative haflstep values from it, which will be adjusted based on the root note.

To specify a chord in CFugue, specify the root and append it with the chord name from the above table. For example, <tt>CMAJ</tt> signifies the C-major chord, which, based on the above halfstep intervals {0, 4, 7}, will automatically expand to <tt>C+E+G</tt> notes (C=0, E=4, G=7).

The default octave for the chord notes is 3. To specify an explicit octave value for a chord, append the chord root with the octave number. For example, a B-Flat, 7th Octave, major chord will be <tt>Bb7Maj</tt>. 

\anchor ancNoteChordInversions <b>Chord Inversions</b>

A chord inversion indicates a different way of playing a chord. The degree of inversion indicates how many notes in the chord that need be shifted in the octave. A first degree inversion indicates that one note (the root note) should be moved up an octave, while the second degree inversion indicates that two notes (the root note and the second note) should be moved to one octave heigherr. When the note is moved up an octave heigher, the remaining notes in the chord become the new bass notes. Typically a chord with <i>n</i> number of notes can have atmost <i>n</i> degree inversions.

The character <tt>^</tt> indicates an inversion in CFugue. There are two ways to use it. The first is to specify <tt>^</tt> as many times as the degree of inversion after the chord name, and the second is to specify <tt>^</tt> after the chord name followed by the new bass note. For example, <tt>Cmaj^^</tt> and <tt>Cmaj^G</tt> are two ways of indicating the same: a second degree inversion for C-Major chord (that makes the <tt>G</tt> as the new bass note).


\subsubsection subsubDuration Duration
Duration in Western music indicates how long a note should be played. CFugue supports below durations for the Western music notes.

\htmlonly
<table align="center">
<tr><td class="indexkey">Character</td><td class="indexkey">Duration</td> </tr>
<tr><td class="indexkey">W</td><td class="indexvalue">Whole Note </td></tr> 
<tr><td class="indexkey">H</td><td class="indexvalue">Half Note </td></tr>
<tr><td class="indexkey">Q</td><td class="indexvalue">Quarter Note </td></tr>
<tr><td class="indexkey">I</td><td class="indexvalue">1/8th Note </td></tr>
<tr><td class="indexkey">S</td><td class="indexvalue">1/16th Note </td></tr>
<tr><td class="indexkey">T</td><td class="indexvalue">1/32th Note </td></tr>
<tr><td class="indexkey">X</td><td class="indexvalue">1/64th Note </td></tr>
<tr><td class="indexkey">O</td><td class="indexvalue">1/128th Note </td></tr>
</table>
\endhtmlonly

These characters can be combined to form any aribtrary lengths of durations as required. For example, the token <tt>Bb6www</tt> indicates a B-Flat note in 6th octave for a duration of three whole notes, and the token <tt>F\#MajH</tt> indicates an F-sharp Major chord for a duration of half-note.

CFugue also supports dotted durations. For example, a dotted quarter note would be a <tt>q</tt> followed by a dot (<tt>q.</tt>). Dotted durations indicate an addendum of extra half to the original duration. Thus a dotted quarter note = quarter note + 1/8th note;

Further, CFugue supports numeric durations also. Use the slash character followed by the decimal portion of whole note to indicate a numeric duration. For example, the token <tt>F#8/0.25</tt> signifies an F-sharp note in eight octave for a duration of quarter note (<tt>0.25</tt> is the quarter note portion of whole note). Similarily <tt>0.5</tt> signifies half-note and <tt>1.0</tt> signifies a while note. Values greater than <tt>1.0</tt> indicate a note that spans multiple measures. For example, the <tt>Bb6www</tt> token discussed above can be represeted numerically as <tt>Bb6/3.0</tt>.

Below are few valid examples of duration specification:
<pre class="fragment">
    player.Play("Gh"); <span class="comment">// G note in default octave for a half-note duration </span>
    player.Play("[65]wq"); <span class="comment">// Middle-F (F5) for a whote+quarter note duration </span>
    player.Play("C3i."); <span class="comment">// C3 dotted-eighth (1/8 + 1/16) duration </span>
</pre>
For Western music, if no duration is specified, a default duration of quarter note is assumed.

In Carnatic music, durations for the notes are determined automatically based on the <i>Talam</i>. A <i>Talam</i> specifies the duration for a group of notes instead of an individual note. CFugue supports below Talams.

\subsubsection subsubVelocity Velocity
CFugue supports attack and decay velocities for notes. These values indicate how a note should be played in terms of its "take off" and "landing" volumes. For example, a note with long attack will sound like being built over a period of time, while a note with long decay sounds like a bell or a string that continues to resonate long after it has been struck. 

In CFugue, attack and decay velocities can be specified using a pair of <tt>V</tt> followed by a numeric value in the range [0, 127] duo. The first <tt>V</tt> number pair stands for attack velocity, followed by the optional second <tt>V</tt> number pair that stand for decay velocity. Both are optional and are independent of each other. Low numeric values indicate quicker attack (or decay), while high values signify longer attack (or decay). And the default value, if unspecified, is 64. Below are few examples of valid usage:
<pre class="fragment">
    player.Play("C6V2V124"); <span class="comment">// C6 note with sharp attack 2 and longer decay 124. The first V stands for attack, second V for decay. </span>
    player.Play("Bb3qhVV3"); <span class="comment">// B-Flat 3rd octave quarter+half duration with default Attack and sharp decay 3. First V value left unspecified, so attack takes default value </span>
    player.Play("Bb3qhV1"); <span class="comment">// B-Flat 3rd octave quarter+half duration with sharp Attack 1 and default decay. Second V left unspecified, so decay takes default value</span>
    player.Play("Bb3qh"); <span class="comment">// B-Flat 3rd octave quarter+half duration with default Attack and default decay - No V at all, so both attack and decay take default values</span>
</pre>

\subsubsection subsubConnectors Connectors

\subsection subRestNotes Specifying Rests
Apart from the regular notes, CFugue also supports <i>Rest</i> notes. Rest notes are nothing but periods of silence. Their use is similar to that of sounded notes, except that Rest notes do not support anything other than a Duration. That is, Rest notes do not have Octaves or Velocities or Connectors. And the durations specifications for Rest notes are same as that of the usual notes. Refer to \ref subsubDuration for details on durations.

Use the symbol <i>R</i> to specify a Rest note in Western music. The usual duration specificiers can be appended as usual. Below are few examples:
<pre class="fragment">
    player.Play("R"); <span class="comment">// Rest note for the detault duration</span>
    player.Play("Rw"); <span class="comment">// Rest note for a whole duration</span>
</pre>
Obviously, you would not be listening any sound being output when you run the above lines of code. Rest notes are designed to be used in conjunct with other regular notes. Usually it is rare that you would want to program MusicStrings made of only rest notes. Place them amidst of regular music notes to actually listen them, such as shown in the below example.
<pre class="fragment">
    player.Play("Cw Rh Dw"); <span class="comment">// Can you "listen" the Rest note ?</span>
</pre>
For Carnatic music, the symbols <tt>,</tt> and <tt>;</tt> signify the Rest notes. The durations are based on the currently selected Talam. The <tt>,</tt> specifies a Rest note for one duration, while the <tt>;</tt> specifies a Rest note for two durations.

These <tt>,</tt> and <tt>;</tt> can also be used in Western music notation, in which case they both act as the default Rest note with no explicit duration. For more details on mixing the Western and Carnatic music notations, please refer \ref secInteroperation.

\subsection subInstruments Specifying Instruments
Instrument tokens in MusicStrings indicates which instrument should be used to play the subsequent notes in the strings. In CFugue, instrument tokens are identified with character <b>I</b> at their beginning. Below are few examples of valid instrument tokens:
<pre class="fragment">
    player.Play("I[PIANO]"); 
    player.Play("I10"); 
    player.Play("I[RAIN]"); 
</pre>
Ofcourse, the above does not produce any music output since there are no notes specified in the input. Usually one supplies a series of note tokens after an instrument token for generting music using the specified instrument. Examples are as shown below:
<pre class="fragment">
    player.Play("I[Piano] C D E F G A B"); <span class="comment">// Play an Octave using Piano instrument </span>
    player.Play("I0 C D E I40 F G A B"); <span class="comment">// I0 is Piano and I40 is Violin</span>
    player.Play("I[Flute] K[Kalyani] S R G M P D N"); <span class="comment">// Play an Octave in Kalyani Ragam with Flute</span> 
</pre>

Complete list of intruments supported by CFugue is as shown below. In the MusicStrings these instruments can be specified either by their name or their MIDI instrument number indicated.
\htmlonly
<table align="center">
<tr>
<td valign=top><table>
        <tr><td class="indexvalue">PIANO</td>                   <td class="indexvalue">0</td></tr>
        <tr><td class="indexvalue">ACOUSTIC_GRAND</td>          <td class="indexvalue">0</td></tr>
        <tr><td class="indexvalue">BRIGHT_ACOUSTIC</td>         <td class="indexvalue">1</td></tr>
        <tr><td class="indexvalue">ELECTRIC_GRAND</td>          <td class="indexvalue">2</td></tr>
        <tr><td class="indexvalue">HONKEY_TONK</td>             <td class="indexvalue">3</td></tr>
        <tr><td class="indexvalue">ELECTRIC_PIANO_1</td>        <td class="indexvalue">4</td></tr>
        <tr><td class="indexvalue">ELECTRIC_PIANO_2</td>        <td class="indexvalue">5</td></tr>
        <tr><td class="indexvalue">HARPISCHORD</td>             <td class="indexvalue">6</td></tr>
        <tr><td class="indexvalue">CLAVINET</td>                <td class="indexvalue">7</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">CELESTA</td>                 <td class="indexvalue">8</td></tr>
        <tr><td class="indexvalue">GLOCKENSPIEL</td>            <td class="indexvalue">9</td></tr>
        <tr><td class="indexvalue">MUSIC_BOX</td>               <td class="indexvalue">10</td></tr>
        <tr><td class="indexvalue">VIBRAPHONE</td>              <td class="indexvalue">11</td></tr>
        <tr><td class="indexvalue">MARIMBA</td>                 <td class="indexvalue">12</td></tr>
        <tr><td class="indexvalue">XYLOPHONE</td>               <td class="indexvalue">13</td></tr>
        <tr><td class="indexvalue">TUBULAR_BELLS</td>           <td class="indexvalue">14</td></tr>
        <tr><td class="indexvalue">DULCIMER</td>                <td class="indexvalue">15</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">DRAWBAR_ORGAN</td>           <td class="indexvalue">16</td></tr>
        <tr><td class="indexvalue">PERCUSSIVE_ORGAN</td>        <td class="indexvalue">17</td></tr>
        <tr><td class="indexvalue">ROCK_ORGAN</td>              <td class="indexvalue">18</td></tr>
        <tr><td class="indexvalue">CHURCH_ORGAN</td>            <td class="indexvalue">19</td></tr>
        <tr><td class="indexvalue">REED_ORGAN</td>              <td class="indexvalue">20</td></tr>
        <tr><td class="indexvalue">ACCORDIAN</td>               <td class="indexvalue">21</td></tr>
        <tr><td class="indexvalue">HARMONICA</td>               <td class="indexvalue">22</td></tr>
        <tr><td class="indexvalue">TANGO_ACCORDIAN</td>         <td class="indexvalue">23</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">GUITAR</td>                  <td class="indexvalue">24</td></tr>
        <tr><td class="indexvalue">NYLON_STRING_GUITAR</td>     <td class="indexvalue">24</td></tr>
        <tr><td class="indexvalue">STEEL_STRING_GUITAR</td>     <td class="indexvalue">25</td></tr>
        <tr><td class="indexvalue">ELECTRIC_JAZZ_GUITAR</td>    <td class="indexvalue">26</td></tr>
        <tr><td class="indexvalue">ELECTRIC_CLEAN_GUITAR</td>   <td class="indexvalue">27</td></tr>
        <tr><td class="indexvalue">ELECTRIC_MUTED_GUITAR</td>   <td class="indexvalue">28</td></tr>
        <tr><td class="indexvalue">OVERDRIVEN_GUITAR</td>       <td class="indexvalue">29</td></tr>
        <tr><td class="indexvalue">DISTORTION_GUITAR</td>       <td class="indexvalue">30</td></tr>
        <tr><td class="indexvalue">GUITAR_HARMONICS</td>        <td class="indexvalue">31</td></tr>
</table></td>
</tr>
<tr>
<td valign=top><table>
        <tr><td class="indexvalue">ACOUSTIC_BASS</td>           <td class="indexvalue">32</td></tr>
        <tr><td class="indexvalue">ELECTRIC_BASS_FINGER</td>    <td class="indexvalue">33</td></tr>
        <tr><td class="indexvalue">ELECTRIC_BASS_PICK</td>      <td class="indexvalue">34</td></tr>
        <tr><td class="indexvalue">FRETLESS_BASS</td>           <td class="indexvalue">35</td></tr>
        <tr><td class="indexvalue">SLAP_BASS_1</td>             <td class="indexvalue">36</td></tr>
        <tr><td class="indexvalue">SLAP_BASS_2</td>             <td class="indexvalue">37</td></tr>
        <tr><td class="indexvalue">SYNTH_BASS_1</td>            <td class="indexvalue">38</td></tr>
        <tr><td class="indexvalue">SYNTH_BASS_2</td>            <td class="indexvalue">39</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">VIOLIN</td>                  <td class="indexvalue">40</td></tr>
        <tr><td class="indexvalue">VIOLA</td>                   <td class="indexvalue">41</td></tr>
        <tr><td class="indexvalue">CELLO</td>                   <td class="indexvalue">42</td></tr>
        <tr><td class="indexvalue">CONTRABASS</td>              <td class="indexvalue">43</td></tr>
        <tr><td class="indexvalue">TREMOLO_STRINGS</td>         <td class="indexvalue">44</td></tr>
        <tr><td class="indexvalue">PIZZICATO_STRINGS</td>       <td class="indexvalue">45</td></tr>
        <tr><td class="indexvalue">ORCHESTRAL_STRINGS</td>      <td class="indexvalue">46</td></tr>
        <tr><td class="indexvalue">TIMPANI</td>                 <td class="indexvalue">47</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">STRING_ENSEMBLE_1</td>       <td class="indexvalue">48</td></tr>
        <tr><td class="indexvalue">STRING_ENSEMBLE_2</td>       <td class="indexvalue">49</td></tr>
        <tr><td class="indexvalue">SYNTH_STRINGS_1</td>         <td class="indexvalue">50</td></tr>
        <tr><td class="indexvalue">SYNTH_STRINGS_2</td>         <td class="indexvalue">51</td></tr>
        <tr><td class="indexvalue">CHOIR_AAHS</td>              <td class="indexvalue">52</td></tr>
        <tr><td class="indexvalue">VOICE_OOHS</td>              <td class="indexvalue">53</td></tr>
        <tr><td class="indexvalue">SYNTH_VOICE</td>             <td class="indexvalue">54</td></tr>
        <tr><td class="indexvalue">ORCHESTRA_HIT</td>           <td class="indexvalue">55</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">TRUMPET</td>                 <td class="indexvalue">56</td></tr>
        <tr><td class="indexvalue">TROMBONE</td>                <td class="indexvalue">57</td></tr>
        <tr><td class="indexvalue">TUBA</td>                    <td class="indexvalue">58</td></tr>
        <tr><td class="indexvalue">MUTED_TRUMPET</td>           <td class="indexvalue">59</td></tr>
        <tr><td class="indexvalue">FRENCH_HORN</td>             <td class="indexvalue">60</td></tr>
        <tr><td class="indexvalue">BRASS_SECTION</td>           <td class="indexvalue">61</td></tr>
        <tr><td class="indexvalue">SYNTHBRASS_1</td>            <td class="indexvalue">62</td></tr>
        <tr><td class="indexvalue">SYNTHBRASS_2</td>            <td class="indexvalue">63</td></tr>
</table></td>
</tr>
<tr>
<td valign=top><table>
        <tr><td class="indexvalue">SOPRANO_SAX</td>             <td class="indexvalue">64</td></tr>
        <tr><td class="indexvalue">ALTO_SAX</td>                <td class="indexvalue">65</td></tr>
        <tr><td class="indexvalue">TENOR_SAX</td>               <td class="indexvalue">66</td></tr>
        <tr><td class="indexvalue">BARITONE_SAX</td>            <td class="indexvalue">67</td></tr>
        <tr><td class="indexvalue">OBOE</td>                    <td class="indexvalue">68</td></tr>
        <tr><td class="indexvalue">ENGLISH_HORN</td>            <td class="indexvalue">69</td></tr>
        <tr><td class="indexvalue">BASSOON</td>                 <td class="indexvalue">70</td></tr>
        <tr><td class="indexvalue">CLARINET</td>                <td class="indexvalue">71</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">PICCOLO</td>                 <td class="indexvalue">72</td></tr>
        <tr><td class="indexvalue">FLUTE</td>                   <td class="indexvalue">73</td></tr>
        <tr><td class="indexvalue">RECORDER</td>                <td class="indexvalue">74</td></tr>
        <tr><td class="indexvalue">PAN_FLUTE</td>               <td class="indexvalue">75</td></tr>
        <tr><td class="indexvalue">BLOWN_BOTTLE</td>            <td class="indexvalue">76</td></tr>
        <tr><td class="indexvalue">SKAKUHACHI</td>              <td class="indexvalue">77</td></tr>
        <tr><td class="indexvalue">WHISTLE</td>                 <td class="indexvalue">78</td></tr>
        <tr><td class="indexvalue">OCARINA</td>                 <td class="indexvalue">79</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">LEAD_SQUARE <i>or</i> SQUARE</td>             <td class="indexvalue">80</td></tr>
        <tr><td class="indexvalue">LEAD_SAWTOOTH <i>or</i> SAWTOOTH</td>           <td class="indexvalue">81</td></tr>
        <tr><td class="indexvalue">LEAD_CALLIOPE <i>or</i> CALLIOPE</td>           <td class="indexvalue">82</td></tr>
        <tr><td class="indexvalue">LEAD_CHIFF <i>or</i> CHIFF</td>              <td class="indexvalue">83</td></tr>
        <tr><td class="indexvalue">LEAD_CHARANG <i>or</i> CHARANG</td>            <td class="indexvalue">84</td></tr>
        <tr><td class="indexvalue">LEAD_VOICE <i>or</i> VOICE</td>              <td class="indexvalue">85</td></tr>
        <tr><td class="indexvalue">LEAD_FIFTHS <i>or</i> FIFTHS</td>             <td class="indexvalue">86</td></tr>
        <tr><td class="indexvalue">LEAD_BASSLEAD <i>or</i> BASSLEAD</td>           <td class="indexvalue">87</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">PAD_NEW_AGE <i>or</i> NEW_AGE</td>             <td class="indexvalue">88</td></tr>
        <tr><td class="indexvalue">PAD_WARM <i>or</i> WARM</td>                <td class="indexvalue">89</td></tr>
        <tr><td class="indexvalue">PAD_POLYSYNTH <i>or</i> POLYSYNTH</td>           <td class="indexvalue">90</td></tr>
        <tr><td class="indexvalue">PAD_CHOIR <i>or</i> CHOIR</td>               <td class="indexvalue">91</td></tr>
        <tr><td class="indexvalue">PAD_BOWED <i>or</i> BOWED</td>               <td class="indexvalue">92</td></tr>
        <tr><td class="indexvalue">PAD_METALLIC <i>or</i> METALLIC</td>            <td class="indexvalue">93</td></tr>
        <tr><td class="indexvalue">PAD_HALO <i>or</i> HALO</td>                <td class="indexvalue">94</td></tr>
        <tr><td class="indexvalue">PAD_SWEEP <i>or</i> SWEEP</td>               <td class="indexvalue">95</td></tr>
</table></td>
</tr>
<tr>
<td valign=top><table>
        <tr><td class="indexvalue">FX_RAIN <i>or</i> RAIN</td>                 <td class="indexvalue">96</td></tr>
        <tr><td class="indexvalue">FX_SOUNDTRACK <i>or</i> SOUNDTRACK</td>           <td class="indexvalue">97</td></tr>
        <tr><td class="indexvalue">FX_CRYSTAL <i>or</i> CRYSTAL</td>              <td class="indexvalue">98</td></tr>
        <tr><td class="indexvalue">FX_ATMOSPHERE <i>or</i> ATMOSPHERE</td>           <td class="indexvalue">99</td></tr>
        <tr><td class="indexvalue">FX_BRIGHTNESS <i>or</i> BRIGHTNESS</td>           <td class="indexvalue">100</td></tr>
        <tr><td class="indexvalue">FX_GOBLINS <i>or</i> GOBLINS</td>              <td class="indexvalue">101</td></tr>
        <tr><td class="indexvalue">FX_ECHOES <i>or</i> ECHOES</td>               <td class="indexvalue">102</td></tr>
        <tr><td class="indexvalue">FX_SCI-FI <i>or</i> SCI-FI</td>               <td class="indexvalue">103</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">SITAR</td>                   <td class="indexvalue">104</td></tr>
        <tr><td class="indexvalue">BANJO</td>                   <td class="indexvalue">105</td></tr>
        <tr><td class="indexvalue">SHAMISEN</td>                <td class="indexvalue">106</td></tr>
        <tr><td class="indexvalue">KOTO</td>                    <td class="indexvalue">107</td></tr>
        <tr><td class="indexvalue">KALIMBA</td>                 <td class="indexvalue">108</td></tr>
        <tr><td class="indexvalue">BAGPIPE</td>                 <td class="indexvalue">109</td></tr>
        <tr><td class="indexvalue">FIDDLE</td>                  <td class="indexvalue">110</td></tr>
        <tr><td class="indexvalue">SHANAI</td>                  <td class="indexvalue">111</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">TINKLE_BELL</td>             <td class="indexvalue">112</td></tr>
        <tr><td class="indexvalue">AGOGO</td>                   <td class="indexvalue">113</td></tr>
        <tr><td class="indexvalue">STEEL_DRUMS</td>             <td class="indexvalue">114</td></tr>
        <tr><td class="indexvalue">WOODBLOCK</td>               <td class="indexvalue">115</td></tr>
        <tr><td class="indexvalue">TAIKO_DRUM</td>              <td class="indexvalue">116</td></tr>
        <tr><td class="indexvalue">MELODIC_TOM</td>             <td class="indexvalue">117</td></tr>
        <tr><td class="indexvalue">SYNTH_DRUM</td>              <td class="indexvalue">118</td></tr>
        <tr><td class="indexvalue">REVERSE_CYMBAL</td>          <td class="indexvalue">119</td></tr>
</table></td>
<td valign=top><table>
        <tr><td class="indexvalue">GUITAR_FRET_NOISE</td>       <td class="indexvalue">120</td></tr>
        <tr><td class="indexvalue">BREATH_NOISE</td>            <td class="indexvalue">121</td></tr>
        <tr><td class="indexvalue">SEASHORE</td>                <td class="indexvalue">122</td></tr>
        <tr><td class="indexvalue">BIRD_TWEET</td>              <td class="indexvalue">123</td></tr>
        <tr><td class="indexvalue">TELEPHONE_RING</td>          <td class="indexvalue">124</td></tr>
        <tr><td class="indexvalue">HELICOPTER</td>              <td class="indexvalue">125</td></tr>
        <tr><td class="indexvalue">APPLAUSE</td>                <td class="indexvalue">126</td></tr>
        <tr><td class="indexvalue">GUNSHOT</td>                 <td class="indexvalue">127</td></tr>
</table></td>
</tr>
</table>
\endhtmlonly

\subsection subKeySignatures Specifying Key Signatures
KeySignature specification is one of the powerful features of CFugue MusicStrings that makes it easy to write music notes and keep them simple and clean. A KeySignature essentially indicates how CFugue should interpret the notes it encouters during the MusicString parsing.

In CFugue, KeySignature tokens are identified with character <b>K</b> at their beginning. Below are few examples of valid KeySignature tokens:
<pre class="fragment">
    player.Play("K[CMAJ]"); 
    player.Play("K[F#MAJ]"); 
    player.Play("K[KANAKANGI]"); 
</pre>
Ofcourse, the above does not produce any music output since there are no notes specified in the input. Usually one supplies a series of note tokens after a KeySignature token for generting music as per the supplied Key. Examples are as shown below:
<pre class="fragment">
    player.Play("K[CMaj] C D E F G A B"); <span class="comment">// Play an Octave in CMajor Scale </span>
    player.Play("K[FMaj] C D E F G A B"); <span class="comment">// Play an Octave in FMajor Scale </span>
    player.Play("K[Kalyani] S R G M P D N"); <span class="comment">// Play an Octave in Kalyani Ragam</span> 
    player.Play("K[Kanakangi] S R G M P D N"); <span class="comment">// Play an Octave in Kanakangi Ragam</span> 
</pre>

When a note is specified using its name, CFugue considers it as a relative half-step value within an octave. The KeySignature directive helps CFugue compute the absolute MIDI numeric value of the note based on the selected Scale/Ragam value. When one specifies tokens such as K[CMaj] or K[Kalyani] in the MusicStrings, CFugue starts interpreting the named notes C, D, E.. S, R, G.. according to their values in CMajor Scale or Kalyani Raga.

For example, in the above, when K[FMaj] is specified, the B note in the octave will be treated as B<i>b</i> automatically. Similarily, for K[Kalyani], the M note will be treated as M2 automatically. 

When no KeySignature is specified, CFugue assumes these default Values: K[CMaj] for Western and K[Shankarabharanam] for Carnatic.

When a KeySignature is specified, it will be applied for all subsequent notes. However, if you do not want the KeySignature to be applied to any specific note, you have to either use the MIDI numeric value directly or use the note modifier <i>n</i> to declare it as a natural note. (Note modifiers are not applicable for Carnatic music. You have to explicitly specify the complete name, such as R1, R2 etc. to make it a natural note.) Natural notes are not affected by KeySignature. By definition, all MIDI numeric notes are natural notes. Thus, the below all five produce the same result.
<pre class="fragment">
    player.Play("K[CMaj] C D E F G A B"); <span class="comment">// Play an Octave in CMajor Scale </span>
    player.Play("K[FMaj] C D E F G A Bn"); <span class="comment">// B is declared as natural. FMaj is ignored for B</span>
    player.Play("K[Shankarabharanam] S R G M P D N"); <span class="comment">// Play an Octave in Shankarabharanam Ragam</span> 
    player.Play("K[Kalyani] S R G M1 P D N"); <span class="comment">// M1 declared explicitly. Kalyani is ignored for M</span> 
    player.Play("K[FMaj] C D E F G A [71]"); <span class="comment">// [71] is numeric note. FMaj is ignored for it</span>
</pre>
 KeySignatures are applicable for the whole song, independent of tracks. They can be redefined as many times as one wants throughout the song. CFugue will use the latest definition that precedes a note to process the note.

 KeySignatures support both Carnatic and Western music modes. In Western mode, a total of 15 KeySignatures are present. These are accessible through their names, such as CMAJ, DMAJ, F\#MIN etc. Complete list of the names is given in the \ref KeySignatureTable "KeySignatures table".

 In Carnatic mode, all the basic 72 KeySignatures, known as Melakartha Janya Ragas, are supported. These are accessible through MELA_x macro, where x specifies a number in the range [1, 72]. In addition, for convenience, MELA_0 and MELA_DEFAULT are available, that map to Melakartha 29 (Shankarabharanam). Further, numerous ragas are accessible directly through their names, such as KALYANI, MOHANA etc. Thus one can either use the MELA_x macro or the raga name for the KeySignature. For example, K[Mela_65] and K[Kalyani] both give same results. Refer to the below listing for complete set of mappings between raga names and their associated Melakartha janyas. Note that when using an incomplete raga (one that does not use all the seven swaras), the missing notes in the raga will default to their corresponding values in the Melakartha janya of the raga. 

 \anchor KeySignatureTable \htmlonly
 <table align="center" border="1">
  <caption ALIGN=bottom><br/>KeySignatures supported by CFugue</caption>
 <tr>
 <td valign=top> 
     <table align="center">
	 <tr><td class="indexkey">Scale Name</td><td class="indexkey">Note Values</td></tr>
     <tr><td class="indexvalue">CBMAJ</td><td class="indexvalue">{ Cb Db Eb Fb Gb Ab Bb }</td></tr>   
     <tr><td class="indexvalue">ABMIN</td><td class="indexvalue">{ Cb Db Eb Fb Gb Ab Bb }</td></tr>   

     <tr><td class="indexvalue">GBMAJ</td><td class="indexvalue">{ Cb Db Eb F  Gb Ab Bb }</td></tr>   
     <tr><td class="indexvalue">EBMIN</td><td class="indexvalue">{ Cb Db Eb F  Gb Ab Bb }</td></tr>   

     <tr><td class="indexvalue">DBMAJ</td><td class="indexvalue">{ C  Db Eb F  Gb Ab Bb }</td></tr>   
     <tr><td class="indexvalue">BBMIN</td><td class="indexvalue">{ C  Db Eb F  Gb Ab Bb }</td></tr>   

     <tr><td class="indexvalue">ABMAJ</td><td class="indexvalue">{ C  Db Eb F  G  Ab Bb }</td></tr>   
     <tr><td class="indexvalue">FMIN</td><td class="indexvalue">{ C  Db Eb F  G  Ab Bb }</td></tr>  

     <tr><td class="indexvalue">EBMAJ</td><td class="indexvalue">{ C  D  Eb F  G  Ab Bb }</td></tr>   
     <tr><td class="indexvalue">CMIN</td><td class="indexvalue">{ C  D  Eb F  G  Ab Bb }</td></tr>  

     <tr><td class="indexvalue">BBMAJ</td><td class="indexvalue">{ C  D  Eb F  G  A  Bb }</td></tr>   
     <tr><td class="indexvalue">GMIN</td><td class="indexvalue">{ C  D  Eb F  G  A  Bb }</td></tr>  

     <tr><td class="indexvalue">FMAJ</td><td class="indexvalue">{ C  D  E  F  G  A  Bb }</td></tr>   
     <tr><td class="indexvalue">DMIN</td><td class="indexvalue">{ C  D  E  F  G  A  Bb }</td></tr>  

     <tr><td class="indexvalue">CMAJ</td><td class="indexvalue">{ C  D  E  F  G  A  B  }</td></tr>   
     <tr><td class="indexvalue">AMIN</td><td class="indexvalue">{ C  D  E  F  G  A  B  }</td></tr>   

     <tr><td class="indexvalue">GMAJ</td><td class="indexvalue">{ C  D  E  F# G  A  B  }</td></tr>    
     <tr><td class="indexvalue">EMIN</td><td class="indexvalue">{ C  D  E  F# G  A  B  }</td></tr>    

     <tr><td class="indexvalue">DMAJ</td><td class="indexvalue">{ C# D  E  F# G  A  B  }</td></tr>    
     <tr><td class="indexvalue">BMIN</td><td class="indexvalue">{ C# D  E  F# G  A  B  }</td></tr>    

     <tr><td class="indexvalue">AMAJ</td><td class="indexvalue">{ C# D  E  F# G# A  B  }</td></tr>   
     <tr><td class="indexvalue">F#MIN</td><td class="indexvalue">{ C# D  E  F# G# A  B  }</td></tr>   

     <tr><td class="indexvalue">EMAJ</td><td class="indexvalue">{ C# D# E  F# G# A  B  }</td></tr>   
     <tr><td class="indexvalue">C#MIN</td><td class="indexvalue">{ C# D# E  F# G# A  B  }</td></tr>   

     <tr><td class="indexvalue">BMAJ</td><td class="indexvalue">{ C# D# E  F# G# A# B  }</td></tr>   
     <tr><td class="indexvalue">G#MIN</td><td class="indexvalue">{ C# D# E  F# G# A# B  }</td></tr>   

     <tr><td class="indexvalue">F#MAJ</td><td class="indexvalue">{ C# D# E# F# G# A# B  }</td></tr>   
     <tr><td class="indexvalue">D#MIN</td><td class="indexvalue">{ C# D# E# F# G# A# B  }</td></tr>   

     <tr><td class="indexvalue">C#MAJ</td><td class="indexvalue">{ C# D# E# F# G# A# B# }</td></tr>   
     <tr><td class="indexvalue">A#MIN</td><td class="indexvalue">{ C# D# E# F# G# A# B# }</td></tr>   
     </table>
 </td>
 <td valign=top>
    <table align="center">
	 <tr><td class="indexkey">Raga Name</td><td class="indexkey">Mela_x</td><td class="indexkey">Swara Values</td></tr>
    <tr><td class="indexvalue">KANAKANGI</td><td class="indexvalue">MELA_1 </td><td class="indexvalue">{ S R1 G1 M1 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">RATNANGI</td><td class="indexvalue">MELA_2 </td><td class="indexvalue">{ S R1 G1 M1 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">GANAMURTHI</td><td class="indexvalue">MELA_3 </td><td class="indexvalue">{ S R1 G1 M1 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">VANASPATHI</td><td class="indexvalue">MELA_4 </td><td class="indexvalue">{ S R1 G1 M1 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">MANAVATHI</td><td class="indexvalue">MELA_5 </td><td class="indexvalue">{ S R1 G1 M1 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">TANARUPI</td><td class="indexvalue">MELA_6 </td><td class="indexvalue">{ S R1 G1 M1 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">SENAVATHI</td><td class="indexvalue">MELA_7 </td><td class="indexvalue">{ S R1 G2 M1 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">HANUMATHODI</td><td class="indexvalue">MELA_8 </td><td class="indexvalue">{ S R1 G2 M1 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">DHENUKA</td><td class="indexvalue">MELA_9 </td><td class="indexvalue">{ S R1 G2 M1 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">NATAKAPRIYA</td><td class="indexvalue">MELA_10</td><td class="indexvalue">{ S R1 G2 M1 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">KOKILAPRIYA</td><td class="indexvalue">MELA_11</td><td class="indexvalue">{ S R1 G2 M1 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">RUPAVATHI</td><td class="indexvalue">MELA_12</td><td class="indexvalue">{ S R1 G2 M1 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">GAYAKAPRIYA</td><td class="indexvalue">MELA_13</td><td class="indexvalue">{ S R1 G3 M1 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">VAKULABHARANAM</td><td class="indexvalue">MELA_14</td><td class="indexvalue">{ S R1 G3 M1 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">MAYAMALAVAGOWLA</td><td class="indexvalue">MELA_15</td><td class="indexvalue">{ S R1 G3 M1 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">CHAKRAVAKAM</td><td class="indexvalue">MELA_16</td><td class="indexvalue">{ S R1 G3 M1 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">SURYAKANTHAM</td><td class="indexvalue">MELA_17</td><td class="indexvalue">{ S R1 G3 M1 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">HATAKAMBARI</td><td class="indexvalue">MELA_18</td><td class="indexvalue">{ S R1 G3 M1 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">JHANKARADHWANI</td><td class="indexvalue">MELA_19</td><td class="indexvalue">{ S R2 G2 M1 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">NATABHAIRAVI</td><td class="indexvalue">MELA_20</td><td class="indexvalue">{ S R2 G2 M1 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">KEERAVANI</td><td class="indexvalue">MELA_21</td><td class="indexvalue">{ S R2 G2 M1 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">KHARAHARAPRIYA</td><td class="indexvalue">MELA_22</td><td class="indexvalue">{ S R2 G2 M1 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">GOWRIMANOHARI</td><td class="indexvalue">MELA_23</td><td class="indexvalue">{ S R2 G2 M1 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">VARUNAPRIYA</td><td class="indexvalue">MELA_24</td><td class="indexvalue">{ S R2 G2 M1 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">MARARANJANI</td><td class="indexvalue">MELA_25</td><td class="indexvalue">{ S R2 G3 M1 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">CHARUKESHI</td><td class="indexvalue">MELA_26</td><td class="indexvalue">{ S R2 G3 M1 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">SARASANGI</td><td class="indexvalue">MELA_27</td><td class="indexvalue">{ S R2 G3 M1 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">HARIKHAMBHOJI</td><td class="indexvalue">MELA_28</td><td class="indexvalue">{ S R2 G3 M1 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">DHEERASHANKARABHARANAM</td><td class="indexvalue">MELA_29</td><td class="indexvalue">{ S R2 G3 M1 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">SHANKARABHARANAM</td><td class="indexvalue">MELA_29</td><td class="indexvalue">{ S R2 G3 M1 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">NAGANANDINI</td><td class="indexvalue">MELA_30</td><td class="indexvalue">{ S R2 G3 M1 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">YAGAPRIYA</td><td class="indexvalue">MELA_31</td><td class="indexvalue">{ S R3 G3 M1 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">RAGAVARDHINI</td><td class="indexvalue">MELA_32</td><td class="indexvalue">{ S R3 G3 M1 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">GANGEYABHUSHINI</td><td class="indexvalue">MELA_33</td><td class="indexvalue">{ S R3 G3 M1 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">VAGADHISHWARI</td><td class="indexvalue">MELA_34</td><td class="indexvalue">{ S R3 G3 M1 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">SHULINI</td><td class="indexvalue">MELA_35</td><td class="indexvalue">{ S R3 G3 M1 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">CHALANATTAI</td><td class="indexvalue">MELA_36</td><td class="indexvalue">{ S R3 G3 M1 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">SALAGAM</td><td class="indexvalue">MELA_37</td><td class="indexvalue">{ S R1 G1 M2 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">JALARNAVAM</td><td class="indexvalue">MELA_38</td><td class="indexvalue">{ S R1 G1 M2 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">JHALAVARALI</td><td class="indexvalue">MELA_39</td><td class="indexvalue">{ S R1 G1 M2 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">NAVANEETHAM</td><td class="indexvalue">MELA_40</td><td class="indexvalue">{ S R1 G1 M2 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">PAVANI</td><td class="indexvalue">MELA_41</td><td class="indexvalue">{ S R1 G1 M2 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">RAGHUPRIYA</td><td class="indexvalue">MELA_42</td><td class="indexvalue">{ S R1 G1 M2 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">GAVAMBODHI</td><td class="indexvalue">MELA_43</td><td class="indexvalue">{ S R1 G2 M2 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">BHAVAPRIYA</td><td class="indexvalue">MELA_44</td><td class="indexvalue">{ S R1 G2 M2 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">SHUBHAPANTUVARALI</td><td class="indexvalue">MELA_45</td><td class="indexvalue">{ S R1 G2 M2 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">PANTUVARALI</td><td class="indexvalue">MELA_45</td><td class="indexvalue">{ S R1 G2 M2 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">SHADVIDHAMARGINI</td><td class="indexvalue">MELA_46</td><td class="indexvalue">{ S R1 G2 M2 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">SUVARNANGI</td><td class="indexvalue">MELA_47</td><td class="indexvalue">{ S R1 G2 M2 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">DIVYAMANI</td><td class="indexvalue">MELA_48</td><td class="indexvalue">{ S R1 G2 M2 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">DHAVALAMBARI</td><td class="indexvalue">MELA_49</td><td class="indexvalue">{ S R1 G3 M2 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">NAMANARAYANI</td><td class="indexvalue">MELA_50</td><td class="indexvalue">{ S R1 G3 M2 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">KAMAVARDHINI</td><td class="indexvalue">MELA_51</td><td class="indexvalue">{ S R1 G3 M2 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">RAMAPRIYA</td><td class="indexvalue">MELA_52</td><td class="indexvalue">{ S R1 G3 M2 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">GAMANASHRAMA</td><td class="indexvalue">MELA_53</td><td class="indexvalue">{ S R1 G3 M2 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">VISHWAMBHARI</td><td class="indexvalue">MELA_54</td><td class="indexvalue">{ S R1 G3 M2 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">SHYAMALANGI</td><td class="indexvalue">MELA_55</td><td class="indexvalue">{ S R2 G2 M2 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">SHANMUKHAPRIYA</td><td class="indexvalue">MELA_56</td><td class="indexvalue">{ S R2 G2 M2 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">SIMHENDRAMADHYAMAM</td><td class="indexvalue">MELA_57</td><td class="indexvalue">{ S R2 G2 M2 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">HEMAVATHI</td><td class="indexvalue">MELA_58</td><td class="indexvalue">{ S R2 G2 M2 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">DHARMAVATHI</td><td class="indexvalue">MELA_59</td><td class="indexvalue">{ S R2 G2 M2 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">NEETHIMATHI</td><td class="indexvalue">MELA_60</td><td class="indexvalue">{ S R2 G2 M2 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">KANTHAMANI</td><td class="indexvalue">MELA_61</td><td class="indexvalue">{ S R2 G3 M2 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">RISHABHAPRIYA</td><td class="indexvalue">MELA_62</td><td class="indexvalue">{ S R2 G3 M2 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">LATHANGI</td><td class="indexvalue">MELA_63</td><td class="indexvalue">{ S R2 G3 M2 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">VACHASPATHI</td><td class="indexvalue">MELA_64</td><td class="indexvalue">{ S R2 G3 M2 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">MECHAKALYANI</td><td class="indexvalue">MELA_65</td><td class="indexvalue">{ S R2 G3 M2 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">KALYANI</td><td class="indexvalue">MELA_65</td><td class="indexvalue">{ S R2 G3 M2 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">CHITRAMBARI</td><td class="indexvalue">MELA_66</td><td class="indexvalue">{ S R2 G3 M2 P D3 N3 }</td></tr>
    <tr><td class="indexvalue">SUCHARITRA</td><td class="indexvalue">MELA_67</td><td class="indexvalue">{ S R3 G3 M2 P D1 N1 }</td></tr>
    <tr><td class="indexvalue">JYOTHISWARUPINI</td><td class="indexvalue">MELA_68</td><td class="indexvalue">{ S R3 G3 M2 P D1 N2 }</td></tr>
    <tr><td class="indexvalue">DHATUVARDHINI</td><td class="indexvalue">MELA_69</td><td class="indexvalue">{ S R3 G3 M2 P D1 N3 }</td></tr>
    <tr><td class="indexvalue">NASIKABHUSHINI</td><td class="indexvalue">MELA_70</td><td class="indexvalue">{ S R3 G3 M2 P D2 N2 }</td></tr>
    <tr><td class="indexvalue">KOSALAM</td><td class="indexvalue">MELA_71</td><td class="indexvalue">{ S R3 G3 M2 P D2 N3 }</td></tr>
    <tr><td class="indexvalue">RASIKAPRIYA</td><td class="indexvalue">MELA_72</td><td class="indexvalue">{ S R3 G3 M2 P D3 N3 }</td></tr>
    </table>
 </td>
 <td valign=top>
	 <table align="center">
	 	 <tr><td class="indexkey">Somemore Ragas</td><td class="indexkey">Janya Mela</td></tr>
		<tr><td class="indexvalue">BILAHARI</td><td class="indexvalue">MELA_29</td></tr>
		<tr><td class="indexvalue">HAMSADHWANI</td><td class="indexvalue">MELA_29</td></tr>
		<tr><td class="indexvalue">HINDOLAM</td><td class="indexvalue">MELA_20</td></tr>
		<tr><td class="indexvalue">KAMBHOJI</td><td class="indexvalue">MELA_28</td></tr>
		<tr><td class="indexvalue">MADHYAMAVATHI</td><td class="indexvalue">MELA_22</td></tr>
		<tr><td class="indexvalue">MOHANAM</td><td class="indexvalue">MELA_28</td></tr>
		<tr><td class="indexvalue">NEELAMBARI</td><td class="indexvalue">MELA_29</td></tr> 
	 </table>
 </td>
 </tr>
 </table>
 \endhtmlonly

For CFugue, Western is the default mode, and CMajor is the default KeySignature. All Parsers start in Western mode with CMajor scale set to their default. If you rather want the Parser to use a Carnatic mode, then you should explicitly supply a KeySignature token with an appropriate value before passing on any Carnatic music notes.

For further details on how to interoperate between Western and Carnatic style of music, refer \ref secInteroperation.

\subsection subVoices Specifying Channels / Tracks
Channels / Tracks are referred to as <i>voices</i> in CFugue. The tokens that start with character <b>V</b> are considered as voice tokens. 

CFugue supports all the 16 channels of MIDI, which are accessible through V0 to 15. Whenever CFugue encounters a voice token in the MusicString, it changes the current track to the specified one in the token, inserting all the subsequent notes into this newly set track. When you want to switch back to your old track to insert notes into it, you simply supply another voice token with the corresponding track number. 

By default, <i>V0</i> is implied at the beginning.

Be noted that channels/tracks/voices play in parallel. For example, the MusicString <tt>CMAJ V1 CMIN</tt> plays CMAJ chord on channel 0 in <i>parallel</i> with CMIN chord on channel 1. Just because <tt>V1 CMIN</tt> comes <i>after</i> CMAJ in the Music String does not make it wait till CMAJ completes playing. Channels play independent of each other and certain MIDI players allow you to turn off selective tracks, so that you can listen only few tracks of your choice. 

In CFugue, if you would like to sync the channels, you have to use the Rest notes. For example, to make CMIN chord on channel 1 play <i>after</i> the CMAJ chord completes playing on channel 0, you need to insert a Rest note with sufficient duration on channel 1, something like <tt>CMAJ V1 R CMIN</tt>. This makes the Rest note play in parallel with CMAJ, effectively making CMIN play after CMAJ is completed. 

\section secInteroperation Interoperating Western and Carnatic Music
A powerful feature of CFugue's MusicString is its ability to work with both Western and Carnatic music with same ease. For the first time in the music world, it is now not only possible to specify 
Carnatic music Swaras side by side with Western Notes, it is also possible to transform between 
the two notations with same ease. 

\note In addition, the same principles can be adapted for Hindustani music also to make it work side by side with Western music.

By default CFugue starts parsing the notes in Western mode. However, when it encounters the K[] signature token with Carnatic music mela as key, it switches to Carnatic music parsing mode.
*/

/////////////////////////////////////////////////////////////////
// Future page
/////////////////////////////////////////////////////////////////
/*! \page pageFuture CFugue Future
\section future The Future

With CFugue still in its infancy, the future of Music Note programming is still shining bright upon it.
The possibilities of what can be achieved through high-level MIDI note programming are endless. From 
automated Music analysis, recognition and generation to experimental cross genre Music research, all 
are going to benefit immesenly from this work. We have kept all the options open, to ensure we include
the new developments in the work as and when we discover the scope for improvements.

*/

/* Code that generates MIDI Note numbers for all notes
#include <stdio.h>
#include <stdlib.h>
void main(int argc, char* argv[])
{
    printf("\n<table>");
    printf("\n<tr><td align=\"center\" class=\"indexkey\">Octave</td><td align=\"center\" class=\"indexkey\">C<br>S</td> <td align=\"center\" class=\"indexkey\">C#/D<i>b</i><br>R1</td> <td align=\"center\" class=\"indexkey\">D<br>R2/G1</td> <td align=\"center\" class=\"indexkey\">D#/E<i>b</i><br>R3/G2</td><td align=\"center\" class=\"indexkey\">E<br>G3</td><td align=\"center\" class=\"indexkey\">F<br>M1</td><td align=\"center\" class=\"indexkey\">F#/G<i>b</i><br>M2</td><td align=\"center\" class=\"indexkey\">G<br>P</td><td align=\"center\" class=\"indexkey\">G#/A<i>b</i><br>D1</td><td align=\"center\" class=\"indexkey\">A<br>D2/N1</td><td align=\"center\" class=\"indexkey\">A#/B<i>b</i><br>N2</td><td align=\"center\" class=\"indexkey\">B<br>N3</td>  </tr>");

    for(int octave=0, nMidiNumber=0; octave <= 10; ++octave)
    {
        printf("\n<tr> <td align=\"center\" class=\"indexkey\">%d</td>", octave);
        for(int halfstep = 0; halfstep <= 11; ++halfstep, nMidiNumber++)
        {
            if(nMidiNumber < 128) printf("\n\t<td align=\"center\" class=\"indexvalue\">%d</td>", nMidiNumber);
        }
        printf("\n</tr>");
    }

    printf("\n</table>\n");
}
*/