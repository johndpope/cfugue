/*
    This is part of CFugue, a C++ Runtime environment for MIDI Score Programming
    Copyright (C) 2009-2010 Gopalakrishna Palem

    For links to further information, or to contact the author,
    see <http://musicnote.sourceforge.net/>.
*/

/////////////////////////////////////////////////////////////////
// Main Page
/////////////////////////////////////////////////////////////////
/*! \mainpage MusicNoteLib, The C++ Music Programming Library

\section mainPageContents Contents
    - \ref overview
    - \ref features
    - \ref pageExamples "How to use CFugue API"
    - \ref pageMusicString "CFugue MusicString Specifications"
    - \ref download "Download CFugue"
    - \ref copyright "Contact us"

\section overview Overview

Popularily referred to as CFugue, meaning Carnatic Fugue or the C/C++
replacement of JFugue, MusicNoteLib is a high level Music Programming Library.

This library makes it possible to play music notes directly from C/C++ programs,
without ever having to deal with the low-level MIDI complexities. This library 
provides a beautiful abstraction that lets you concetrate on programming the <i>Music</i> rather
than worry about the MIDI nuances.

Provides features that makes it possible to use this library directly from many 
platforms, including but not limited to ASP pages, .Net applications and even non-Windows
based systems.

\section usage Usage

Using this library to play music is as easy as writing plain music notes. Just create
a Player object and call the Play method on it, supplying the Music notes to be played. Simple.
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {
        MusicNoteLib::Player player; <span class="comment">/ Create the Player Object</span>
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

CFugue is ready to be used in your applications. It can be found at the downloads section of http://sourceforge.net/projects/musicnote/

\section limitations Limitations

CFugue is under active development, with much of the features, such as rendering to sheet music, etc. 
pending. By the time the development completes, we hope to have this Limitations section to be empty.

CFugue aims to be platform independent. At present the primary development is happening on Win32 and the real work of porting and testing
to ensure the full platform compliance is still in-progress.

\section copyright Copyright

This is a product of CineFx Research Labs, made available free of charge for personal and research use. 
For commercial usage, please contact the author. 

CFugue is distributed with the hope that it will be useful.
No warranty of what-so-ever is implied, including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 Author: <a href="http://gpalem.web.officelive.com/index.html">Gopalakrishna Palem</a>

 Project: http://musicnote.sourceforge.net

 Copyright (C) 2009-2010 <a href="http://www.CineFxLabs.com">CineFx Digital Media Pvt Ltd.</a>

*/

/////////////////////////////////////////////////////////////////
// Examples page
/////////////////////////////////////////////////////////////////

