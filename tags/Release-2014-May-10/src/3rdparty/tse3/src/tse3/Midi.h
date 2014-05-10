/*
 * @(#)Midi.h 3.00 14 May 1999
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

#ifndef TSE3_MIDI_H
#define TSE3_MIDI_H

namespace TSE3
{
    /**
     * The Clock class describes a time at which an @ref MidiCommand or
     * other @ref Event may be transmitted/recieved. It is used primarily by
     * the @ref MidiEvent class.
     *
     * The time is a signed integer value of pulses, where a pulse is
     * a division of a quarter note (crotchet) as defined by the
     * @ref Clock::PPQN variable.
     *
     * This is a value type.
     *
     * @short   Time of a @ref MidiEvent
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiEvent
     * @see     PPQN
     */
    struct Clock
    {
        /**
         * PPQN is the number of Pulses Per Quarter Note.
         */
        static const int PPQN = 96;

        /**
         * Construct a Clock with the given number of Pulses.
         *
         * @param pulses Number of pulses.
         */
        Clock(int pulses = 0) : pulses(pulses)   {}
        Clock(const Clock &m) : pulses(m.pulses) {}

        /**
         * Convenience method to obtain the number of whole quarter notes.
         *
         * @return Number of whole quarter notes.
         * @see    pulse
         */
        int beat() const { return pulses / PPQN; }

        /**
         * Convenince method to obtain the number of pulses not
         * part of a whole quarter note.
         *
         * @return Number of remaining pulses.
         * @see    beat
         */
        int pulse() const { return pulses % PPQN; }

        /**
         * The number of pulses of this Clock.
         */
        int pulses;

        /**
         * Convert a time from one PPQN resolution to this one.
         *
         * @param  time      Clock in other PPQN resolution
         * @param  otherPPQN The other PPQN resolution
         * @return The Clock value in this PPQN resolution
         */
        static Clock convert(Clock time, int otherPPQN)
        {
            return (time * Clock::PPQN) / otherPPQN;
        }

        /*
         * Dull stuff to make Clock behave like the ints do.
         */

        operator int() const { return pulses; }

        const Clock operator+(const Clock &c)
        {
            Clock ret;
            ret.pulses = pulses + c.pulses;
            return ret;
        }
        const Clock operator+(int i)
        {
            Clock ret;
            ret.pulses = pulses + i;
            return ret;
        }
        Clock operator+=(const Clock &c)
        {
            pulses += c.pulses;
            return *this;
        }
        const Clock operator-(const Clock &c)
        {
            Clock ret;
            ret.pulses = pulses - c.pulses;
            return ret;
        }
        const Clock operator-(int i)
        {
            Clock ret;
            ret.pulses = pulses - i;
            return ret;
        }
        Clock operator-=(const Clock &c)
        {
            pulses -= c.pulses;
            return *this;
        }
        const Clock operator*(const Clock &c)
        {
            Clock ret;
            ret.pulses = pulses * c.pulses;
            return ret;
        }
        const Clock operator*(int i)
        {
            Clock ret;
            ret.pulses = pulses * i;
            return ret;
        }
        Clock operator*=(const Clock &c)
        {
            pulses *= c.pulses;
            return *this;
        }
        Clock operator*=(int i)
        {
            pulses *= i;
            return *this;
        }
        const Clock operator/(const Clock &c)
        {
            Clock ret;
            ret.pulses = pulses / c.pulses;
            return ret;
        }
        const Clock operator/(int i)
        {
            Clock ret;
            ret.pulses = pulses / i;
            return ret;
        }
        Clock operator/=(const Clock &c)
        {
            pulses /= c.pulses;
            return *this;
        }
        Clock operator/=(int i)
        {
            pulses /= i;
            return *this;
        }
        const Clock operator%(const Clock &c)
        {
            Clock ret;
            ret.pulses = pulses % c.pulses;
            return ret;
        }
        const Clock operator%(int i)
        {
            Clock ret;
            ret.pulses = pulses % i;
            return ret;
        }
        Clock operator%=(const Clock &c)
        {
            pulses %= c.pulses;
            return *this;
        }
        Clock operator%=(int i)
        {
            pulses %= i;
            return *this;
        }
    };

    /**
     * An Event is a generic class that can schedule something to a
     * @ref Clock time.
     *
     * It's comparison operators only compare times, NOT the data contents
     * as well.
     *
     * Note that the @ref MidiEvent class does not use this as a base class
     * since it is a 'special case'.
     *
     * This is a value type.
     *
     * @short   A scheduled event
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Midievent
     * @see     Clock
     */
    template <class etype>
    struct Event
    {
        typedef etype event_type;

        /**
         * Creates an Event with the given data and @ref Clock.
         *
         * @param data The event data (of template parameter type)
         * @param time The event time
         */
        Event(etype data, Clock time) : data(data), time(time) {}

        /**
         * This event's data.
         */
        etype data;

        /**
         * This event's @ref Clock.
         */
        Clock time;

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator<(const Event<etype> &e) const
        {
            return time.pulses < e.time.pulses;
        }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator<=(const Event<etype> &e) const
        {
            return time.pulses <= e.time.pulses;
        }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator>(const Event<etype> &e) const
        {
            return time.pulses > e.time.pulses;
        }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator>=(const Event<etype> &e) const
        {
            return time.pulses >= e.time.pulses;
        }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator==(const Event<etype> &e) const
        {
            return time.pulses == e.time.pulses;
        }

        /**
         * Compares two Events for equality, checking both the times and
         * the data.
         */
        bool equals(const Event<etype> &e) const
        {
            return (*this == e) && data == e.data;
        }

        /**
         * This functor returns the value of equals for the @ref Event
         * class.
         *
         * @see equals
         */
        class equal_to
        {
            public:
                equal_to(const Event<etype> &e) : e1(e) {}
                bool operator()(const Event<etype> &e2) const
                {
                    return e1.equals(e2);
                }
            private:
                const Event<etype> &e1;
        };
    };


    /**************************************************************************
     * A selection of useful MIDI command definitions.
     *************************************************************************/

    /**
     * This enum type describes the top four bits of a MIDI command status byte
     * as defined by the MIDI standard.
     *
     * The values should be passed as 'status' to a @ref MidiCommand object.
     *
     * Values in the range 0x8 - 0xf are defined by the MIDI standard
     * and will be transmitted to MIDI devices. Values in the range 0x0 - 0x7
     * are internal TSE3 'meta' status bytes which are used to encode extra
     * information within @ref MidiData objects.
     *
     * The 'channel messages' which are addressed to one of 16 specific
     * MIDI channels are:
     * @li @p MidiCommand_NoteOff          - Switch a note off
     * @li @p MidiCommand_NoteOn           - Switch a note on
     * @li @p MidiCommand_KeyPressure      - Change velocity of a note
     * @li @p MidiCommand_ControlChange    - Send a controller value
     * @li @p MidiCommand_ProgramChange    - Change voice
     * @li @p MidiCommand_ChannelPressure  - Chnage pressure of entire channel
     * @li @p MidiCommand_PitchBend        - Change note pitch of entire channel
     * @li @p MidiCommand_System           - Special system command
     *                                       (see @ref MidiSystemCommands)
     *
     * Current meta events are:
     * @li @p MidiCommand_Invalid          - A 'nothing' command. These
     *                                       progate around the system quite a
     *                                       lot and so you must be prepared
     *                                       to see (and ignore) these
     *                                       commands.
     * @li @p MidiCommand_TSE_Meta         - A TSE3 library defined meta event.
     *                                       See the @ref TSEMetaMidiCommands.
     *                                       These include tempo and timesig.
     * @li @p MidiCommand_NoteEdit_Meta    - A range of metas reserved for the
     *                                       NoteEdit program.
     */
    enum MidiCommands
    {
        // Channel messages
        MidiCommand_NoteOff             = 0x8,
        MidiCommand_NoteOn              = 0x9,
        MidiCommand_KeyPressure         = 0xa,
        MidiCommand_ControlChange       = 0xb,
        MidiCommand_ProgramChange       = 0xc,
        MidiCommand_ChannelPressure     = 0xd,
        MidiCommand_PitchBend           = 0xe,
        MidiCommand_System              = 0xf,

        // Meta MIDI messages
        MidiCommand_Invalid             = 0x0,
        MidiCommand_TSE_Meta            = 0x1,
        MidiCommand_NoteEdit_Meta       = 0x2
    };

    /**
     * This enum type defines the data 1 bytes for meta MidiCommands with
     * the status byte @ref MidiCommand_TSE_Meta. The data 2 byte will contain
     * some data related to this command.
     *
     * The defined bytes are:
     * @li @p MidiCommand_TSE_Meta_Tempo   - Data 2 holds a new tempo value
     *                                       See @ref Tempo.
     * @li @p MidiCommand_TSE_Meta_TimeSig - Data 2 is 0xAB. 'A' has the time
     *                                       sig top value, 'B' has the time
     *                                       sig bottom value.
     *                                       See @ref TimeSig.
     * @li @p MidiCommand_TSE_Meta_KeySig  - Data 2 is 0xAB. 'A' has the
     *                                       incidentals values, 'B' has the
     *                                       minor/major value.
     *                                       See @ref KeySig.
     * @li @p MidiCommand_TSE_Meta_MoveTo  - Data 2 is ignored, and the
     *                                       second event in the @ref MidiEvent
     *                                       holds the time to move to. This
     *                                       is generated by the
     *                                       @ref SongIterator.
     */
    enum TSEMetaMidiCommands
    {
        MidiCommand_TSE_Meta_Tempo      = 0x00,
        MidiCommand_TSE_Meta_TimeSig    = 0x01,
        MidiCommand_TSE_Meta_KeySig     = 0x02,
        MidiCommand_TSE_Meta_MoveTo     = 0x03
    };

    /**
     * Look up table for the number of data bytes following a MidiCommand.
     *
     * <b>Note:</b> You should prefer to use the @ref MidiCommand member
     * function noDataBytes() where possible.
     */
    extern unsigned int MidiCommand_NoDataBytes[];

    /**
     * This enum type describes the @ref MidiCommand_System messages
     * as defined by the MIDI standard. If the @ref MidiCommand status
     * byte is @ref MidiCommand_System, then these values will be found in
     * the 'channel' data bits.
     */
    enum MidiSystemCommands
    {
        MidiSystem_SysExStart           = 0x0,
        MidiSystem_MidiTimeCode         = 0x1,
        MidiSystem_SongPosition         = 0x2,
        MidiSystem_SongSelect           = 0x3,
        MidiSystem_TuneRequest          = 0x6,
        MidiSystem_SysExEnd             = 0x7,
        MidiSystem_TimingClock          = 0x8,
        MidiSystem_Start                = 0xa,
        MidiSystem_Continue             = 0xb,
        MidiSystem_Stop                 = 0xc,
        MidiSystem_ActiveSensing        = 0xe,
        MidiSystem_SystemReset          = 0xf
    };

    /**
     * This enum type defines data 1 values for the @ref MidiCommand
     * @ref MidiCommand_ControlChange. The data 2 value will be a number
     * related to this control change type.
     *
     * Some controllers are continuous values in the range 0-127. Some act
     * as on off switches, where a value of 63 or below is 'off' and 64-127
     * is 'on'.
     */
    enum MidiControlChanges
    {
        MidiControl_BankSelectMSB       = 0x00,
        MidiControl_ModulationMSB       = 0x01,
        MidiControl_BreathControllerMSB = 0x02,
        MidiControl_FootController      = 0x04,
        MidiControl_PortamentoTimeMSB   = 0x05,
        MidiControl_DataEntryMSB        = 0x06,
        MidiControl_ChannelVolumeMSB    = 0x07,
        MidiControl_BalanceMSB          = 0x08,
        MidiControl_PanMSB              = 0x0a,
        MidiControl_ExpressionCtrlMSB   = 0x0b,
        MidiControl_EffectCtrl1MSB      = 0x0c,
        MidiControl_EffectCtrl2MSB      = 0x0d,
        MidiControl_GeneralCtrl1MSB     = 0x10,
        MidiControl_GeneralCtrl2MSB     = 0x11,
        MidiControl_GeneralCtrl3MSB     = 0x12,
        MidiControl_GeneralCtrl4MSB     = 0x13,

        MidiControl_BankSelectLSB       = 0x20,
        MidiControl_ModulationLSB       = 0x21,
        MidiControl_BreathControllerLSB = 0x22,
        MidiControl_FootControlLer      = 0x24,
        MidiControl_PortamentoTimeLSB   = 0x25,
        MidiControl_DataEntryLSB        = 0x26,
        MidiControl_ChannelVolumeLSB    = 0x27,
        MidiControl_BalanceLSB          = 0x28,
        MidiControl_PanLSB              = 0x2a,
        MidiControl_ExpressionCtrlLSB   = 0x2b,
        MidiControl_EffectCtrl1LSB      = 0x2c,
        MidiControl_EffectCtrl2LSB      = 0x2d,
        MidiControl_GeneralCtrl1LSB     = 0x30,
        MidiControl_GeneralCtrl2LSB     = 0x31,
        MidiControl_GeneralCtrl3LSB     = 0x32,
        MidiControl_GeneralCtrl4LSB     = 0x33,

        MidiControl_SustainPedal        = 0x40,
        MidiControl_Portamento          = 0x41,
        MidiControl_Sostenuto           = 0x42,
        MidiControl_SoftPedal           = 0x43,
        MidiControl_Legato              = 0x44,
        MidiControl_Hold2               = 0x45,

        MidiControl_SoundVariationLSB   = 0x46,
        MidiControl_Timbre              = 0x47,
        MidiControl_ReleaseTime         = 0x48,
        MidiControl_AttackTime          = 0x49,
        MidiControl_Brightness          = 0x4a,

        MidiControl_GeneralCtrl5        = 0x50,
        MidiControl_GeneralCtrl6        = 0x51,
        MidiControl_GeneralCtrl7        = 0x52,
        MidiControl_GeneralCtrl8        = 0x53,
        MidiControl_PortamentoSource    = 0x54,
        MidiControl_ReverbDepth         = 0x5b,
        MidiControl_TremoloDepth        = 0x5c,
        MidiControl_ChorusDepth         = 0x5d,
        MidiControl_CelesteDepth        = 0x5e,
        MidiControl_PhaserDepth         = 0x5f,
        MidiControl_DataIncrement       = 0x60,
        MidiControl_DataDecrement       = 0x61,
        MidiControl_NRPM_LSB            = 0x62,
        MidiControl_NRPM_MSB            = 0x63,
        MidiControl_RPM_LSB             = 0x64,
        MidiControl_RPM_MSB             = 0x65,

        MidiControl_AllSoundOff         = 0x78,
        MidiControl_ResetAllControllers = 0x79,
        MidiControl_LocalControl        = 0x7a,
        MidiControl_AllNotesOff         = 0x7b,
        MidiControl_OmniModeOff         = 0x7c,
        MidiControl_OmniModeOn          = 0x7d,
        MidiControl_MonoModeOn          = 0x7e,
        MidiControl_PolyModeOn          = 0x7f
    };

    /**
     * The MidiCommand describes one complete MIDI message, including
     * status byte and any data associated with it.
     *
     * This is a value type.
     *
     * @short   Data type of a single complete MIDI message
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     MidiEvent
     */
    struct MidiCommand
    {
        /**
         * Construct a MidiCommmand with two data bytes
         *
         * @param status  Status value (4 bits).
         * @param channel Channel value (0-15).
         * @param port    Port value.
         * @param data1   First data byte value (0-127).
         * @param data2   Second data byte value (0-127).
         */
        MidiCommand(int status, int channel, int port,
                    int data1, int data2)
            : port(port), status(status), channel(channel),
              data1(data1), data2(data2), selected(0) {}

        /**
         * Construct a MidiCommmand with one data byte
         *
         * @param status  Status value (4 bits).
         * @param channel Channel value (0-15).
         * @param port    Port value.
         * @param data1   Data byte value (0-127).
         */
        MidiCommand(int status, int channel, int port,
                    int data1)
            : port(port), status(status), channel(channel),
              data1(data1), data2(0), selected(0) {}

        /**
         * Construct a MidiCommand with status MidiCommand_Invalid.
         */
        MidiCommand()
            : port(0), status(MidiCommand_Invalid), channel(0),
              data1(0), data2(0), selected(0) {}

        /**
         * Returns the number of data bytes this type of MidiCommand has.
         *
         * @return The value 1 or 2
         */
        int noDataBytes() const
        {
            return MidiCommand_NoDataBytes[status << 4];
        }

        /**
         * Returns whether this is a 'channel' MIDI command or not
         *
         * @return Whether this is a 'channel' MIDI command
         */
        bool isChannel() const
        {
            return (status >= MidiCommand_NoteOn
                    && status <= MidiCommand_PitchBend);
        }

        /**
         * Returns whether the data1 value is defined to be a note number.
         *
         * @return Whether data1 contains a note value
         */
        bool isNote() const
        {
            return (status >= MidiCommand_NoteOff
                    && status <= MidiCommand_KeyPressure);
        }

        /**
         * The port value. Valid port numbers are positive integers.
         * There are some defined minus numbers that have special
         * meanings, too.
         */
        int port;

        /**
         * The status (type of MIDI message identifier) value (4 bits).
         */
        unsigned status   : 4;

        /**
         * The channel value (0-15, with some magic minus numbers).
         */
        signed channel  : 5;

        /**
         * The data1 value (0-127).
         */
        unsigned data1    : 8;

        /**
         * The data2 value (0-127).
         */
        unsigned data2    : 8;

        /**
         * Whether this command is 'selected' or not (0/1). This may be used
         * by some sort of Phrase editor.
         * @see Phrase
         */
        unsigned selected : 1;

        /**
         * These are some "reserved" port numbers. Valid port numbers
         * are positive integers. Negative integers have specific meanings.
         */
        enum MagicPortNumbers
        {
            NoPort   = -1,
            AllPorts = -2,
            SamePort = -3
        };

        /**
         * These are some "reserved" channel numbers. Valid channel numbers
         * are 0-15. Negative integers have specific meanings.
         */
        enum MagicChannelNumbers
        {
            NoChannel   = -1,
            AllChannels = -2,
            SameChannel = -3
        };

        /**
         * MidiCommands are considered equal if all data members except
         * selected match.
         */
        int operator==(const MidiCommand &c) const
        {
            return channel == c.channel
                   && status == c.status
                   && data1  == c.data1
                   && data2  == c.data2
                   && port   == c.port;
        }
    };


    /**
     * The MidiEvent is a special kind of event, since any
     * @ref MidiCommand_NoteOn that is sent must be ballanced by a matching
     * @ref MidiCommand_NoteOff to prevent 'hanging notes'.
     *
     * To facilitate this, this class wraps two MidiCommand events together.
     * The second is only used if the first is a MidiCommand_NoteOn and will
     * always be a ballancing MidiCommand_NoteOff.
     *
     * (The MidiEvent class contents are not @ref Event<@ref MidiCommand>s
     * because the syntax becomes cumbersome.)
     *
     * When you create a MidiEvent which contains a @ref MidiCommand_NoteOn you
     * will generally always want to fill in the second @ref MidiCommand's
     * details to be the matching @ref MidiCommand_NoteOff. You will only
     * not honour this in the case where there is no relevant matching note
     * off. This is explained in more detail in the documentation for the
     * @ref PhraseEdit class.
     *
     * @short   A MidiCommand with associated Clock scheduling time
     *          and possible ballancing MidiCommand_NoteOff.
     * @author  Pete Goodliffe
     * @version 3.01
     * @see     MidiCommand
     */
    struct MidiEvent
    {
        MidiEvent()
        : data(MidiCommand()), time(0), offData(MidiCommand()), offTime(0) {}

        /**
         * Creates a MidiEvent with the given MidiCommand and Clock.
         * This should be used for non-note events.
         *
         * @param mc @ref MidiCommand for this event; should not be a
         *           @ref MidiCommand_NoteOn
         * @param t  Time of this event
         */
        MidiEvent(MidiCommand mc, Clock t)
        : data(mc), time(t), offData(MidiCommand()), offTime(0) {}

        /**
         * Creates a MidiEvent for a note MidiCommand. Takes the MIDI note
         * on MidiCommand and the related note off MidiCommand.
         *
         * @param mc @ref MidiCommand for this event; should be a
         *           @ref MidiCommand_NoteOn
         * @param t  Time of this event
         * @param oc @ref MidiCommand for the matching note off event; should
         *           be a @ref MidiCommand_NoteOff
         * @param ot Time of the matching MidiCommand_NoteOff
         */
        MidiEvent(MidiCommand mc, Clock t, MidiCommand oc, Clock ot)
        : data(mc), time(t), offData(oc), offTime(ot) {}

        /**
         * Creates a MidiEvent for a note MidiCommand. Takes the MIDI note
         * on MidiCommand and the related off velocity of the note off
         * MidiCommand. The note off MidiCommand is calculated from these
         * values.
         *
         * @param mc     @ref MidiCommand for this event; should be a
         *               @ref MidiCommand_NoteOn
         * @param t      Time of this event
         * @param offVel The velocity of the matching note off.
         * @param ot     Time of the matching MidiCommand_NoteOff
         */
        MidiEvent(MidiCommand mc, Clock t, int offVel, Clock ot)
        : data(mc), time(t), offData(mc), offTime(ot)
        {
            offData.status = MidiCommand_NoteOff;
            offData.data2 = offVel;
        }

        /**
         * The event's MIDI command
         */
        MidiCommand data;

        /**
         * The event's @ref Clock time.
         */
        Clock time;

        /**
         * If @ref data is a @ref MidiCommand_NoteOn, this holds the
         * matching @ref MidiCommand_NoteOff that ballances it.
         */
        MidiCommand offData;

        /**
         * The time of the matching @ref MidiCommand_NoteOff (if there is
         * one).
         */
        Clock offTime;

        /*
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator<(const MidiEvent &e) const { return time < e.time; }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator<=(const MidiEvent &e) const { return time <= e.time; }

        /*
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator>(const MidiEvent &e) const { return time > e.time; }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator>=(const MidiEvent &e) const { return time >= e.time; }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator==(const MidiEvent &e) const { return time == e.time; }

        /**
         * This comparison operator only compares event times, NOT the data
         * contents.
         */
        int operator!=(const MidiEvent &e) const { return time != e.time; }

        /**
         * Compares two Events for equality, checking both the times and
         * the data.
         */
        bool equals(const MidiEvent &e) const
        {
            return (time == e.time) && (data == e.data);
            // XXX: what about MidiCommand_NoteOffs ???
        }

        /**
         * This functor returns the value of operator==.
         *
         * @see equals
         */
        class equal_to
        {
            public:
                equal_to(const MidiEvent &e) : e1(e) {}
                bool operator()(const MidiEvent &e2) const
                {
                    return e1.equals(e2);
                }
            private:
                const MidiEvent &e1;
        };
    };
}

#endif
