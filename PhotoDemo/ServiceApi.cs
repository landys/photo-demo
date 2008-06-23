using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace PhotoDemo
{
    public class ServiceApi
    {
        [DllImport("E2LSH.dll", EntryPoint = "setUpIndex", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern void setUpIndex(char[] dataFile, char[] index, double R, double W, int K, int L);

        [DllImport("E2LSH.dll", EntryPoint = "query", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern void query(char[] queryFile, char[] index, char[] output);

        [DllImport("SiftAPI.dll", EntryPoint = "ShowSift", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
        public static extern void showSift(char[] imagenamefile, char[] out_file_name, int img_dbl, double contr_thr);

//        [DllImport("SiftAPI.dll", EntryPoint = "initialeigs", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
//        public static extern void initialeigs(char[] eigsfile);
    }
}
