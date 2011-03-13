/*
 * @(#)Mutex.h 3.00 2 October 2000
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

#ifndef TSE3_MUTEX_H
#define TSE3_MUTEX_H

namespace TSE3
{
    namespace Impl
    {
        /**
         * This class provides an abtract interface for a mutex implementation.
         *
         * The @ref NullMutexImpl class inherits from this base class, and
         * provides a 'null' implementation: performing no lock or unlock
         * operations.
         *
         * This class provides a way of specifying mutex behaviour in a
         * platform independant manner.
         *
         * If you want to use TSE3 in a thread-safe manner then you will
         * need to implement a MutexImpl class and pass it to the @ref Mutex
         * class.
         *
         * A MutexImpl is created in an unlocked state.
         *
         * @short   Mutex implementation base class
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Mutex
         */
        class MutexImpl
        {
            public:

                virtual ~MutexImpl() {}

                /**
                 * Locks the mutex.
                 *
                 * If the mutex implementation is already locked by a different
                 * thread, then this thread will block until the previous one
                 * unlocks the mutex.
                 *
                 * A single thread can lock the mutex multiple times. However,
                 * subsequent calls to lock have no effect. There must be the
                 * same number of calls to @ref unlock before the MutexImpl
                 * is unlocked, though.
                 *
                 * @see unlock
                 */
                virtual void lock() = 0;

                /**
                 * Unlocks the mutex. To unlock the mutex fully, as many
                 * unlocks must be called as locks.
                 *
                 * If the MutexImpl is already unlocked, then nothing
                 * will happen.
                 *
                 * @see lock
                 */
                virtual void unlock() = 0;

                /**
                 * Returns true if the MutexImpl is locked, false otherwise.
                 *
                 * @see lock
                 * @see unlock
                 */
                virtual bool locked() = 0;
        };

        /**
         * A default, 'null' implementation of the @ref MutexImpl class that
         * is used by thread-unsafe versions of the TSE3 library.
         *
         * The @ref lock and @ref unlock methods are essentially no-ops - they
         * don't perform any locking whatsoever.
         *
         * @short   'Null' @ref MutexImpl class
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     MutexImpl
         */
        class NullMutexImpl : public MutexImpl
        {
            public:

                NullMutexImpl() : _locked(0) {}

                virtual ~NullMutexImpl() {}

                /**
                 * @reimplemented
                 */
                virtual void lock() { ++_locked; }

                /**
                 * @reimplemented
                 */
                virtual void unlock() { if (_locked) --_locked; }

                /**
                 * @reimplemented
                 */
                virtual bool locked() { return _locked; }

            private:

                int _locked;
        };

        /**
         * The Mutex class is used by TSE3 to ensure thread safety. All
         * potentially contenious TSE3 methods claim a Mutex to prevent TSE3
         * being entered by multiple threads.
         *
         * This Mutex class provides a platform independant interface
         * to the underlying mutex implementation (which is accessed through
         * the @ref MutexImpl class interface).
         *
         * There is a single, global TSE3 Mutex object that is used by the
         * entire library. This is accessed by calling the @ref mutex
         * static member function.
         *
         * On each different platform supported by TSE3 there will be a
         * specific @ref MutexImpl class.
         *
         * By default, the TSE3 library is not configured to be thread safe:
         * the default @ref MutexImpl that will be used is the
         * @ref NullMutexImpl. This implementation performs no operations
         * for lock and unlock.
         *
         * In order to make TSE3 thread safe, you must have an implementation
         * of the @ref MutexImpl class, and pass this to the static
         * @ref setImpl method <b>BEFORE</b> using any of the other TSE3 API.
         * (In fact, you only need set this before the first call to the
         * static @ref mutex method, but most API functions will call this
         * at some time).
         *
         * A simplification of the Mutex API is provided by the
         * @ref CritSec class. In some cases this is a more
         * convenient interface to use.
         *
         * @short   Mutex class used to ensure TSE3 thread safety
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     MutexImpl
         * @see     NullMutexImpl
         * @see     CritSec
         */
        class Mutex
        {
            public:

                /**
                 * Creates a Mutex object with the specified implementation.
                 *
                 * This @ref MutexImpl object is considered to be owned by
                 * the Mutex class, and will be deleted when the Mutex is
                 * deleted - you need not delete it yourself.
                 *
                 * This does imply that there must be a one-to-one relationship
                 * between Mutex objects and @ref MutexImpl objects.
                 *
                 * A Mutex is created in an unlocked state.
                 *
                 * @param impl @ref MutexImpl object to use
                 */
                Mutex(MutexImpl *i) : impl(i) {}

                ~Mutex();

                /**
                 * Sets the @ref MutexImpl class that will be used by the
                 * global Mutex object.
                 *
                 * It is very important to call this method BEFORE
                 * calling @ref mutex to use the global Mutex object. Since
                 * most TSE3 API methods call mutex, it is safest to set
                 * the @ref MutexImpl before calling any other TSE3 method.
                 *
                 * The impl you specify must exist for as long as you use
                 * the TSE3 library.
                 *
                 * If you do not specify a @ref MutexImpl then a default
                 * @ref NullMutexImpl will be used, and TSE3 will not
                 * be thread-safe.
                 *
                 * You may only call this once: subsequent calls are ignored
                 * and the first @ref MutexImpl specified is used.
                 *
                 * @param impl @ref MutexImpl to use. This will be deleted
                 *             by the Mutex class - you may forget about it.
                 */
                static void setImpl(MutexImpl *impl);

                /**
                 * Provides access to the global Mutex object used to lock
                 * the TSE3 library.
                 *
                 * The first time this method is called, a Mutex object will
                 * be created with the @ref MutexImpl specified by
                 * @ref setImpl.
                 */
                static Mutex *mutex();

                /**
                 * Locks the Mutex.
                 *
                 * If the Mutex is already locked by a different
                 * thread, then this thread will block until the previous one
                 * unlocks the mutex.
                 *
                 * A single thread can lock the mutex multiple times. However,
                 * subsequent calls to lock have no effect. There must be the
                 * same number of calls to @ref unlock before the Mutex
                 * is unlocked, though.
                 *
                 * @see unlock
                 */
                void lock()
                {
#ifndef TSE3_WITHOUT_MUTEX
                    impl->lock();
#endif
                }

                /**
                 * Unlocks the Mutex. To unlock the mutex fully, as many
                 * unlocks must be called as locks.
                 *
                 * If the Mutex is already unlocked, then nothing will happen.
                 *
                 * @see lock
                 */
                void unlock()
                {
#ifndef TSE3_WITHOUT_MUTEX
                    impl->unlock();
#endif
                }

            private:

                MutexImpl        *impl;

                static MutexImpl *globalImpl;
        };

        /**
         * A 'critical section' class that provides a somewhat more convenient
         * interface to the @ref Mutex class. A CritSec object simply locks
         * the global @ref Mutex upon creation and unlocks it on destruction.
         *
         * Therefore, this means that rather than write:
         * <pre>
         *     {
         *         TSE3::Impl::Mutex::mutex()->lock();
         *         // do something
         *         TSE3::Impl::Mutex::mutex()->unlock();
         *     }
         * </pre>
         *
         * You can simply write:
         * <pre>
         *     {
         *         TSE3::Impl::CritSec cs;
         *         // do something
         *     }
         * </pre>
         *
         * Clearly, this prevents a possible source of error, where you might
         * forget to unlock the mutex after having locked it. It is also in
         * most cases a lot more convenient to use.
         *
         * The CritSec class is small, simple and implemented with inline
         * functions so the resultant generated code for both of the above
         * examples is identical.
         *
         * @short   Convenient API for @ref Mutex class
         * @author  Pete Goodliffe
         * @version 3.00
         * @see     Mutex
         */
        class CritSec
        {
            public:

                CritSec()
                {
#ifndef TSE3_WITHOUT_MUTEX
                    Mutex::mutex()->lock();
#endif
                }

                ~CritSec()
                {
#ifndef TSE3_WITHOUT_MUTEX
                    Mutex::mutex()->unlock();
#endif
                }
        };
    }
}

#endif
