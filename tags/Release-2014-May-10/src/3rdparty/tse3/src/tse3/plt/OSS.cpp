/*
 * @(#)OSS.cpp 3.00 20 July 1999
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

#include "tse3/plt/OSS.h"

#include "tse3/Error.h"
#include "tse3/util/MulDiv.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/types.h>

// These #includes are linux specific and I need to do some autoconf
// magic to work out how to conditionally include them

#ifdef TSE3_WITH_OSS
#include <sys/ioctl.h>
#endif

#ifdef HAVE_LINUX_AWE_VOICE_H
#include <linux/awe_voice.h>
#elif HAVE_AWE_VOICE_H
#include <awe_voice.h>
#elif HAVE__USR_SRC_SYS_I386_ISA_SOUND_AWE_VOICE_H
#include "/usr/src/sys/i386/isa/sound/awe_voice.h"
#elif HAVE__USR_SRC_SYS_GNU_I386_ISA_SOUND_AWE_VOICE_H
#include "/usr/src/sys/gnu/i386/isa/sound/awe_voice.h"
#endif

#ifndef TSE3_WITH_OSS
    // Euch!
    // This is a somewhat nasty hack that will make the file compile quietly
    // if we're not building with OSS support.
    struct synth_info { int nr_voices; };
    struct midi_info  { int fish;      };
#endif

#include <algorithm>

using namespace TSE3;
using namespace TSE3::Plt;


/******************************************************************************
 * Private implementation classes
 *****************************************************************************/

namespace TSE3
{
    namespace Plt
    {
        /**
         * An class that provides support for dynamic voice id allocation for
         * soundcard MIDI output emulation.
         *
         * MIDI devices will accept MidiCommand_NoteOns as they come in. If
         * they get too many then they will have a policy to deal with it:
         * perhaps not play the additional notes, perhaps switch off the oldest
         * note, perhaps something more elaborate. Soundcards, on the other
         * hand, generally have a fixed number of voice generators that one
         * must address individually. In this case we need a list of which
         * voices are not yet in use to play a new note along side others. We
         * also need a list of which voice is assigned to which note (and with
         * which patch) so that we can switch off the correct voice. That's the
         * functionality provided by this class.
         *
         * It is used by versions of the @ref OSSMidiScheduler_SynthDevice
         * class to manage which voices are being played at any time.
         *
         * This is an internal class - you don't need to directly manipulate it
         * unless you specifically require it's functionality.
         *
         * @short   Soundcard voice id allocator
         * @author  Pete Goodliffe
         * @version 0.00
         * @internal
         */
        class VoiceManager
        {
            public:

                /**
                 * Create a VoiceManager that will handle noVoices voices.
                 */
                VoiceManager(int noVoices);
                ~VoiceManager();

                /**
                 * Allocate a voice from those available for the
                 * given soundcard. Returns an ID handle for the voice.
                 *
                 * It will first attempt to allocate a currently unused voice.
                 * If there are no unused voices then it will reallocate the
                 * last used voice.
                 *
                 * This method will not perform any OSS system calls to set the
                 * voice up.
                 */
                int allocate(int channel, int note);

                /**
                 * Deallocate a previously allocated voice.
                 *
                 * This method will not perform any OSS system calls to shut
                 * a voice down - it must have been stopped before deallocate
                 * is called.
                 */
                void deallocate(int id);

                /**
                 * Perform a search throught the currently allocated voices
                 * for one matching the given channel.
                 *
                 * Call first time with pos = -1. If returns -1 then there
                 * are no voices on the given channel. Call subsequently
                 * with pos set to the return ID to get the next match.
                 */
                int search(int channel, int pos);

                /**
                 * Similar to search above, but includes the note as a
                 * search criteria.
                 */
                int search(int channel, int note, int pos);

            private:

                int noVoices;
                struct Voice
                {
                    const int  id;
                    int        channel;
                    int        note;
                    bool       used;
                    Voice(int id) : id(id), used(false) {}
                };
                Voice             **voices;       // array of all Voice structs
                std::list<Voice*>   usedVoices;   // used voices pushed on back
                std::list<Voice*>   unusedVoices; // unused voices stored here
        };

        /**
         * A base class for classes that handle different types of OSS device.
         *
         * The class uses a number of member variables of the
         * @ref OSSMidiScheduler object - these are the OSS interface data
         * memebers that the OSS macros write to. They either have to be shared
         * in this manner, or the functionality for each type of device must be
         * rolled into the @ref OSSMidiScheduler class (which is the least
         * desirable of the two).
         *
         * This is an internal class - you don't need to directly manipulate it
         * unless you specifically require it's functionality.
         *
         * @short   Soundcard playback handler
         * @author  Pete Goodliffe
         * @version 0.00
         * @internal
         */
        class OSSMidiScheduler_SynthDevice
        {
            public:

                /**
                 * Constructor. Pass in the OSS interface data and the
                 * device number that the class is to provice support for.
                 */
                OSSMidiScheduler_SynthDevice(int             deviceno,
                                             synth_info     &synthinfo,
                                             int             seqfd,
                                             unsigned char *&_seqbuf,
                                             int            &_seqbuflen,
                                             int            &_seqbufptr);
                virtual ~OSSMidiScheduler_SynthDevice() = 0;

                virtual void noteOff(int ch, int note, int vel) = 0;
                virtual void noteOn(int ch, int note, int vel) = 0;
                virtual void keyPressure(int ch, int note, int vel) = 0;
                virtual void controlChange(int ch, int ctrl, int val) = 0;
                virtual void programChange(int ch, int prog) = 0;
                virtual void channelPressure(int ch, int vel) = 0;
                virtual void pitchBend(int ch, int lsb, int msb) = 0;

            protected:

                int deviceno; // OSS device the class handles

                // These data members are created by the OSSMidiScheduler class
                // and we use them here. We do not delete them in the
                // constructor.
                int             seqfd;
                synth_info     &synthinfo;
                unsigned char *&_seqbuf;
                int            &_seqbuflen;
                int            &_seqbufptr;
                void            seqbuf_dump();
                void            seqbuf_clean();

                // Cached information about each channel's state
                unsigned char _programChange[16];
                unsigned char _pitchBendLSB[16];
                unsigned char _pitchBendMSB[16];
                unsigned char _channelPressure[16];
        };


        /**
         * OSS NULL device handler - it does nothing.
         *
         * This is an internal class, used by the @ref OSSMidiScheduler class.
         *
         * @short   Unknown soundcard playback handler
         * @author  Pete Goodliffe
         * @version 0.00
         * @internal
         */
        class OSSMidiScheduler_NULLDevice : public OSSMidiScheduler_SynthDevice
        {
            public:

                OSSMidiScheduler_NULLDevice(int            deviceno,
                                           synth_info     &synthinfo,
                                           int             seqfd,
                                           unsigned char *&_seqbuf,
                                           int            &_seqbuflen,
                                           int            &_seqbufptr);
                virtual ~OSSMidiScheduler_NULLDevice();

