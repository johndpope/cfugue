using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace PlayerTestApp
{
    public partial class MainForm : Form
    {
        [DllImport("MusicNoteDll.Dll")]
        public static extern void PlayMusicString([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes);

        [DllImport("MusicNoteDll.Dll")]
        public static extern void PlayMusicStringWithOpts([MarshalAs(UnmanagedType.LPStr)] String szMusicNotes, int nMidiOutPortID, int nTimerResMS);

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {

        }

        private void button_Play_Click(object sender, EventArgs e)
        {
            if (textBox_Notes.Text.Length != 0)
            {
                string strNotes="";

                if(checkBox_PlaySelected.Checked) // Do we need to play only the selected text?
                    strNotes = textBox_Notes.SelectedText;

                if(strNotes.Length == 0 || checkBox_PlaySelected.Checked == false)
                    strNotes = textBox_Notes.Text; //if selected text is empty or if we need to play complete text

                PlayMusicString(strNotes);
            }
        }

        private void linkLabel_TextBoxProperties_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            propertyGrid1.Visible = !propertyGrid1.Visible;
            linkLabel_TextBoxProperties.Text = propertyGrid1.Visible ? "Hide Properties" : "TextBox Properties";
        }
    }
}
