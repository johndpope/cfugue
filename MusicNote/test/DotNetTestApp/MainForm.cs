/*
	This is part of CFugue, a C++ Runtime for MIDI Score Programming
	Copyright (C) 2009 Gopalakrishna Palem

	For links to further information, or to contact the author,
	see <http://musicnote.sourceforge.net/>.
*/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using Microsoft.Win32;

namespace PlayerTestApp
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        #region WinMM Declarations to populate the Device ComboBox on the Form

        [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
        private struct MIDIOUTCAPS
        {
            public ushort wMid;
            public ushort wPid;
            public uint vDriverVersion;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst=32)]
            public string szPname;
            public ushort wTechnology;
            public ushort wVoices;
            public ushort wNotes;
            public ushort wChannelMask;
            public uint dwSupport;
        }

        [DllImport("Winmm.dll", CharSet=CharSet.Auto)]
        private static extern uint midiOutGetDevCaps(uint uDeviceID, out MIDIOUTCAPS lpMidiOutCaps, int cbMidiOutCaps);

        [DllImport("Winmm.dll")]
        private static extern uint midiOutGetNumDevs();

        #endregion

        // Registry Keys
        const string regUserRoot = "HKEY_CURRENT_USER";
        const string regSubkey = "Software\\CineFxLabs\\MusicNoteLib\\PlayerTestApp\\LastSettings";
        const string regKeyName = regUserRoot + "\\" + regSubkey;

        /// <summary>
        /// Form Closing Event. Save the current settings to registry
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Save the currently selected MIDI Output Device ID into the registry
            if(comboBox_MIDIOutDevs.Items.Count > 0)
                Registry.SetValue(regKeyName, "OutputDevice", comboBox_MIDIOutDevs.SelectedIndex);
            // Save the current Timer Resolution
            Registry.SetValue(regKeyName, "TimerResolution", (int) numericUpDown_TimerResolution.Value);
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            int nLastSelectedMIDIOutDev;
            try
            {
                // Load the last selected MIDI Output Device ID from the registry
                nLastSelectedMIDIOutDev = (int)Registry.GetValue(regKeyName, "OutputDevice", 0);
                // Load the last set Timer Resolution
                numericUpDown_TimerResolution.Value = (decimal)(int)Registry.GetValue(regKeyName, "TimerResolution", (int)20);
            }
            catch (NullReferenceException)
            {
                nLastSelectedMIDIOutDev = 0;
                numericUpDown_TimerResolution.Value = 20;
            }

            // Populate the MIDI Output Devices

            uint nOutputDevices = midiOutGetNumDevs();

            for (uint i = 0; i < nOutputDevices; ++i)
            {
                MIDIOUTCAPS caps;
                midiOutGetDevCaps(i, out caps, Marshal.SizeOf(typeof(MIDIOUTCAPS)));
                comboBox_MIDIOutDevs.Items.Add(caps.szPname);
            }

            if (comboBox_MIDIOutDevs.Items.Count > nLastSelectedMIDIOutDev)
                comboBox_MIDIOutDevs.SelectedIndex = nLastSelectedMIDIOutDev;
        }

        struct PlayOpts
        {
            public String strNotes;
            public int nMidiOutPort;
            public uint nTimerRes;
        };

        private void button_Play_Click(object sender, EventArgs e)
        {
            listView_Log.Items.Clear();

            if (textBox_Notes.Text.Length != 0)
            {
                this.button_ToMIDI.Enabled = false;
                this.button_Play.Enabled = false;
                this.numericUpDown_TimerResolution.Enabled = false;
                this.comboBox_MIDIOutDevs.Enabled = false;

                string strNotes="";

                if(checkBox_PlaySelected.Checked) // Do we need to play only the selected text?
                    strNotes = textBox_Notes.SelectedText;

                if(strNotes.Length == 0 || checkBox_PlaySelected.Checked == false)
                    strNotes = textBox_Notes.Text; //if selected text is empty or if we need to play complete text

                PlayOpts opts;
                opts.strNotes = strNotes;
                opts.nMidiOutPort = comboBox_MIDIOutDevs.SelectedIndex;
                opts.nTimerRes = (uint)numericUpDown_TimerResolution.Value;

                // Start playing the notes on background worker thread
                this.bgWorker_Play.RunWorkerAsync(opts);
            }
        }

        private void linkLabel_TextBoxProperties_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            propertyGrid1.Visible = !propertyGrid1.Visible;
            linkLabel_TextBoxProperties.Text = propertyGrid1.Visible ? "Hide Properties" : "TextBox Properties";
        }

        private void button_ToMIDI_Click(object sender, EventArgs e)
        {
            if (textBox_Notes.Text.Length != 0)
            {
                SaveFileDialog dlg = new SaveFileDialog();
                dlg.DefaultExt = "mid";
                dlg.FileName = "Output.mid";
                if(dlg.ShowDialog() != DialogResult.OK) return;

                this.Cursor = Cursors.WaitCursor;
                MusicNoteLib.SaveAsMidiFile(textBox_Notes.Text, dlg.FileName);
                this.Cursor = Cursors.Default;
            }
            else MessageBox.Show("Text box is empty !! Enter few notes and then use this option to save them to MIDI output file");
        }

        private void listView_Log_Resize(object sender, EventArgs e)
        {
            listView_Log.Columns[0].Width = listView_Log.ClientSize.Width;
        }

        private void bgWorker_Play_DoWork(object sender, DoWorkEventArgs e)
        {
            BackgroundWorker bw = sender as BackgroundWorker;

            PlayOpts opts = (PlayOpts)e.Argument;
            
            MusicNoteLib.PlayMusicStringWithOptsCB(opts.strNotes, opts.nMidiOutPort, opts.nTimerRes,
                new MusicNoteLib.ParserTraceDelegate(OnParseTrace),
                new MusicNoteLib.ParserErrorDelegate(OnParseError), IntPtr.Zero);
        }

        private void bgWorker_Play_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            this.button_Play.Enabled = true;
            this.button_ToMIDI.Enabled = true;
            this.comboBox_MIDIOutDevs.Enabled = true;
            this.numericUpDown_TimerResolution.Enabled = true;
        }

        private void bgWorker_Play_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            string szTraceMsg = (string)e.UserState;

            if (splitContainer1.Panel2Collapsed)
                splitContainer1.Panel2Collapsed = false;

            listView_Log.Items.Add(szTraceMsg);
        }
#if UNICODE
        public void OnParseTrace(IntPtr userData, [MarshalAs(UnmanagedType.LPWStr)] String szTraceMsg)
        {
            bgWorker_Play.ReportProgress(0, szTraceMsg);
        }

        public void OnParseError(IntPtr userData, int errCode, [MarshalAs(UnmanagedType.LPWStr)] String szErrorMsg, [MarshalAs(UnmanagedType.LPWStr)] String szToken)
        {
            bgWorker_Play.ReportProgress(0, szErrorMsg + " while parsing: " + szToken);
        }

#else // MBCS
        public void OnParseTrace(IntPtr userData, [MarshalAs(UnmanagedType.LPStr)] String szTraceMsg)
        {
            bgWorker_Play.ReportProgress(0, szTraceMsg);
        }

        public void OnParseError(IntPtr userData, int errCode, [MarshalAs(UnmanagedType.LPStr)] String szErrorMsg, [MarshalAs(UnmanagedType.LPStr)] String szToken)
        {
            bgWorker_Play.ReportProgress(0, szErrorMsg + " while parsing: " + szToken);
        }
#endif // UNICODE
    }
}
