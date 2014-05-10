/*
 * @(#)Notifier.h 3.00 13 July 2000
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

#ifndef TSE3_NOTIFIER_H
#define TSE3_NOTIFIER_H

#include <iostream>

/******************************************************************************
 * This file contains the implementation of the TSE3 Notifier framework.
 * It is the TSE3 implementation of the observer design pattern (GoF book).
 *
 * The public API (the Notifier and Listener classes) for this framework can
 * be found in the bottom half of the file, the top half contains internal
 * implementation concerns.
 *
 * This implementation can cope with listener event methods with between
 * zero and four parameters. This can be extended, by adding extra code
 * at the positions marked with (*)
 *****************************************************************************/

/*
 * A few annoying presentation choices have been made in this file because
 * KDOC (2.0.36) falls over. Notably some short classes have been spread
 * out, and there is a ";" at the end of the Impl namespace.
 *
 * Other namespaces in the TSE3 library have a similar problem. In each
 * case the trailing ";" is intentional. Sigh.
 */

namespace TSE3
{
    template <class interface_type> class Listener;

    /**************************************************************************
     * Notifier implementation details: ignore this bit
     *************************************************************************/

    /**
     * Internal implementation namespace.
     *
     * You can ignore the classes in this namespace. They are internal
     * implementation details for the TSE3 library and are not part of the
     * normal public API.
     *
     * The contents include implementation details for the @ref Notifier class
     * and the @ref Impl::Mutex classes which provide TSE3 thread-safety.
     *
     * @short   TSE3 private implementation namespace
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Notifier
     * @see     Listener
     * @internal
     */
    namespace Impl
    {
        /**
         * This is a minimal default type. It is used by the @ref Event class
         * to represent 'no type specified'.
         *
         * @internal
         */
        class def_type
        {
        };

        /**
         * This class template is used as a compile time utility to count
         * how many parameters have been passed to the @ref Event class
         * template. For most types its value is 1. The @ref def_type is the
         * only exception, its value is 0.
         *
         * @internal
         */
        template <typename T>
        struct arg_count
        {
            enum { count=1 };
        };
        template <>
        struct arg_count<def_type>
        {
            enum { count=0 };
        };

        /**
         * A type used to overload member functions based on integer
         * values. This is used in @ref Event to figure out which
         * @ref Event::invokeImpl member function to call.
         *
         * @internal
         */
        template<unsigned>
        class num_type
        {
        };

        /**
         * A very simple "vector" class. It holds a list of void *s. It
         * has enough vector-like methods that the Notifier framework can use
         * it.
         *
         * It is a disgusting thing to include in this library when we should
         * just be using std::vector instead. In fact, this class is
         * implemented with a std::vector. The reason for its inclusion is the
         * fact that it redces the size of libtse3.so.0.0.0 by about a third.
         * This can be accounted for by the reduction in size of link names.
         *
         * @internal
         */
        class void_list
        {
            public:

                void_list();
                void_list(const void_list &);
                ~void_list();

                /**
                 * Push a new void* onto the back of the list.
                 *
                 * You cannot insert a duplicate entry.
                 *
                 * @param p Elemnt to push
                 * @return Whether the insert was successful: if @p p
                 *         was already inserted, then false will
                 *         be returned.
                 */
                bool push_back(void *p);

                /**
                 * Returns true if @p p was in the void_list, false otherwise.
                 *
                 * @param  p Element to remove
                 * @return True if anything was erased
                 */
                bool erase(void *p);

                /**
                 * Returns the number of void*s in the void_list.
                 *
                 * @return Number of elements in list
                 */
                unsigned int size() const;

                /**
                 * Returns the void*s at index @p index
                 *
                 * @param index Index of item to retrieve
                 * @return void* at @p index
                 */
                void *operator[](unsigned int index);

                /**
                 * Returns whether or not @p p is in the void_list
                 *
                 * @param p Element to test
                 * @return Whether @p p is in the void_list
                 */
                bool contains(void *p) const;

            private:

                void_list &operator=(const void_list &);

                class impl;
                impl *pimpl;
        };

        /**
         * The Event class is an internal part of the Notifier/Listener
         * framework, used to multicast a particular event to a set of
         * Listener objects.
         *
         * It is a class that can 'hold' zero or more parameters. The upper
         * limit is set by the number of template parameters and
         * @ref invokeImpl member functions - more can be added if you really
         * need them.
         *
         * If a parameter is not needed, the template declaration will provide
         * @ref def_type which counts as 'no parameter'.
         *
         * @internal
         */
        template <class interface_type, typename listener_func,
                  typename p1_type = def_type,
                  typename p2_type = def_type,
                  typename p3_type = def_type,
                  typename p4_type = def_type>                           // (*)
        class Event
        {
            public:

