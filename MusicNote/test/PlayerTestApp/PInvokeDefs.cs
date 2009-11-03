
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

        public delegate void ParserErrorDelegate(IntPtr pParser, ref ErrorEventHandlerArgs evArgs);
        public delegate void ParserTraceDelegate(IntPtr userData, [MarshalAs(UnmanagedType.LPStr)] String szTraceMsg);
        [DllImport("MusicNoteDll.Dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Parse([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, 
                                        [MarshalAs(UnmanagedType.FunctionPtr)] ParserTraceDelegate d,
                                        IntPtr userData);

        [DllImport("MusicNoteDll.Dll")]
        public static extern bool PlayMusicString([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes);

        [DllImport("MusicNoteDll.Dll")]
        public static extern bool PlayMusicStringWithOpts([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, int nMidiOutPortID, uint nTimerResMS);

        [DllImport("MusicNoteDll.Dll")]
        public static extern bool SaveAsMidiFile([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, [MarshalAs(UnmanagedType.LPStr)] String szOutputFilePath);
    }
}