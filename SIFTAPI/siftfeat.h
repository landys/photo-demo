#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif


extern "C" DLL_EXPORT int showSift(const char* imagenamefile, const char* out_file_name, int img_dbl, double contr_thr, int n_max, const char* logFileName);
//extern "C" DLL_EXPORT int siftImage(const char* imagename, const char* out_file_name, int img_dbl, double contr_thr, int max_n, Long64T id=0);

