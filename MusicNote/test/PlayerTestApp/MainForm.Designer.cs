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
            this.button1 = new System.Windows.Forms.Button();
            this.button_Play = new System.Windows.Forms.Button();
            this.linkLabel_TextBoxProperties = new System.Windows.Forms.LinkLabel();
            this.checkBox_PlaySelected = new System.Windows.Forms.CheckBox();
            this.propertyGrid1 = new System.Windows.Forms.PropertyGrid();
            this.listView_Log = new System.Windows.Forms.ListView();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panel_Commands.SuspendLayout();
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
            this.splitContainer1.Size = new System.Drawing.Size(792, 555);
            this.splitContainer1.SplitterDistance = 400;
            this.splitContainer1.TabIndex = 0;
            // 
            // splitter_PropertyGrid
            // 
            this.splitter_PropertyGrid.Dock = System.Windows.Forms.DockStyle.Right;
            this.splitter_PropertyGrid.Location = new System.Drawing.Point(593, 0);
            this.splitter_PropertyGrid.Name = "splitter_PropertyGrid";
            this.splitter_PropertyGrid.Size = new System.Drawing.Size(3, 471);
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
            this.textBox_Notes.Size = new System.Drawing.Size(596, 471);
            this.textBox_Notes.TabIndex = 7;
            // 
            // panel_Commands
            // 
            this.panel_Commands.Controls.Add(this.button1);
            this.panel_Commands.Controls.Add(this.button_Play);
            this.panel_Commands.Controls.Add(this.linkLabel_TextBoxProperties);
            this.panel_Commands.Controls.Add(this.checkBox_PlaySelected);
            this.panel_Commands.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel_Commands.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
            this.panel_Commands.Location = new System.Drawing.Point(0, 471);
            this.panel_Commands.Name = "panel_Commands";
            this.panel_Commands.Padding = new System.Windows.Forms.Padding(0, 8, 16, 8);
            this.panel_Commands.Size = new System.Drawing.Size(596, 84);
            this.panel_Commands.TabIndex = 6;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(502, 11);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 32);
            this.button1.TabIndex = 1;
            this.button1.Text = "To MIDI";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // button_Play
            // 
            this.panel_Commands.SetFlowBreak(this.button_Play, true);
            this.button_Play.Location = new System.Drawing.Point(421, 11);
            this.button_Play.Name = "button_Play";
            this.button_Play.Size = new System.Drawing.Size(75, 32);
            this.button_Play.TabIndex = 0;
            this.button_Play.Text = "Play !!";
            this.button_Play.UseVisualStyleBackColor = true;
            this.button_Play.Click += new System.EventHandler(this.button_Play_Click);
            // 
            // linkLabel_TextBoxProperties
            // 
            this.linkLabel_TextBoxProperties.Location = new System.Drawing.Point(450, 46);
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
            this.checkBox_PlaySelected.Location = new System.Drawing.Point(328, 49);
            this.checkBox_PlaySelected.Name = "checkBox_PlaySelected";
            this.checkBox_PlaySelected.Size = new System.Drawing.Size(116, 21);
            this.checkBox_PlaySelected.TabIndex = 2;
            this.checkBox_PlaySelected.Text = "Play Selected";
            this.checkBox_PlaySelected.UseVisualStyleBackColor = true;
            // 
            // propertyGrid1
            // 
            this.propertyGrid1.Dock = System.Windows.Forms.DockStyle.Right;
            this.propertyGrid1.Location = new System.Drawing.Point(596, 0);
            this.propertyGrid1.Name = "propertyGrid1";
            this.propertyGrid1.SelectedObject = this.textBox_Notes;
            this.propertyGrid1.Size = new System.Drawing.Size(196, 555);
            this.propertyGrid1.TabIndex = 5;
            this.propertyGrid1.Visible = false;
            // 
            // listView_Log
            // 
            this.listView_Log.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("listView_Log.BackgroundImage")));
            this.listView_Log.BackgroundImageTiled = true;
            this.listView_Log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listView_Log.FullRowSelect = true;
            this.listView_Log.GridLines = true;
            this.listView_Log.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.listView_Log.Location = new System.Drawing.Point(0, 0);
            this.listView_Log.MultiSelect = false;
            this.listView_Log.Name = "listView_Log";
            this.listView_Log.Size = new System.Drawing.Size(792, 151);
            this.listView_Log.TabIndex = 0;
            this.listView_Log.UseCompatibleStateImageBehavior = false;
            this.listView_Log.View = System.Windows.Forms.View.Details;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(792, 555);
            this.Controls.Add(this.splitContainer1);
            this.Name = "MainForm";
            this.Text = "MusicString Player Test Application";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.panel_Commands.ResumeLayout(false);
            this.panel_Commands.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TextBox textBox_Notes;
        private System.Windows.Forms.FlowLayoutPanel panel_Commands;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button_Play;
        private System.Windows.Forms.LinkLabel linkLabel_TextBoxProperties;
        private System.Windows.Forms.CheckBox checkBox_PlaySelected;
        private System.Windows.Forms.PropertyGrid propertyGrid1;
        private System.Windows.Forms.ListView listView_Log;
        private System.Windows.Forms.Splitter splitter_PropertyGrid;

    }
}