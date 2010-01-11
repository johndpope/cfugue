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
a Player object and call the Play method on it supplying the Music notes to be played. Simple.
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {
        MusicNoteLib::Player player; <span class="comment">/ Create the Player Object</span>
        player.Play("C D E F G A B"); <span class="comment">// Play the Music Notes on the default MIDI output port</span>
    }
</pre>
And the music notes are not restricted to be of just Western sytle either. CFugue fully 
supports both Western and Carnatic Music notations - just with a hint from the KeySignature
directive, one should be able to switch between them seemlessly.

For more usage demonstrations, please refer to \ref pageExamples "CFugue examples".

\section features Features
CFugue is a C++ runtime environment for music note programming with below features:
	- Highlevel music programming library with direct support for C++ bindings and .Net P/Invoke calls
	- Provides COM interfaces to enable easy integration with ASP clients and COM compatible clients, such as Delphi, VB etc.
	- Easy to use notation. Writing music for CFugue is as easy as writing plain music notes.
	- Event subscription model allows easy extension of library to allow advanced features such as customized music renderers and parsers.
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
<b>Playing the Notes</b>
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {
        MusicNoteLib::Player player; <span class="comment">/ Create the Player Object</span>
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

<b>Saving Music Notes to MIDI output file</b>
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

        player.Play("ci di K[MELA_22] Pa M G R"); <span class="comment">// Play few Music Notes on MIDI output port</span>

        player.SaveToMidiFile("Output.mid"); <span class="comment">// Now, save the played content to a MIDI output file</span>
    }
</pre>

<b>Asynchronous Play</b>

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
            while(player.IsPlaying()) <span class="comment">// Wait while the play is still in progress</span>
                Sleep(1000);

        player.StopPlay(); <span class="comment">// Good practice to match each PlayAsync with a StopPlay</span>
    }
</pre>
At any time after the call to PlayAsync, the Player::IsPlaying() method can be used
to know if the play is still in progress. Player::StopPlay() allows you
to stop the play if it is still in progress. Calling StopPlay() after every PlayAsync()
call is a good practice (no matter if play is in progress or not). A typical usage of these methods
is demonstrated in the above code snippet.

<b>Accessing the Parser events</b>

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

    void OnParseTrace(const MusicNoteLib::CParser*, MusicNoteLib::CParser::TraceEventHandlerArgs* pEvArgs)
    {
        OutputDebugString(_T("\n"));
        OutputDebugString(pEvArgs->szTraceMsg);
    }

    void OnParseError(const MusicNoteLib::CParser*, MusicNoteLib::CParser::ErrorEventHandlerArgs* pEvArgs)
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

        player.Parser().evTrace.Subscribe(&OnParseTrace); <span class="comment">// Access the Player's Parser and Subscribe to its Trace Events</span>
        player.Parser().evError.Subscribe(&OnParseError); <span class="comment">// Access the Player's Parser and Subscribe to its Error Events</span>        

        player.SaveAsMidiFile(_T("Cq Dw Ex"), "MidiOutput.midi"); <span class="comment">// Parse the Notes and Save the content to a MIDI file</span>
     }
</pre>*/


/////////////////////////////////////////////////////////////////
// CFugue MusicString page
/////////////////////////////////////////////////////////////////
/*! \page pageMusicString CFugue MusicString

One of the capabilities of CFugue that makes it so elegant and easy to use, is the concept of Music Note Strings 
(or simply <i>MusicString</i>s, as referred to by JFugue). MusicStrings provide the ability to specify 
Music Notes and MIDI instructions as a sequence of characters (and tokens) in human readable form,
as opposed to the usual complex binary data byte form, making it easy to learn and master.

A powerful feature of CFugue's MusicString is its ability to work with both Western and Carnatic music 
with same ease. For the first time in the music world, it is now not only possible to specify 
Carnatic music Swaras side by side with Western Notes, it is also possible to transform between 
the two notations with same ease. 

\note In addition, the same principles can be adapted for Hindustani music also to make it work side by side with Western music.

In the following we discuss at length the details of what constitues a MusicString and how it can be defined for using with CFugue.

\section secMusicString Components of MusicString
In CFugue, a valid MusicString is formed out of a series of music tokens. Seperated with whitespace, each of them represents either a music note related data or a CFugue related command. The first character of a token usually determines if the token should be considered as a note token or a command token. 

Typical examples of note tokens are notes, chords, ties and so on, while MIDI instructions, such as instrument change, voice command etc. are few examples of CFugue related commands.

When CFugue parses a MusicString, it uses the first character of the tokens to identify the notes and commands. If no decision can be made about a token, CFugue will simply ignore the token and moves on. This is unlike any typical parser behavior, where an unrecognized token will halt the parsing.

Below are the first characters and their associated token classifications for CFugue MusicStrings:
<table>    
    <tr><td>Character</td> <td>API Constant</td>  <td>Significance</td>  </tr>
    <tr><td>V </td> <td>TOKEN_START_VOICE  </td> <td>Specifies a Voice token.</td></tr> 
    <tr><td>T </td> <td>TOKEN_START_TEMPO  </td> <td>Specifies a Tempo token.</td></tr>
    <tr><td>I </td> <td>TOKEN_START_INSTRUMENT  </td> <td>Specifies a Instrument token.</td></tr> 
    <tr><td>L </td> <td>TOKEN_START_LAYER  </td> <td>Specifies a Layer token.</td></tr> 
    <tr><td>K </td> <td>TOKEN_START_SIGNATURE  </td> <td>Specifies a Signature token.</td></tr> 
    <tr><td>X </td> <td>TOKEN_START_CONTROLLER  </td> <td>Specifies a Controller token.</td></tr> 
    <tr><td>@ </td> <td>TOKEN_START_TIME  </td> <td>Specifies a Time token.</td></tr> 
    <tr><td>* </td> <td>TOKEN_START_KEYPRESSURE  </td> <td>Specifies a Keypressure token.</td></tr>  
    <tr><td>+ </td> <td>TOKEN_START_CHANNELPRESSURE  </td> <td>Specifies a Channel pressure token.</td></tr> 
    <tr><td>& </td> <td>TOKEN_START_PITCHBEND  </td> <td>Specifies a Pitchbend token. </td></tr> 
    <tr><td>| </td> <td>TOKEN_START_MEASURE  </td> <td>Specifies a Measure token.</td></tr> 
    <tr><td>$ </td> <td>TOKEN_START_DICTIONARY  </td> <td>Specifies a Dictionary token.</td></tr><!--     <tr><td>[ </td> <td>TOKEN_START_NOTE  </td> <td>Specifies a numeric Note token.</td></tr>  -->
</table>
When a token starts with a different character than any of the above, CFugue tries to read the token as a note token. If it succeeds in that, it will generate note events for further processing. And if it fails, it will ignore the token and moves on.

CFugue MusicStrings are case-insensitive. That is, the above characters, and any character for that matter, can be used in any case for the MusicStrings. CFugue will internally convert all the MusicStrings to upper case before processing them. Use this fact to your advantage by mixing the case for your MusicString tokens and making them more readable and easy to understand. Examples of few valid MusicStrings that showcase this advantage are:
<pre>
    MusicNoteLib::Player player;
    player.Play("C");
    player.Play("FMaj");
    player.Play("Cq+Eh+Gq");
    player.Play("C6q+E4h+G8q D5Maj7w");
    player.Play("T[120] V0 I[Flute] Cs B4s A4i B4i C");
    player.Play("I[XyloPhone] K[Mela_29] S R G M P D N S'");
</pre>
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