                /**
                 * Ctor. You specify the member function of the
                 * @p interface_type class to call, and the parameters to call
                 * it with.
                 */
                explicit Event(listener_func func,
                               const p1_type &p1 = p1_type(),
                               const p2_type &p2 = p2_type(),
                               const p3_type &p3 = p3_type(),
                               const p4_type &p4 = p4_type())            // (*)
                    : func(func), p1(p1), p2(p2), p3(p3), p4(p4) {}      // (*)

                /**
                 * Calls 'func' on the specified set of @p listeners.
                 *
                 * This function delegates to the appropriate @ref invokeImpl
                 * method. Which one to call is worked out using the
                 * @ref arg_count utility.
                 */
                void callOnEvery(void_list &listeners)
                {
                    const unsigned int argCount = arg_count<p1_type>::count
                                                + arg_count<p2_type>::count
                                                + arg_count<p3_type>::count
                                                + arg_count<p4_type>::count;
                                                                         // (*)
                    void_list copy_list(listeners);
                    for (unsigned int i = 0; i < copy_list.size(); ++i)
                    {
                        if (listeners.contains(copy_list[i]))
                        {
                            typedef Listener<interface_type> listener_type;
                            interface_type *listener
                                = static_cast<listener_type*>(copy_list[i]);
                            invokeImpl(listener, num_type<argCount>());
                        }
                    }
                }

            private:

                /**
                 * There is a family of invokeImpl classes which will pass the
                 * parameters to the @p listener. Although we know that the
                 * callee is of type @p interface_type we abstract it as a
                 * template type (@p T) so that the functions are not
                 * instantiated by the compiler unless actually called.
                 *
                 * This is vital, since only one of the @ref invokeImpl
                 * functions will be valid for each event - only that one will
                 * be instantiated.
                 */
                template <class T>
                void invokeImpl(T *listener, num_type<0>) const
                {
                    (void)(listener->*func)();
                }
                template <class T>
                void invokeImpl(T *listener, num_type<1>) const
                {
                    (void)(listener->*func)(p1);
                }
                template <class T>
                void invokeImpl(T *listener, num_type<2>) const
                {
                    (void)(listener->*func)(p1, p2);
                }
                template <class T>
                void invokeImpl(T *listener, num_type<3>) const
                {
                    (void)(listener->*func)(p1, p2, p3);
                }
                template <class T>
                void invokeImpl(T *listener, num_type<4>) const
                {
                    (void)(listener->*func)(p1, p2, p3, p4);
                }                                                        // (*)

                const listener_func  func;
                const p1_type       &p1;
                const p2_type       &p2;
                const p3_type       &p3;
                const p4_type       &p4;                                 // (*)
        };

    }

    /**************************************************************************
     * Public Notifier framework API
     *************************************************************************/

    /**
     * The Notifier template base class is an implementation of the
     * observer design pattern (GoF book).
     *
     * Objects that need to send out specific callback events derive from this
     * base class. Each Notifier class may have any number of callback events,
     * and each of these events may take any number (and type) of parameters.
     * The callback system is type safe and easy to use.
     *
     * Events sent from the Notifier class are received by objects of the
     * @ref Listener class.
     *
     * @sect Usage
     *
     * A separate interface class detailing each callback event and the
     * type of Notifier source class must be written, for example:
     * <pre>
     *     class Exhibitionist;
     *
     *     class ExhibitionistListener
     *     {
     *         public:
     *             typedef Exhibitionist notifier_type;
     *
     *             virtual void eventOne(Exhibitionist *)        = 0;
     *             virtual void eventTwo(Exhibitionist *, int a) = 0;
     *     };
     * </pre>
     * The first parameter of callback events must always be a pointer to the
     * source object.
     *
     * You may choose to provide a default implementation for the callback
     * methods. This will save you having to reimplement every callback, only
     * the ones for the events you are interested in.
     *
     * You can now declare a @ref Notifier class thus:
     * <pre>
     *     class Exhibitionist : public Notifier<ExhibtionistListener>
     *     {
     *         // Other contents....
     *     };
     * </pre>
     * and implement your functionality.
     *
     * The Exhibitionist class can send callbacks to any attached listener by
     * calling the @ref notify member function. For example:
     * <pre>
     *     notify(&ExhibitionistListener::eventTwo, 1);
     * </pre>
     * The source object pointer parameter is automatically provided and does
     * not need to be specified.
     *
     * To receive events from a Notifier class, you must write a class that
     * derives from @ref Listener. The documentation for @ref Listener
     * describes how to do this.
     *
     * @short   Base class for objects that multicast events to listeners
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Listener
     */
    template <class interface_type>
    class Notifier
    {
        public:

