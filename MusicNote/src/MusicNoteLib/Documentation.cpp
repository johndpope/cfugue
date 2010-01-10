/*
	This is part of CFugue, a C++ Runtime environment for MIDI Score Programming
	Copyright (C) 2009-2010 Gopalakrishna Palem

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/ //TODO: Convert this to LGPL

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

The elegance comes from the concept of <i>MusicString</i>, a string composed of Music Notes 
and MIDI instruction tokens. CFugue parses the MusicString and identifies the notes and MIDI
instructions represented in it. For each token identified, the Parser raises internal events 
that are handled by appropriate registered handlers. These handlers take the responsibility 
of generating music either on your MIDI output port or rendering it to a Music Sheet or 
MIDI file. CFugue allows these events, handlers and output formats be accessible through
simple methods as indicated in the following examples.

<b>Playing the Notes</b>
<pre class="fragment">
    \#include "MusicNoteLib.h"

    void main()
    {
        MusicNoteLib::Player player; <span class="comment">/ Create the Player Object</span>
        player.Play("C D E F G A B"); <span class="comment">// Play the Music Notes on the default MIDI output port</span>
    }
</pre>
And the Notes are not restricted to be of just Western sytle either. CFugue fully 
supports both Western and Carnatic Music notations - just with a hint from the KeySignature
directive, one should be able to switch between them seemlessly.
A Carnatic notation example playing all the seven notes of the default medium level pitch is shown below.
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
</pre>

\section theory Theory of Operation

The core feature of CFugue that makes it so elegant is the concept of Music Note Strings 
(or simply <i>MusicString</i>s, as referred to by JFugue). MusicStrings provide the ability to specify 
Music Notes and MIDI instructions as a sequence of characters (and tokens) in human readable form
as opposed to the usual complex binary data byte form, making it easy to learn and master.

Another powerful feature of CFugue is its ability to work with both Western and Carnatic music 
with same ease. For the first time in the Music world, it is now not only possible to specify 
Carnatic music Swaras side by side with Western Notes, it is also possible to transform between 
the two notations with same ease. (With few minor adjustments, Carnatic translation principles 
should be able to satisfy the needs of Hindustani Music enthusiasts also. In principle, there is
nothing that prevents this library from being used for Hindustani music side by side with 
Western music.)


\section limitations Limitations

Though the MusicString parsing is successfully working for both Western and Carnatic music notations, 
CFugue is still under development, with much of the features, such as rendering to sheet music, etc. 
pending. By the time the library completes the development and the project moves to the Released status, 
we hope to have this Limitations section to be empty.

CFugue aims to be platform independent. However, the primary development is happening on Win32 system
with enough precautions taken into view to make it portable. Yet, the real work of porting and testing
to ensure the full platform compliance is still pending.

\section download Download

CFugue is ready to be used in your code at the downloads section of http://sourceforge.net/projects/musicnote/

\section future The Future

With CFugue still in its infancy, the future of Music Note programming is still shining bright upon it.
The possibilities of what can be achieved through high-level MIDI note programming is endless. From 
automated Music analysis, recognition and generation to experimental cross genre Music research, all 
are going to benefit immesenly from this work. We have kept all the options open, to ensure we include
the new developments in the work as and when we discover the scope for improvements.

\section copyright Copyright

This is a product of CineFx Research Labs, made available free of charge for personal and research use. 
For commercial usage, please contact the author. 

CFugue is distributed with the hope that it will be useful.
No warranty of what-so-ever is implied, including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 Author: <a href="http://gpalem.web.officelive.com/index.html">Gopalakrishna Palem</a>

 Project: http://musicnote.sourceforge.net

 Copyright (C) 2009-2010 <a href="http://www.CineFxLabs.com">CineFx Digital Media Pvt Ltd.</a>

*/