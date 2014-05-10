/*
 * @(#)TSE3.cpp 3.00 20 Jan 2000
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

#include "tse3/TSE3.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef VERSION
#define VERSION    "0.0.0 (no autoconf)"
#endif

#define COPYRIGHT  "(c) Pete Goodliffe, 2000-2002. A Trax Software Product."

using namespace TSE3;

const char *TSE3::TSE3_Version()
{
    return VERSION;
}


const char *TSE3::TSE3_Copyright()
{
    return COPYRIGHT;
}
