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

#include "tse3/Notifier.h"

#include <vector>
#include <algorithm>

using namespace TSE3;

namespace TSE3
{
    namespace Impl
    {
        /**
         * Implementation of the void_list class. This stores the void*
         * data items in a vector. We hide the vector in the file rather than
         * make it a data member of void_impl because it has been shown to
         * dramatically reduce the size of the TSE3 library.
         */
        class void_list::impl
        {
            public:
                std::vector<void*> vec;
        };


        void_list::void_list()
            : pimpl(new void_list::impl)
        {
        }


        void_list::void_list(const void_list &vl)
            : pimpl(new void_list::impl(*(vl.pimpl)))
        {
        }


        void_list::~void_list()
        {
            delete pimpl;
        }


        bool void_list::push_back(void *p)
        {
            if (!p)
            {
                std::cerr << "TSE3: void_list::push_back(0)"
                          << " - may be an error\n";
            }

            std::vector<void*>::iterator i
                = find(pimpl->vec.begin(), pimpl->vec.end(), p);
            if (i == pimpl->vec.end())
            {
                pimpl->vec.push_back(p);
                return true;
            }
            else
            {
                return false;
            }
        }


        bool void_list::erase(void *p)
        {
            std::vector<void*>::iterator i
                = find(pimpl->vec.begin(), pimpl->vec.end(), p);
            if (i != pimpl->vec.end())
            {
                pimpl->vec.erase(i);
                return true;
            }
            else
            {
                std::cerr << "TSE3: void_list use error\n";
                return false;
            }
        }


        unsigned int void_list::size() const
        {
            return pimpl->vec.size();
        }


        void *void_list::operator[](unsigned int index)
        {
            return pimpl->vec[index];
        }


        bool void_list::contains(void *p) const
        {
            std::vector<void*>::iterator i
                = find(pimpl->vec.begin(), pimpl->vec.end(), p);
            return i != pimpl->vec.end();
        }
    }
}

