/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009-2010 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace PlayerTestApp
{
    public static class MusicNoteLib
    {
        public interface EventHandlerArgs
        {
            void Method1();
        }
        [StructLayout(LayoutKind.Sequential)]
		public struct ErrorEventHandlerArgs
		{  
			public int	errCode; ///< Error Code
            [MarshalAs(UnmanagedType.LPStr)] 
            public String szErrMsg; ///< The Error message sent by Parser
			[MarshalAs(UnmanagedType.LPStr)] 
            public String szToken; ///< Token the error is applicable for
			public bool bShouldStopParsing; ///< Should Parser continue to parse or should stop now? (only for PARSE_ERRORS)
		};
        [StructLayout(LayoutKind.Sequential)]
		public struct TraceEventHandlerArgs : EventHandlerArgs
		{  
            [MarshalAs(UnmanagedType.LPStr)] 
            public String szErrMsg; ///< The Error message sent by Parser
            void EventHandlerArgs.Method1() { }
		};

        //public delegate void ParserErrorDelegate(IntPtr pParser, ref ErrorEventHandlerArgs evArgs);
        public delegate void ParserTraceDelegate(IntPtr userData, [MarshalAs(UnmanagedType.LPStr)] String szTraceMsg);
        public delegate void ParserErrorDelegate(IntPtr userData, int errCode,
                [MarshalAs(UnmanagedType.LPStr)] String szErrorMsg,
                [MarshalAs(UnmanagedType.LPStr)] String szToken);
#if DEBUG
        [DllImport("MusicNoteDlld.Dll")]
        public static extern bool PlayMusicString([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes);

        [DllImport("MusicNoteDlld.Dll")]
        public static extern bool PlayMusicStringWithOpts([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, int nMidiOutPortID, uint nTimerResMS);

        [DllImport("MusicNoteDlld.Dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlayMusicStringCB([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, 
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserTraceDelegate td,
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserErrorDelegate ed,
                                        IntPtr userData);

        [DllImport("MusicNoteDlld.Dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlayMusicStringWithOptsCB([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, int nMidiOutPortID, uint nTimerResMS,
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserTraceDelegate td,
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserErrorDelegate ed,
                                        IntPtr userData);

        [DllImport("MusicNoteDlld.Dll")]
        public static extern bool SaveAsMidiFile([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, [MarshalAs(UnmanagedType.LPStr)] String szOutputFilePath);
#else
        [DllImport("MusicNoteDll.Dll")]
        public static extern bool PlayMusicString([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes);

        [DllImport("MusicNoteDll.Dll")]
        public static extern bool PlayMusicStringWithOpts([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, int nMidiOutPortID, uint nTimerResMS);

        [DllImport("MusicNoteDll.Dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlayMusicStringCB([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, 
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserTraceDelegate td,
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserErrorDelegate ed,
                                        IntPtr userData);

        [DllImport("MusicNoteDll.Dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PlayMusicStringWithOptsCB([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, int nMidiOutPortID, uint nTimerResMS,
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserTraceDelegate td,
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserErrorDelegate ed,
                                        IntPtr userData);

        [DllImport("MusicNoteDll.Dll")]
        public static extern bool SaveAsMidiFile([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, [MarshalAs(UnmanagedType.LPStr)] String szOutputFilePath);
#endif
    }
}