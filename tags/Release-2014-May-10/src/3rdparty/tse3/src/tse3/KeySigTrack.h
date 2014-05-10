/*
 * @(#)KeySigTrack.h 3.00 30 May 2000
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

#ifndef TSE3_KEYSIGTRACK_H
#define TSE3_KEYSIGTRACK_H

#include "tse3/listen/KeySigTrack.h"

#include "tse3/Notifier.h"
#include "tse3/Playable.h"
#include "tse3/Serializable.h"
#include "tse3/EventTrack.h"

namespace TSE3
{
    /**
     * KeySig objects are values of key signature change that occur in
     * the @ref Song, held in the @ref KeySigTrack.
     *
     * This class defines the type that is used to create KeySig events,
     * which are of type @ref Event<@ref KeySig>.
     *
     * When streamed from a @ref Playable interface (using the
     * @ref KeySigTrackIterator class) KeySig objects are represented by a
     * @ref MidiEvent containing a @ref MidiCommand_TSE_Meta @ref MidiCommand
     * with data 1 byte as @ref MidiCommand_TSE_Meta_KeySig.
     *
     * KeySig is a value type.
     *
     * @short   A key signature change
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     KeySigTrack
     * @see     Event
     */
    class KeySig
    {
        public:

            /**
             * This enum type describes the number of incidentals in a key,
             * this will determine it as one of a pair of major/minor
             * keys. The @ref #KeySigType enum resolves exactly which key
             * is represented.
             *
             * Negative values for this type represent the number of flats in
             * the key. Positive values represent the number of sharps in the
             * key.
             *
             * Since the names are major ones (for all the difference that
             * makes) you can use the @ref #KeySigIncidentalsMinor to specify
             * minor names. Remember that this name means nothing, whether
             * the key is minor or major is determined by the @ref KeySigType
             * value alone.
             *
             * @see #KeySigType
             * @see #KeySigIncidentalsMinor
             */
            enum KeySigIncidentals
            {
                Cb  = -7, // Cb major, 7 flats
                Gb  = -6, // Gb major, 6 flats
                Db  = -5, // Dd major, 5 flats
                Ab  = -4, // Ab major, 4 flats
                Eb  = -3, // Eb major, 3 flats
                Bb  = -2, // Bb major, 2 flats
                F   = -1, // F major,  1 flat
                C   =  0, // C major,  0 sharps
                G   =  1, // G major,  1 sharp
                D   =  2, // D major,  2 sharps
                A   =  3, // A major,  3 sharps
                E   =  4, // E major,  4 sharps
                B   =  5, // B major,  5 sharps
                Fs  =  6, // F# major, 6 sharps
                Cs  =  7  // C# major, 7 sharps
            };

            /**
             * Minor version of the @ref KeySigIncidentals enum type.
             *
             * @see #KeySigIncidentals
             */
            enum KeySigIncidentalsMinor
            {
                Abm = -7, // Cb minor, 7 flats
                Fbbm= -6, // Fbb minor, 6 flats
                Bbm = -5, // Dd minor, 5 flats
                Gbm = -4, // Ab minor, 4 flats
                Cm  = -3, // Eb minor, 3 flats
                Dm  = -2, // Bb minor, 2 flats
                Fm  = -1, // F minor,  1 flat
                Am  =  0, // A minor,  0 sharps
                Em  =  1, // G minor,  1 sharp
                Bm  =  2, // D minor,  2 sharps
                Fsm =  3, // A minor,  3 sharps
                Csm =  4, // E minor,  4 sharps
                Gsm =  5, // B minor,  5 sharps
                Dsm =  6, // F# minor, 6 sharps
                Asm =  7  // C# minor, 7 sharps
            };

            /**
             * Represents whether a key is major or minor.
             *
             * Values are:
             * @li @p Major Represents a major key
             * @li @p Minor Represents a minor key
             *
             * @see #KeySigIncidentals
             */
            enum KeySigType
            {
                Major = 0, // Represents a major key
                Minor = 1  // Represents a minor key
            };

            /**
             * Create a KeySig.
             *
             * @param incidentals No sharps/flats in key
             * @param type        Whether it is a major/minor key
             */
            KeySig(int incidentals = C, int type = Major)
                : incidentals(incidentals), type(type) {}

            /**
             * The number of incidentals (sharps/flats) in this key signature.
             */
            int incidentals;

            /**
             * The type of this key signature (major/minor).
             */
            int type;

            int operator==(const KeySig &k) const
            {
                return (incidentals == k.incidentals) && (type == k.type);
            }
    };

    /**
     * The KeySigTrack provides a simple time ordered list of @ref KeySig
     * changes in the @ref Song.
     *
     * There is one KeySigTrack per @ref Song.
     *
     * @short   A list of KeySig events
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Event
     * @see     KeySig
     */
    class KeySigTrack : public EventTrack<KeySig>,
                        public Serializable
    {
        public:

            /**
             * Creates an empty KeySigTrack.
             */
            KeySigTrack();
            virtual ~KeySigTrack();

            /**
             * Returns the status of the KeySigTrack.
             *
             * A value of true means @ref KeySig events are generated, a value
             * of false means no @ref KeySig events are generated
             *
             * @return KeySigTrack status
             * @see setStatus
             */
            bool status() const { return _status; }

            /**
             * Set the KeySigTrack status value.
             *
             * @param s New KeySigTrack status
             * @see status
             */
            void setStatus(bool s) { _status = s; }

            /**
             * @reimplemented
             */
            virtual PlayableIterator *iterator(Clock index);

            /**
             * @reimplemented
             */
            virtual Clock lastClock() const;

            /**
             * @reimplemented
             */
            virtual void save(std::ostream &o, int i) const;

            /**
             * @reimplemented
             */
            virtual void load(std::istream &i, SerializableLoadInfo &info);

        private:

            KeySigTrack &operator=(const KeySigTrack &);
            KeySigTrack(const KeySigTrack &);

            /**
             * Used by load to read events
             */
            void loadEvents(std::istream &i, int filePPQN);

            bool _status;
    };
}

#endif
