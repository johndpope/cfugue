/*
 * @(#)Notifier.cpp 3.00 13 July 2000
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

#include "tse3/Mutex.h"

#include <iostream>

using namespace TSE3::Impl;

MutexImpl *Mutex::globalImpl = 0;


Mutex::~Mutex()
{
#ifndef TSE3_WITHOUT_MUTEX
    while (impl->locked())
    {
        // If this is the case, something nasty is up!
        std::cerr << "TSE3: Mutex deleted whilst still locked\n";
        impl->unlock();
    }
    delete impl;
#endif
}


void Mutex::setImpl(MutexImpl *impl)
{
    if (!globalImpl)
    {
        globalImpl = impl;
        // OK, so there is a potential memory leak, you might specify
        // a MutexImpl created with new and then not call mutex() to create
        // a Mutex. The destructor will therefore never delete the impl.
        //
        // This is not really very likely, though, so we ignore it.
    }

#ifndef TSE3_WITHOUT_MUTEX
    std::cerr << "TSE3: *Warning* MutexImpl supplied to a TSE3 library which\n"
              << "      has been built without multi-thread support.\n"
              << "      The MutexImpl will not be used, and you may\n"
              << "      experience incorrect TSE3 behaviour in the presence\n"
              << "      of multiple threads.\n\n";
#endif

}


Mutex *Mutex::mutex()
{
    if (!globalImpl)
    {
        globalImpl = new NullMutexImpl();
    }

    static Mutex *mutex = new Mutex(globalImpl);

    return mutex;
}

