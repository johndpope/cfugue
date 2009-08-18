#ifndef __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__
#define __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__

#include "MString.h"
#include "MusicStringParser.h"
#include "MIDIRenderer.h"

namespace MusicNoteLib
{
	class Player
	{			
		MIDIRenderer m_Renderer;
	public:

		Player(void)
		{
		}

		~Player(void)
		{
		}

		/// <Summary>
		/// Plays a string of music notes
		/// </Summary>
		void Play(const MString& strMusicNotes)
		{
			MusicStringParser parser;
			parser.AddListener(&m_Renderer);
			parser.Parse(strMusicNotes);
			m_Renderer.BeginRenderAsync();
		}

		void Stop()
		{
			m_Renderer.EndRenderAsync();
		}
	};

} // namespace MusicNoteLib

#endif // __PLAYER_H__83B70D04_18A9_48ff_B08B_81BF613138DB__