using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace PhotoDemo
{
    public partial class ImageDemo : Form
    {
        private static string currDir = Environment.CurrentDirectory;
        private static string dataDir = currDir + "\\data";
        private static string keypointFileForIndex = dataDir + "\\keypointIndex";
        private static string keypointFileForMatch = dataDir + "\\keypointMatch";
        private static string indexDir = dataDir + "\\indexes";
        private static string initEigsFile = dataDir + "\\eigs.txt";
        private static string indexInfoFile = indexDir + "\\indexInfo.txt";
        private static string indexFilePre = indexDir + "\\index"; // i.e. index1, begin from 1.
        private static string imgsForIndexFile = dataDir + "\\imgsForIndex.txt";
        private static string imgsForMatchFile = dataDir + "\\imgsForMatch.txt";
        private static string outputImgFile = dataDir + "\\outputImg.txt";
        private static Boolean imgFlag = false;

        private static int indexCount = 0;
        private static int MIN_MATCH = 10;

        /// <summary>
        /// Constructor. Initialize data, components and result directories.
        /// </summary>
        public ImageDemo()
        {
            /*
            if (!File.Exists(initEigsFile))
            {
                MessageBox.Show("The initial file \"" + initEigsFile + "\" is not exist.", "Error Message");
                this.Close();
                return;
            }
            // Call PCA_Sift_API.dll#initialeigs to initilize data.
            ServiceApi.initialeigs(initEigsFile.ToCharArray());
            */

            // Create directories if needed.
            if (!Directory.Exists(currDir))
            {
                Directory.CreateDirectory(currDir);
            }

            if (!Directory.Exists(currDir))
            {
                Directory.CreateDirectory(currDir);
            }

            if (!Directory.Exists(indexDir))
            {
                Directory.CreateDirectory(indexDir);
            }

            if (File.Exists(indexInfoFile))
            {
                try
                {
                    using (StreamReader sr = new StreamReader(indexInfoFile))
                    {
                        
                        String line = sr.ReadLine();
                        indexCount = int.Parse(line);
                        sr.Close();
                    }
                }
                catch (Exception e)
                {
                    // Do nothing.
                }

            }

            // Initialize componenets.
            InitializeComponent();

            btnTransfer.Enabled = false;
            btnMatch.Enabled = false;

            for (int i=1; i<=indexCount; i++)
            {
                string file = indexFilePre + i;
                if (File.Exists(file))
                {
                    ListItem item = new ListItem("index" + i, file);
                    cmbMatch.Items.Add(item);
                }
            }
            cmbMatch.DisplayMember = "Name";
            cmbMatch.ValueMember = "Value";
            if (cmbMatch.Items.Count > 0)
            {
                cmbMatch.SelectedIndex = 0;
            }
        }


        /// <summary>
        /// Multi-select files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnOpenFile_Click(object sender, EventArgs e)
        {
            dlgOpenImage.Filter = "jpeg (*.jpg;*.jpeg;*.jpe;*.jfif)|*.jpg;*.jpeg;*.jpe;*.jfif|All Image files|*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.png;*.bmp;*.ico;*.tif;*.tiff|All files (*.*)|*.*";
            dlgOpenImage.FilterIndex = 2;
            dlgOpenImage.Multiselect = true;
            if (dlgOpenImage.ShowDialog() == DialogResult.OK)
            {
                StreamWriter sw = File.CreateText(imgsForIndexFile);
                
                if (dlgOpenImage.FileNames != null)
                {
                    for (int i = 0; i < dlgOpenImage.FileNames.Length; i++)
                    {
                        sw.WriteLine(dlgOpenImage.FileNames[i]);
                    }
                    txtInfo.Text = dlgOpenImage.FileNames.Length + " files selected.";
                }
                else
                {
                    sw.WriteLine(dlgOpenImage.FileName);
                    txtInfo.Text = "1 file selected.";
                }

                btnCreateIndex.Enabled = true;

                sw.Close();
            }

        }

        /// <summary>
        /// Select files directly in the directory, including *.jpg, *.gif, *.png, *.bmp.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnFolder_Click(object sender, EventArgs e)
        {
            if (dlgOpenFolder.ShowDialog() == DialogResult.OK)
            {
                string[] jpgFiles = Directory.GetFiles(dlgOpenFolder.SelectedPath, "*.jpg");
                string[] gifFiles = Directory.GetFiles(dlgOpenFolder.SelectedPath, "*.gif");
                string[] pngFiles = Directory.GetFiles(dlgOpenFolder.SelectedPath, "*.png");
                string[] bmpFiles = Directory.GetFiles(dlgOpenFolder.SelectedPath, "*.bmp");
                int nFiles = jpgFiles.Length + gifFiles.Length + pngFiles.Length + bmpFiles.Length;
                if (nFiles > 0)
                {
                    StreamWriter sw = File.CreateText(imgsForIndexFile);
                    for (int i = 0; i < jpgFiles.Length; i++)
                    {
                        sw.WriteLine(jpgFiles[i]);
                    }
                    for (int i = 0; i < gifFiles.Length; i++)
                    {
                        sw.WriteLine(gifFiles[i]);
                    }
                    for (int i = 0; i < pngFiles.Length; i++)
                    {
                        sw.WriteLine(pngFiles[i]);
                    }
                    for (int i = 0; i < bmpFiles.Length; i++)
                    {
                        sw.WriteLine(bmpFiles[i]);
                    }

                    sw.Close();
                    btnCreateIndex.Enabled = true;
                }
                else
                {
                    btnCreateIndex.Enabled = false;
                }
                txtInfo.Text = nFiles + " file" + (nFiles > 1 ? "s" : "") + " selected.";
            }
        }

        private void btnBrowseImage_Click(object sender, EventArgs e)
        {
            dlgOpenImage.Filter = "jpeg (*.jpg;*.jpeg;*.jpe;*.jfif)|*.jpg;*.jpeg;*.jpe;*.jfif|All Image files|*.jpg;*.jpeg;*.jpe;*.jfif;*.gif;*.png;*.bmp;*.ico;*.tif;*.tiff|All files (*.*)|*.*";
            dlgOpenImage.FilterIndex = 2;
            dlgOpenImage.Multiselect = false;
            if (dlgOpenImage.ShowDialog() == DialogResult.OK)
            {
                StreamWriter sw = File.CreateText(imgsForMatchFile);
                sw.WriteLine(dlgOpenImage.FileName);
                sw.Close();
                imgForMatch.Image = Image.FromFile(dlgOpenImage.FileName);
                imgFlag = true;

                if (cmbMatch.SelectedIndex >= 0)
                {
                    btnTransfer.Enabled = true;
                    btnMatch.Enabled = true; 
                }
            }
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            panTransferImages.Controls.Clear();
        }

        private void btnCreateIndex_Click(object sender, EventArgs e)
        {
            if (!File.Exists(imgsForIndexFile))
            {
                btnCreateIndex.Enabled = false;
                MessageBox.Show("\"" + imgsForIndexFile + "\" not exists", "Error Message");
                return;
            }
            btnCreateIndex.Enabled = false;
            Int32 imgDbl = Int32.Parse(txtImgDbl.Text);
            // imgDbl can only be 0/1.
            if (imgDbl != 0)
            {
                imgDbl = 1;
            }
            ServiceApi.showSift(imgsForIndexFile.ToCharArray(), keypointFileForIndex.ToCharArray(), imgDbl, Double.Parse(txtContrThr.Text));
            string file = indexFilePre + ++indexCount;
            try
            {
                ServiceApi.setUpIndex(keypointFileForIndex.ToCharArray(), file.ToCharArray(), Double.Parse(txtBoxR.Text), Double.Parse(txtBoxW.Text), Int32.Parse(txtBoxK.Text), Int32.Parse(txtBoxL.Text));
            }
            catch (FormatException fe)
            {
                MessageBox.Show("Parameters must be a number");
            }
            catch (OverflowException ofe)
            {
                MessageBox.Show("Parameters is out of range");
            }
            StreamWriter sw = File.CreateText(indexInfoFile);
            sw.WriteLine(indexCount);
            sw.Close();

            if (File.Exists(file))
            {
                ListItem item = new ListItem("index" + indexCount, file);
                cmbMatch.Items.Add(item);
                if (cmbMatch.Items.Count > 0)
                {
                    cmbMatch.SelectedIndex = cmbMatch.Items.Count-1;
                    if (imgFlag)
                    {
                        if (!btnTransfer.Enabled)
                        {
                            btnTransfer.Enabled = true;
                        }
                        if (!btnMatch.Enabled)
                        {
                            btnMatch.Enabled = true;
                        }
                    }
                    
                }
            }
            btnCreateIndex.Enabled = true;
        }

        private void picTransfer_Click(object sender, EventArgs e)
        {
            PictureBox pic = (PictureBox)sender;
            imgForMatch.Image = pic.Image;
        }

        private void btnTransfer_Click(object sender, EventArgs e)
        {
            btnTransfer.Enabled = false;
            if (File.Exists(outputImgFile))
            {
                try
                {
                    using (StreamReader sr = new StreamReader(outputImgFile))
                    {

                        String line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            if (File.Exists(line))
                            {
                                Image img = Image.FromFile(line);
                                PictureBox pic = new PictureBox();
                                pic.Size = new System.Drawing.Size(72, 90);
                                pic.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
                                pic.Image = img;
                                pic.Click += new System.EventHandler(this.picTransfer_Click);
                                panTransferImages.Controls.Add(pic);
                            }

                        }

                        sr.Close();
                    }
                }
                catch (Exception ex)
                {
                    // Do nothing.
                }
            }
            btnTransfer.Enabled = true;
        }

        private void btnMatch_Click(object sender, EventArgs e)
        {
            if (!File.Exists(imgsForMatchFile))
            {
                btnCreateIndex.Enabled = false;
                MessageBox.Show("\"" + imgsForMatchFile + "\" not exists", "Error Message");
                return;
            }
            string file = indexDir + "\\" + cmbMatch.SelectedItem.ToString();
            
            if (!File.Exists(file))
            {
                btnCreateIndex.Enabled = false;
                MessageBox.Show("Index file \"" + file + "\" not exists", "Error Message");
                return;
            }
            lblGeneratingKeypointsTime.Text = "Unfinished.";
            lblQueryingIndexTime.Text = "Unfinished.";
            Refresh();

            // start
            btnMatch.Enabled = false;
            panMatchImages.Controls.Clear();

            // phrase 1:
            Stopwatch sw = new Stopwatch();
            sw.Start();
            Int32 imgDbl = Int32.Parse(txtImgDbl.Text);
            // imgDbl can only be 0/1.
            if (imgDbl != 0)
            {
                imgDbl = 1;
            }
            ServiceApi.showSift(imgsForMatchFile.ToCharArray(), keypointFileForMatch.ToCharArray(), imgDbl, Double.Parse(txtContrThr.Text));
            sw.Stop();
            lblGeneratingKeypointsTime.Text = sw.ElapsedMilliseconds + "ms";

            lblKeyPointCount.Text = getLineCount(keypointFileForMatch).ToString();
            
            Refresh();

            // phrase 2:
            sw.Reset();
            sw.Start();
            ServiceApi.query(keypointFileForMatch.ToCharArray(), file.ToCharArray(), outputImgFile.ToCharArray());
            sw.Stop();
            lblQueryingIndexTime.Text = sw.ElapsedMilliseconds + "ms";

            if (File.Exists(outputImgFile))
            {
                try
                {
                    using (StreamReader sr = new StreamReader(outputImgFile))
                    {

                        string line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            string[] ss = line.Split(new Char[]{'*'});
                            for (int i = 1; i < ss.Length; i += 2)
                            {
                                string fn = ss[i-1].Trim();
                                int matchNumber = int.Parse(ss[i].Trim());
                                if (File.Exists(fn) && matchNumber > MIN_MATCH)
                                {

                                    Image img = Image.FromFile(fn);
                                    PictureBox pic = new PictureBox();
                                    pic.Size = new System.Drawing.Size(110, 130);
                                    pic.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
                                    pic.Image = img;
                                    panMatchImages.Controls.Add(pic);
                                    Label lb = new Label();
                                    lb.Size = new System.Drawing.Size(50, 130);
                                    lb.TextAlign = ContentAlignment.MiddleCenter;
                                    lb.Text = ss[i];
                                    panMatchImages.Controls.Add(lb);
                                    //group.CreateControl();

                                }
                            }   
                        }
                        
                        sr.Close();
                    }
                }
                catch (Exception ex)
                {
                    // Do nothing.
                }
            }

            btnMatch.Enabled = true;
        }

        private int getLineCount(string keypointFileForMatch)
        {
            StreamReader sr = File.OpenText(keypointFileForMatch);
            int ret = 0;

            while (sr.ReadLine() != null)
            {
                ret++;
            }

            sr.Close();
            return ret;
        }

    }
}
