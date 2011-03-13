/*
 * @(#)util/PowerQuantise.h 3.00 2 April 2000
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

#ifndef TSE3_UTIL_POWERQUANTISE_H
#define TSE3_UTIL_POWERQUANTISE_H

#include "tse3/Midi.h"

#include <vector>
#include <cstddef>

namespace TSE3
{
    class PhraseEdit;

    namespace Util
    {
        /**
         * This utility class provides you with more quantising capabilities
         * than you ever dreamt possible.
         *
         * The class is used by specifying the set of parameters to run the
         * PowerQuantise with, and then providing a (tidied) @ref PhraseEdit
         * object containing the source @ref MidiData to quantise. Once the
         * PowerQuantiser is run, the @ref PhraseEdit will contain tidied,
         * (powerfully) quantised @ref MidiData.
         *
         * Some of the facilities provided by PowerQuantise are:
         * @li Quantise to user-supplied pattern (or 'groove')
         * @li Shift events by percentage
         * @li Windowing
         * @li Only quantise selection
         * @li Only quantise note events
         * @li Spread continuous controllers intelligently between new note
         *     positions
         * @li Quantise notes forwards, backwards, or to the nearest snap point
         * @li Quantise note length or note off time
         * @li Humanisation
         *
         * @sect To be completed
         * This class isn't a Notifer yet, but it will be. I imagine that the
         * @ref PowerQuantise::Pattern class will also be Serializable so that
         * you can save a library of named patterns. Watch this space.
         *
         * @short   Staggeringly impressive Phrase quantisation
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     PhraseEdit
         * @see     PowerQuantise::Pattern
         */
        class PowerQuantise
        {
            public:

                /**
                 * Creates a PowerQuantise object with a sensible default
                 * set-up.
                 */
                PowerQuantise();

                /**
                 * This class represents the @ref PowerQuantise pattern, or
                 * 'groove'. It is basically a set of @ref Clock points which
                 * the @ref PowerQuantise should snap events to.
                 *
                 * @see PowerQuantise
                 */
                class Pattern
                {
                    public:

                        /**
                         * The default pattern is a 4/4 bar of crotchets.
                         */
                        Pattern();

                        /**
                         * Returns the 'length' of the pattern. This is the time
                         * it spans before it repeats again. (If the Pattern
                         * contains any points after the length they are
                         * ignored).
                         *
                         * @see setLength
                         */
                        TSE3::Clock length() const { return _length; }

                        /**
                         * Sets the Pattern length.
                         *
                         * @see length
                         */
                        void setLength(TSE3::Clock length);

                        /**
                         * Returns the number of points in this Pattern.
                         */
                        size_t size() const { return points.size(); }

                        /**
                         * Returns the nth @ref Clock in this Pattern object.
                         */
                        TSE3::Clock const &operator[](size_t n) const
                        {
                            return points[n];
                        }

                        /**
                         * Adds a @ref Clock point to the Pattern.
                         */
                        void insert(TSE3::Clock point);

                        /**
                         * Removes the nth @ref Clock from the Pattern.
                         */
                        void erase(size_t n);

                        /**
                         * Removes a @ref Clock point from the Pattern.
                         */
                        void erase(TSE3::Clock point);

                    private:

                        std::vector<TSE3::Clock> points;
                        TSE3::Clock              _length;
                };

                Pattern &pattern() { return _pattern; }

                /**
                 * Returns the percentage by which events will be quantised
                 * towards a snap point. 100% will move the command exactly to
                 * the snap point. 50% will move it half way towards it. 110%
                 * will move the event just past the snap point.
                 *
                 * This is a way of preventing the quantised @ref MidiData from
                 * sounding too mechanised.
                 *
                 * @see setBy
                 */
                int by() const { return _by; }

                /**
                 * Sets the percentage shift value.
                 *
                 * @see by
                 */
                void setBy(int by);

                /**
                 * This is a percentage value that controls which MIDI events
                 * get quantised. At 0% no events will be quantised. As the
                 * value increases, events near to (and then further away from)
                 * snaps points begin to get quantised. At 100% all MIDI events
                 * will be quantised.
                 *
                 * @see setWindow
                 */
                int window() const { return _window; }

                /**
                 * Sets the window value.
                 *
                 * @see setWindow
                 */
                void setWindow(int window);

                /**
                 * If true, this parameter forces PowerQuantise to only alter
                 * selected events. (This uses the selected bit in a
                 * @ref MidiCommand). If false, PowerQuantise alters all events
                 * (that pass other event filters).
                 *
                 * @see setOnlySelected
                 */
                bool onlySelected() const { return _onlySelected; }

                /**
                 * Sets the onlySelected value.
                 *
                 * @see onlySelected
                 */
                void setOnlySelected(bool s);

                /**
                 * If true, this parameter forces PowerQuantise to only alter
                 * note MIDI events. If false, PowerQuantise alters all events
                 * (that pass other event filters).
                 *
                 * @see setOnlyNotes
                 */
                bool onlyNotes() const { return _onlyNotes; }

                /**
                 * Sets the onlyNotes value.
                 *
                 * @see onlyNotes
                 */
                void setOnlyNotes(bool n);

                /**
                 * This is a very powerful feature that prevents continuous
                 * MIDI controller events (pitch bend, pan, reverb, etc) being
                 * pulled toward snap points as most sequencers will do. This
                 * tends to remove the continuous nature of the pattern of these
                 * events.
                 *
                 * Enabling this feature spreads the controllers between their
                 * surrounding note events after the quantisation. This spread
                 * will follow the same pattern that they originally were
                 * positoned in. This means that pitch bend slides, for
                 * example, will sound pretty much as they did prior to the
                 * quantise.
                 *
                 * @see setSpreadCtrl
                 */
                bool spreadCtrl() const { return _spreadCtrl; }

                /**
                 * Sets the spreadCtrl value.
                 *
                 * @see spreadCtrl
                 */
                void setSpreadCtrl(bool s);

                enum Direction
                {
                    nearest,
                    backwards,
                    forwards
                };

                /**
                 * This returns which snap point a MIDI event moves towards.
                 * This will normally be the nearest available snap point, but
                 * you can shoose to snap all events forwards or backwards.
                 *
                 * @see setDirection
                 */
                Direction direction() const { return _direction; }

                /**
                 * Sets the direction value.
                 *
                 * @see direction
                 */
                void setDirection(Direction d);

                enum Length
                {
                    quantiseOffTime,
                    quantiseLength
                };

                /**
                 * This returns what PowerQuantise does with
                 * MidiCommand_NoteOffs. You can choose to either snap the
                 * length of a MidiCommand_NoteOn to MidiCommand_NoteOff to the
                 * pattern, or quantise the actual time of the
                 * MidiCommand_NoteOff.
                 *
                 * You can also specify separate percentage shift and humanise
                 * values for note lengths.
                 *
                 * @see setDirection
                 */
                Length length() const { return _length; }

                /**
                 * Sets the length value.
                 *
                 * @see length
                 */
                void setLength(Length l);

                /**
                 * Returns the percentage by which note lengths are moved
                 * towards their quantised value.
                 *
                 * @see by
                 * @see setLengthBy
                 */
                int lengthBy() const { return _lengthBy; }

                /**
                 * Sets the percentage shift value.
                 *
                 * @see lengthBy
                 */
                void setLengthBy(int by);

                /**
                 * After an event is quantised, you can choose to have it moved
                 * by a random value (up to the a specified number of pulses
                 * which is returned by this function).
                 *
                 * This is an alternative way of preventing the quantised @ref
                 * MidiData sounding 'mechanised' by introducing slight
                 * human-like errors.
                 *
                 * A value of zero will disable the humanise facility.
                 *
                 * @see setHumanise
                 * @see lengthHumanzie
                 */
                TSE3::Clock humanise() const { return _humanise; }

                /**
                 * Sets the humanise value.
                 *
                 * @see humanise
                 */
                void setHumanise(TSE3::Clock h);

                /**
                 * This is a separate value of humanise which applies to
                 * @ref MidiCommand_NoteOff MIDI events.
                 *
                 * @see humanise
                 * @see setLengthHumanise
                 */
                TSE3::Clock lengthHumanise() const { return _lengthHumanise; }

                /**
                 * Sets the length humanise value.
                 *
                 * @see lengthHumanise
                 */
                void setLengthHumanise(TSE3::Clock h);

                /**
                 * Do the PowerQuantise on the given PhraseEdit.
                 */
                void go(TSE3::PhraseEdit *phraseEdit);

            private:

                /**
                 * Returns whether or not the given @ref MidiEvent should be
                 * quantised or not.
                 */
                bool shouldBeQuantised(TSE3::MidiEvent e)
                {
                    using namespace TSE3;
                    return (!_onlyNotes || e.data.status == MidiCommand_NoteOn)
                           && (!_onlySelected || e.data.selected)
                           && !shouldBeSpread(e);
                }

                /**
                 * Returns whether or not the given @ref MidiEvent should be
                 * spread or not.
                 */
                bool shouldBeSpread(TSE3::MidiEvent e)
                {
                    return (_spreadCtrl && isContinuous(e))
                           && (!_onlySelected || e.data.selected);
                }

                /**
                 * Returns whether or not the @ref MidiEvent is a continuous
                 * event.
                 */
                bool isContinuous(TSE3::MidiEvent e)
                {
                    using namespace TSE3;
                    bool b = false;
                    switch (e.data.status)
                    {
                        case MidiCommand_KeyPressure:
                        case MidiCommand_ControlChange:
                        case MidiCommand_ChannelPressure:
                        case MidiCommand_PitchBend:
                            b = true;
                    }
                    return b;
                }

                /**
                 * Quantise the specified time, according to the @ref Pattern
                 * and by the given percentage.
                 */
                TSE3::Clock quantise(TSE3::Clock time, int percentage);

                /**
                 * Humanise the specified time, with the specified maximum
                 * offset value.
                 */
                TSE3::Clock humanise(TSE3::Clock time,
                                     TSE3::Clock maxVal);

                /**
                 * Calculates the 'spread' time of the specified event in the
                 * PhraseEdit (a continuous controller). The events up to pos
                 * will have been quantised, and lastNonCont_Original contains
                 * the time of the last non-continuous event (prior to
                 * quantisation) and lastNonCont_Quantised conatins the
                 * quantised value of this event.
                 */
                TSE3::Clock spreadContinuous(
                    TSE3::PhraseEdit *phraseEdit,
                    size_t pos,
                    TSE3::Clock lastNonCont_Original,
                    TSE3::Clock lastNonCont_Quantised);

                Pattern         _pattern;
                int             _by;
                int             _window;
                bool            _onlySelected;
                bool            _onlyNotes;
                bool            _spreadCtrl;
                Direction       _direction;
                Length          _length;
                int             _lengthBy;
                TSE3::Clock     _humanise;
                TSE3::Clock     _lengthHumanise;
        };
    }
}

#endif