/*! \page pageExamples CFugue usage examples

\section secExampleContents Contents
    - \ref ancPlay "Playing the Notes"
    - \ref ancSaveMIDI "Saving to MIDI file"
    - \ref ancAsynchPlay "Playing Asynchronously"
    - \ref ancParserEvents "Accessing the Parser events"
    - \ref index "Back to main page"

\anchor ancPlay <b>Playing the Notes</b>
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {		
        MusicNoteLib::Player player; <span class="comment">// Create the Player Object</span>
        
        player.Play("C D E F G A B"); <span class="comment">// Play the Music Notes on the default MIDI output port</span>
    }
</pre>
Playing them Carnatic style:
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {
        MusicNoteLib::Player player; <span class="comment">// Create the Player Object</span>
        
        player.Play("K[MELA_DEFAULT] S R G M P D N"); <span class="comment">// Play the Music Notes on the default MIDI output port</span>
    }
</pre>

\anchor ancSaveMIDI <b>Saving Music Notes to MIDI output file</b>
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {   
        MusicNoteLib::Player player; <span class="comment">// Create the Player Object </span>
        
        player.SaveAsMidiFile("Cq Dw Ex", "MidiOutput.midi"); <span class="comment">// Save the Music Notes to Midi file directly, without playing</span>
    }
</pre>
Sometimes you might want to play the Notes first on the MIDI output and then save the played 
content to an output file. You can achieve this using the Player::SaveToMidiFile() method.
You need to call this method <i>after</i> the call to Player::Play() or Player::PlayAsync().
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {   
        MusicNoteLib::Player player; <span class="comment">// Create the Player Object </span> 

        <span class="comment">// Play few Music Notes on MIDI output port</span>
        player.Play("ci di K[MELA_22] Pa M G R"); 

        <span class="comment">// Now, save that played content to a MIDI output file</span>
        player.SaveToMidiFile("Output.mid"); 
    }
</pre>

\anchor ancAsynchPlay <b>Asynchronous Play</b>

The Player::Play() method plays the music notes synchronously, i.e. the method 
will not return till all the music notes are played completely. However, if you would like to play
the MusicString content asynchronously, you can use the Player::PlayAsync() method
instead. It works similar to the Play() method, except that it returns immediately.

<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {
        MusicNoteLib::Player player; <span class="comment">// Create the Player object</span>

        if(player.PlayAsync("Cx Dh Ah.")) <span class="comment">// Start Playing Asynchronously </span>
            while(player.IsPlaying()) <span class="comment">// Wait while the play is in progress</span>
                Sleep(1000);

        player.StopPlay(); <span class="comment">// Match every PlayAsync with a StopPlay</span>
    }
</pre>
At any time after the call to PlayAsync, the Player::IsPlaying() method can be used
to know if the play is still in progress. Player::StopPlay() allows you
to stop the play if it is still in progress. Calling StopPlay() after every PlayAsync()
call is a good practice (no matter if play is in progress or not). A typical usage of these methods
is demonstrated in the above code snippet.

\anchor ancParserEvents <b>Accessing the Parser events</b>

A MusicString need be parsed before it can be played or rendered as sheet output. While performing
the parsing operation, parser generates Trace events and Error events to let the caller
know of the status. Callers can handle these events and output the Trace and Error information
to a log file or appropriate UI.

Use the Player::Parser() method to access the parser object associated with the player
and subscribe to its evTrace and evError events.
Once subscribed, the attached handlers will be invoked during the parse phase whenever any of the
Play or Save methods is invoked on the player. A sample demonstrating the procedure is below.
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void OnParseTrace(const MusicNoteLib::CParser*, 
                MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs)
    {
        OutputDebugString(_T("\n"));
        OutputDebugString(pEvArgs->szTraceMsg);
    }

    void OnParseError(const MusicNoteLib::CParser*,
                MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs)
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
        MusicNoteLib::Player player; <span class="comment">// Create the Player Object</span>

        <span class="comment">// Access the Player's Parser and Subscribe to its Trace and Error Events</span>
        player.Parser().evTrace.Subscribe(&OnParseTrace); 
        player.Parser().evError.Subscribe(&OnParseError);

        <span class="comment">// Parse the Notes and Save the content to a MIDI file</span>
        player.SaveAsMidiFile(_T("Cq Dw Ex"), "MidiOutput.midi"); 
     }
</pre>*/


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
        - \ref subKeySignatures
    - \ref secInteroperation
    - \ref index "Back to main page"

\section introduction Introduction
One of the capabilities of CFugue that makes it so elegant and easy to use, is the concept of Music Note Strings 
(or simply <i>MusicString</i>s, as referred to by JFugue). MusicStrings provide the ability to specify 
Music Notes and MIDI instructions as a sequence of characters (and tokens) in human readable form,
as opposed to the usual complex binary data byte form. This makes it easy to learn and master. Below is an example of a MusicString that plays a Mid-C note followed by a C-Major chord. (Please refer \ref pageExamples "CFugue API examples" for more such examples.)
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {		
        MusicNoteLib::Player player; <span class="comment">// Create the Player Object</span>
        
        player.Play("C CMaj"); <span class="comment">// Play a Mid-C followed by C-Major Chord </span>
    }
