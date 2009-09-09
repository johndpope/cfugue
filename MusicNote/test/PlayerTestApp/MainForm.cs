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
                PlayMusicString(textBox_Notes.Text);
            }
        }
    }
}
