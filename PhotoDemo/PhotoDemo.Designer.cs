namespace PhotoDemo
{
    partial class ImageDemo
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
            this.grpIndex = new System.Windows.Forms.GroupBox();
            this.parameters = new System.Windows.Forms.GroupBox();
            this.txtContrThr = new System.Windows.Forms.TextBox();
            this.txtImgDbl = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.txtBoxL = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.txtBoxK = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.txtBoxW = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtBoxR = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.btnCreateIndex = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.btnFolder = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.txtInfo = new System.Windows.Forms.TextBox();
            this.btnOpenFile = new System.Windows.Forms.Button();
            this.grpGenerate = new System.Windows.Forms.GroupBox();
            this.lblKeyPointCount = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.lblQueryingIndexTime = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.lblGeneratingKeypointsTime = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.btnMatch = new System.Windows.Forms.Button();
            this.btnTransfer = new System.Windows.Forms.Button();
            this.panTransferImages = new System.Windows.Forms.FlowLayoutPanel();
            this.cmbMatch = new System.Windows.Forms.ComboBox();
            this.cmbTransfer = new System.Windows.Forms.ComboBox();
            this.btnBrowseImage = new System.Windows.Forms.Button();
            this.btnClear = new System.Windows.Forms.Button();
            this.imgForMatch = new System.Windows.Forms.PictureBox();
            this.grpMatch = new System.Windows.Forms.GroupBox();
            this.panMatchImages = new System.Windows.Forms.FlowLayoutPanel();
            this.dlgOpenFolder = new System.Windows.Forms.FolderBrowserDialog();
            this.dlgOpenImage = new System.Windows.Forms.OpenFileDialog();
            this.grpIndex.SuspendLayout();
            this.parameters.SuspendLayout();
            this.grpGenerate.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.imgForMatch)).BeginInit();
            this.grpMatch.SuspendLayout();
            this.SuspendLayout();
            // 
            // grpIndex
            // 
            this.grpIndex.Controls.Add(this.parameters);
            this.grpIndex.Controls.Add(this.btnCreateIndex);
            this.grpIndex.Controls.Add(this.label4);
            this.grpIndex.Controls.Add(this.btnFolder);
            this.grpIndex.Controls.Add(this.label1);
            this.grpIndex.Controls.Add(this.txtInfo);
            this.grpIndex.Controls.Add(this.btnOpenFile);
            this.grpIndex.Location = new System.Drawing.Point(5, 7);
            this.grpIndex.Name = "grpIndex";
            this.grpIndex.Size = new System.Drawing.Size(720, 123);
            this.grpIndex.TabIndex = 0;
            this.grpIndex.TabStop = false;
            this.grpIndex.Text = "Create Index";
            // 
            // parameters
            // 
            this.parameters.Controls.Add(this.txtContrThr);
            this.parameters.Controls.Add(this.txtImgDbl);
            this.parameters.Controls.Add(this.label11);
            this.parameters.Controls.Add(this.label8);
            this.parameters.Controls.Add(this.txtBoxL);
            this.parameters.Controls.Add(this.label6);
            this.parameters.Controls.Add(this.txtBoxK);
            this.parameters.Controls.Add(this.label5);
            this.parameters.Controls.Add(this.txtBoxW);
            this.parameters.Controls.Add(this.label3);
            this.parameters.Controls.Add(this.txtBoxR);
            this.parameters.Controls.Add(this.label2);
            this.parameters.Location = new System.Drawing.Point(8, 10);
            this.parameters.Name = "parameters";
            this.parameters.Size = new System.Drawing.Size(287, 100);
            this.parameters.TabIndex = 7;
            this.parameters.TabStop = false;
            this.parameters.Text = "参数设置";
            // 
            // txtContrThr
            // 
            this.txtContrThr.Location = new System.Drawing.Point(235, 56);
            this.txtContrThr.Name = "txtContrThr";
            this.txtContrThr.Size = new System.Drawing.Size(44, 21);
            this.txtContrThr.TabIndex = 11;
            this.txtContrThr.Text = "0.04";
            // 
            // txtImgDbl
            // 
            this.txtImgDbl.Location = new System.Drawing.Point(235, 17);
            this.txtImgDbl.Name = "txtImgDbl";
            this.txtImgDbl.Size = new System.Drawing.Size(44, 21);
            this.txtImgDbl.TabIndex = 10;
            this.txtImgDbl.Text = "0";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(169, 57);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(59, 12);
            this.label11.TabIndex = 9;
            this.label11.Text = "CONTR_THR";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(171, 20);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(47, 12);
            this.label8.TabIndex = 8;
            this.label8.Text = "IMG_DBL";
            // 
            // txtBoxL
            // 
            this.txtBoxL.Location = new System.Drawing.Point(110, 54);
            this.txtBoxL.Name = "txtBoxL";
            this.txtBoxL.Size = new System.Drawing.Size(42, 21);
            this.txtBoxL.TabIndex = 7;
            this.txtBoxL.Text = "0";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(93, 57);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(11, 12);
            this.label6.TabIndex = 6;
            this.label6.Text = "L";
            // 
            // txtBoxK
            // 
            this.txtBoxK.Location = new System.Drawing.Point(25, 54);
            this.txtBoxK.Name = "txtBoxK";
            this.txtBoxK.Size = new System.Drawing.Size(49, 21);
            this.txtBoxK.TabIndex = 5;
            this.txtBoxK.Text = "0";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 57);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(11, 12);
            this.label5.TabIndex = 4;
            this.label5.Text = "K";
            // 
            // txtBoxW
            // 
            this.txtBoxW.Location = new System.Drawing.Point(110, 15);
            this.txtBoxW.Name = "txtBoxW";
            this.txtBoxW.Size = new System.Drawing.Size(42, 21);
            this.txtBoxW.TabIndex = 3;
            this.txtBoxW.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(93, 18);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(11, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "W";
            // 
            // txtBoxR
            // 
            this.txtBoxR.Location = new System.Drawing.Point(25, 15);
            this.txtBoxR.Name = "txtBoxR";
            this.txtBoxR.Size = new System.Drawing.Size(49, 21);
            this.txtBoxR.TabIndex = 1;
            this.txtBoxR.Text = "0";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 18);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(11, 12);
            this.label2.TabIndex = 0;
            this.label2.Text = "R";
            // 
            // btnCreateIndex
            // 
            this.btnCreateIndex.Enabled = false;
            this.btnCreateIndex.Location = new System.Drawing.Point(578, 38);
            this.btnCreateIndex.Name = "btnCreateIndex";
            this.btnCreateIndex.Size = new System.Drawing.Size(112, 23);
            this.btnCreateIndex.TabIndex = 6;
            this.btnCreateIndex.Text = "Create Index";
            this.btnCreateIndex.UseVisualStyleBackColor = true;
            this.btnCreateIndex.Click += new System.EventHandler(this.btnCreateIndex_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(303, 40);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(41, 12);
            this.label4.TabIndex = 5;
            this.label4.Text = "Notes:";
            // 
            // btnFolder
            // 
            this.btnFolder.Location = new System.Drawing.Point(578, 10);
            this.btnFolder.Name = "btnFolder";
            this.btnFolder.Size = new System.Drawing.Size(112, 23);
            this.btnFolder.TabIndex = 4;
            this.btnFolder.Text = "Browse Folder...";
            this.btnFolder.UseVisualStyleBackColor = true;
            this.btnFolder.Click += new System.EventHandler(this.btnFolder_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(301, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(149, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "Browse and Select files:";
            // 
            // txtInfo
            // 
            this.txtInfo.AcceptsReturn = true;
            this.txtInfo.Location = new System.Drawing.Point(363, 38);
            this.txtInfo.Name = "txtInfo";
            this.txtInfo.ReadOnly = true;
            this.txtInfo.Size = new System.Drawing.Size(207, 21);
            this.txtInfo.TabIndex = 1;
            this.txtInfo.Text = "Browse to select files first.";
            // 
            // btnOpenFile
            // 
            this.btnOpenFile.Location = new System.Drawing.Point(459, 10);
            this.btnOpenFile.Name = "btnOpenFile";
            this.btnOpenFile.Size = new System.Drawing.Size(111, 23);
            this.btnOpenFile.TabIndex = 0;
            this.btnOpenFile.Text = "Browse Files...";
            this.btnOpenFile.UseVisualStyleBackColor = true;
            this.btnOpenFile.Click += new System.EventHandler(this.btnOpenFile_Click);
            // 
            // grpGenerate
            // 
            this.grpGenerate.Controls.Add(this.lblKeyPointCount);
            this.grpGenerate.Controls.Add(this.label10);
            this.grpGenerate.Controls.Add(this.lblQueryingIndexTime);
            this.grpGenerate.Controls.Add(this.label9);
            this.grpGenerate.Controls.Add(this.lblGeneratingKeypointsTime);
            this.grpGenerate.Controls.Add(this.label7);
            this.grpGenerate.Controls.Add(this.btnMatch);
            this.grpGenerate.Controls.Add(this.btnTransfer);
            this.grpGenerate.Controls.Add(this.panTransferImages);
            this.grpGenerate.Controls.Add(this.cmbMatch);
            this.grpGenerate.Controls.Add(this.cmbTransfer);
            this.grpGenerate.Controls.Add(this.btnBrowseImage);
            this.grpGenerate.Controls.Add(this.btnClear);
            this.grpGenerate.Controls.Add(this.imgForMatch);
            this.grpGenerate.Location = new System.Drawing.Point(5, 136);
            this.grpGenerate.Name = "grpGenerate";
            this.grpGenerate.Size = new System.Drawing.Size(720, 217);
            this.grpGenerate.TabIndex = 1;
            this.grpGenerate.TabStop = false;
            this.grpGenerate.Text = "Generate Pictures";
            // 
            // lblKeyPointCount
            // 
            this.lblKeyPointCount.AutoSize = true;
            this.lblKeyPointCount.Location = new System.Drawing.Point(324, 162);
            this.lblKeyPointCount.Name = "lblKeyPointCount";
            this.lblKeyPointCount.Size = new System.Drawing.Size(83, 12);
            this.lblKeyPointCount.TabIndex = 17;
            this.lblKeyPointCount.Text = "Not Available";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(195, 162);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(107, 12);
            this.label10.TabIndex = 16;
            this.label10.Text = "KeyPoints Number:";
            // 
            // lblQueryingIndexTime
            // 
            this.lblQueryingIndexTime.AutoSize = true;
            this.lblQueryingIndexTime.Location = new System.Drawing.Point(292, 195);
            this.lblQueryingIndexTime.Name = "lblQueryingIndexTime";
            this.lblQueryingIndexTime.Size = new System.Drawing.Size(23, 12);
            this.lblQueryingIndexTime.TabIndex = 15;
            this.lblQueryingIndexTime.Text = "0ms";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(195, 195);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(95, 12);
            this.label9.TabIndex = 14;
            this.label9.Text = "Querying Index:";
            // 
            // lblGeneratingKeypointsTime
            // 
            this.lblGeneratingKeypointsTime.AutoSize = true;
            this.lblGeneratingKeypointsTime.Location = new System.Drawing.Point(324, 179);
            this.lblGeneratingKeypointsTime.Name = "lblGeneratingKeypointsTime";
            this.lblGeneratingKeypointsTime.Size = new System.Drawing.Size(23, 12);
            this.lblGeneratingKeypointsTime.TabIndex = 13;
            this.lblGeneratingKeypointsTime.Text = "0ms";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(195, 179);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(131, 12);
            this.label7.TabIndex = 12;
            this.label7.Text = "Generating KeyPoints:";
            // 
            // btnMatch
            // 
            this.btnMatch.Location = new System.Drawing.Point(294, 131);
            this.btnMatch.Name = "btnMatch";
            this.btnMatch.Size = new System.Drawing.Size(74, 23);
            this.btnMatch.TabIndex = 11;
            this.btnMatch.Text = "Match";
            this.btnMatch.UseVisualStyleBackColor = true;
            this.btnMatch.Click += new System.EventHandler(this.btnMatch_Click);
            // 
            // btnTransfer
            // 
            this.btnTransfer.Location = new System.Drawing.Point(294, 56);
            this.btnTransfer.Name = "btnTransfer";
            this.btnTransfer.Size = new System.Drawing.Size(75, 23);
            this.btnTransfer.TabIndex = 10;
            this.btnTransfer.Text = "Transfer";
            this.btnTransfer.UseVisualStyleBackColor = true;
            this.btnTransfer.Click += new System.EventHandler(this.btnTransfer_Click);
            // 
            // panTransferImages
            // 
            this.panTransferImages.AutoScroll = true;
            this.panTransferImages.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panTransferImages.Location = new System.Drawing.Point(378, 15);
            this.panTransferImages.Name = "panTransferImages";
            this.panTransferImages.Size = new System.Drawing.Size(336, 196);
            this.panTransferImages.TabIndex = 9;
            // 
            // cmbMatch
            // 
            this.cmbMatch.FormattingEnabled = true;
            this.cmbMatch.Location = new System.Drawing.Point(197, 132);
            this.cmbMatch.Name = "cmbMatch";
            this.cmbMatch.Size = new System.Drawing.Size(91, 20);
            this.cmbMatch.TabIndex = 7;
            // 
            // cmbTransfer
            // 
            this.cmbTransfer.FormattingEnabled = true;
            this.cmbTransfer.Location = new System.Drawing.Point(197, 57);
            this.cmbTransfer.Name = "cmbTransfer";
            this.cmbTransfer.Size = new System.Drawing.Size(91, 20);
            this.cmbTransfer.TabIndex = 5;
            // 
            // btnBrowseImage
            // 
            this.btnBrowseImage.Location = new System.Drawing.Point(194, 21);
            this.btnBrowseImage.Name = "btnBrowseImage";
            this.btnBrowseImage.Size = new System.Drawing.Size(175, 23);
            this.btnBrowseImage.TabIndex = 4;
            this.btnBrowseImage.Text = "Browse Image...";
            this.btnBrowseImage.UseVisualStyleBackColor = true;
            this.btnBrowseImage.Click += new System.EventHandler(this.btnBrowseImage_Click);
            // 
            // btnClear
            // 
            this.btnClear.Location = new System.Drawing.Point(194, 95);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(175, 23);
            this.btnClear.TabIndex = 3;
            this.btnClear.Text = "Clear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // imgForMatch
            // 
            this.imgForMatch.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.imgForMatch.Location = new System.Drawing.Point(6, 20);
            this.imgForMatch.Name = "imgForMatch";
            this.imgForMatch.Size = new System.Drawing.Size(182, 191);
            this.imgForMatch.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.imgForMatch.TabIndex = 1;
            this.imgForMatch.TabStop = false;
            // 
            // grpMatch
            // 
            this.grpMatch.Controls.Add(this.panMatchImages);
            this.grpMatch.Location = new System.Drawing.Point(5, 359);
            this.grpMatch.Name = "grpMatch";
            this.grpMatch.Size = new System.Drawing.Size(720, 314);
            this.grpMatch.TabIndex = 2;
            this.grpMatch.TabStop = false;
            this.grpMatch.Text = "Match Pictures";
            // 
            // panMatchImages
            // 
            this.panMatchImages.AutoScroll = true;
            this.panMatchImages.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panMatchImages.Location = new System.Drawing.Point(6, 15);
            this.panMatchImages.Name = "panMatchImages";
            this.panMatchImages.Size = new System.Drawing.Size(708, 293);
            this.panMatchImages.TabIndex = 0;
            // 
            // dlgOpenImage
            // 
            this.dlgOpenImage.Multiselect = true;
            this.dlgOpenImage.RestoreDirectory = true;
            // 
            // ImageDemo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(732, 674);
            this.Controls.Add(this.grpMatch);
            this.Controls.Add(this.grpGenerate);
            this.Controls.Add(this.grpIndex);
            this.MaximizeBox = false;
            this.Name = "ImageDemo";
            this.Text = "ImageDemo";
            this.grpIndex.ResumeLayout(false);
            this.grpIndex.PerformLayout();
            this.parameters.ResumeLayout(false);
            this.parameters.PerformLayout();
            this.grpGenerate.ResumeLayout(false);
            this.grpGenerate.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.imgForMatch)).EndInit();
            this.grpMatch.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox grpIndex;
        private System.Windows.Forms.GroupBox grpGenerate;
        private System.Windows.Forms.GroupBox grpMatch;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtInfo;
        private System.Windows.Forms.Button btnOpenFile;
        private System.Windows.Forms.FolderBrowserDialog dlgOpenFolder;
        private System.Windows.Forms.OpenFileDialog dlgOpenImage;
        private System.Windows.Forms.FlowLayoutPanel panMatchImages;
        private System.Windows.Forms.PictureBox imgForMatch;
        private System.Windows.Forms.Button btnFolder;
        private System.Windows.Forms.ComboBox cmbMatch;
        private System.Windows.Forms.ComboBox cmbTransfer;
        private System.Windows.Forms.Button btnBrowseImage;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.FlowLayoutPanel panTransferImages;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button btnCreateIndex;
        private System.Windows.Forms.Button btnMatch;
        private System.Windows.Forms.Button btnTransfer;
        private System.Windows.Forms.GroupBox parameters;
        private System.Windows.Forms.TextBox txtBoxL;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtBoxK;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtBoxW;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox txtBoxR;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label lblQueryingIndexTime;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label lblGeneratingKeypointsTime;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label lblKeyPointCount;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtContrThr;
        private System.Windows.Forms.TextBox txtImgDbl;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label8;
    }
}

