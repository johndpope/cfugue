/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://cfugue.sourceforge.net/>.

    $LastChangedDate: 2013-12-18 10:59:57 +0530 (Wed, 18 Dec 2013) $
    $Rev: 197 $
    $LastChangedBy: krishnapg $
*/

#if defined WIN32 || defined _WIN32
#include <windows.h>
#include <MMSystem.h>
#else
#include <chrono>
#include <unistd.h>
#endif

#include "MIDITimer.h"

namespace CFugue
{
    unsigned long MidiTimer::CurrentTimeOffset()
    {
#if defined WIN32 || defined _WIN32
        return timeGetTime();
#else
        auto t0 = std::chrono::high_resolution_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(t0.time_since_epoch()).count();
#endif
    }

    void MidiTimer::Sleep(unsigned long milliSeconds)
    {
#if defined WIN32 || defined _WIN32
		Sleep( (DWORD) milliSeconds );
#else
		usleep( (unsigned long) (milliSeconds * 1000.0) );
#endif
    }

} // namespace CFugue
