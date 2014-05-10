/*
 * @(#)plt/midiswi.h 3.00 28 June 1995
 *
 * Copyright (c) 2000 Pete Goodliffe (pete@cthree.org)
 *
 * This file is part of TSE3 - the Trax Sequencer Engine version 3.00.
 *
 * This library is modifiable/redistributable under the terms of the GNU
 * General Public License.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING. If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

/*
 * These are SWI defintions used by the RISC OS MidiScheduler implementation.
 */

#ifndef __midiswis_h
#define __midiswis_h

#define MIDI_Sound_Enable              0x404c0
#define MIDI_SetMode                   0x404c1
#define MIDI_SetTxChannel              0x404c2
#define MIDI_SetTxActiveSensing        0x404c3
#define MIDI_InqSongPositionPointer    0x404c4
#define MIDI_InqBufferSize             0x404c5
#define MIDI_InqError                  0x404c6

#define MIDI_RxByte                    0x404c7
#define MIDI_RxCommand                 0x404c8

#define MIDI_TxByte                    0x404c9
#define MIDI_TxCommand                 0x404ca
#define MIDI_TxNoteOff                 0x404cb
#define MIDI_TxNoteOn                  0x404cc
#define MIDI_TxPolyKeyPressure         0x404cd
#define MIDI_TxControlChange           0x404ce
#define MIDI_TxLocalControl            0x404cf
#define MIDI_TxAllNotesOff             0x404d0
#define MIDI_TxOmniOff                 0x404d1
#define MIDI_TxOmniModeOn              0x404d2
#define MIDI_TxMonoModeOn              0x404d3
#define MIDI_TxPolyModeOn              0x404d4
#define MIDI_TxProgramChange           0x404d5
#define MIDI_TxChannelPressure         0x404d6
#define MIDI_TxPitchWheel              0x404d7
#define MIDI_TxSongPositionPointer     0x404d8
#define MIDI_TxSongSelect              0x404d9
#define MIDI_TxTuneRequest             0x404da
#define MIDI_TxStart                   0x404db
#define MIDI_TxContinue                0x404dc
#define MIDI_TxStop                    0x404dd
#define MIDI_TxSystemReset             0x404de

#define MIDI_IgnoreTiming              0x404df
#define MIDI_SynchSoundScheduler       0x404e0
#define MIDI_FastClock                 0x404e1
#define MIDI_Init                      0x404e2
#define MIDI_SetBufferSize             0x404e3
#define MIDI_Interface                 0x404e4

#endif
