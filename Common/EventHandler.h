#ifndef __EVENTHANDLER_H_105D52D8_13EC_4ea3_A86B_076B726541EB_
#define __EVENTHANDLER_H_105D52D8_13EC_4ea3_A86B_076B726541EB_

#include "AtlColl.h"

namespace OIL /// Object Introspection Library
{
	class CEventSource;	//Forward declaration

	/// <Summary>
	/// Class that is used to pass arguments to the event handler procedures.
	/// <para> See CEventT for sample usage.</para>
	/// </Summary>
    class CEventHandlerArgs
	{
	public:
		CEventHandlerArgs() {}
		virtual ~CEventHandlerArgs() { }
	};

	/// <Summary>
	/// For Class-based Event Subscriptions, a class can receive
	/// events only if it is derived from CEventReceiver.
	/// <para> See CEventT for sample usage.</para>
	/// </Summary>
	class CEventReceiver
	{
	protected:
		// Protected constructor. CEventReceiver objects cannot be created directly.
		// This Class should be inherited.
		CEventReceiver() {}		

		// Protected destructor. 
		// No need to declare as virtual because delete can't be applied on CEventReceiver pointers.
		~CEventReceiver() { }
	};

	/// <Summary>
	/// Class for Event Subscription Function Objects.
	/// </Summary>
	class IEventFunctor
	{
	public:
		virtual ~IEventFunctor() { }
		virtual void operator()(CEventReceiver* pReceiver, CEventSource* pSender, CEventHandlerArgs* pArgs) = 0;
	};

	/// <Summary>
	/// A Receiver can Subscribe for an Event only once.
	/// If a Receiver requests a second subscription for the same Event,
	/// its previous subscription would be replaced with the new subscription.
	///
	/// A Receiver can subscribe for as many different events as it wants. 
	/// But for each, only once.
	///
	/// And an Event can have as many Receivers as it likes.
	///
	/// CEventT take template arguments that indicate the prototype of the
	/// handler proc that should used to subscribe to that event. By default,
	/// CEvent uses the handlers of the form
	/// <code>void EventReceptionFunc(const CEventSource* pSrc, CEventHandlerArgs* pArgs)</code>
	/// You can use different prototypes by simply defining the sender and handler
	/// args in the template definition for CEventT.
	///
	/// For example, CEventT<Sender, HandlerArgs> expects its subscriber functions
	/// to be of the form
	/// <code>void EventReceptionFunc(Sender* pSrc, HandlerArgs* pArgs)</code>
	/// </Summary>
	/// Following Snippet demonstrates sample usage.
    /** <code><pre>
    \#include "EventHandler.h"
    using namespace OIL;

    struct MyEventHandlerArgs : public CEventHandlerArgs
    {
        int m_nVal;
    };

     class MySender : public CEventSource
     {
        public:

        CEvent m_Event;	// Default handler prototype

        CEventT < const MySender, MyEventHandlerArgs > m_Event2;	// Custom handler prototype

        void MyFunction()
        {
            RaiseEvent(&m_Event, &CEventHandlerArgs());	// Invoke the Event

            RaiseEvent(&m_Event2, &MyEventHandlerArgs());	// Invoke the Event with custom prototype
        }
     };

    class MyReceiver: public CEventReceiver
    {
        MySender* m_pMySenderObject;

    public:

        MyReceiver(MySender* pSender) : m_pMySenderObject(pSender) // Subscribe for the Events
        {	
            m_pMySenderObject->m_Event.Subscribe(this, & MyReceiver::EventReceptionFunc);

            m_pMySenderObject->m_Event2.Subscribe(this, & MyReceiver::MyEventReceptionFunc2);
        }

        void EventReceptionFunc(const CEventSource* pSrc, CEventHandlerArgs* pArgs)
        {			
            printf("\n Event Received\n"); // Code for Handling the Event
        }

        void MyEventReceptionFunc2(const MySender* pSrc, const MyEventHandlerArgs* pArgs)
        {			
            printf("\nEvent2 Received"); // Code for Handling the Event
        }
    };

    void main()
    {
        MySender senderObj;

        MyReceiver receiverObj(senderObj);

        senderObj.MyFunction(); // Do something on Sender that will Raise the Events to be received on Receiver
    }
    </pre></code> */
	template<typename TEventSource = const CEventSource, typename TEventHandlerArgs = CEventHandlerArgs>
	class CEventT
	{
		/// <Summary>
		/// Implements IEventFunctor for Handling Class-based Subscriptions.
		/// </Summary>
		template<typename TEventReceiver>
		class CClassFunctor : public IEventFunctor
		{
			typedef void (TEventReceiver::*FuncType)(TEventSource* pSender, TEventHandlerArgs* pArgs);

			FuncType m_pFunc;

			CClassFunctor(FuncType pFunc) : m_pFunc(pFunc) { }

			inline void operator()(CEventReceiver* pReceiver, CEventSource* pSender, CEventHandlerArgs* pArgs)
			{
				(((TEventReceiver*)pReceiver)->*m_pFunc)((TEventSource*)pSender, (TEventHandlerArgs*)pArgs);
			}