            /**
             * The type of @ref Listener that this Notifier class works with.
             */
            typedef Listener<interface_type> listener_type;

            friend class Listener<interface_type>;

        protected:

            /**
             * Creates a Notifier with no listeners.
             *
             * Use @ref Listener::attachTo to add listeners.
             *
             * You can only subclass this type, not instanitate it directly.
             */
            Notifier() {}

            /**
             * The concrete Notifier type (i.e. the class that will
             * be subclassing Notifier<interface_type>.
             */
            typedef typename interface_type::notifier_type c_notifier_type;

            /**
             * There is a family of @p notify functions. The allow a Notifier
             * to multicast an event to every attached @ref Listener object.
             *
             * They are overloaded on the different number of parameters each
             * listener event function takes.
             *
             * You call a notify method specifying first the member function of
             * the @p interface_type to call, and then the parameters to call
             * it with.
             *
             * Every listener function has a first parameter which is a pointer
             * to this (source) object. This is automatically provided by the
             * notify function, so you need not supply it. If a funciton takes
             * any extra parameters then you must specify them.
             *
             * The first notify function is for events with no extra
             * parameters.
             */
            template <typename func_type>
            void notify(func_type func)
            {
                typedef
                    Impl::Event<interface_type, func_type, c_notifier_type*>
                    event_type;
                event_type(func, static_cast<c_notifier_type*>(this))
                    .callOnEvery(listeners);
            }

            /**
             * Notify function for events with one extra parameter.
             */
            template <typename func_type, typename p1_type>
            void notify(func_type func, const p1_type &p1)
            {
                typedef
                    Impl::Event<interface_type, func_type, c_notifier_type *,
                                p1_type>
                    event_type;
                event_type(func, static_cast<c_notifier_type*>(this), p1)
                    .callOnEvery(listeners);
            }

            /**
             * Notify function for events with two extra parameters.
             */
            template <typename func_type, typename p1_type, typename p2_type>
            void notify(func_type func, const p1_type &p1, const p2_type &p2)
            {
                typedef
                    Impl::Event<interface_type, func_type, c_notifier_type *,
                                p1_type, p2_type>
                    event_type;
                event_type(func, static_cast<c_notifier_type*>(this), p1, p2)
                    .callOnEvery(listeners);
            }

            /**
             * Notify function for events with three extra parameters.
             */
            template <typename func_type, typename p1_type, typename p2_type,
                      typename p3_type>
            void notify(func_type func, const p1_type &p1, const p2_type &p2,
                        const p3_type &p3)
            {
                typedef
                    Impl::Event<interface_type, func_type, c_notifier_type *,
                                p1_type, p2_type, p3_type>
                    event_type;
                event_type
                    (func, static_cast<c_notifier_type*>(this), p1, p2, p3)
                    .callOnEvery(listeners);
            }                                                            // (*)

            /**
             * The dtor tells every attached @ref Listener that this object
             * is being deleted.
             */
            virtual ~Notifier()
            {
                // LOCK
                for (unsigned int i = 0; i < listeners.size(); ++i)
                {
                    listener_type *l
                        = static_cast<listener_type*>(listeners[i]);
                    l->NotifierImpl_Deleted
                        (reinterpret_cast<c_notifier_type*>(this));
                }
            }

            unsigned int numListeners() const { return listeners.size(); }

        private:

            /**
             * Attach a listener to this Notifier object.
             *
             * You can only attach a given listener object once - any
             * subsequent attempt to attach it results in nothing happening;
             * it is not an error, but the object will still only ever receive
             * one copy of any event.
             *
             * This function is only used by the @ref Listener class'
             * @ref Listener::attachTo method.
             *
             * @param  listener The listener to attach
             * @return Whether the attach was successful: if the
             *         @p listener_type was already attached, then false will
             *         be returned.
             */
            bool attach(listener_type *listener)
            {
                // LOCK
                return listeners.push_back(listener);
            }

            /**
             * Detach a listener from this Notifier object.
             *
             * If the listener is not already attached no error is raised.
             *
             * This function is only used by the @ref Listener class'
             * @ref Listener::detachFrom method.
             *
             * @param listener The listener to detach
             */
            void detach(listener_type *listener)
            {
                // LOCK
                listeners.erase(listener);
            }

            typedef Notifier<interface_type> self_type;

            Notifier(const self_type &);
            self_type &operator=(const self_type &);

