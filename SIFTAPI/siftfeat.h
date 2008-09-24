#ifdef WIN32
extern "C" __declspec(dllexport) int showSift(const char* imagenamefile, const char* out_file_name, int img_dbl, double contr_thr);
extern "C" __declspec(dllexport) int siftImage(const char* imagename, const char* out_file_name, int img_dbl, double contr_thr);
#else
extern "C" int showSift(const char* imagenamefile, const char* out_file_name, int img_dbl, double contr_thr);
extern "C" int siftImage(const char* imagename, const char* out_file_name, int img_dbl, double contr_thr);
#endif