                /**
                 * @reimplemented
                 */
                virtual void noteOff(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void noteOn(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void keyPressure(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void controlChange(int ch, int ctrl, int val);

                /**
                 * @reimplemented
                 */
                virtual void programChange(int ch, int prog);

                /**
                 * @reimplemented
                 */
                virtual void channelPressure(int ch, int vel);

                /**
                 * @reimplemented
                 */
                virtual void pitchBend(int ch, int lsb, int msb);
        };

        /**
         * OSS FM synth device handler.
         *
         * This is an internal class, used by the @ref OSSMidiScheduler class.
         * However, you <b>will</b> need to call @ref setPatchesDirectory()
         * before you create an OSSMidiScheduler.
         *
         * @short   FM soundcard playback handler
         * @author  Pete Goodliffe
         * @version 0.00
         * @internal
         */
        class OSSMidiScheduler_FMDevice : public OSSMidiScheduler_SynthDevice
        {
            public:

                OSSMidiScheduler_FMDevice(int             deviceno,
                                          synth_info     &synthinfo,
                                          int             seqfd,
                                          unsigned char *&_seqbuf,
                                          int            &_seqbuflen,
                                          int            &_seqbufptr);
                virtual ~OSSMidiScheduler_FMDevice();

                /**
                 * @reimplemented
                 */
                virtual void noteOff(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void noteOn(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void keyPressure(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void controlChange(int ch, int ctrl, int val);

                /**
                 * @reimplemented
                 */
                virtual void programChange(int ch, int prog);

                /**
                 * @reimplemented
                 */
                virtual void channelPressure(int ch, int vel);

                /**
                 * @reimplemented
                 */
                virtual void pitchBend(int ch, int lsb, int msb);

                /**
                 * This returns in which directory the device will look for
                 * patch files. This must be set before the device is created.
                 *
                 * The patches directory path can contain multiple paths,
                 * separated by colons.
                 *
                 * @see setPatchesDirectory
                 */
                static std::string &patchesDirectory()
                {
                    return _patchesDirectory;
                }

                /**
                 * Call this before creating the @ref OSSMidiScheduler object
                 * to set up where to look for FM patches files.
                 *
                 * @see patchesDirectory
                 */
                static void setPatchesDirectory(const std::string &dir);

            private:

                VoiceManager voiceman;

                bool patchLoaded[256];      // 0-127 patches, 128-255 drums.
                void loadPatches();         // Load the patches into the device.
                int  getPatch(int patchNo); // returns patchNo if that patchNo
                                            // is loaded, else another patch no
                                            // or 256 if couldn't find any.
                static std::string _patchesDirectory;
                int opl;       // for FM devices whether to do opl 3 (== 3)
        };

        /**
         * OSS AWE synth device handler.
         *
         * This is an internal class, used by the @ref OSSMidiScheduler class.
         *
         * @short   AWE soundcard playback handler
         * @author  Pete Goodliffe
         * @version 0.00
         * @internal
         */
        class OSSMidiScheduler_AWEDevice : public OSSMidiScheduler_SynthDevice
        {
            public:

                OSSMidiScheduler_AWEDevice(int             deviceno,
                                           synth_info     &synthinfo,
                                           int             seqfd,
                                           unsigned char *&_seqbuf,
                                           int            &_seqbuflen,
                                           int            &_seqbufptr);
                virtual ~OSSMidiScheduler_AWEDevice();

                /**
                 * @reimplemented
                 */
                virtual void noteOff(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void noteOn(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void keyPressure(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void controlChange(int ch, int ctrl, int val);

                /**
                 * @reimplemented
                 */
                virtual void programChange(int ch, int prog);

                /**
                 * @reimplemented
                 */
                virtual void channelPressure(int ch, int vel);

                /**
                 * @reimplemented
                 */
                virtual void pitchBend(int ch, int lsb, int msb);
        };

        /**
         * OSS GUS synth device handler.
         *
         * This is an internal class, used by the @ref OSSMidiScheduler class.
         * However, you <b>will</b> need to call @ref setPatchesDirectory()
         * before you create an OSSMidiScheduler.
         *
         * @short   GUS soundcard playback handler
         * @author  Pete Goodliffe
         * @version 0.00
         * @internal
         */
        class OSSMidiScheduler_GUSDevice : public OSSMidiScheduler_SynthDevice
        {
            public:

                OSSMidiScheduler_GUSDevice(int             deviceno,
                                           synth_info     &synthinfo,
                                           int             seqfd,
                                           unsigned char *&_seqbuf,
                                           int            &_seqbuflen,
                                           int            &_seqbufptr);
                virtual ~OSSMidiScheduler_GUSDevice();

                /**
                 * @reimplemented
                 */
                virtual void noteOff(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void noteOn(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void keyPressure(int ch, int note, int vel);

                /**
                 * @reimplemented
                 */
                virtual void controlChange(int ch, int ctrl, int val);

                /**
                 * @reimplemented
                 */
                virtual void programChange(int ch, int prog);

                /**
                 * @reimplemented
                 */
                virtual void channelPressure(int ch, int vel);

                /**
                 * @reimplemented
                 */
                virtual void pitchBend(int ch, int lsb, int msb);

                /**
                 * This returns in which directory the device will look for
                 * patch files. This must be set before the device is created.
                 * (In fact, it must be set before each voice is loaded - they
                 * are loaded on demand. However, it is safest to set this
                 * before creating the device.)
                 *
                 * The patches directory path can contain multiple paths,
                 * separated by colons.
                 *
                 * @see setPatchesDirectory
                 */
                static std::string &patchesDirectory()
                {
                    return _patchesDirectory;
                }

                /**
                 * Call this before creating the @ref OSSMidiScheduler object
                 * to set up where to look for GUS patches files.
                 *
                 * @see patchesDirectory
                 */
                static void setPatchesDirectory(const std::string &dir);

            private:

                VoiceManager voiceman;

                size_t  nobits;                 // patch resolution; 8 or 16
                int     totalMemory;            // total memory in soundcard
                int     freeMemory;             // free memory in soundcard
                bool    patchLoaded[256];       // 0-127 patches, 128-255 drums.
                bool    patchLoadedFailed[256]; // whether a load attempy failed
                int     getPatch(int patchNo);  // returns patchNo if that
                                                // patchNo is loaded, else
                                                // another patch no or 256 if
                                                // couldn't find any.

                static std::string _patchesDirectory;

                /**
                 * Returns the filename for the given program change number.
                 * Calls @ref loadPatch() if the patch hasn't been loaded yet.
                 */
                static char *patchName(int pgm);

                /**
                 * Load a patch. 0..127 is a GM program, 128-255 is a drum
                 * patch. Sets patchLoaded and patchLoadedFailed appropriately.
                 *
                 * @return Boolean indicating sucess.
                 */
                bool loadPatch(int patch);
        };
    }
}


/******************************************************************************
 * Patch filename search facilities
 *****************************************************************************/

namespace
{
    /**
     * The delimiter that separates paths
     */
    const char PATH_DELIM     = ':';
    const char DEF_PATH[]     = "/etc/midi:/etc";

    /**
     * Looks for the file named @p leaf in the directories held in
     * @p path. There can be more than one directory path in @path, each
     * separated by a colon.
     *
     * The opened file handle that is returned has been opened in "rb" mode.
     *
     * @param  leaf File's leaf name
     * @param  path Directory path to search in
     * @return File handle of an open path
     */
    FILE *findFileInPaths(std::string &leaf, std::string &path)
    {
        size_t noPaths = std::count(path.begin(),
                                    path.end(),
                                    PATH_DELIM)+1;
        std::string::iterator pos1 = path.begin();
        std::string::iterator pos2 = find(path.begin(),
                                          path.end()-1,
                                          PATH_DELIM);
        FILE *f = 0;
        while (!f && noPaths)
        {
            std::string path(pos1, pos2);
            std::string file = path  + "/" + leaf;
            pos1 = pos2+1;
            pos2 = find(pos1, path.end()-1, PATH_DELIM);
            noPaths--;

            f = fopen(file.c_str(), "rb");
        }
        return f;
    }
}


/******************************************************************************
 * The following stuff is used in the GUS device handler
 *****************************************************************************/

#ifdef TSE3_WITH_OSS
namespace
{
    struct pat_header
    {
        char            magic[12];
        char            version[10];
        char            description[60];
        unsigned char   instruments;
        char            voices;
        char            channels;
        unsigned short  nr_waveforms;
        unsigned short  master_volume;
        unsigned long   data_size;
    };
    struct sample_header
    {
        char            name[7];
        unsigned char   fractions;
        long            len;
        long            loop_start;
        long            loop_end;
        unsigned short  base_freq;
        long            low_note;
        long            high_note;
        long            base_note;
        short           detune;
        unsigned char   panning;

        unsigned char   envelope_rate[6];
        unsigned char   envelope_offset[6];

        unsigned char   tremolo_sweep;
        unsigned char   tremolo_rate;
        unsigned char   tremolo_depth;

        unsigned char   vibrato_sweep;
        unsigned char   vibrato_rate;
        unsigned char   vibrato_depth;

        char            modes;

        short           scale_frequency;
        unsigned short  scale_factor;
    };

    int get_dint(unsigned char *p)
    {
        unsigned int v=0;

        for (int i=0;i<4;++i)
        {
            v |= (p[i] << (i*8));
        }
        return (int)v;
    }

    unsigned short get_word(unsigned char *p)
    {
        unsigned short v=0;

        for (int i=0;i<2;++i)
            v |= (*p++ << (i*8));
        return (short)v;
    }

    /**
     * This massive structure holds file names for GUS patches.
     */
    char GUSDevice_PatchNames[256][9] =
    {
        /*   0 */	"acpiano",
        /*   1 */	"britepno",
        /*   2 */	"synpiano",
        /*   3 */	"honky",
        /*   4 */	"epiano1",
        /*   5 */	"epiano2",
        /*   6 */	"hrpschrd",
        /*   7 */	"clavinet",
        /*   8 */	"celeste",
        /*   9 */	"glocken",
        /*  10 */	"musicbox",
        /*  11 */	"vibes",
        /*  12 */	"marimba",
        /*  13 */	"xylophon",
        /*  14 */	"tubebell",
        /*  15 */	"santur",
        /*  16 */	"homeorg",
        /*  17 */	"percorg",
        /*  18 */	"rockorg",
        /*  19 */	"church",
        /*  20 */	"reedorg",
        /*  21 */	"accordn",
        /*  22 */	"harmonca",
        /*  23 */	"concrtna",
        /*  24 */	"nyguitar",
        /*  25 */	"acguitar",
        /*  26 */	"jazzgtr",
        /*  27 */	"cleangtr",
        /*  28 */	"mutegtr",
        /*  29 */	"odguitar",
        /*  30 */	"distgtr",
        /*  31 */	"gtrharm",
        /*  32 */	"acbass",
        /*  33 */	"fngrbass",
        /*  34 */	"pickbass",
        /*  35 */	"fretless",
        /*  36 */	"slapbas1",
        /*  37 */	"slapbas2",
        /*  38 */	"synbass1",
        /*  39 */	"synbass2",
        /*  40 */	"violin",
        /*  41 */	"viola",
        /*  42 */	"cello",
        /*  43 */	"contraba",
    //	/*  44 */	"marcato",
        /*  44 */	"tremstr",
        /*  45 */	"pizzcato",
        /*  46 */	"harp",
        /*  47 */	"timpani",
        /*  48 */	"marcato",
        /*  49 */	"slowstr",
        /*  50 */	"synstr1",
        /*  51 */	"synstr2",
        /*  52 */	"choir",
        /*  53 */	"doo",
        /*  54 */	"voices",
        /*  55 */	"orchhit",
        /*  56 */	"trumpet",
        /*  57 */	"trombone",
        /*  58 */	"tuba",
        /*  59 */	"mutetrum",
        /*  60 */	"frenchrn",
        /*  61 */	"hitbrass",
        /*  62 */	"synbras1",
        /*  63 */	"synbras2",
        /*  64 */	"sprnosax",
        /*  65 */	"altosax",
        /*  66 */	"tenorsax",
        /*  67 */	"barisax",
        /*  68 */	"oboe",
        /*  69 */	"englhorn",
        /*  70 */	"bassoon",
        /*  71 */	"clarinet",
        /*  72 */	"piccolo",
        /*  73 */	"flute",
        /*  74 */	"recorder",
        /*  75 */	"woodflut",
        /*  76 */	"bottle",
        /*  77 */	"shakazul",
        /*  78 */	"whistle",
        /*  79 */	"ocarina",
        /*  80 */	"sqrwave",
        /*  81 */	"sawwave",
        /*  82 */	"calliope",
        /*  83 */	"chiflead",
    //	/*  84 */	"voxlead",
        /*  84 */	"charang",
            /*  85 */	"voxlead",
        /*  86 */	"lead5th",
        /*  87 */	"basslead",
        /*  88 */	"fantasia",
        /*  89 */	"warmpad",
        /*  90 */	"polysyn",
        /*  91 */	"ghostie",
        /*  92 */	"bowglass",
        /*  93 */	"metalpad",
        /*  94 */	"halopad",
        /*  95 */	"sweeper",
        /*  96 */	"aurora",
        /*  97 */	"soundtrk",
        /*  98 */	"crystal",
        /*  99 */	"atmosphr",
        /* 100 */	"freshair",
        /* 101 */	"unicorn",
    //	/* 102 */	"sweeper",
        /* 102 */	"echovox",
            /* 103 */	"startrak",
        /* 104 */	"sitar",
        /* 105 */	"banjo",
        /* 106 */	"shamisen",
        /* 107 */	"koto",
        /* 108 */	"kalimba",
        /* 109 */	"bagpipes",
        /* 110 */	"fiddle",
        /* 111 */	"shannai",
        /* 112 */	"carillon",
        /* 113 */	"agogo",
        /* 114 */	"steeldrm",
        /* 115 */	"woodblk",
        /* 116 */	"taiko",
        /* 117 */	"toms",
        /* 118 */	"syntom",
        /* 119 */	"revcym",
        /* 120 */	"fx-fret",
        /* 121 */	"fx-blow",
        /* 122 */	"seashore",
        /* 123 */	"jungle",
        /* 124 */	"telephon",
        /* 125 */	"helicptr",
        /* 126 */	"applause",
        /* 127 */	"pistol",

        "",	/* 128 = drum 0*/
        "",	/* 129 = drum 1*/
        "",	/* 130 = drum 2*/
        "",	/* 131 = drum 3*/
        "",	/* 132 = drum 4*/
        "",	/* 133 = drum 5*/
        "",	/* 134 = drum 6*/
        "",	/* 135 = drum 7*/
        "",	/* 136 = drum 8*/
        "",	/* 137 = drum 9*/
        "",	/* 138 = drum 10*/
        "",	/* 139 = drum 11*/
        "",	/* 140 = drum 12*/
        "",	/* 141 = drum 13*/
        "",	/* 142 = drum 14*/
        "",	/* 143 = drum 15*/
        "",	/* 144 = drum 16*/
        "",	/* 145 = drum 17*/
        "",	/* 146 = drum 18*/
        "",	/* 147 = drum 19*/
        "",	/* 148 = drum 20*/
        "",	/* 149 = drum 21*/
        "",	/* 150 = drum 22*/
        "",	/* 151 = drum 23*/
        "",	/* 152 = drum 24*/
        "",	/* 153 = drum 25*/
        "",	/* 154 = drum 26*/
        "highq",	/* 155 = drum 27*/
        "slap",		/* 156 = drum 28*/
        "scratch1",	/* 157 = drum 29*/
        "scratch2",	/* 158 = drum 30*/
        "sticks",	/* 159 = drum 31*/
        "sqrclick",	/* 160 = drum 32*/
        "metclick",	/* 161 = drum 33*/
        "metbell",	/* 162 = drum 34*/
        "kick1",	/* 163 = drum 35*/
        "kick2",	/* 164 = drum 36*/
        "stickrim",	/* 165 = drum 37*/
        "snare1",	/* 166 = drum 38*/
        "claps",	/* 167 = drum 39*/
        "snare2",	/* 168 = drum 40*/
        "tomlo2",	/* 169 = drum 41*/
        "hihatcl",	/* 170 = drum 42*/
        "tomlo1",	/* 171 = drum 43*/
        "hihatpd",	/* 172 = drum 44*/
        "tommid2",	/* 173 = drum 45*/
        "hihatop",	/* 174 = drum 46*/
        "tommid1",	/* 175 = drum 47*/
        "tomhi2",	/* 176 = drum 48*/
        "cymcrsh1",	/* 177 = drum 49*/
        "tomhi1",	/* 178 = drum 50*/
        "cymride1",	/* 179 = drum 51*/
        "cymchina",	/* 180 = drum 52*/
        "cymbell",	/* 181 = drum 53*/
        "tamborin",	/* 182 = drum 54*/
        "cymsplsh",	/* 183 = drum 55*/
        "cowbell",	/* 184 = drum 56*/
        "cymcrsh2",	/* 185 = drum 57*/
        "vibslap",	/* 186 = drum 58*/
        "cymride2",	/* 187 = drum 59*/
        "bongohi",	/* 188 = drum 60*/
        "bongolo",	/* 189 = drum 61*/
        "congahi1",	/* 190 = drum 62*/
        "congahi2",	/* 191 = drum 63*/
        "congalo",	/* 192 = drum 64*/
        "timbaleh",	/* 193 = drum 65*/
        "timbalel",	/* 194 = drum 66*/
        "agogohi",	/* 195 = drum 67*/
        "agogolo",	/* 196 = drum 68*/
        "cabasa",	/* 197 = drum 69*/
        "maracas",	/* 198 = drum 70*/
        "whistle1",	/* 199 = drum 71*/
        "whistle2",	/* 200 = drum 72*/
        "guiro1",	/* 201 = drum 73*/
        "guiro2",	/* 202 = drum 74*/
        "clave",	/* 203 = drum 75*/
        "woodblk1",	/* 204 = drum 76*/
        "woodblk2",	/* 205 = drum 77*/
        "cuica1",	/* 206 = drum 78*/
        "cuica2",	/* 207 = drum 79*/
        "triangl1",	/* 208 = drum 80*/
        "triangl2",	/* 209 = drum 81*/
        "shaker",	/* 210 = drum 82*/
        "jingles",	/* 211 = drum 83*/
        "belltree",	/* 212 = drum 84*/
        "castinet",	/* 213 = drum 85*/
        "surdo1",	/* 214 = drum 86*/
        "surdo2",	/* 215 = drum 87*/
        "",	/* 216 = drum 88*/
        "",	/* 217 = drum 89*/
        "",	/* 218 = drum 90*/
        "",	/* 219 = drum 91*/
        "",	/* 220 = drum 92*/
        "",	/* 221 = drum 93*/
        "",	/* 222 = drum 94*/
        "",	/* 223 = drum 95*/
        "",	/* 224 = drum 96*/
        "",	/* 225 = drum 97*/
        "",	/* 226 = drum 98*/
        "",	/* 227 = drum 99*/
        "",	/* 228 = drum 100*/
        "",	/* 229 = drum 101*/
        "",	/* 230 = drum 102*/
        "",	/* 231 = drum 103*/
        "",	/* 232 = drum 104*/
        "",	/* 233 = drum 105*/
        "",	/* 234 = drum 106*/
        "",	/* 235 = drum 107*/
        "",	/* 236 = drum 108*/
        "",	/* 237 = drum 109*/
        "",	/* 238 = drum 110*/
        "",	/* 239 = drum 111*/
        "",	/* 240 = drum 112*/
        "",	/* 241 = drum 113*/
        "",	/* 242 = drum 114*/
        "",	/* 243 = drum 115*/
        "",	/* 244 = drum 116*/
        "",	/* 245 = drum 117*/
        "",	/* 246 = drum 118*/
        "",	/* 247 = drum 119*/
        "",	/* 248 = drum 120*/
        "",	/* 249 = drum 121*/
        "",	/* 250 = drum 122*/
        "",	/* 251 = drum 123*/
        "",	/* 252 = drum 124*/
        "",	/* 253 = drum 125*/
        "",	/* 254 = drum 126*/
        ""	/* 255 = drum 127*/
    };
}
#endif


/******************************************************************************
 * VoiceManager class
 *****************************************************************************/

VoiceManager::VoiceManager(int noVoices)
: noVoices(noVoices)
{
    voices = new Voice*[noVoices];
    for (int n = 0; n < noVoices; ++n)
    {
        voices[n] = new Voice(n);
        unusedVoices.push_back(voices[n]);
    }
}


VoiceManager::~VoiceManager()
{
    for (int n = 0; n < noVoices; ++n)
    {
        delete voices[n];
    }
    delete [] voices;
}


int VoiceManager::allocate(int channel, int note)
{
    Voice *voice = 0;
    if (unusedVoices.size())
    {
        voice = *(unusedVoices.begin());
        unusedVoices.remove(voice);
    }
    else
    {
        voice = *(usedVoices.begin());
        usedVoices.remove(voice);
    }
    voice->channel = channel;
    voice->note    = note;
    voice->used    = true;
    usedVoices.push_back(voice);
    return voice->id;
}


void VoiceManager::deallocate(int id)
{
    Voice *voice = voices[id];
    if (!voice->used) return;
    voice->used = false;
    usedVoices.remove(voice);
    unusedVoices.push_back(voice);
}


int VoiceManager::search(int channel, int pos)
{
    for (int n = pos+1; n < noVoices; ++n)
    {
        if (voices[n]->used && voices[n]->channel == channel)
            return n;
    }
    return -1;
}


int VoiceManager::search(int channel, int note, int pos)
{
    for (int n = pos+1; n < noVoices; ++n)
    {
        if (voices[n]->used && voices[n]->channel == channel
            && voices[n]->note == note)
            return n;
    }
    return -1;
}


/******************************************************************************
 * OSSMidiScheduler_SynthDevice class
 *****************************************************************************/

OSSMidiScheduler_SynthDevice::OSSMidiScheduler_SynthDevice(
    int             deviceno,
    synth_info     &synthinfo,
    int             seqfd,
    unsigned char *&_seqbuf,
    int            &_seqbuflen,
    int            &_seqbufptr)
: deviceno(deviceno), seqfd(seqfd), synthinfo(synthinfo),
  _seqbuf(_seqbuf), _seqbuflen(_seqbuflen),
  _seqbufptr(_seqbufptr)
{
    for (int ch = 0; ch < 16; ++ch)
    {
        _programChange[ch]   = 0;
        _pitchBendLSB[ch]    = 0;
        _pitchBendMSB[ch]    = 0x40;
        _channelPressure[ch] = 127;
    }
}


OSSMidiScheduler_SynthDevice::~OSSMidiScheduler_SynthDevice()
{
}


void OSSMidiScheduler_SynthDevice::seqbuf_dump()
{
#ifdef TSE3_WITH_OSS
    // This is canonical OSS code: after OSS macros are used to fill the
    // buffer defined by SEQ_DEFINEBUF (or rather, our class private version
    // of it)
    if (_seqbufptr)
        if (write(seqfd, _seqbuf, _seqbufptr) == -1)
            perror("Can't write to MIDI device");
    _seqbufptr = 0;
#endif
}


void OSSMidiScheduler_SynthDevice::seqbuf_clean()
{
    _seqbufptr = 0;
}


/******************************************************************************
 * OSSMidiScheduler_NULLDevice class
 *****************************************************************************/

OSSMidiScheduler_NULLDevice::OSSMidiScheduler_NULLDevice(
    int             deviceno,
    synth_info     &synthinfo,
    int             seqfd,
    unsigned char *&_seqbuf,
    int            &_seqbuflen,
    int            &_seqbufptr)
: OSSMidiScheduler_SynthDevice(deviceno, synthinfo, seqfd, _seqbuf,
                               _seqbuflen, _seqbufptr)
{
}


OSSMidiScheduler_NULLDevice::~OSSMidiScheduler_NULLDevice()
{
}


void OSSMidiScheduler_NULLDevice::noteOff(int /*ch*/, int /*note*/, int /*vel*/)
{
}


void OSSMidiScheduler_NULLDevice::noteOn(int /*ch*/, int /*note*/, int /*vel*/)
{
}


void OSSMidiScheduler_NULLDevice::keyPressure(int /*ch*/, int /*note*/,
                                              int /*vel*/)
{
}


void OSSMidiScheduler_NULLDevice::controlChange(int /*ch*/, int /*ctrl*/,
                                                int /*val*/)
{
}


void OSSMidiScheduler_NULLDevice::programChange(int /*ch*/, int /*prog*/)
{
}


void OSSMidiScheduler_NULLDevice::channelPressure(int /*ch*/, int /*vel*/)
{
}


void OSSMidiScheduler_NULLDevice::pitchBend(int /*ch*/, int /*lsb*/,
                                            int /*msb*/)
{
}


/******************************************************************************
 * OSSMidiScheduler_FMDevice class
 *****************************************************************************/

OSSMidiScheduler_FMDevice::OSSMidiScheduler_FMDevice(int             deviceno,
                                                     synth_info     &synthinfo,
                                                     int             seqfd,
                                                     unsigned char *&_seqbuf,
                                                     int            &_seqbuflen,
                                                     int            &_seqbufptr)
: OSSMidiScheduler_SynthDevice(deviceno, synthinfo, seqfd, _seqbuf,
                               _seqbuflen, _seqbufptr),
  voiceman(synthinfo.nr_voices), opl(2)
{
#ifdef TSE3_WITH_OSS
    if (opl == 3) ioctl(seqfd, SNDCTL_FM_4OP_ENABLE, &deviceno);
    SEQ_VOLUME_MODE(deviceno, VOL_METHOD_LINEAR);
    for (int n = 0; n < synthinfo.nr_voices; ++n)
    {
        SEQ_CONTROL(deviceno, n, SEQ_VOLMODE, VOL_METHOD_LINEAR);
//        SEQ_STOP_NOTE(deviceno, n, vman->note(n), 64);
    }

    // We load every FM patch before the device is used
    loadPatches();
#endif
}


OSSMidiScheduler_FMDevice::~OSSMidiScheduler_FMDevice()
{
}

void adjustfm(char *buf, int key)
{
#ifdef TSE3_WITH_OSS
    bool reverb = true;
    unsigned char pan = ((rand() % 3) + 1) << 4;

    if (key == FM_PATCH) {
	buf[39] &= 0xc0;
	if (buf[46] & 1)
	    buf[38] &= 0xc0;
	buf[46] = (buf[46] & 0xcf) | pan;
	if (reverb) {
	    unsigned val;
	    val = buf[43] & 0x0f;
	    if (val > 0)
		val--;
	    buf[43] = (buf[43] & 0xf0) | val;
	}
    } else {
	int mode;
	if (buf[46] & 1)
	    mode = 2;
	else
	    mode = 0;
	if (buf[57] & 1)
	    ++mode;
	buf[50] &= 0xc0;
	if (mode == 3)
	    buf[49] &= 0xc0;
	if (mode == 1)
	    buf[39] &= 0xc0;
	if (mode == 2 || mode == 3)
	    buf[38] &= 0xc0;
	buf[46] = (buf[46] & 0xcf) | pan;
	buf[57] = (buf[57] & 0xcf) | pan;
	if (mode == 1 && reverb) {
	    unsigned val;
	    val = buf[43] & 0x0f;
	    if (val > 0)
		val--;
	    buf[43] = (buf[43] & 0xf0) | val;
	    val = buf[54] & 0x0f;
	    if (val > 0)
		val--;
	    buf[54] = (buf[54] & 0xf0) | val;
	}
    }
#else
    buf = buf;
    key = key;
#endif
}


void OSSMidiScheduler_FMDevice::loadPatches()
{
#ifdef TSE3_WITH_OSS
    if (0) std::cerr << "FM: Loading patches\n";
    for (int n = 0; n < 256; ++n) patchLoaded[n] = false;
    if (0) std::cerr << "opl is " << opl << "\n";

    /**************************************************************************
     * Normal voice patches
     *************************************************************************/

    // Find the patches file
    std::string leaf;
    size_t      size;
    if (opl == 3)
    {
        leaf = "std.o3";
        size = 60;
    }
    else
    {
        leaf = "std.sb";
        size = 52;
    }
    FILE *f = findFileInPaths(leaf, _patchesDirectory);
    if (!f)
    {
        std::cerr << "Opening FM patch file failed\n";
        return;
    }

    // Load the patches file
    if (0) std::cerr << "Success: loading patches \n";
    char tmp[60];
    sbi_instrument instr;
    for (int n = 0; n < 128; ++n)
    {
        if (fread(tmp, 1, size, f) != size)
        {
            std::cerr << "TSE3: (OSS) FM patch load error (" << n << ")\n";
        }
        patchLoaded[n] = true;
        if (0) std::cerr << "[" << n << "] " ;
        instr.key    = (strncmp(tmp, "4OP", 3) == 0) ? OPL3_PATCH : FM_PATCH;
        int datasize = (strncmp(tmp, "4OP", 3) == 0) ? 22         : 11;
        instr.device = deviceno;
        instr.channel = n;
        //tmp[46] = (tmp[46] & 0xcf) | ((++stereoeffect)<<4); // stereo
        //steroeffect=stereoeffect%3;
        adjustfm(tmp, instr.key);
        for (int j = 0; j < 32; ++j)
        {
            instr.operators[j] = (j < datasize) ? tmp[j+36] : 0;
        }
        SEQ_WRPATCH(&instr,sizeof(instr));
    }
    if (0) std::cerr << "\n";
    fclose(f);

    /**************************************************************************
     * Drum patches
     *************************************************************************/

    // Find the patches file
    if (opl == 3)
    {
        leaf = "drums.o3";
    }
    else
    {
        leaf = "drums.sb";
    }
    f = findFileInPaths(leaf, _patchesDirectory);
    if (!f)
    {
        std::cerr << "Opening FM patch file failed\n";
        return;
    }

    // Load the patches file
    if (0) std::cerr << "Success\n";
    for (int n = 128; n < 256; ++n)
    {
        if (0) std::cerr << "[" << n << "] " ;
        if (fread(tmp, 1, size, f) != size)
        {
            std::cerr << "TSE3: (OSS) FM drum patch load error (" << n << ")\n";
        }
        patchLoaded[n] = true;
        instr.key    = (strncmp(tmp, "4OP", 3) == 0) ? OPL3_PATCH : FM_PATCH;
        int datasize = (strncmp(tmp, "4OP", 3) == 0) ? 22         : 11;
        instr.device = deviceno;
        instr.channel = n;
        // stereo effect same as before if you want it
        adjustfm(tmp, instr.key);
        for (int j = 0; j < 32; ++j)
        {
            instr.operators[j] = (j < datasize) ? tmp[j+36] : 0;
        }
        SEQ_WRPATCH(&instr, sizeof(instr));
    }
    if (0) std::cerr << "\n";
    fclose(f);
#endif
}


std::string OSSMidiScheduler_FMDevice::_patchesDirectory = DEF_PATH;


void OSSMidiScheduler_FMDevice::setPatchesDirectory(const std::string &dir)
{
    if (0) std::cerr << "FM: Setting patches dir to " << dir << "\n";
    _patchesDirectory = dir;
}


int OSSMidiScheduler_FMDevice::getPatch(int patchNo)
{
    if (patchLoaded[patchNo]) return patchNo;
    patchNo = (patchNo < 128) ? 0 : 128;
    while (patchNo < 256 && !patchLoaded[patchNo])
    {
        ++patchNo;
    }
    return patchNo;
}


void OSSMidiScheduler_FMDevice::noteOff(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    if (0)
        std::cerr << "FM_noteOff (" << deviceno << "," << ch << ": " << note
                  << "," << vel << ") ";
    int n = -1;
    while ((n = voiceman.search(ch, note, n)) != -1)
    {
        SEQ_STOP_NOTE(deviceno, n, note, vel);
        voiceman.deallocate(n);
    }
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_FMDevice::noteOn(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    if (0)
        std::cerr << "FM_noteOn (" << deviceno << "," << ch << ": " << note
                  << "," << vel << ") ";
    if (vel == 0)
    {
        noteOff(ch, note, vel);
        return;
    }

    if ((ch == 9 && !patchLoaded[note+128]) || !patchLoaded[_programChange[ch]])
    {
        if (0) std::cerr << "no patch loaded - I'll use the nearest\n";
        //return;
    }
    int voice = voiceman.allocate(ch, note);
    if (ch == 9)
    {
        SEQ_SET_PATCH(deviceno, voice, getPatch(note+128));
        if (note+128 < 175) return;
    }
    else
    {
        SEQ_SET_PATCH(deviceno, voice, getPatch(_programChange[ch]));
    }
    SEQ_BENDER(deviceno, voice,
               (_pitchBendLSB[ch]&0x7f)|(_pitchBendMSB[ch]<<7));
    SEQ_START_NOTE(deviceno, voice, note, vel);
    SEQ_CHN_PRESSURE(deviceno, voice, _channelPressure[ch]);
    // pan
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_FMDevice::keyPressure(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    if (ch == 9) return; // don't do this for percussion XXX WHY NOT???
    int n = -1;
    while ((n = voiceman.search(ch, note, n)) != -1)
    {
        SEQ_KEY_PRESSURE(deviceno, n, note, vel);
    }
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_FMDevice::controlChange(int /*ch*/, int /*ctrl*/,
                                              int /*val*/)
{
    // TODO
}


void OSSMidiScheduler_FMDevice::programChange(int ch, int prog)
{
    if (0)
        std::cerr << "FM_programChange: (" << deviceno << "," << ch
                  << ": " << prog << ")\n";
    _programChange[ch] = prog;
}


void OSSMidiScheduler_FMDevice::channelPressure(int ch, int vel)
{
#ifdef TSE3_WITH_OSS
    _channelPressure[ch] = vel;
    int n = -1;
    while ((n = voiceman.search(ch, n)) != -1)
    {
        SEQ_CHN_PRESSURE(deviceno, n, vel);
    }
#else
    ch   = ch;
    vel  = vel;
#endif
}


void OSSMidiScheduler_FMDevice::pitchBend(int ch, int lsb, int msb)
{
#ifdef TSE3_WITH_OSS
    _pitchBendLSB[ch] = lsb;
    _pitchBendMSB[ch] = msb;
    int n = -1;
    while ((n = voiceman.search(ch, n)) != -1)
    {
        SEQ_BENDER(deviceno, n, (lsb&0x7f)|(msb<<7));
    }
#else
    ch  = ch;
    lsb = lsb;
    msb = msb;
#endif
}


/******************************************************************************
 * OSSMidiScheduler_AWEDevice class
 *****************************************************************************/

OSSMidiScheduler_AWEDevice::OSSMidiScheduler_AWEDevice(
    int             deviceno,
    synth_info     &synthinfo,
    int             seqfd,
    unsigned char *&_seqbuf,
    int            &_seqbuflen,
    int            &_seqbufptr)
: OSSMidiScheduler_SynthDevice(deviceno, synthinfo, seqfd, _seqbuf,
                               _seqbuflen, _seqbufptr)
{
#ifdef TSE3_WITH_OSS
    // Happily, if you switch -pedantic on in gcc these macros will always
    // throw up warnings. Marvel at the beauty of awe_voice.h for reasons why.

    AWE_SET_CHANNEL_MODE(deviceno, AWE_PLAY_MULTI);
    AWE_SET_CHANNEL_MODE(deviceno, 1);
    AWE_DRUM_CHANNELS(deviceno, 1<<9);
    AWE_TERMINATE_ALL(deviceno);
    seqbuf_dump();
#endif
}


OSSMidiScheduler_AWEDevice::~OSSMidiScheduler_AWEDevice()
{
}


void OSSMidiScheduler_AWEDevice::noteOff(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    SEQ_STOP_NOTE(deviceno, ch, note, vel);
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_AWEDevice::noteOn(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    if (vel == 0)
    {
        SEQ_STOP_NOTE(deviceno, ch, note, vel);
    }
    else
    {
        SEQ_START_NOTE(deviceno, ch, note, vel);
    }
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_AWEDevice::keyPressure(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    SEQ_KEY_PRESSURE(deviceno, ch, note, vel);
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_AWEDevice::controlChange(int ch, int ctrl, int val)
{
#ifdef TSE3_WITH_OSS
    SEQ_CONTROL(deviceno, ch, ctrl, val);
#else
    ch   = ch;
    ctrl = ctrl;
    val  = val;
#endif
}


void OSSMidiScheduler_AWEDevice::programChange(int ch, int prog)
{
#ifdef TSE3_WITH_OSS
    _programChange[ch] = prog;
    SEQ_SET_PATCH(deviceno, ch, prog);
#else
    ch   = ch;
    prog = prog;
#endif
}


void OSSMidiScheduler_AWEDevice::channelPressure(int ch, int vel)
{
#ifdef TSE3_WITH_OSS
    _channelPressure[ch] = vel;
    SEQ_CHN_PRESSURE(deviceno, ch, vel);
#else
    ch   = ch;
    vel  = vel;
#endif
}


void OSSMidiScheduler_AWEDevice::pitchBend(int ch, int lsb, int msb)
{
#ifdef TSE3_WITH_OSS
    _pitchBendLSB[ch] = lsb;
    _pitchBendMSB[ch] = msb;
    SEQ_BENDER(deviceno, ch, (lsb&0x7f)|(msb<<7));
#else
    ch  = ch;
    lsb = lsb;
    msb = msb;
#endif
}


/******************************************************************************
 * OSSMidiScheduler_GUSDevice class
 *****************************************************************************/

OSSMidiScheduler_GUSDevice::OSSMidiScheduler_GUSDevice(
    int             deviceno,
    synth_info     &synthinfo,
    int             seqfd,
    unsigned char *&_seqbuf,
    int            &_seqbuflen,
    int            &_seqbufptr)
: OSSMidiScheduler_SynthDevice(deviceno, synthinfo, seqfd, _seqbuf,
                               _seqbuflen, _seqbufptr),
  voiceman(synthinfo.nr_voices), nobits(16),
  totalMemory(0)
{
#ifdef TSE3_WITH_OSS
    for (size_t n = 0; n < 256; ++n)
    {
        patchLoaded[n]       = false;
        patchLoadedFailed[n] = false;
    }

    // Establish how much memory is available on the synth
    ioctl(seqfd, SNDCTL_SEQ_RESETSAMPLES, &deviceno);
    totalMemory = deviceno;
    ioctl(seqfd, SNDCTL_SYNTH_MEMAVL, &totalMemory);
    freeMemory = totalMemory;

    for (int n = 0; n < synthinfo.nr_voices; ++n)
    {
        SEQ_CONTROL(deviceno, n, SEQ_VOLMODE, VOL_METHOD_LINEAR);
        // stop notes for all channels
    }

    // Patches are loaded on demand, so we don't need to initialise them here
#endif
}


OSSMidiScheduler_GUSDevice::~OSSMidiScheduler_GUSDevice()
{
}


std::string OSSMidiScheduler_GUSDevice::_patchesDirectory = DEF_PATH;


void OSSMidiScheduler_GUSDevice::setPatchesDirectory(const std::string &dir)
{
    if (0) std::cerr << "GUS: Setting patches dir to " << dir << "\n";
    _patchesDirectory = dir;
}


int OSSMidiScheduler_GUSDevice::getPatch(int patchNo)
{
    if (patchLoaded[patchNo] || loadPatch(patchNo)) return patchNo;
    patchNo = (patchNo < 128) ? 0 : 128;
    while (patchNo < 256 && !patchLoaded[patchNo]) ++patchNo;
    return patchNo;
}


char *OSSMidiScheduler_GUSDevice::patchName(int pgm)
{
#ifdef TSE3_WITH_OSS
    if (pgm > 0 && pgm < 256)
    {
        return GUSDevice_PatchNames[pgm];
    }
    else
#else
    pgm = pgm;
#endif
    {
        return 0;
    }
}


bool OSSMidiScheduler_GUSDevice::loadPatch(int pgm)
{
#ifdef TSE3_WITH_OSS
    if (patchLoaded[pgm])
    {
        std::cerr << "TSE3: (OSS) Attempting to reload a patch. Oops!\n";
        return false;
    }
    if (!patchName(pgm) || !(patchName(pgm)[0]))
    {
        std::cerr << "TSE3: (OSS) No GUS name for this patch. Oops!\n";
        patchLoadedFailed[pgm] = true;
        return false;
    }

    // Find the patche file
    std::string leaf = std::string(patchName(pgm)) + ".pat";
    FILE *f = findFileInPaths(leaf, _patchesDirectory);
    if (!f) std::cerr << "TSE3: (OSS) Opening GUS patch file failed\n";
    if (!f) patchLoadedFailed[pgm] = true;
    if (!f) return false;
    if (0) std::cerr << "Success: loading patch \n";

    // Read file
    unsigned char tmp[256];
    if (fread(tmp, 1, 0xef, f) != 0xef)
    {
        fclose(f);
        std::cerr << "TSE3: (OSS) GUS file was trucated.\n";
        patchLoadedFailed[pgm] = true;
        return false;
    }

    // Patch header
    pat_header header;
    memcpy((char *) &header, tmp, sizeof(header));
    if (strncmp(header.magic, "GF1PATCH110", 12))
    {
        fclose(f);
        std::cerr << "TSE3: (OSS) GUS file is corrupt\n";
        patchLoadedFailed[pgm] = true;
        return false;
    }
    if (!strncmp(header.version, "ID#0000002", 10))
    {
        fclose(f);
        std::cerr << "TSE3: (OSS) GUS file version unknown\n";
        patchLoadedFailed[pgm] = true;
        return false;
    }

    // Load patch waves
    unsigned short nWaves = *(unsigned short *)&tmp[85];
    size_t offset = 0xef;
    for (size_t i = 0; i < nWaves; ++i)
    {
        fseek(f, offset, SEEK_SET);

        sample_header sample;
        if (fread(tmp, 1, sizeof(sample), f) != sizeof(sample))
        {
            fclose(f);
            std::cerr << "TSE3: (OSS) GUS file is truncated in waves\n";
            patchLoadedFailed[pgm] = true;
            return false;
        }
        memcpy((char *) &sample, tmp, sizeof(sample));
        sample.fractions     = (char)tmp[7];
        sample.len           = get_dint(&tmp[8]);
        sample.loop_start    = get_dint(&tmp[12]);
        sample.loop_end      = get_dint(&tmp[16]);
        sample.base_freq     = get_word(&tmp[20]);
        sample.low_note      = get_dint(&tmp[22]);
        sample.high_note     = get_dint(&tmp[26]);
        sample.base_note     = get_dint(&tmp[30]);
        sample.detune        = (short)get_word(&tmp[34]);
        sample.panning       = (unsigned char) tmp[36];

        memcpy(sample.envelope_rate,   &tmp[37], 6);
        memcpy(sample.envelope_offset, &tmp[43], 6);

        sample.tremolo_sweep   = (unsigned char) tmp[49];
        sample.tremolo_rate    = (unsigned char) tmp[50];
        sample.tremolo_depth   = (unsigned char) tmp[51];

        sample.vibrato_sweep   = (unsigned char) tmp[52];
        sample.vibrato_rate    = (unsigned char) tmp[53];
        sample.vibrato_depth   = (unsigned char) tmp[54];
        sample.modes           = (unsigned char) tmp[55];
        sample.scale_frequency = (short)get_word(&tmp[56]);
        sample.scale_factor    = get_word(&tmp[58]);

        offset = offset + 96;

        char *patch_char  = new char[sizeof(patch_info)+sample.len];
        patch_info *patch = reinterpret_cast<patch_info *>(patch_char);

        patch->key        = GUS_PATCH;
        patch->device_no  = deviceno;
        patch->instr_no   = pgm;
        patch->mode       = sample.modes | WAVE_TREMOLO
                          | WAVE_VIBRATO | WAVE_SCALE;
        patch->len        = sample.len;
        patch->loop_start = sample.loop_start;
        patch->loop_end   = sample.loop_end;
        patch->base_note  = sample.base_note;
        patch->high_note  = sample.high_note;
        patch->low_note   = sample.low_note;
        patch->base_freq  = sample.base_freq;
        patch->detuning   = sample.detune;
        patch->panning    = (sample.panning - 7) * 16;

        memcpy(patch->env_rate,   sample.envelope_rate,   6);
        memcpy(patch->env_offset, sample.envelope_offset, 6);

        patch->tremolo_sweep   = sample.tremolo_sweep;
        patch->tremolo_rate    = sample.tremolo_rate;
        patch->tremolo_depth   = sample.tremolo_depth;

        patch->vibrato_sweep   = sample.vibrato_sweep;
        patch->vibrato_rate    = sample.vibrato_rate;
        patch->vibrato_depth   = sample.vibrato_depth;

        patch->scale_frequency = sample.scale_frequency;
        patch->scale_factor    = sample.scale_factor;

        patch->volume          = header.master_volume;

        if (fseek(f, offset, 0) == -1)
        {
            fclose(f);
            std::cerr << "TSE3: (OSS) GUS file seek error\n";
            patchLoadedFailed[pgm] = true;
            return false;
        }

        if ((long)fread(patch->data, 1, sample.len, f) != sample.len)
        {
            fclose(f);
            std::cerr << "TSE3: (OSS) GUS truncation after waves\n";
            patchLoadedFailed[pgm] = true;
            return false;
        }

        SEQ_WRPATCH(patch, sizeof(*patch) + sample.len);
        offset += sample.len;

        delete patch_char;
    }
    patchLoaded[pgm] = true;

    fclose(f);
    freeMemory = deviceno;
    ioctl(seqfd, SNDCTL_SYNTH_MEMAVL, &freeMemory);
    return true;
#else
    pgm = pgm;
    return false;
#endif
}


void OSSMidiScheduler_GUSDevice::noteOff(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    int n = -1;
    while ((n = voiceman.search(ch, note, n)) != -1)
    {
        SEQ_STOP_NOTE(deviceno, n, note, vel);
        voiceman.deallocate(n);
    }
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_GUSDevice::noteOn(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    if (vel == 0)
    {
        noteOff(ch, note, vel);
        return;
    }

    if ((ch == 9 && !patchLoaded[note+128]) || !patchLoaded[_programChange[ch]])
    {
        if (0) std::cerr << "no patch loaded - I'll use the nearest\n";
        //return;
    }
    int voice = voiceman.allocate(ch, note);
    if (ch == 9)
    {
        SEQ_SET_PATCH(deviceno, voice, getPatch(note+128));
    }
    else
    {
        SEQ_SET_PATCH(deviceno, voice, getPatch(_programChange[ch]));
    }
    SEQ_BENDER(deviceno, voice,
               (_pitchBendLSB[ch]&0x7f)|(_pitchBendMSB[ch]<<7));
    SEQ_START_NOTE(deviceno, voice, note, vel);
    //SEQ_CONTROL(deviceno, voice, CTL_MAIN_VOLUME, something);
    SEQ_CHN_PRESSURE(deviceno, voice, _channelPressure[ch]);
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_GUSDevice::keyPressure(int ch, int note, int vel)
{
#ifdef TSE3_WITH_OSS
    if (ch == 9) return; // don't do this for percussion XXX WHY NOT???
    int n = -1;
    while ((n = voiceman.search(ch, note, n)) != -1)
    {
        SEQ_KEY_PRESSURE(deviceno, n, note, vel);
    }
#else
    ch   = ch;
    note = note;
    vel  = vel;
#endif
}


void OSSMidiScheduler_GUSDevice::controlChange(int ch, int ctrl, int val)
{
#ifdef TSE3_WITH_OSS
    int n = -1;
    while ((n = voiceman.search(ch, n)) != -1)
    {
        SEQ_CONTROL(deviceno, n, ctrl, val);
    }
#else
    ch   = ch;
    ctrl = ctrl;
    val  = val;
#endif
}


void OSSMidiScheduler_GUSDevice::programChange(int ch, int prog)
{
    if (ch == 9)
    {
        //if (!patchLoaded[prog]) return;
        return;
    }
    _programChange[ch] = prog;
}


void OSSMidiScheduler_GUSDevice::channelPressure(int ch, int vel)
{
#ifdef TSE3_WITH_OSS
    _channelPressure[ch] = vel;
    int n = -1;
    while ((n = voiceman.search(ch, n)) != -1)
    {
        SEQ_CHN_PRESSURE(deviceno, n, vel);
    }
#else
    ch   = ch;
    vel  = vel;
#endif
}


void OSSMidiScheduler_GUSDevice::pitchBend(int ch, int lsb, int msb)
{
#ifdef TSE3_WITH_OSS
    _pitchBendLSB[ch] = lsb;
    _pitchBendMSB[ch] = msb;
    int n = -1;
    while ((n = voiceman.search(ch, n)) != -1)
    {
        SEQ_BENDER(deviceno, n, (lsb&0x7f)|(msb<<7));
    }
#else
    ch  = ch;
    lsb = lsb;
    msb = msb;
#endif
}


/******************************************************************************
 * OSSMidiScheduler class
 *****************************************************************************/

OSSMidiScheduler::OSSMidiScheduler()
: seqfd(-1), lastTxTime(0)
{
#ifdef TSE3_WITH_OSS
    // What SEQ_DEFINEBUF would have done
    _seqbuflen = 1024;
    _seqbuf    = new unsigned char[_seqbuflen]; // XXX this must be tidied later
    _seqbufptr = 0;

    // Open /dev/sequencer and work out what we're attached to
    seqfd = open("/dev/sequencer", O_RDWR|O_NONBLOCK);
    if (seqfd == -1)
    {
        throw TSE3::MidiSchedulerError(TSE3::MidiSchedulerCreateErr);
    }
    int i = 0;
    if (ioctl(seqfd, SNDCTL_MIDI_PRETIME, &i) == -1)
    {
        perror("SNDCTL_MIDI_PRETIME");
    }

    if (ioctl(seqfd, SNDCTL_SEQ_NRSYNTHS, &nosynths))
    {
        // there is no soundcard
        throw TSE3::MidiSchedulerError(TSE3::MidiSchedulerCreateErr);
    }
    ioctl(seqfd, SNDCTL_SEQ_NRMIDIS, &nomidis);
    rate = 0;
    ioctl(seqfd, SNDCTL_SEQ_CTRLRATE, &rate);
    if (rate == -1 || rate <= 0) rate = 100;
    rateDivisor = 1000/rate;
    nodevices = nosynths + nomidis;

    synthinfo  = new synth_info[nosynths];
    midiinfo   = new midi_info[nomidis];
    devices    = new OSSMidiScheduler_SynthDevice*[nosynths];
    running    = new unsigned char[nodevices];
    useRunning = new bool[nodevices];

    for (size_t n = 0; n < nodevices; n++)
    {
        running[n] = 0;
        useRunning[n] = true;
    }

    const int verbose = 0;
    if (verbose) std::cout << nosynths << " synth devices available:\n";
    for (unsigned int n = 0; n < nosynths; ++n)
    {
        synthinfo[n].device = n;
        if (ioctl(seqfd, SNDCTL_SYNTH_INFO, &synthinfo[n]) != -1)
        {
            if (verbose)
            {
                std::cout << "Synth device " << n << ": " << synthinfo[n].name
                          << "(";
                switch (synthinfo[n].synth_type)
                {
                    case SYNTH_TYPE_FM:     std::cout << "FM\n";     break;
                    case SYNTH_TYPE_SAMPLE: std::cout << "sample\n"; break;
                    case SYNTH_TYPE_MIDI:   std::cout << "MIDI\n";   break;
                    default:                std::cout << "?\n";      break;
                }
                std::cout << "-";
                switch (synthinfo[n].synth_subtype)
                {
                    case FM_TYPE_ADLIB:    std::cout << "Adlib";   break;
                    case FM_TYPE_OPL3:     std::cout << "OPL3";    break;
                    case MIDI_TYPE_MPU401: std::cout << "MPU-401"; break;
                    case SAMPLE_TYPE_GUS:  std::cout << "GUS";     break;
                    default:               std::cout << "?";       break;
                }
                std::cout << ")\n";
            }
            if (synthinfo[n].synth_type == SYNTH_TYPE_SAMPLE
                && synthinfo[n].synth_subtype == SAMPLE_TYPE_AWE32)
            {
                devices[n]
                    = new OSSMidiScheduler_AWEDevice(n, synthinfo[n], seqfd,
                                                     _seqbuf, _seqbuflen,
                                                     _seqbufptr);
                if (verbose) std::cout << " -- awe type device\n";
            }
            else if (synthinfo[n].synth_type == SYNTH_TYPE_SAMPLE
                     && synthinfo[n].synth_subtype == SAMPLE_TYPE_GUS)
            {
                devices[n]
                    = new OSSMidiScheduler_GUSDevice(n, synthinfo[n], seqfd,
                                                     _seqbuf, _seqbuflen,
                                                     _seqbufptr);
                if (verbose) std::cout << " -- gus type device\n";
            }
            else if (synthinfo[n].synth_type == SYNTH_TYPE_FM
                )//|| synthinfo[n].synth_subtype == FM_TYPE_OPL3)
            {
                devices[n]
                    = new OSSMidiScheduler_FMDevice(n, synthinfo[n], seqfd,
                                                    _seqbuf, _seqbuflen,
                                                    _seqbufptr);
                if (verbose) std::cout << " -- fm type device\n";
            }
            else
            {
                devices[n]
                    = new OSSMidiScheduler_NULLDevice(n, synthinfo[n], seqfd,
                                                      _seqbuf, _seqbuflen,
                                                      _seqbufptr);
                if (verbose) std::cout << " -- null type device\n";
            }
        }
    }
    if (verbose) std::cout << "\n" << nomidis << " MIDI devices available:\n";
    for (unsigned int n = 0; n < nomidis; ++n)
    {
       midiinfo[n].device = n;
       if (ioctl(seqfd, SNDCTL_MIDI_INFO, &midiinfo[n]) != -1)
       {
           if (verbose)
               std::cout << "MIDI device: " << n << ": " << midiinfo[n].name
                         << "(" << midiinfo[n].dev_type << ")\n";
           if (!std::strcmp(midiinfo[n].name, "AWE Midi Emu"))
           {
               useRunning[n] = false;
               if (verbose)
                   std::cout << "  -- this device has broken OSS running "
                             << "status support. Disabling running status\n";
           }
       }
    }

    // Now let the public API know about these devices
    for (unsigned int n = 0; n < nodevices; ++n)
    {
        addPort(n, n >= nosynths, n); // XXX isInternal correct?
    }

    ioctl(seqfd, SNDCTL_SEQ_RESET);
    // XXX perhaps send a panic on all devices?
#else
    throw TSE3::MidiSchedulerError(MidiSchedulerCreateErr);
#endif
}


OSSMidiScheduler::~OSSMidiScheduler()
{
    // if playing, stop first!
    if (MidiScheduler::running()) stop();

#ifdef TSE3_WITH_OSS
    close(seqfd);
    delete [] _seqbuf;
    delete [] midiinfo;
    delete [] synthinfo;
    for (unsigned int n = 0; n < nosynths; ++n) delete devices[n];
    delete [] devices;

    delete [] running;
    delete [] useRunning;
#endif
}


const char *OSSMidiScheduler::impl_implementationName() const
{
#ifdef TSE3_WITH_OSS
    return "OSSMidiScheduler version 1.00";
#else
    return "Unsupported OSS device. Will not work!";
#endif
}


const char *OSSMidiScheduler::impl_portName(int port) const
{
#ifdef TSE3_WITH_OSS
    if (isSynth(port))
    {
        return synthinfo[port].name;
    }
    else
    {
        return midiinfo[port-nosynths].name;
    }
#else
    port = port;
    return "None";
#endif
}


const char *OSSMidiScheduler::impl_portType(int port) const
{
#ifdef TSE3_WITH_OSS
    if (isSynth(port))
    {
        switch (synthinfo[port].synth_subtype)
        {
            case FM_TYPE_ADLIB:    return "Adlib";
            case FM_TYPE_OPL3:     return "FM";
            case MIDI_TYPE_MPU401: return "MPU 401";
            case SAMPLE_TYPE_GUS:  return "GUS";
            default:               return "Unknown";
        }
    }
    else
    {
        return "External MIDI port";
    }
#else
    port = port;
    return "None";
#endif
}


bool OSSMidiScheduler::impl_portReadable(int port) const
{
#ifdef TSE3_WITH_OSS
    return isMidi(port);
#else
    port = port;
    return false;
#endif
}


bool OSSMidiScheduler::impl_portWriteable(int /*port*/) const
{
#ifdef TSE3_WITH_OSS
    return true;
#else
    return false;
#endif
}


void OSSMidiScheduler::impl_tx(MidiCommand mc)
{
    tx(mc, true);
}


void OSSMidiScheduler::tx(MidiCommand mc, bool outOfBand)
{
#ifdef TSE3_WITH_OSS
    //std::cout << "Trying to output MIDI with " << MidiCommand_NoDataBytes[mc.status] << " data bytes: ("
    //     << mc.status << ":" << mc.channel << ":"
    //     << mc.data1 << ":" << mc.data2 << ")\n";

    if (mc.port >= (int)nodevices || mc.status == MidiCommand_Invalid) return;

    if (isMidi(mc.port))
    {
        // MIDI output: easy
        mc.port -= nosynths;
        unsigned char status = (mc.status<<4) + mc.channel;
        if (!useRunning[mc.port] || status != running[mc.port])
        {
            SEQ_MIDIOUT(mc.port, status);
            running[mc.port] = status;
        }
        SEQ_MIDIOUT(mc.port, mc.data1);
        if (MidiCommand_NoDataBytes[mc.status] == 2)
        {
            SEQ_MIDIOUT(mc.port, mc.data2);
        }
    }
    else
    {
        // Soundcard (synth) output: passed on to a handler object
        switch (mc.status)
        {
            case MidiCommand_NoteOff:
                devices[mc.port]->noteOff(mc.channel, mc.data1, mc.data2);
                break;
            case MidiCommand_NoteOn:
                devices[mc.port]->noteOn(mc.channel, mc.data1, mc.data2);
                break;
            case MidiCommand_KeyPressure:
                devices[mc.port]->keyPressure(mc.channel, mc.data1, mc.data2);
                break;
            case MidiCommand_ControlChange:
                devices[mc.port]
                    ->controlChange(mc.channel, mc.data1, mc.data2);
                break;
            case MidiCommand_ProgramChange:
                devices[mc.port]->programChange(mc.channel, mc.data1);
                break;
            case MidiCommand_ChannelPressure:
                devices[mc.port]->channelPressure(mc.channel, mc.data1);
                break;
            case MidiCommand_PitchBend:
                devices[mc.port]->pitchBend(mc.channel, mc.data1, mc.data2);
                break;
        }
    }

    if (!outOfBand)
    {
        seqbuf_dump();
    }
    else
    {
        for (int n = 0; n < _seqbufptr; n += 4)
        {
            // What happens to the race conditions *in the driver*?
            // (i.e. we're half way through sending the three MIDI bytes, and
            // a timed event goes off sending another three in the middle of
            // them)
            ioctl(seqfd, SNDCTL_SEQ_OUTOFBAND, _seqbuf+n);
        }
        seqbuf_clean();
    }
#else
    mc        = mc;
    outOfBand = outOfBand;
#endif
}


void OSSMidiScheduler::impl_start(const Clock start)
{
    lastTxTime = start;
    time       = 0;
    // set tempo ???
#ifdef TSE3_WITH_OSS
    SEQ_START_TIMER();
    seqbuf_dump();
#endif
    clockStarted(start);
}


void OSSMidiScheduler::impl_stop(Clock t)
{
    if (t != -1)
    {
#ifdef TSE3_WITH_OSS
        SEQ_WAIT_TIME(clockToMs(t)/rateDivisor);
#endif
    }
#ifdef TSE3_WITH_OSS
    SEQ_STOP_TIMER();
    seqbuf_dump();
    // *** these are probably in the wrong order
    if (t == -1)
    {
        //ioctl(seqfd, SNDCTL_SEQ_RESET);
        //ioctl(seqfd, SNDCTL_SEQ_PANIC);
    }
    //ioctl(seqfd, SNDCTL_SEQ_SYNC);
#endif
    clockStopped(t);
}


void OSSMidiScheduler::impl_moveTo(const Clock moveTime, const Clock newTime)
{
    lastTxTime = newTime;
    clockMoved(moveTime, newTime);
}


Clock OSSMidiScheduler::impl_clock()
{
    int centi = 0;
#ifdef TSE3_WITH_OSS
    ioctl(seqfd, SNDCTL_SEQ_GETTIME, &centi);
#endif
    return msToClock(centi*rateDivisor);
}


int OSSMidiScheduler::impl_msecs()
{
    int centi = 0;
#ifdef TSE3_WITH_OSS
    ioctl(seqfd, SNDCTL_SEQ_GETTIME, &centi);
#endif
    return centi * rateDivisor;
}


void OSSMidiScheduler::impl_setTempo(int newTempo, Clock changeTime)
{
#ifdef TSE3_WITH_OSS
    SEQ_SET_TEMPO(newTempo); // XXX ? is this necessary
    seqbuf_dump();
#endif

    tempoChanged(newTempo, changeTime);
}


void OSSMidiScheduler::readInput()
{
#ifdef TSE3_WITH_OSS
    static const int        inputBufferSize = 4;          // OSS input size
    static unsigned char    inputBuffer[inputBufferSize]; // OSS input buffer
    static int              lastStatusByte  = 0x90;       // running status
    static int              noDataBytesLeft = 2;          // OSS input status
    static int              noDataBytes     = 0;          // OSS input status
    static int              data[2];                      // OSS input data

    if (input) return;

    while (!input)
    {

        int out = read(seqfd, &inputBuffer, inputBufferSize);
        if (out < 1) return;
        if (out != 4)
        {
            std::cerr << "TSE3: (OSS) Input was not 4 bytes from OSS input. "
                      << "Arse.\n";
        }

        switch (inputBuffer[0])
        {
            case SEQ_WAIT:
                time = msToClock( ((inputBuffer[3]<<16) | (inputBuffer[2]<<8)
                                  | (inputBuffer[1])) * rateDivisor);
                break;
            case SEQ_ECHO:
                std::cerr << "TSE3: (OSS) Input SEQ_ECHO event.\n";
                break;
            case SEQ_MIDIPUTC:
                if (inputBuffer[1] & 0x80)
                {
                    // it was a status byte
                    lastStatusByte = inputBuffer[1];
                    if (inputBuffer[1] == MidiCommand_System)
                    {
                        std::cerr << "TSE3: (OSS) System byte received\n";
                    } else {
                        noDataBytesLeft
                            = MidiCommand_NoDataBytes[inputBuffer[1]>>4];
                        noDataBytes     = 0;
                    }
                }
                else
                {
                    data[noDataBytes++] = inputBuffer[1];
                    noDataBytesLeft--;
                    if (!noDataBytesLeft)
                    {
                        command = MidiCommand(lastStatusByte >> 4,
                                              lastStatusByte & 0x0f,
                                              0, data[0], data[1]);
                        input = true;
                        noDataBytesLeft
                            = MidiCommand_NoDataBytes[lastStatusByte>>4];
                        noDataBytes     = 0;
                    }
                }
                break;
        }

    }
#endif
}


bool OSSMidiScheduler::impl_eventWaiting()
{
    readInput();
    return input;
}


MidiEvent OSSMidiScheduler::impl_rx()
{
    readInput();
    if (!input) return MidiEvent();
    MidiEvent e(command, time);
    input = false;
    return e;
}


void OSSMidiScheduler::impl_tx(MidiEvent e)
{
#ifdef TSE3_WITH_OSS
    if (e.time > lastTxTime)
    {
        SEQ_WAIT_TIME(clockToMs(e.time)/rateDivisor);
        lastTxTime = e.time;
    }
    tx(e.data, false);
#else
    e = e;
#endif
}


void OSSMidiScheduler::impl_txSysEx(int /*port*/,
                                    const unsigned char * /*data*/,
                                    size_t /*size*/)
{
    // TODO XXX
    std::cerr << "TSE3: (OSS) OSSMidiScheduler::txSysEx not implemented\n";
}


void OSSMidiScheduler::seqbuf_dump()
{
#ifdef TSE3_WITH_OSS
    // This is canonical OSS code: after OSS macros are used to fill the
    // buffer defined by SEQ_DEFINEBUF (or rather, our class private version
    // of it)
    if (_seqbufptr)
        if (write(seqfd, _seqbuf, _seqbufptr) == -1)
            perror("Can't write to MIDI device");
    _seqbufptr = 0;
#endif
}


void OSSMidiScheduler::seqbuf_clean()
{
    _seqbufptr = 0;
}


std::string &OSSMidiScheduler::fmPatchesDirectory()
{
    return OSSMidiScheduler_FMDevice::patchesDirectory();
}


void OSSMidiScheduler::setFmPatchesDirectory(const std::string &dir)
{
    OSSMidiScheduler_FMDevice::setPatchesDirectory(dir);
}


std::string &OSSMidiScheduler::gusPatchesDirectory()
{
    return OSSMidiScheduler_GUSDevice::patchesDirectory();
}


void OSSMidiScheduler::setGusPatchesDirectory(const std::string &dir)
{
    OSSMidiScheduler_GUSDevice::setPatchesDirectory(dir);
}