			friend class CEventT; // Only CEventT Objects can use the CClassFunctor Objects
		};

		typedef ATL::CMapToAutoPtr<CEventReceiver*, IEventFunctor> CLASS_HANDLER_MAP;
		typedef void (*STATICHANDLER)(TEventSource*, TEventHandlerArgs*);
		typedef ATL::CAtlList<STATICHANDLER> STATIC_HANDLER_LIST;

		CLASS_HANDLER_MAP	m_ClassSubscribers;
		STATIC_HANDLER_LIST	m_StaticSubscribers;

		/// <Summary>
		/// Events can be Invoked only through CEventSource::RaiseEvent()
		/// </Summary>
		void Invoke(TEventSource* pSender, TEventHandlerArgs* pArgs)
		{
			POSITION pos = m_ClassSubscribers.GetStartPosition();

			// Invoke operator() on each of the CClassFunctor Objects
			while( NULL != pos)
			{
				(*m_ClassSubscribers.GetValueAt(pos))(m_ClassSubscribers.GetKeyAt(pos), (CEventSource*)pSender, (CEventHandlerArgs*)pArgs);
				m_ClassSubscribers.GetNext(pos);
			}

			pos = m_StaticSubscribers.GetHeadPosition();

			// Invoke operator() on each of the CStaticFunctor Objects
			while(NULL != pos)
				(*m_StaticSubscribers.GetNext(pos))(pSender, pArgs);
		}

		/// <Summary>
		/// Takes ownership of the supplied IEventFunctor pointer.
		/// Assumes that it is allocated with new, and calls delete 
		/// automatically upon it when going out of scope.
		/// Duplicate or Multiple Subscriptions are not allowed.
		/// If the Receiver object already has a subscription for this event,
		/// the old subscription is removed before adding the new subcription.
		/// </Summary>
		template<typename TReceiverClass>
		inline void Subscribe(TReceiverClass* pReceiver, ATL::CAutoPtr<IEventFunctor>& spFunctor)
		{
			ATLASSERT(pReceiver != NULL && spFunctor.m_p != NULL);

			m_ClassSubscribers[pReceiver] = spFunctor;
		}		

		friend class CEventSource;

		CEventT& operator = (const CEventT& );	// Prohibit Assignment

		CEventT(const CEventT& );		// Prohibit Copy Constructor

	public:
		inline CEventT() { }

		inline ~CEventT() {	}

		typedef ATL::CAutoPtr<IEventFunctor> IEventFunctorPtr;

		/// <Summary>
		/// Creates an IEventFunctor object for the given function and
		/// subcribes it to be called upon the given object whenever the event
		/// is raised.
		/// Duplicates or Multiple Subscriptions are not allowed.
		/// If the Receiver object already has a subscription for this event,
		/// the old subscription is removed before adding the new subcription.
		/// </Summary>
		template<typename TEventReceiver>
		inline void Subscribe(TEventReceiver* pReceiver, void (TEventReceiver::*lpfnHandler)(TEventSource*, TEventHandlerArgs* ))
		{
			ATLASSERT(pReceiver != NULL);

			Subscribe(pReceiver, IEventFunctorPtr(new CClassFunctor<TEventReceiver>(lpfnHandler)));
		}

		/// <Summary>
		/// Subscribes the given Function to be called whenever the event is raised.
		/// Duplicates are not Allowed. 
		/// If the given Function is already a subscriber, it would not be added again.
		/// </Summary>
		inline void Subscribe(void (*lpfnHandler)(TEventSource*, TEventHandlerArgs*))
		{
			ATLASSERT(NULL != lpfnHandler);

			if( NULL == m_StaticSubscribers.Find(lpfnHandler) )	// Do not Add if Already Exists
				m_StaticSubscribers.AddTail( lpfnHandler );
		}

		/// <Summary>
		/// UnSubscribes the given Function from being called.
		/// Nothing Happens if the given Function is not a Subscriber.
		/// </Summary>
		inline void UnSubscribe(void (*lpfnHandler)(TEventSource*, TEventHandlerArgs*))
		{
			POSITION pos = m_StaticSubscribers.Find(lpfnHandler);
			if( NULL != pos )
				m_StaticSubscribers.RemoveAt(pos);
		}

		/// <Summary>
		/// UnSubscribes the pReceiver from the Subscription.
		/// Nothing Happens if the supplied pReceiver object is not a Subscriber.
		/// </Summary>
		template<typename TReceiverClass>
		inline void UnSubscribe(TReceiverClass* pReceiver)
		{
			m_ClassSubscribers.RemoveKey(pReceiver);
		}

		/// <Summary>
		/// UnSubscribes all receivers. SubscriberCount() would become Zero.
		/// </Summary>
		inline void UnSubscribeAll()
		{
			// Remove Static Subscribers
			m_StaticSubscribers.RemoveAll();
			// Remove Class-based Subscribers
			m_ClassSubscribers.RemoveAll();
		}

