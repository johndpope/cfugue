/*
 * @(#)DisplayParams.cpp 3.00 26 June 1999
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

#include "tse3/DisplayParams.h"

#include "tse3/FileBlockParser.h"
#include "tse3/Mutex.h"

#include <sstream>

using namespace TSE3;

namespace
{
    const char *presetStrings[] =
    {
        "Intro",
        "Verse",
        "Chorus",
        "Bridge",
        "Coda",
        "Refrain",
        "Melody",
        "Solo",
        "Backing",
        "Brass",
        "Percussion",
        "Drums",
        "Guitar",
        "Bass",
        "Flute",
        "Strings",
        "Keyboard",
        "Piano",
        "Saxophone"
    };
}

/******************************************************************************
 * DisplayParams class
 *****************************************************************************/

DisplayParams::DisplayParams()
: _style(Default), _r(0xff), _g(0xff), _b(0xff), _preset(Verse)
{
}


DisplayParams::DisplayParams(const DisplayParams &p)
: Notifier<DisplayParamsListener>(), Serializable(),
  _style(p._style), _r(p._r), _g(p._g), _b(p._b), _preset(p._preset)
{
}


DisplayParams &DisplayParams::operator=(const DisplayParams &p)
{
    Impl::CritSec cs;

    _style  = p._style;
    _r      = p._r;
    _g      = p._g;
    _b      = p._b;
    _preset = p._preset;

    notify(&DisplayParamsListener::DisplayParams_Altered);
    return *this;
}


DisplayParams::~DisplayParams()
{
}


const char *DisplayParams::presetColourString(int pc)
{
    return (pc >= 0 && pc < NoPresetColours) ? presetStrings[pc] : 0;
}


void DisplayParams::setStyle(int s)
{
    if (s != _style)
    {
        _style = s;
        notify(&DisplayParamsListener::DisplayParams_Altered);
    }
}


void DisplayParams::setColour(int r, int g, int b)
{
    Impl::CritSec cs;

    bool donotify = false;

    if (r != _r)
    {
        _r = r;
        donotify = true;
    }
    if (g != _g)
    {
        _g = g;
        donotify = true;
    }
    if (b != _b)
    {
        _b = b;
        donotify = true;
    }

    if (donotify)
    {
        notify(&DisplayParamsListener::DisplayParams_Altered);
    }
}


void DisplayParams::setPresetColour(int pc)
{
    Impl::CritSec cs;

    if (pc != _preset && pc >= 0 && pc < NoPresetColours)
    {
        _preset = pc;
        notify(&DisplayParamsListener::DisplayParams_Altered);
    }
}


/******************************************************************************
 * DisplayParams Serializable interface
 *****************************************************************************/

void DisplayParams::save(std::ostream &o, int i) const
{
    o << indent(i)   << "{\n";
    o << indent(i+1) << "Style:" << _style << "\n";
    o << indent(i+1) << "Colour:" << _r << "," << _g << "," << _b << "\n";
    if (_style == PresetColour)
    {
        o << indent(i+1) << "Preset:" << presetColourString(_preset) << "\n";
    }
    o << indent(i)   << "}\n";
}


namespace
{
    /**
     * A FileItemParser to handle colour data items.
     */
    class FileItemParser_Colour : public FileItemParser
    {
        public:
            FileItemParser_Colour(DisplayParams *d)
            : d(d) {}
            virtual ~FileItemParser_Colour() {}
            virtual void parse(const std::string &line)
            {
                int r, g, b;
                std::istringstream si(line);
                si >> r;
                si.ignore(1); // skip comma
                si >> g;
                si.ignore(1); // skip comma
                si >> b;
                d->setColour(r, g, b);
            }
        private:
            DisplayParams *d;
    };

    /**
     * A FileItemParser to handle PresetColour data items.
     */
    class FileItemParser_PresetColour : public FileItemParser
    {
        public:
            FileItemParser_PresetColour(DisplayParams *d)
            : d(d) {}
            virtual ~FileItemParser_PresetColour() {}
            virtual void parse(const std::string &line)
            {
                int pr = 0;
                while (pr < DisplayParams::NoPresetColours
                       && line != presetStrings[pr])
                {
                    ++pr;
                }

                if (pr < DisplayParams::NoPresetColours)
                {
                }
                else
                {
                    std::istringstream si(line);
                    si >> pr;
                }

                d->setPresetColour(pr);
            }
        private:
            DisplayParams *d;
    };
}


void DisplayParams::load(std::istream &in, SerializableLoadInfo &info)
{
    FileItemParser_Number<DisplayParams> s(this, &DisplayParams::setStyle);
    FileItemParser_Colour                c(this);
    FileItemParser_PresetColour          p(this);
    FileBlockParser parser;
    parser.add("Style",  &s);
    parser.add("Colour", &c);
    parser.add("Preset", &p);
    parser.parse(in, info);
}


/******************************************************************************
 * PresetColours class
 *****************************************************************************/

namespace
{
    struct colour { int r, g, b; };
    colour default_colours[] =
    {
        {0x8f, 0xbc, 0x8f}, // Intro
        {0x66, 0x99, 0xcc}, // Verse
        {0xdd, 0xad, 0xdd}, // Chorus
        {0xdd, 0xd7, 0x6b}, // Bridge
        {0x6e, 0x8b, 0x3d}, // Coda
        {0x89, 0x68, 0xcd}, // Refrain
        {0xe0, 0xe8, 0xff}, // Melody
        {0xe4, 0x57, 0x37}, // Solo
        {0xd8, 0xff, 0xa0}, // Backing

        {0xff, 0xa8, 0x58}, // Brass
        {0x7e, 0xc0, 0xee}, // Percussion
        {0x9f, 0xb6, 0xcd}, // Drums
        {0x9a, 0xcd, 0x32}, // Guitar
        {0x7a, 0x9d, 0x22}, // Bass
        {0xee, 0x82, 0xee}, // Flute
        {0xcd, 0xcd, 0x00}, // Strings
        {0x96, 0xcd, 0xcd}, // Keyboard
        {0xcd, 0xc0, 0xb0}, // Piano
        {0xde, 0xb8, 0x87}  // Saxophone
    };
}

PresetColours::PresetColours()
{
    for (int n = 0; n < DisplayParams::NoPresetColours; ++n)
    {
        _r[n] = default_colours[n].r;
        _g[n] = default_colours[n].g;
        _b[n] = default_colours[n].b;
    }
}

void PresetColours::setColour(int preset, int r, int g, int b)
{
    if (preset >= 0 && preset < DisplayParams::NoPresetColours)
    {
        Impl::CritSec cs;

        bool donotify = false;

        if (r != _r[preset])
        {
            _r[preset] = r;
            donotify = true;
        }
        if (g != _g[preset])
        {
            _g[preset] = g;
            donotify = true;
        }
        if (b != _b[preset])
        {
            _b[preset] = b;
            donotify = true;
        }

        if (donotify)
        {
            notify(&PresetColoursListener::PresetColours_Altered, preset);
        }
    }
}

