/*
 * @(#)Instrument.h 3.00 3 August 1999
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

#include "tse3/ins/Instrument.h"

#include "tse3/Progress.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace TSE3;
using namespace TSE3::Ins;

namespace
{
    /**
     * The .ins file comment divider. We use this when we write .ins files.
     *
     * This is the standard divider from one of Cakewalk's files.
     */
    const char *ins_divider = "; ----------------------------------"
                              "------------------------------------\n";

    /**
     * .ins files come from Windows/MS-DOS. They have ASCII 13 at the ends of
     * the lines. We need to strip them out before we handle the line. That's
     * what this we functon does.
     *
     * We also need to strip comments from lines. So we do that here too.
     */
    void clean_string(std::string &str)
    {
        if (str[str.length()-1] == 13) // for empty lines?
        {
            str = str.substr(0, str.length()-1);
        }
        if (str.find(';') != str.npos)
        {
            str = str.substr(0,str.find(';'));
        }
    }
}


/******************************************************************************
 * Voice class
 *****************************************************************************/

Voice::Voice(int bank, int patch)
{
    first  = bank;
    second = patch;
}


Voice::Voice(int bankMSB, int bankLSB, int patch)
{
    first  = (bankMSB << 7) + bankLSB;
    second = patch;
}


int Voice::operator <(const Voice &v) const
{
    if (first != v.first)
    {
        return first < v.first;
    }
    else
    {
        return second < v.second;
    }
}


/******************************************************************************
 * Instrument class
 *****************************************************************************/

Instrument::Instrument(const std::string &title, const std::string &filename,
                       TSE3::Progress *p)
: _title(title), _filename(filename),
  _bankSelMethod(0), _useNotesAsControllers(false),
  _control(0), _rpn(0), _nrpn(0)
{
    std::ifstream in(filename.c_str());
    if (in.good())
    {
        load(in, p);
    }
}


void Instrument::setTitle(const std::string &n)
{
    _title = n;
}


void Instrument::setBankSelMethod(int b)
{
    if (b <= BankSelMethod_Normal || b >= BankSelMethod_Patch)
        return;
    _bankSelMethod = b;
}


void Instrument::setUseNotesAsControllers(bool u)
{
    _useNotesAsControllers = u;
}


int Instrument::bankLSB(int index) const
{
    return banks[index] == -1 ? -1 : banks[index] & 0xff;
}


int Instrument::bankMSB(int index) const
{
    return banks[index] == -1 ? -1 : banks[index] >> 7;
}


int Instrument::bank(const Voice &/*voice*/) const
{
    return -2;
    // XXX
//#warning This is not implemented
}


PatchData *Instrument::patchForBank(int bank) const
{
    std::vector<int>::const_iterator i
        = std::find(banks.begin(), banks.end(), bank);

    if (i == banks.end() && bank != -1)
    {
        // Ok, so that failed. Perhaps there's a catch-all patch set?
        i = std::find(banks.begin(), banks.end(), -1);
    }

    if (i != banks.end())
    {
        return patches[i-banks.begin()];
    }
    else
    {
        return 0;
    }
}


PatchData *Instrument::patchForBank(int bankLSB, int bankMSB) const
{
    return patchForBank(bankFromBytes(bankLSB, bankMSB));
}


NoteData *Instrument::keyForVoice(const Voice &voice) const
{
    std::vector<std::pair<Voice, NoteData*> >::const_iterator i = keys.begin();
    while (i != keys.end() && i->first != voice) i++;

    if (i != keys.end())
    {
        return i->second;
    }
    else
    {
        return 0;
    }
}


bool Instrument::isDrum(const Voice &voice) const
{
    return
        std::find(drumFlags.begin(), drumFlags.end(), voice) != drumFlags.end();
}


/******************************************************************************
 * Instrument class saving/loading
 *****************************************************************************/

void Instrument::load(std::istream &in, TSE3::Progress *progress)
{
    if (progress)
    {
        progress->progressRange(0, 100);
        progress->progress(0);
    }

    // 1. Find the ".Instrument Definitions" line (0% - 10%)

    in.seekg(0, std::ios::beg);
    std::string line;
    while (!in.eof() && line != ".Instrument Definitions")
    {
        std::getline(in, line);
        clean_string(line);
    }
    if (line != ".Instrument Definitions") return;
    if (progress)
    {
        progress->progress(10);
    }

    // 2. Find the instrument definition section (10% - 20%)

    std::string matchstr = std::string("[") + _title + std::string("]");
    while (!in.eof() && line != matchstr)
    {
        std::getline(in, line);
        clean_string(line);
    }
    if (progress)
    {
        progress->progress(20);
    }

    // 3. Parse each of the bits in it (20% - 100%)

    std::streampos defnFilePos = in.tellg();
    std::streampos defnEndPos  = defnFilePos;
    if (progress)
    {
        // Find how big this section is for %age
        while (!in.eof() && line.size() != 0)
        {
            std::getline(in, line);
            clean_string(line);
            if (line[0] == '[') line ="";
        }
        defnEndPos = in.tellg();
        in.seekg(defnFilePos, std::ios::beg);
    }

    line = " ";
    while (!in.eof() && line.size() != 0)
    {
        if (progress)
        {
            progress->progress(20
                               + ((in.tellg()-defnFilePos) * 80
                                  / (defnEndPos - defnFilePos)));
        }
        std::getline(in, line);
        clean_string(line);
        if (line[0] != '[')
        {
            parseLine(line, in);
        }
        else
        {
            line = "";
        }
    }
    if (progress)
    {
        progress->progress(100);
    }
}


