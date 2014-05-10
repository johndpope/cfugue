/*
 * @(#)ins/Instrument.h 3.00 23 August 1999
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

#ifndef TSE3_INS_INSTRUMENT_H
#define TSE3_INS_INSTRUMENT_H

#include <list>
#include <utility>
#include <vector>
#include <string>
#include <iosfwd>

namespace TSE3
{
    class Progress;

    /**
     * The @p Ins namespace contains classes that implement the Cakewalk
     * instrument file parsing routines and provide name lookup for bank/patch
     * and controller numbers.
     *
     * The facilities offered here are utilities that an application may use,
     * and are not part of the core @ref TSE3 functionality.
     *
     * The @ref TSE3::Ins::CakewalkInstrumentFile class is the main entrance
     * onto @p Ins facilities.
     *
     * You can get Cakewalk instrument definition files for practically
     * every piece of MIDI hardware in existence, which is why they have been
     * adopted by the TSE3 library. They are most easily obtained from the
     * @p www.cakewalk.com website; follow the "Download" link and select
     * "Instrument Definitions".
     *
     * @short   Utility classes for MIDI instrument definitions
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     TSE3
     */
    namespace Ins
    {
        class PatchData;
        class ControlData;
        class RpnData;
        class NrpnData;
        class NoteData;

        /**
         * Bank select values can be expressed as single 14 bit numbers
         * (Cakewalk instrument files use this format) or as separate LSB and
         * MSBs (the MIDI format uses this format).
         *
         * This function converts a 14 bit bank number into the LSB portion.
         * If @p bank is -1, returns -1.
         *
         * @param  bank 14 bit bank select number
         * @return LSB value
         * @see    bankMSB
         * @see    bankFromBytes
         */
        inline int bankToLSB(int bank)
        {
            return (bank < 0) ? bank : bank & 0x7f;
        }

        /**
         * Bank select values can be expressed as single 14 bit numbers
         * (Cakewalk instrument files use this format) or as separate LSB and
         * MSBs (the MIDI format uses this format).
         *
         * This function converts a 14 bit bank number into the MSB portion.
         * If @p bank is -1, returns -1.
         *
         * @param  bank 14 bit bank select number
         * @return MSB value
         * @see    bankLSB
         * @see    bankFromBytes
         */
        inline int bankToMSB(int bank)
        {
            return (bank < 0) ? bank : bank >> 7;
        }

        /**
         * Bank select values can be expressed as single 14 bit numbers
         * (Cakewalk instrument files use this format) or as separate LSB and
         * MSBs (the MIDI format uses this format).
         *
         * This function converts a bank LSB and MSB into a 14 bit bank number.
         * If @p bank is -1, returns -1.
         *
         * @param  bankLSB Bank LSB value
         * @param  bankMSB Bank MSB value
         * @return bank 14 bit bank select number
         * @see    bankLSB
         * @see    bankMSB
         */
        inline int bankFromBytes(int bankLSB, int bankMSB)
        {
            return (bankLSB < 0 || bankMSB < 0) ? -1 : (bankMSB<<7) | bankLSB;
        }

        /**
         * A Voice struct holds information about a voice - the bank and patch
         * values. It is based on pair<int,int> where the first in is the
         * bank value and the second int is the patch value.
         *
         * Bank values are defined to be (MSB<<7)+LSB.
         *
         * The value -1 denotes a wildcard - it matches any bank/patch.
         *
         * This is a value type.
         *
         * @short   Instrument voice definition
         * @author  Pete Goodliffe
         * @version 3.00
         */
        struct Voice : public std::pair<int, int>
        {
            /**
             * Creates a Voice with the given bank and patch values.
             *
             * @param bank  New bank value in the form (MSB<<7)+LSB
             * @param patch New patch value
             */
            Voice(int bank, int patch);

            /**
             * Creates a Voice with the given bank and patch values.
             *
             * @param bankMSB Bank select MSB
             * @param bankLSB Bank select LSB
             * @param patch   New patch value
             */
            Voice(int bankMSB, int bankLSB, int patch);

            /**
             * Returns the bank value in the form (bankMSB<<7)+bankLSB.
             */
            int bank() const { return first; }

            /**
             * Returns the bank MSB value.
             */
            int bankMSB()  const { return first >> 7; }

            /**
             * Returns the bank LSB value.
             */
            int bankLSB()  const { return first & 0x7f; }

            /**
             * Returns the patch value.
             */
            int patch() const { return second; }

            /**
             * Comparison operator. Compares banks first, then patches.
             */
            int operator <(const Voice &v) const;
        };

        /**
         * The Instrument class holds information about a specific MIDI
         * instrument. This includes the voices it provides, control commands
         * it understands, drum note names and so on.
         *
         * The Instrument class is based on the instrument definitions supplied
         * in Cakewalk .ins instrument definition files.
         *
         * @short   MIDI Instrument definition
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class Instrument
        {
            public:

                /**
                 * Creates an instrument with the given name from information
                 * contained in the given file. This file will be a
                 * Cakewalk .ins file.
                 *
                 * Whilst the file is being loaded, the progess can be reported
                 * via the @ref TSE3::Progess interface.
                 *
                 * @param title   The title of this instrument
                 * @param file    The file to take input from
                 * @param progess @ref TSE3::Progress callback, or zero for
                 *                no callback
                 */
                Instrument(const std::string &title,
                           const std::string &filename,
                           TSE3::Progress    *progess = 0);

                /**
                 * Returns the title of this instrument.
                 *
                 * @return Title of this instrument
                 */
                const std::string &title() const { return _title; }

                /**
                 * Returns the filename of the source of the instrument
                 * definition.
                 *
                 * @return Filename of this instrument's definition
                 */
                const std::string &filename() const { return _filename; }

                /**
                 * Sets the title.
                 *
                 * @param title New instrument title
                 */
                void setTitle(const std::string &title);

                /**
                 * Returns the BankSelMethod, the values for this are defined
                 * as the BankSelMethod_XXX constants.
                 *
                 * @return Bank select method
                 */
                int bankSelMethod() const { return _bankSelMethod; }

                /**
                 * Sets the BankSelMethod.
                 *
                 * @param b New bank select method
                 */
                void setBankSelMethod(int b);

                /**
                 * An enum type defining the Instrument's bank select method.
                 * It has the following values
                 * @li @p BankSelMethod_Normal
                 *     For normal instruments: uses LSB, MSB and patch.
                 * @li @p BankSelMethod_MSB
                 *     For instruments that only use MSB and patch.
                 * @li @p BankSelMethod_LSB
                 *     For instruments that only use LSB and patch.
                 * @li @p BankSelMethod_Patch
                 *     For instruments that only use patch.
                 */
                enum BankSelMethod
                {
                    BankSelMethod_Normal = 0,
                    BankSelMethod_MSB    = 1,
                    BankSelMethod_LSB    = 2,
                    BankSelMethod_Patch  = 3
                };

                /**
                 * Returns the UseNotesAsControllers value.
                 *
                 * @return Whenther to use notes as controllers
                 */
                bool useNotesAsController() const
                {
                    return _useNotesAsControllers;
                }

                /**
                 * Sets the UseNotesAsControllers value.
                 *
                 * @param u New use notes as controllers value
                 */
                void setUseNotesAsControllers(bool u);

                /**
                 * Returns the number of banks of patch data defined
                 * by this instrument.
                 *
                 * @return Number of banks of patches
                 */
                size_t numBanks() const { return banks.size(); }

                /**
                 * Returns the bank number in the form:
                 * <pre>
                 *      bankLSB + (bankMSB<<7)
                 * </pre>
                 * for the bank with index @p index.
                 *
                 * If you call this method with an invalid parameter, the
                 * result is undefined.
                 *
                 * @return Bank change values for bank with given index
                 */
                int bank(int index) const { return banks[index]; }

                /**
                 * Returns the bank number in the form:
                 * <pre>
                 *      bankLSB + (bankMSB<<7)
                 * </pre>
                 * for the bank for @ref Voice @p voice. If there is no such
                 * @ref Voice defined, then -2 will be returned.
                 *
                 * @return Bank change values for bank with given index
                 */
                int bank(const Voice &voice) const;

                /**
                 * Returns the bank LSB for the set of patches with index
                 * @p index.
                 *
                 * If you call this method with an invalid parameter, the
                 * result is undefined.
                 *
                 * @return Bank LSB value for bank with given index
                 * @see    bankMSB
                 */
                int bankLSB(int index) const;

                /**
                 * Returns the bank MSB for the set of patches with index
                 * @p index.
                 *
                 * If you call this method with an invalid parameter, the
                 * result is undefined.
                 *
                 * @return Bank MSB value for bank with given index
                 * @see    bankLSB
                 */
                int bankMSB(int index) const;

                /**
                 * Returns the @ref PatchData object for the given bank.
                 *
                 * @param  index Bank index
                 * @return Pointer to @ref PatchData for bank, or 0
                 */
                PatchData *patch(int index) const { return patches[index]; }

                /**
                 * Returns the @ref PatchData object for the given bank, or 0
                 * if there is none.
                 *
                 * Note that this function takes the bank change number (as
                 * read from @ref bank()), not the bank index.
                 *
                 * If there is no PatchData for this bank, then zero is
                 * returned.
                 *
                 * You can specify @p bank as -1 to find the 'catch all' bank,
                 * and if your bank number is undefined, but there is a
                 * ctach all patch set, that will be returned.
                 *
                 * @param  bank Bank number
                 * @return Pointer to @ref PatchData for bank, or 0
                 */
                PatchData *patchForBank(int bank) const;

                /**
                 * Like the @ref patchForBank(int) above, but takes the
                 * LSB and MSB parameters separately. This function actually
                 * fowards responsibility onto the other version. It is
                 * provided as a convenience.
                 *
                 * If either of the LSB or MSB parameters are -1, then the
                 * overal bank value passed on is -1.
                 *
                 * @param  bankLSB Bank number LSB
                 * @param  bankMSB Bank number MSB
                 * @return Pointer to @ref PatchData for bank, or 0
                 */
                PatchData *patchForBank(int bankLSB, int bankMSB) const;

                /**
                 * Returns the number of sets of @ref NoteData defined
                 * by this instrument.
                 *
                 * @return Number of patches @ref NoteData objects
                 */
                size_t numKeys() const { return keys.size(); }

                /**
                 * Returns the @ref NoteData with the given @p index.
                 *
                 * If you call this method with an invalid parameter, the
                 * result is undefined.
                 *
                 * @return @ref NoteData for index
                 */
                NoteData *key(size_t index) const { return keys[index].second; }

                /**
                 * Returns the @ref NoteData for the given @ref Voice.
                 *
                 * If there is no such @ref Voice, then zero is returned.
                 *
                 * @return @ref NoteData for index
                 */
                NoteData *keyForVoice(const Voice &voice) const;

                /**
                 * Returns the number of drum statuses defined by this
                 * instrument.
                 *
                 * @return Number of drum statuses
                 */
                size_t numDrums() const { return drumFlags.size(); }

                /**
                 * Returns the drum @ref Voice with the given @p index.
                 *
                 * If you call this method with an invalid parameter, the
                 * result is undefined.
                 *
                 * @return @ref Voice for index
                 */
                Voice drum(size_t index) const { return drumFlags[index]; }

                /**
                 * Returns whether the specified @p voice is defined to be
                 * a drum sound or not (this implies that note data should
                 * be opened in a "drum" editor).
                 *
                 * @return Whether voice is a drum sound
                 */
                bool isDrum(const Voice &voice) const;

                /**
                 * Returns the @ref ControlData for this Instrument, if there
                 * is any defined, or zero if there is none.
                 *
                 * @return ControlData for this instrument
                 */
                ControlData *control() const { return _control; }

                /**
                 * Returns the @ref RpnData for this Instrument, if there
                 * is any defined, or zero if there is none.
                 *
                 * @return RpnData for this instrument
                 */
                RpnData *rpn() const { return _rpn; }

                /**
                 * Returns the @ref NrpnData for this Instrument, if there
                 * is any defined, or zero if there is none.
                 *
                 * @return NrpnData for this instrument
                 */
                NrpnData *nrpn() const { return _nrpn; }

                /**
                 * Write the minimal .ins file for this instrument.
                 *
                 * @param out ostream to write output to
                 */
                void write(std::ostream &out);

            private:

                /**
                 * Loads the instrument from the given .ins file.
                 * Pre: title has been set up already. Other values have
                 * defaults.
                 */
                void load(std::istream &in, TSE3::Progress *progress);

                /**
                 * Parses a line of the instrument definition.
                 * The istream is not used, but may be passed onto child
                 * objects.
                 */
                void parseLine(const std::string &line, std::istream &in);

                std::string _title;
                std::string _filename;
                int         _bankSelMethod;
                bool        _useNotesAsControllers;

                std::vector<PatchData *>                    patches;
                std::vector<int>                            banks;

                std::vector<std::pair<Voice, NoteData *> >  keys;
                std::vector<Voice>                          drumFlags;

                ControlData                                *_control;
                RpnData                                    *_rpn;
                NrpnData                                   *_nrpn;
        };

        /**
         * A base class for instrument data: many .ins file sections are based
         * on simple lists of 0..127 values. This is a base class for such
         * lists.
         *
         * @short   Instrument data container class
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class InstrumentData
        {
            public:

                /**
                 * Returns the title of this data group.
                 *
                 * @return Title of the data group
                 */
                const std::string &title() const { return _title; }

                /**
                 * Returns the name of the item with the given @p index.
                 * the index value must be between 0 and 127, or the results
                 * are undefined.
                 *
                 * If no name has been defined for this element, returns
                 * an empty string.
                 *
                 * @return Name of the data element
                 */
                const std::string &name(size_t index) const
                {
                    std::string *s = _names[index];
                    return s ? *s : empty;
                }

                /**
                 * Write the .ins file subsection.
                 *
                 * @param out ostream to write output to
                 */
                void write(std::ostream &out) const;

            protected:

                /**
                 * Contructor is private since this is a base class.
                 */
                InstrumentData(std::string const &title,
                               std::string const &insHeading,
                               std::istream      &in);

                /**
                 * The .ins file heading for this section type. Derived classes
                 * <b>MUST</b> override this for load) to work.
                 *
                 * @see load
                 */
                const std::string insHeading;

                /**
                 * Loads the subsection 'secname' from the .ins section
                 * 'insHeading'.
                 * Pre: title and insHeading have been set up.
                 */
                void load(const std::string &secname, std::istream &in);

                std::string  _title;
                std::string *_names[128];

                static std::string empty;
        };

        /**
         * This class represents a group of related patches - they will have
         * the same bank select values.
         *
         * @short   Instrument patch data group
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class PatchData : public InstrumentData
        {
            public:
                PatchData(std::string const &title, std::istream &in)
                    : InstrumentData(title, ".Patch Names", in) {}
        };

        /**
         * This class represents a group of note names.
         *
         * @short   Instrument note data group
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class NoteData : public InstrumentData
        {
            public:
                NoteData(std::string const &title, std::istream &in)
                    : InstrumentData(title, ".Note Names", in) {}
        };

        /**
         * This class represents a group of MIDI control change defintions.
         *
         * @short   Instrument MIDI controller data group
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class ControlData : public InstrumentData
        {
            public:
                ControlData(std::string const &title, std::istream &in)
                    : InstrumentData(title, ".Controller Names", in) {}
        };

        /**
         * This class represents a group of NRPN defintions.
         *
         * @short   Instrument NRPN data group
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class NrpnData : public InstrumentData
        {
            public:
                NrpnData(std::string const &title, std::istream &in)
                    : InstrumentData(title, ".NRPN Names", in) {}
        };

        /**
         * This class represents a group of RPN defintions.
         *
         * @short   Instrument RPN data group
         * @author  Pete Goodliffe
         * @version 3.00
         */
        class RpnData : public InstrumentData
        {
            public:
                RpnData(std::string const &title, std::istream &in)
                    : InstrumentData(title, ".RPN Names", in) {}
        };

        /**
         * The class represents a Cakewalk .ins file.
         *
         * It provides a mechanism for listing all instruments provided by a
         * particular .ins file, and for creating an Instrument object from it.
         *
         * The .ins file format is not documented. However, the documentation
         * that ships with the TSE3 library contains a description of this
         * format.
         *
         * @short   Cakewalk .ins file parser
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Instrument
         */
        class CakewalkInstrumentFile
        {
            public:

                /**
                 * Create an object for the given file.
                 *
                 * @param filename The name of the Cakewalk .ins file
                 */
                CakewalkInstrumentFile(const std::string &filename);

                /**
                 * Returns a set of the instrument titles in the .ins file.
                 *
                 * The first time you call this method, the file will be
                 * searched. Whilst this is being done you can be informted
                 * of progress via the @ref TSE3::Progress interface.
                 *
                 * @param  progress @ref TSE3::Progess callback, or zero for no
                 *                  callback
                 * @return List of instrument titles in the .ins file
                 */
                const std::list<std::string>
                    &instruments(TSE3::Progress *progress = 0);

                /**
                 * Factory method that creates an Instrument object for the
                 * given instrument title from this CakewalkInstrumentFile.
                 *
                 * You can supply a @ref TSE3::Progess interface to be informed
                 * of progress.
                 *
                 * @param  title    The title of the instrument to create
                 * @param  progress @ref TSE3::Progess callback, or zero for no
                 *                  callback
                 * @return New instrument object - you must delete this object
                 */
                Instrument *instrument(const std::string &title,
                                       TSE3::Progress *progress = 0);

            private:

                std::string            filename;
                bool                   searched_yet;
                std::list<std::string> ins;
        };
    }
}

#endif
