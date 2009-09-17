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

        public void OnErrorMethod(IntPtr userData, [MarshalAs(UnmanagedType.LPStr)] String szTraceMsg)
        {
            if (splitContainer1.Panel2Collapsed)
                splitContainer1.Panel2Collapsed = false;
            listView_Log.Items.Add(szTraceMsg);
            Application.DoEvents();
        }


        private void button_Play_Click(object sender, EventArgs e)
        {
            listView_Log.Items.Clear();

            if (textBox_Notes.Text.Length != 0)
            {
                string strNotes="";

                if(checkBox_PlaySelected.Checked) // Do we need to play only the selected text?
                    strNotes = textBox_Notes.SelectedText;

                if(strNotes.Length == 0 || checkBox_PlaySelected.Checked == false)
                    strNotes = textBox_Notes.Text; //if selected text is empty or if we need to play complete text
                
                this.Cursor = Cursors.WaitCursor;
               // MusicNoteLib.PlayMusicStringWithOpts(strNotes, comboBox_MIDIOutDevs.SelectedIndex, (uint)numericUpDown_TimerResolution.Value);
                MusicNoteLib.Parse(strNotes, new MusicNoteLib.ParserTraceDelegate(OnErrorMethod), IntPtr.Zero);
                this.Cursor = Cursors.Arrow;
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
                dlg.DefaultExt = "midi";
                dlg.FileName = "Output.midi";
                if(dlg.ShowDialog() != DialogResult.OK) return;

                MusicNoteLib.SaveAsMidiFile(textBox_Notes.Text, dlg.FileName);
            }
            else MessageBox.Show("Text box is empty !! Enter few notes and then use this option to save them to MIDI output file");
        }

        private void listView_Log_Resize(object sender, EventArgs e)
        {
            listView_Log.Columns[0].Width = listView_Log.ClientSize.Width;
        }


    }
}