void Instrument::parseLine(const std::string &line, std::istream &in)
{
    if (line == "UseNotesAsControllers=1")
    {
        _useNotesAsControllers = true;
    }
    else if (line.substr(0,8) == "Control=")
    {
        std::string title(line.substr(8));
        delete _control;
        _control = new ControlData(title, in);
    }
    else if (line.substr(0,4) == "RPN=")
    {
        std::string title(line.substr(4));
        delete _rpn;
        _rpn = new RpnData(title, in);
    }
    else if (line.substr(0,5) == "NRPN=")
    {
        std::string title(line.substr(5));
        delete _nrpn;
        _nrpn = new NrpnData(title, in);
    }
    else if (line.substr(0,14) == "BankSelMethod=")
    {
        std::istringstream si(line.c_str()+14);
        si >> _bankSelMethod;
    }
    else if (line.substr(0,6) == "Patch[")
    {
        std::string bank_s = line.substr(6, line.find(']')-6);
        int bank = -1;
        if (bank_s != "*")
        {
            std::istringstream si(line.c_str()+6);
            si >> bank;
        }
        std::string title(line.substr(line.find('=')+1));
        banks.push_back(bank);
        patches.push_back(new PatchData(title, in));
    }
    else if (line.substr(0,4) == "Key[")
    {
        std::string bank_s(line.substr(4,line.find(',')-4));
        int a = line.find(',')+1;
        std::string patch_s(line.substr(a, line.find(']')-a));
        int bank = -1, patch = -1;
        if (bank_s != "*")
        {
            std::istringstream si(bank_s);
            si >> bank;
        }
        if (patch_s != "*")
        {
            std::istringstream si(patch_s);
            si >> patch;
        }
        std::string title(line.substr(line.find('=')+1));

        // This has been split onto a separate line to keep gcc happy
        std::pair<Voice, NoteData *> pr
            (Voice(bank,patch), new NoteData(title, in));
        keys.push_back(pr);
    }
    else if (line.substr(0,5) == "Drum[")
    {
        std::string bank_s(line.substr(5,line.find(',')-5));
        int a = line.find(',')+1;
        std::string patch_s(line.substr(a, line.find(']')-a));
        int bank = -1, patch = -1;
        if (bank_s != "*")
        {
            std::istringstream si(bank_s);
            si >> bank;
        }
        if (patch_s != "*")
        {
            std::istringstream si(patch_s);
            si >> patch;
        }
        std::string title(line.substr(line.find('=')+1));
        drumFlags.push_back(Voice(bank,patch));
    }
}


void Instrument::write(std::ostream &out)
{
    out << "\n"
        << ins_divider
        << "; Instrument definition file save by TSE3 library\n"
        << "; Defines the " << _title << " instrument only\n"
        << "; Pete Goodliffe\n\n";

    out << ins_divider << "\n.Patch Names\n\n";
    {
        std::vector<PatchData*>::iterator ip = patches.begin();
        while (ip != patches.end())
        {
            (*ip)->write(out);
            ++ip;
        }
    }

    out << ins_divider << "\n.Note Names\n\n";
    {
        std::vector<std::pair<Voice,NoteData*> >::iterator i = keys.begin();
        while (i != keys.end())
        {
            (*i).second->write(out);
            ++i;
        }
    }

    out << ins_divider << "\n.Controller Names\n\n";
    if (_control) _control->write(out);

    out << ins_divider << "\n.RPN Names\n\n";
    // hmm?

    out << ins_divider << "\n.NRPN Names\n\n";
    if (_nrpn) _nrpn->write(out);

    out << ins_divider << "\n.Instrument Definitions\n\n";
    out << "[" << _title << "]\n";
    if (_useNotesAsControllers) out << "UseNotesAsControllers=1\n";
    if (_control)               out << "Control=" << _control->title() << "\n";
    if (_nrpn)                  out << "NRPN=" << _nrpn->title() << "\n";
    if (_bankSelMethod)         out << "BankSelMethod=" << _bankSelMethod
                                    << "\n";
    {
        std::vector<PatchData*>::iterator ip = patches.begin();
        std::vector<int>::iterator        ib = banks.begin();
        while (ip != patches.end())
        {
            out << "Patch[";
            if (*ib == -1)
                out << "*";
            else
                out << *ib;
            out << "]=" << (*ip)->title() << "\n";
            ++ip;
            ++ib;
        }
    }
    {
        std::vector<std::pair<Voice,NoteData*> >::iterator i = keys.begin();
        while (i != keys.end())
        {
            out << "Key[";
            if ((*i).first.bank() == -1)
                out << "*";
            else
                out << (*i).first.bank();
            out << ",";
            if ((*i).first.patch() == -1)
                out << "*";
            else
                out << (*i).first.patch();
            out << "]=" << (*i).second->title() << "\n";
            ++i;
        }
    }
    {
        std::vector<Voice>::iterator i = drumFlags.begin();
        while (i != drumFlags.end())
        {
            out << "Drum[";
            if ((*i).bank() == -1)
                out << "*";
            else
                out << (*i).bank();
            out << ",";
            if ((*i).patch() == -1)
                out << "*";
            else
                out << (*i).patch();
            out << "]=1\n";
            ++i;
        }
    }
    out << "\n";
}


