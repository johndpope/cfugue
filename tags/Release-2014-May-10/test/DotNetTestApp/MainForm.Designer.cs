namespace PlayerTestApp
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitter_PropertyGrid = new System.Windows.Forms.Splitter();
            this.textBox_Notes = new System.Windows.Forms.TextBox();
            this.panel_Commands = new System.Windows.Forms.FlowLayoutPanel();
            this.button_ToMIDI = new System.Windows.Forms.Button();
            this.button_Play = new System.Windows.Forms.Button();
            this.flowLayoutPanel_MIDIOutDevices = new System.Windows.Forms.FlowLayoutPanel();
            this.label1 = new System.Windows.Forms.Label();
            this.comboBox_MIDIOutDevs = new System.Windows.Forms.ComboBox();
            this.flowLayoutPanel_TimerResolution = new System.Windows.Forms.FlowLayoutPanel();
            this.label2 = new System.Windows.Forms.Label();
            this.numericUpDown_TimerResolution = new System.Windows.Forms.NumericUpDown();
            this.linkLabel_TextBoxProperties = new System.Windows.Forms.LinkLabel();
            this.checkBox_PlaySelected = new System.Windows.Forms.CheckBox();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.listView_Log = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.bgWorker_Play = new System.ComponentModel.BackgroundWorker();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panel_Commands.SuspendLayout();
            this.flowLayoutPanel_MIDIOutDevices.SuspendLayout();
            this.flowLayoutPanel_TimerResolution.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_TimerResolution)).BeginInit();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitter_PropertyGrid);
            this.splitContainer1.Panel1.Controls.Add(this.textBox_Notes);
            this.splitContainer1.Panel1.Controls.Add(this.panel_Commands);
            this.splitContainer1.Panel1.Controls.Add(this.propertyGrid1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.listView_Log);
            this.splitContainer1.Panel2Collapsed = true;
            this.splitContainer1.Size = new System.Drawing.Size(832, 435);
            this.splitContainer1.SplitterDistance = 200;
            this.splitContainer1.TabIndex = 0;
            // 
            // splitter_PropertyGrid
            // 
            this.splitter_PropertyGrid.Dock = System.Windows.Forms.DockStyle.Right;
            this.splitter_PropertyGrid.Location = new System.Drawing.Point(633, 0);
            this.splitter_PropertyGrid.Name = "splitter_PropertyGrid";
            this.splitter_PropertyGrid.Size = new System.Drawing.Size(3, 351);
            this.splitter_PropertyGrid.TabIndex = 8;
            this.splitter_PropertyGrid.TabStop = false;
            // 
            // textBox_Notes
            // 
            this.textBox_Notes.AutoCompleteCustomSource.AddRange(new string[] {
            "A",
            "B",
            "C",
            "D",
            "E",
            "F",
            "G",
            "AX",
            "BX",
            "CX",
            "DX",
            "EX",
            "FX",
            "GX",
            "AI",
            "BI",
            "CI",
            "DI",
            "EI",
            "FI",
            "GI",
            "AI.",
            "BI.",
            "CI.",
            "DI.",
            "EI.",
            "FI.",
            "GI.",
            "A4I",
            "B4I",
            "C4I",
            "D4I",
            "E4I",
            "F4I",
            "G4I",
            "A6I",
            "B6I",
            "C6I",
            "D6I",
            "E6I",
            "F6I",
            "G6I"});
            this.textBox_Notes.CharacterCasing = System.Windows.Forms.CharacterCasing.Upper;
            this.textBox_Notes.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox_Notes.HideSelection = false;
            this.textBox_Notes.Location = new System.Drawing.Point(0, 0);
            this.textBox_Notes.Multiline = true;
            this.textBox_Notes.Name = "textBox_Notes";
            this.textBox_Notes.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox_Notes.Size = new System.Drawing.Size(636, 351);
            this.textBox_Notes.TabIndex = 7;
            this.textBox_Notes.Text = "I[FLUTE] C D E F G A B";
            // 
            // panel_Commands
            // 
            this.panel_Commands.Controls.Add(this.button_ToMIDI);
            this.panel_Commands.Controls.Add(this.button_Play);
            this.panel_Commands.Controls.Add(this.flowLayoutPanel_MIDIOutDevices);
            this.panel_Commands.Controls.Add(this.flowLayoutPanel_TimerResolution);
            this.panel_Commands.Controls.Add(this.linkLabel_TextBoxProperties);
            this.panel_Commands.Controls.Add(this.checkBox_PlaySelected);
            this.panel_Commands.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel_Commands.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
            this.panel_Commands.Location = new System.Drawing.Point(0, 351);
            this.panel_Commands.Name = "panel_Commands";
            this.panel_Commands.Padding = new System.Windows.Forms.Padding(0, 8, 16, 8);
            this.panel_Commands.Size = new System.Drawing.Size(636, 84);
            this.panel_Commands.TabIndex = 6;
            // 
            // button_ToMIDI
            // 
            this.button_ToMIDI.Location = new System.Drawing.Point(542, 11);
            this.button_ToMIDI.Name = "button_ToMIDI";
            this.button_ToMIDI.Size = new System.Drawing.Size(75, 32);
            this.button_ToMIDI.TabIndex = 1;
            this.button_ToMIDI.Text = "To MIDI";
            this.button_ToMIDI.UseVisualStyleBackColor = true;
            this.button_ToMIDI.Click += new System.EventHandler(this.button_ToMIDI_Click);
            // 
            // button_Play
            // 
            this.button_Play.Location = new System.Drawing.Point(461, 11);
            this.button_Play.Name = "button_Play";
            this.button_Play.Size = new System.Drawing.Size(75, 32);
            this.button_Play.TabIndex = 0;
            this.button_Play.Text = "Play !!";
            this.button_Play.UseVisualStyleBackColor = true;
            this.button_Play.Click += new System.EventHandler(this.button_Play_Click);
            // 
            // flowLayoutPanel_MIDIOutDevices
            // 
            this.flowLayoutPanel_MIDIOutDevices.AutoSize = true;
            this.flowLayoutPanel_MIDIOutDevices.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.flowLayoutPanel_MIDIOutDevices.Controls.Add(this.label1);
            this.flowLayoutPanel_MIDIOutDevices.Controls.Add(this.comboBox_MIDIOutDevs);
            this.flowLayoutPanel_MIDIOutDevices.Location = new System.Drawing.Point(191, 11);
            this.flowLayoutPanel_MIDIOutDevices.Name = "flowLayoutPanel_MIDIOutDevices";
            this.flowLayoutPanel_MIDIOutDevices.Size = new System.Drawing.Size(264, 32);
            this.flowLayoutPanel_MIDIOutDevices.TabIndex = 5;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(3, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(70, 24);
            this.label1.TabIndex = 1;
            this.label1.Text = "MIDI Out:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // comboBox_MIDIOutDevs
            // 
            this.comboBox_MIDIOutDevs.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_MIDIOutDevs.FormattingEnabled = true;
            this.comboBox_MIDIOutDevs.Location = new System.Drawing.Point(79, 3);
            this.comboBox_MIDIOutDevs.Name = "comboBox_MIDIOutDevs";
            this.comboBox_MIDIOutDevs.Size = new System.Drawing.Size(180, 24);
            this.comboBox_MIDIOutDevs.TabIndex = 0;
            // 
            // flowLayoutPanel_TimerResolution
            // 
            this.flowLayoutPanel_TimerResolution.AutoSize = true;
            this.flowLayoutPanel_TimerResolution.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.flowLayoutPanel_TimerResolution.Controls.Add(this.label2);
            this.flowLayoutPanel_TimerResolution.Controls.Add(this.numericUpDown_TimerResolution);
            this.panel_Commands.SetFlowBreak(this.flowLayoutPanel_TimerResolution, true);
            this.flowLayoutPanel_TimerResolution.Location = new System.Drawing.Point(43, 11);
            this.flowLayoutPanel_TimerResolution.Name = "flowLayoutPanel_TimerResolution";
            this.flowLayoutPanel_TimerResolution.Size = new System.Drawing.Size(142, 30);
            this.flowLayoutPanel_TimerResolution.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(3, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(80, 22);
            this.label2.TabIndex = 0;
            this.label2.Text = "Timer Res:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // numericUpDown_TimerResolution
            // 
            this.numericUpDown_TimerResolution.Location = new System.Drawing.Point(89, 3);
            this.numericUpDown_TimerResolution.Maximum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.numericUpDown_TimerResolution.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.numericUpDown_TimerResolution.Name = "numericUpDown_TimerResolution";
            this.numericUpDown_TimerResolution.Size = new System.Drawing.Size(48, 22);
            this.numericUpDown_TimerResolution.TabIndex = 1;
            this.numericUpDown_TimerResolution.Value = new decimal(new int[] {
            10,
            0,
            0,
            0});
            // 
            // linkLabel_TextBoxProperties
            // 
            this.linkLabel_TextBoxProperties.Location = new System.Drawing.Point(490, 46);
            this.linkLabel_TextBoxProperties.Name = "linkLabel_TextBoxProperties";
            this.linkLabel_TextBoxProperties.Size = new System.Drawing.Size(127, 24);
            this.linkLabel_TextBoxProperties.TabIndex = 3;
            this.linkLabel_TextBoxProperties.TabStop = true;
            this.linkLabel_TextBoxProperties.Text = "TextBox Properties";
            this.linkLabel_TextBoxProperties.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.linkLabel_TextBoxProperties.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel_TextBoxProperties_LinkClicked);
            // 
            // checkBox_PlaySelected
            // 
            this.checkBox_PlaySelected.AutoSize = true;
            this.checkBox_PlaySelected.Checked = true;
            this.checkBox_PlaySelected.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBox_PlaySelected.Location = new System.Drawing.Point(368, 49);
            this.checkBox_PlaySelected.Name = "checkBox_PlaySelected";
            this.checkBox_PlaySelected.Size = new System.Drawing.Size(116, 21);
            this.checkBox_PlaySelected.TabIndex = 2;
            this.checkBox_PlaySelected.Text = "Play Selected";
            this.checkBox_PlaySelected.UseVisualStyleBackColor = true;
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Right;
            this.propertyGrid1.Location = new System.Drawing.Point(636, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.SelectedObject = this.textBox_Notes;
            this.propertyGrid1.Size = new System.Drawing.Size(196, 435);
            this.propertyGrid1.TabIndex = 5;
            this.propertyGrid1.Visible = false;
            // 
            // listView_Log
            // 
            this.listView_Log.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("listView_Log.BackgroundImage")));
            this.listView_Log.BackgroundImageTiled = true;
            this.listView_Log.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
            this.listView_Log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView_Log.FullRowSelect = true;
            this.listView_Log.GridLines = true;
            this.listView_Log.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.listView_Log.Location = new System.Drawing.Point(0, 0);
            this.listView_Log.MultiSelect = false;
            this.listView_Log.Name = "listView_Log";
            this.listView_Log.Size = new System.Drawing.Size(150, 46);
            this.listView_Log.TabIndex = 0;
            this.listView_Log.UseCompatibleStateImageBehavior = false;
            this.listView_Log.View = System.Windows.Forms.View.Details;
            this.listView_Log.Resize += new System.EventHandler(this.listView_Log_Resize);
            // 
            // bgWorker_Play
            // 
            this.bgWorker_Play.WorkerReportsProgress = true;
            this.bgWorker_Play.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgWorker_Play_DoWork);
            this.bgWorker_Play.RunWorkerCompleted += new System.ComponentModel.RunWorkerCompletedEventHandler(this.bgWorker_Play_RunWorkerCompleted);
            this.bgWorker_Play.ProgressChanged += new System.ComponentModel.ProgressChangedEventHandler(this.bgWorker_Play_ProgressChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(832, 435);
            this.Controls.Add(this.splitContainer1);
            this.Name = "MainForm";
            this.Text = "MusicString Player Test Application";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.panel_Commands.ResumeLayout(false);
            this.panel_Commands.PerformLayout();
            this.flowLayoutPanel_MIDIOutDevices.ResumeLayout(false);
            this.flowLayoutPanel_TimerResolution.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_TimerResolution)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TextBox textBox_Notes;
        private System.Windows.Forms.FlowLayoutPanel panel_Commands;
        private System.Windows.Forms.Button button_ToMIDI;
        private System.Windows.Forms.Button button_Play;
        private System.Windows.Forms.LinkLabel linkLabel_TextBoxProperties;
        private System.Windows.Forms.CheckBox checkBox_PlaySelected;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.ListView listView_Log;
        private System.Windows.Forms.Splitter splitter_PropertyGrid;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox comboBox_MIDIOutDevs;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel_MIDIOutDevices;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel_TimerResolution;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown numericUpDown_TimerResolution;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.ComponentModel.BackgroundWorker bgWorker_Play;

    }
}