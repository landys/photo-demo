using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Diagnostics;
using PhotoDemo;

namespace SiftTest
{
    class SiftTest
    {
        private static string currDir = Environment.CurrentDirectory;
        private static string dataDir = currDir + "\\data";
        //private static string keypointFileForIndex = dataDir + "\\keypointIndex";
        private static string keypointFileForMatch = dataDir + "\\keypointTest";
        private static string indexDir = dataDir + "\\indexes";
        //private static string initEigsFile = dataDir + "\\eigs.txt";
        //private static string indexInfoFile = indexDir + "\\indexInfo.txt";
        private static string indexFile = indexDir + "\\index4";
        //private static string imgsForIndexFile = dataDir + "\\imgsForIndex.txt";
        private static string imgsForMatchFile = dataDir + "\\imgsForTest.txt";
        private static string testPicFile = "E:\\testpics\\moviepic\\250.jpg";
        private static string outputImgFile = dataDir + "\\outputImg.txt";
        private static string testResultFile = dataDir + "\\testResult.txt";

        //private static int MIN_MATCH = 10;
        //private static Int32 imgDbl = 1;
        //private static double contrThr = 0.04;

        private void testSift()
        {
            if (!File.Exists(imgsForMatchFile))
            {
                Console.WriteLine("\"" + imgsForMatchFile + "\" not exists", "Error Message");
                return;
            }
            string file = indexFile;

            if (!File.Exists(file))
            {
                Console.WriteLine("Index file \"" + file + "\" not exists", "Error Message");
                return;
            }

            // begin
            using (StreamWriter wr = File.CreateText(testResultFile))
            {
                // header of the result file
                wr.WriteLine("#pic_size 304K, pic_number=300");
                wr.WriteLine("sn   contrThr imgDbl keypoints sift_time query_time correct matched_files");

                // contrThr can be [0, 1].
                int iCount = 0;
                for (int iContrThr = 0; iContrThr < 100; ++iContrThr)
                {
                    Console.WriteLine("start iContrThr=" + iContrThr);
                    double contrThr = iContrThr / 100.0;
                    // imgDbl can be 0/1
                    for (int imgDbl = 0; imgDbl <= 1; ++imgDbl)
                    {
                        // phrase 1:
                        Stopwatch sw = new Stopwatch();
                        sw.Start();

                        int keypoints = ServiceApi.showSift(imgsForMatchFile.ToCharArray(), keypointFileForMatch.ToCharArray(), imgDbl, contrThr);
                        if (keypoints == -1)
                        {
                            Console.WriteLine("Create SIFT error: imgDbl=" + imgDbl + ",contrThr=" + contrThr);
                            return;
                        }

                        sw.Stop();
                        long siftTime = sw.ElapsedMilliseconds;

                        // phrase 2:
                        sw.Reset();
                        sw.Start();
                        ServiceApi.query(keypointFileForMatch.ToCharArray(), file.ToCharArray(), outputImgFile.ToCharArray());
                        sw.Stop();
                        long queryTime = sw.ElapsedMilliseconds;

                        bool correct = false;
                        if (File.Exists(outputImgFile))
                        {
                            try
                            {
                                using (StreamReader sr = new StreamReader(outputImgFile))
                                {
                                    string line;
                                    bool isMatched = false;
                                    while ((line = sr.ReadLine()) != null)
                                    {
                                        string[] ss = line.Split(new Char[] { '*' });
                                        if (ss.Length >= 2)
                                        {
                                            string fn = ss[0].Trim();
                                            if (File.Exists(fn) && testPicFile.Equals(fn))
                                            {
                                                correct = true;
                                            }
                                            isMatched = true;
                                            wr.WriteLine("{0,-5}{1,-9}{2,-7}{3,-10}{4,-10}{5,-11}{6,-8}{7}", ++iCount, contrThr, imgDbl, keypoints, siftTime, queryTime, correct, line);
                                        }
                                    }
                                    if (!isMatched)
                                    {
                                        wr.WriteLine("{0,-5}{1,-9}{2,-7}{3,-10}{4,-10}{5,-11}{6,-8}{7}", ++iCount, contrThr, imgDbl, keypoints, siftTime, queryTime, correct, "");
                                    }

                                    sr.Close();
                                }
                            }
                            catch (Exception ex)
                            {
                                // Do nothing.
                            }
                        }
                    }
                }
            }

        }

        static void Main(string[] args)
        {
            SiftTest st = new SiftTest();
            st.testSift();
        }
    }
}
