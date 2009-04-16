#ifdef WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

// The dimension of the points.
/*extern int pointsDimension;

extern double parameterR;

extern int parameterK;

extern int parameterL;*/


extern "C" DLL_EXPORT void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR = 50/*parameterR*/, double inputW = 4.0/*PARAMETER_W_DEFAULT*/, int inputK = 16/*parameterK*/, int inputL = 40/*parameterL*/);
extern "C" DLL_EXPORT void query(char* queryFileStr, char* indexNameStr, char* outputFileStr);
extern "C" DLL_EXPORT void addToIndex(char* dataFileStr, char* indexNameStr);
