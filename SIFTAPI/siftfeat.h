#ifdef WIN32
typedef __int64 Long64T;
#define LONG64T_TEXT "%I64d"
extern "C" __declspec(dllexport) int showSift(const char* imagenamefile, const char* out_file_name, int img_dbl, double contr_thr);
extern "C" __declspec(dllexport) int siftImage(const char* imagename, const char* out_file_name, int img_dbl, double contr_thr, Long64T id=0);
#else
typedef long long Long64T;
#define LONG64T_TEXT "%lld"
extern "C" int showSift(const char* imagenamefile, const char* out_file_name, int img_dbl, double contr_thr);
extern "C" int siftImage(const char* imagename, const char* out_file_name, int img_dbl, double contr_thr, Long64T id=0);
#endif
