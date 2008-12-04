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
        private static string keypointFileTemp = indexDir + "\\keypointTemp";
        private static string keypointFileForMatch = dataDir + "\\keypointMatch";
        private static string indexDir = dataDir + "\\indexes";
        private static string initEigsFile = dataDir + "\\eigs.txt";
        private static string indexInfoFile = indexDir + "\\indexInfo.txt";
        private static string keypointFileForIndex = indexDir + "\\keypointIndex"; // i.e. keypointIndex1, begin from 1.
        private static string indexFilePre = indexDir + "\\index"; // i.e. index1, begin from 1.
        private static string imgsForIndexFile = dataDir + "\\imgsForIndex.txt";
        private static string imgsForMatchFile = dataDir + "\\imgsForMatch.txt";
        private static string outputImgFile = dataDir + "\\outputImg.txt";
        private static string configFile = dataDir + "\\config.txt";
        private static string mapFile = indexDir + "\\fileNameMap.txt";
        private static Boolean imgFlag = false;

        private static int indexCount = 0;
        private static int MIN_MATCH = 10;
        private static Dictionary<long, string> fileNameMap;

        private int maxImageId;
        
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

            // file name map used for show
            fileNameMap = new Dictionary<long,string>();
            if (File.Exists(mapFile))
            {
                try
                {
                    using (StreamReader sr = new StreamReader(mapFile))
                    {
                        string s = null;
                        while ((s = sr.ReadLine()) != null) 
                        {
                            int i = s.IndexOf(' ');
                            if (i == -1)
                            {
                                continue;
                            }
                            fileNameMap.Add(int.Parse(s.Substring(0, i)), s.Substring(i + 1));
                        }
                       
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

                int imgIndex = MaxImageId;
                if (dlgOpenImage.FileNames != null)
                {
                    for (int i = 0; i < dlgOpenImage.FileNames.Length; i++)
                    {
                        sw.WriteLine("{0} {1}", ++imgIndex, dlgOpenImage.FileNames[i]);
                    }
                    txtInfo.Text = dlgOpenImage.FileNames.Length + " files selected.";
                }
                else
                {
                    
                    sw.WriteLine("{0} {1}", ++imgIndex, dlgOpenImage.FileName);
                    
                    txtInfo.Text = "1 file selected.";
                }

                MaxImageId = imgIndex;

                btnCreateIndex.Enabled = true;
                btnAddToIndex.Enabled = true;

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
                    int imgIndex = MaxImageId;
                    StreamWriter sw = File.CreateText(imgsForIndexFile);
                    for (int i = 0; i < jpgFiles.Length; i++)
                    {
                        sw.WriteLine("{0} {1}", ++imgIndex, jpgFiles[i]);
                    }
                    for (int i = 0; i < gifFiles.Length; i++)
                    {
                        sw.WriteLine("{0} {1}", ++imgIndex, gifFiles[i]);
                    }
                    for (int i = 0; i < pngFiles.Length; i++)
                    {
                        sw.WriteLine("{0} {1}", ++imgIndex, pngFiles[i]);
                    }
                    for (int i = 0; i < bmpFiles.Length; i++)
                    {
                        sw.WriteLine("{0} {1}", ++imgIndex, bmpFiles[i]);
                    }
                    MaxImageId = imgIndex;

                    sw.Close();
                    btnCreateIndex.Enabled = true;
                    btnAddToIndex.Enabled = true;
                }
                else
                {
                    btnCreateIndex.Enabled = false;
                    btnAddToIndex.Enabled = false;
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
                btnAddToIndex.Enabled = false;
                MessageBox.Show("\"" + imgsForIndexFile + "\" not exists", "Error Message");
                return;
            }
            btnCreateIndex.Enabled = false;
            btnAddToIndex.Enabled = false;
            Int32 imgDbl = Int32.Parse(txtImgDbl.Text);
            // imgDbl can only be 0/1.
            if (imgDbl != 0)
            {
                imgDbl = 1;
            }
            string keypointFile = keypointFileForIndex + ++indexCount;
            if (ServiceApi.showSift(imgsForIndexFile.ToCharArray(), keypointFile.ToCharArray(), imgDbl, Double.Parse(txtContrThr.Text)) == -1)
            {
                MessageBox.Show("Create SIFT error.");
                btnCreateIndex.Enabled = true;
                btnAddToIndex.Enabled = true;
                return;
            }
            string file = indexFilePre + indexCount;
            try
            {
                ServiceApi.setUpIndex(keypointFile.ToCharArray(), file.ToCharArray(), Double.Parse(txtBoxR.Text), Double.Parse(txtBoxW.Text), Int32.Parse(txtBoxK.Text), Int32.Parse(txtBoxL.Text));
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

            addToFileNameMap(imgsForIndexFile);

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
            btnAddToIndex.Enabled = true;
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
                btnAddToIndex.Enabled = false;
                MessageBox.Show("\"" + imgsForMatchFile + "\" not exists", "Error Message");
                return;
            }
            string file = indexDir + "\\" + cmbMatch.SelectedItem.ToString();
            
            if (!File.Exists(file))
            {
                btnCreateIndex.Enabled = false;
                btnAddToIndex.Enabled = false;
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
            if (ServiceApi.showSift(imgsForMatchFile.ToCharArray(), keypointFileForMatch.ToCharArray(), imgDbl, Double.Parse(txtContrThr.Text)) == -1)
            {
                MessageBox.Show("Create SIFT error.");
                btnMatch.Enabled = true;
                return;
            }

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
                    using (FileStream fs = File.OpenRead(outputImgFile))
                    {
                        long id = readLongNativeEndian(fs);
                        int matchNumber = readIntNativeEndian(fs);
                        while (!(id == -1 && matchNumber == -1))
                        {
                            string fn = fileNameMap[id];
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
                                lb.Text = matchNumber.ToString();
                                panMatchImages.Controls.Add(lb);
                                //group.CreateControl();

                            }
                            id = readLongNativeEndian(fs);
                            matchNumber = readIntNativeEndian(fs);
                        }
                        
                        fs.Close();
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

        private void btnAddToIndex_Click(object sender, EventArgs e)
        {
            if (cmbMatch.Items.Count == 0)
            {
                btnCreateIndex_Click(null, null);
                return;
            }

            if (!File.Exists(imgsForIndexFile))
            {
                btnCreateIndex.Enabled = false;
                btnAddToIndex.Enabled = false;
                MessageBox.Show("\"" + imgsForIndexFile + "\" not exists", "Error Message");
                return;
            }
            
            string file = indexFilePre + (cmbMatch.SelectedIndex + 1);

            btnCreateIndex.Enabled = false;
            btnAddToIndex.Enabled = false;
            Int32 imgDbl = Int32.Parse(txtImgDbl.Text);
            // imgDbl can only be 0/1.
            if (imgDbl != 0)
            {
                imgDbl = 1;
            }
            if (ServiceApi.showSift(imgsForIndexFile.ToCharArray(), keypointFileTemp.ToCharArray(), imgDbl, Double.Parse(txtContrThr.Text)) == -1)
            {
                MessageBox.Show("Create SIFT error.");
                btnCreateIndex.Enabled = true;
                btnAddToIndex.Enabled = true;
                return;
            }
            
            try
            {
                ServiceApi.addToIndex(keypointFileTemp.ToCharArray(), file.ToCharArray());
            }
            catch (FormatException fe)
            {
                MessageBox.Show("Parameters must be a number");
            }
            catch (OverflowException ofe)
            {
                MessageBox.Show("Parameters is out of range");
            }

            addToFileNameMap(imgsForIndexFile);

            btnCreateIndex.Enabled = true;
            btnAddToIndex.Enabled = true;
        }

        public int MaxImageId
        {
            get 
            {
                if (File.Exists(configFile))
                {
                    using (StreamReader sw = File.OpenText(configFile))
                    {
                        string s = sw.ReadLine();
                        maxImageId = int.Parse(s);
                        sw.Close();
                    }
                  
                }
                else
                {
                    maxImageId = 0;
                }
                

                return maxImageId; 
            }
            set 
            {
                maxImageId = value;
                using (StreamWriter sw = File.CreateText(configFile))
                {
                    sw.WriteLine(maxImageId);
                    sw.Close();
                }
                 
            }
        }


        private long readLongNativeEndian(FileStream fs) {

            // 8 bytes
            long accum = 0;
            for (int shiftBy = 0; shiftBy < 64; shiftBy += 8) {
                // must cast to long or shift done modulo 32
                accum |= (long) (fs.ReadByte() & 0xff) << shiftBy;
            }

            return accum;
        }


        private int readIntNativeEndian(FileStream fs) {

            // 4 bytes
            int accum = 0;
            for (int shiftBy = 0; shiftBy < 32; shiftBy += 8) {
                accum |= (fs.ReadByte() & 0xff) << shiftBy;
            }
            return accum;
        }

        private void addToFileNameMap(string imageFile)
        {
            StreamWriter sw = File.AppendText(mapFile);
            using (StreamReader sr = File.OpenText(imageFile)) 
            {
                string s = "";
                while ((s = sr.ReadLine()) != null) 
                {
                    int i = s.IndexOf(' ');
                    if (i == -1)
                    {
                        continue;
                    }
                    fileNameMap.Add(int.Parse(s.Substring(0, i)), s.Substring(i+1));
                    sw.WriteLine(s);
                }
                sr.Close();
                sw.Close();
            }

        }
    }
}