</pre>
<!-- A powerful feature of CFugue's MusicString is its ability to work with both Western and Carnatic music 
with same ease. For the first time in the music world, it is now not only possible to specify 
Carnatic music Swaras side by side with Western Notes, it is also possible to transform between 
the two notations with same ease. 

\note In addition, the same principles can be adapted for Hindustani music also to make it work side by side with Western music.
-->
In the following we discuss at length the details of what constitues a MusicString and how it can be used for creating music with CFugue.

\section secMusicString Components of MusicString
In CFugue, a valid MusicString is formed out of a series of music tokens. Tokens are nothing but sequence of alpha numeric charaters. Seperated with whitespace, each of them represents either a music note related data or a CFugue related command. The first character of a token usually determines if the token should be considered as a note token or a command token. 

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
    <tr><td class="indexkey">K </td> <td class="indexvalue">TOKEN_START_SIGNATURE  </td> <td class="indexvalue">Specifies a Signature token.</td></tr> 
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
</pre> The K[MELA] directive informs the parser to switch to Carnatic mode of parsing and to interpret the subsequent notes in that mode. For further discussion on this, please refer \ref subKeySignatures "Key Signatures".

A note can also be specified using its MIDI numeric value directly. Usually, when a note is specified using its name, such as C, D, E .. or S, R, G.., it is treated as a relative note. Its absolute position will be internally re-computed based on the Octave specifiers and the Scale/Raga settings later once the parsing is complete. On the otherhand, when a note is specified with its MIDI numeric value, it is treated as an absolute note and will not be affected by the current Scale or Octave settings. An example of such numeric note is:
<pre class="fragment">
    <span class="comment">// Play a Mid-C </span>
    player.Play("[60]");
</pre>
Observe that we need to include the numeric value with in square brackets []. Failing to do so will make the CFugue ignore the token. Below is the complete listing of MIDI numeric values for all the notes.
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
--Octave details go here..to be completed on..--

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

For example, in the above, when K[FMaj] is specified, the B note in the octave will be treated as B<i>b</i> automatically. Similarily, for K[Kalyani], the M note will be treated as M2 automatically. This makes it very convenient to change the Scale/Raga easily without modifying the notes.

When no KeySignature is specified, CFugue assumes these default Values: K[CMaj] for Western and K[Shankarabharanam] for Carnatic.

When a KeySignature is specified, it will be applied for all subsequent notes. However, if you do not want the KeySignature to be applied to any specific note, you have to either use the MIDI numeric value directly or use the note modifier <i>n</i> to declare it as a natural note. (Note modifiers are not applicable for Carnatic music. You have to explicitly specify the complete name, such as R1, R2 etc. to make it a natural note.) Natural notes are not affected by KeySignature. By definition, all MIDI numeric notes are natural notes. Thus, the below all five produce the same result.
<pre class="fragment">
    player.Play("K[CMaj] C D E F G A B"); <span class="comment">// Play an Octave in CMajor Scale </span>
    player.Play("K[FMaj] C D E F G A Bn"); <span class="comment">// B is declared as natural. FMaj is ignored for B</span>
    player.Play("K[Shankarabharanam] S R G M P D N"); <span class="comment">// Play an Octave in Shankarabharanam Ragam</span> 
    player.Play("K[Kalyani] S R G M1 P D N"); <span class="comment">// M1 declared explicitly. Kalyani is ignored for M</span> 
    player.Play("K[FMaj] C D E F G A [71]"); <span class="comment">// [71] is numeric note. FMaj is ignored for it</span>
</pre>

For details on how to interoperate between Western and Carnatic style of music, refer \ref secInteroperation.

\section secInteroperation Interoperating Western and Carnatic Music
By default CFugue starts parsing the notes in Western mode. However, 
*/

/////////////////////////////////////////////////////////////////
// Future page
/////////////////////////////////////////////////////////////////
/*! \page pageFuture CFugue Future
\section future The Future

With CFugue still in its infancy, the future of Music Note programming is still shining bright upon it.
The possibilities of what can be achieved through high-level MIDI note programming is endless. From 
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