            Impl::void_list listeners;
    };

    /**
     * The Listener class receives events sent from a @ref Notifier class.
     *
     * Listener inherits from the specified @p interface_type so you don't need
     * to mupltiply inherit from both Listener the @p interface_type in your
     * derived class.
     *
     * To instatiate the Listener class inherit from Listener<NotifierType>.
     * Following the example in the @ref Notifier documentation, this would
     * be:
     * <pre>
     *     class Voyeur : public Listener<ExhibtionistListener>
     *     {
     *         public:
     *             virtual void eventOne(Exhibitionist *src)        { ... }
     *             virtual void eventTwo(Exhibitionist *src, int a) { ... }
     *             virtual void Notifier_Deleted(Exhibitionist *src)
     *             {
     *                 // The 'src' is being deleted - you do not need to
     *                 // detach from it, the link is automatically broken.
     *
     *                 // This method is not defined in the
     *                 // ExhibitionistListener base class, but is defined by
     *                 // the Listener<ExhibitionistListener> class. You
     *                 // don't have to implement it if you don't need it.
     *             }
     *     };
     * </pre>
     *
     * Upon deletion, the Listener class ensures that it is detached from every
     * @ref Notifier it has been attached to. This prevents a @ref Notifier
     * later trying to callback to an invalid object.
     *
     * @short   Listener interface for @ref Notifier events
     * @author  Pete Goodliffe
     * @version 3.00
     * @see     Notifier
     */
    template <class interface_type>
    class Listener : public interface_type
    {
        public:

            /**
             * The type of @ref Notifier class this listener works with.
             */
            typedef Notifier<interface_type> notifier_type;

            /**
             * Attaches this Listener to @p notifier.
             *
             * You can only attach to a @ref Notifer object once - any
             * subsequent attempt to attach to it results in nothing happening;
             * it is not an error, but the Listener will still only ever
             * receive one copy of any event.
             *
             * @param notifier The @ref Notifier object to attach to
             */
            void attachTo(notifier_type *notifier)
            {
                if (notifier->attach(this)) notifiers.push_back(notifier);
            }

            /**
             * Detaches this Listener from @p notifier.
             *
             * If the listener is not already attached to this @ref Notifier no
             * error is raised.
             *
             * @param notifier The @ref Notifier object to detach from
             */
            void detachFrom(notifier_type *notifier)
            {
                if (notifiers.erase(notifier)) notifier->detach(this);
            }

            friend class Notifier<interface_type>;

        protected:

            /**
             * Creates a new Listener which is not attached to any
             * @ref Notifier.
             *
             * Use @ref attachTo to attach to @ref Notifiers.
             *
             * You can only subclass this type, not instanitate it directly.
             */
            Listener() {}

            /**
             * The concrete Notifier type (i.e. the class that derives from
             * Notifier<interface_type>.
             */
            typedef typename interface_type::notifier_type c_notifier_type;

            /**
             * This may be implemented by Listener classes if they care about
             * a @ref Notifier object being deleted.
             *
             * It is called by the @ref Notifer destructor - so the @p notifier
             * is in the process of being deleted. Therefore it is not safe to
             * call methods on the event source object any more.
             *
             * You do not need to @ref detachFrom the the @ref Notifier
             * since the link is automatically broken for you.
             *
             * @param notifier The @ref Notifier that has been deleted
             */
            virtual void Notifier_Deleted(c_notifier_type * /*notifier*/) {}

            /**
             * The destructor ensures that the Listener is detached from every
             * @ref Notifier it has been attached to.
             *
             * This means that you don't have to specifically detach from all
             * @ref Notifier sources yourself, although it is still considered
             * good practice to do so.
             */
            virtual ~Listener()
            {
                // LOCK
                for (unsigned int i = 0; i < notifiers.size(); ++i)
                {
                    notifier_type *n
                        = static_cast<notifier_type*>(notifiers[i]);
                    n->detach(this);
                }
            }

        private:

            /**
             * This is called by the corresponding @ref Notifier type's
             * destructor.
             *
             * This ensures that if a notifier_type that the Listener is
             * attached to is deleted, it is removed from the notifiers list,
             * so we will never attempt to detach from it later.
             *
             * It also calls the internal @ref Notifier_Deleted method.
             */
            void NotifierImpl_Deleted(c_notifier_type *src)
            {
                notifiers.erase(reinterpret_cast<notifier_type*>(src));
                this->Notifier_Deleted(src);
            }

            typedef Listener<interface_type> self_type;

            Listener(const self_type &);
            self_type &operator=(const self_type &);

            Impl::void_list notifiers;
    };
}

#endif
