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
            this.panel1 = new System.Windows.Forms.Panel();
            this.textBox_Notes = new System.Windows.Forms.TextBox();
            this.panel_Commands = new System.Windows.Forms.FlowLayoutPanel();
            this.button_Play = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.panel_Commands.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.textBox_Notes);
            this.panel1.Controls.Add(this.panel_Commands);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(792, 455);
            this.panel1.TabIndex = 1;
            // 
            // textBox_Notes
            // 
            this.textBox_Notes.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textBox_Notes.Location = new System.Drawing.Point(0, 0);
            this.textBox_Notes.Multiline = true;
            this.textBox_Notes.Name = "textBox_Notes";
            this.textBox_Notes.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.textBox_Notes.Size = new System.Drawing.Size(790, 369);
            this.textBox_Notes.TabIndex = 2;
            // 
            // panel_Commands
            // 
            this.panel_Commands.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel_Commands.Controls.Add(this.button1);
            this.panel_Commands.Controls.Add(this.button_Play);
            this.panel_Commands.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel_Commands.FlowDirection = System.Windows.Forms.FlowDirection.RightToLeft;
            this.panel_Commands.Location = new System.Drawing.Point(0, 369);
            this.panel_Commands.Name = "panel_Commands";
            this.panel_Commands.Padding = new System.Windows.Forms.Padding(0, 16, 32, 0);
            this.panel_Commands.Size = new System.Drawing.Size(790, 84);
            this.panel_Commands.TabIndex = 1;
            // 
            // button_Play
            // 
            this.button_Play.Location = new System.Drawing.Point(597, 19);
            this.button_Play.Name = "button_Play";
            this.button_Play.Size = new System.Drawing.Size(75, 32);
            this.button_Play.TabIndex = 0;
            this.button_Play.Text = "Play !!";
            this.button_Play.UseVisualStyleBackColor = true;
            this.button_Play.Click += new System.EventHandler(this.button_Play_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(678, 19);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 32);
            this.button1.TabIndex = 1;
            this.button1.Text = "To MIDI";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(792, 455);
            this.Controls.Add(this.panel1);
            this.Name = "MainForm";
            this.Text = "MusicString Player Test Application";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel_Commands.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox textBox_Notes;
        private System.Windows.Forms.FlowLayoutPanel panel_Commands;
        private System.Windows.Forms.Button button_Play;
        private System.Windows.Forms.Button button1;
    }
}