		/// <Summary>
		/// Gives the number of active Subcriptions for this event.
		/// It includes the Class-based Subscriptions as well the Function-based ones.
		/// </Summary>
		inline size_t SubscriberCount() const	
		{	
			return m_ClassSubscribers.GetCount() + m_StaticSubscribers.GetCount();
		}
	};

    ///<Summary>
    /// Default form of CEventT that uses 
    /// <code>void EventReceptionFunc(const CEventSource* pSrc, CEventHandlerArgs* pArgs)</code>
    /// as the Event handler procedure
    ///</Summary>
	typedef CEventT<> CEvent;

	/// <Summary>
	/// Only CEventSource derived classes can raise events.
	/// So to raise events from your class, declare CEventT 
	/// variables in your class and derive it from CEventSource. Use
	/// CEventSource::RaiseEvent() method when required to raise
	/// the event.
	///
	/// <para>See CEventT for sample usage.</para>
	///
	/// If you want to use events outside classes, then use the CInvokableEventT class.
	/// </Summary>
	class CEventSource
	{	
	protected:
		// CEventSource objects cannot be created directly due to the protected constructor. 
		// This class can Only be Inherited.
		CEventSource() {}

		// Protected destructor. 
		// No need to declare as virtual because delete can not be applied on CEventSource pointers.
		~CEventSource() { }

		/// <Summary>
		/// Invokes the Event.
		/// </Summary>
		template<typename TEventSource, typename TEventHandlerArgs>
		inline void RaiseEvent(CEventT<TEventSource, TEventHandlerArgs>* pEvent, TEventHandlerArgs* pArgs)
		{
			ATLASSERT(pEvent != NULL);

			pEvent->Invoke((TEventSource *)this, (TEventHandlerArgs *)pArgs);
		}

		/// <Summary>
		/// Invokes the Event.
		/// </Summary>
		template<typename TEventSource, typename TEventHandlerArgs>
		inline void RaiseEvent(CEventT<TEventSource, const TEventHandlerArgs>* pEvent, const TEventHandlerArgs* pArgs)
		{
			ATLASSERT(pEvent != NULL);

			pEvent->Invoke((TEventSource *)this, (const TEventHandlerArgs *)pArgs);
		}

	};

	/// <Summary>
	/// An event that can be invoked by itself.
	///
	/// A CEventT event can only be used from within a class. To use events
	/// outside classes, you need to use CInvokableEventT.
	/// 
	/// Just declare the variable of CInvokableEventT anywhere in your code,
	/// and use its RaiseEvent() method to invoke it. 
	///
	/// Use its subscribe() methods to subscribe either static metods or
    /// class based methods.
    ///
    /// Note that in this case the EventSource is same as the event object 
    /// itself, which means the handler prototype will be always of the form
    /// <code> void EventHandlerProc(const CEventSource* pSender, T* pArgs)  </code>
    /// where T is the template argument passed while declaring the CInvokableEventT variable.
	/// </Summary>
	/// Following Snippet demonstrates sample usage.
	/** <code><pre>
     struct MyEventHandlerArgs : public CEventHandlerArgs
     {
        int m_nVal;
     };
     
     CInvokableEventT \<MyEventHandlerArgs\> Event;
    
     class MyPseudoClass
     {
     public:
         MyPseudoClass()
         {
             Event.RaiseEvent(&MyEventHandlerArgs());
         }
     };
    
     void EventHandlerProc(const CEventSource* pSender, MyEventHandlerArgs* pArgs)
     {
         printf("\nEvent Received");
     }
    
     class MyListener : public CEventReceiver
     {
     public:
         void EventHandlerProc(const CEventSource* pSender, MyEventHandlerArgs* pArgs)
         {
             printf("\nListener Received the Event");
         }
     };
    
     int _tmain(int argc, _TCHAR* argv[])
     {
         Event.Subscribe(&EventHandlerProc); // Subscribe a non-class method
    
         MyListener listener; 
         Event.Subscribe(&listener, &MyListener::EventHandlerProc); // Subscribe a class method
    
         MyPseudoClass Obj; // Raise the Event in the constructor
    
         Event.RaiseEvent(&MyEventHandlerArgs()); // Event can be raised explicitly with from anywhere !!
    
         return 0;
     }
     </pre></code>*/
	template<typename TEventHandlerArgs = CEventHandlerArgs>
	class CInvokableEventT : protected CEventSource, public CEventT<const CEventSource, TEventHandlerArgs>
	{
	public:
		inline void RaiseEvent(TEventHandlerArgs* pArgs)
		{
			CEventSource::RaiseEvent(this, pArgs);
		}
	};

    ///<Summary>
    /// Default form of CInvokableEventT that uses 
    /// <code>void EventReceptionFunc(const CEventSource* pSrc, CEventHandlerArgs* pArgs)</code>
    /// as the Event handler procedure
    ///</Summary>
    typedef CInvokableEventT<> CInvokableEvent;

}	// namespace OIL

#endif	// __EVENTHANDLER_H_105D52D8_13EC_4ea3_A86B_076B726541EB_