/******************************************************************************
 * InstrumentData class
 *****************************************************************************/

std::string InstrumentData::empty;

InstrumentData::InstrumentData(std::string const &title,
                               std::string const &insHeading,
                               std::istream &in)
: insHeading(insHeading), _title(title)
{
    for (int n = 0; n < 128; ++n) _names[n] = 0;
    load(_title, in);
}


void InstrumentData::load(const std::string &secname, std::istream &in)
{
    //out << "Loading ["<<secname<<"] from "<<insHeading<<"\n";
    std::streampos fpos = in.tellg();
    in.seekg(0, std::ios::beg);
    std::string line;
    bool         success = false;
    while (!in.eof() && line != insHeading)
    {
        std::getline(in, line);
        clean_string(line);
    }
    if (line == insHeading)
    {
        line = "";
        std::string matchstr = "[" + secname + "]";
        while (!in.eof()
               && line != matchstr
               && (line.size() == 0 || line[0] != '.'))
        {
            std::getline(in, line);
            clean_string(line);
        }

        if (line == matchstr)
        {
            line    = "";
            success = true;
            while (!in.eof()
                   && (line.size() == 0 || (line[0] != '.' && line[0] != '[')))
            {
                std::getline(in, line);
                clean_string(line);
                if (line.substr(0, 7) == "BasedOn") load(line.substr(8), in);
                if (line.find('=') != line.npos)
                {
                    int no = 0;
                    {
                        std::istringstream si(line);
                        si >> no;
                    }
                    std::string title = line.substr(line.find('=')+1);
                    delete _names[no];
                    _names[no] = new std::string(title);
                }
            }
        }
    }
    in.seekg(fpos, std::ios::beg);
    if (!success)
    {
        std::cerr << "TSE3: Failed to load data [" << secname.c_str()
                  << "] from instrument file section "<< insHeading << "\n";
    }
}


void InstrumentData::write(std::ostream &out) const
{
    out << "[" << _title << "]\n";
    for (int n = 0; n < 128; ++n)
        if (_names[n]) out << n << "=" << *(_names[n])   << "\n";
    out << "\n";
}


/******************************************************************************
 * CakewalkInstrumentFile class
 *****************************************************************************/

CakewalkInstrumentFile::CakewalkInstrumentFile(const std::string &filename)
: filename(filename), searched_yet(false)
{
}


const std::list<std::string> &CakewalkInstrumentFile::instruments
    (TSE3::Progress *progress)
{
    if (!searched_yet)
    {
        size_t progressCount = 0;
        searched_yet = true;
        std::ifstream in(filename.c_str());
        if (!in.good())
        {
            return ins;
        }
        if (progress)
        {
            in.seekg(0, std::ios::end);
            progress->progressRange(0, in.tellg());
            in.seekg(0, std::ios::beg);
        }
        std::string line;
        while (!in.eof() && line != ".Instrument Definitions")
        {
            std::getline(in, line);
            clean_string(line);
            if (progress && !(progressCount%20))
            {
                progress->progress(in.tellg());
            }
            progressCount++;
        }
        if (line != ".Instrument Definitions") return ins;
        while (!in.eof())
        {
            std::getline(in, line);
            clean_string(line);
            if (line.size() && line[0] == '[')
            {
                ins.push_back(line.substr(1,line.size()-2));
            }
            if (progress && !(progressCount%20))
            {
                progress->progress(in.tellg());
            }
            progressCount++;
        }
    }
    return ins;
}


Instrument *CakewalkInstrumentFile::instrument(const std::string &title,
                                               TSE3::Progress *p)
{
    return new Instrument(title, filename, p);
}
