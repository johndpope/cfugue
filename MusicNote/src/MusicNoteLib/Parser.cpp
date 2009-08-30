#include "StdAfx.h"

#include "ParserListener.h"
#include "Parser.h"

namespace MusicNoteLib
{
	///<Summary> Subscribes a Listener object for all events </Summary>
	void CParser::AddListener(CParserListener* pListener)
	{
		evNote.Subscribe(pListener, &CParserListener::OnNoteEvent);
	}

	///<Summary> UnSubscribes the Listener Object from the events </Summary>
	void CParser::RemoveListener(CParserListener* pListener)
	{
		evNote.UnSubscribe(pListener);
	}

} // namespace 