// The dimension of the points.
/*extern int pointsDimension;

extern double parameterR;

extern int parameterK;

extern int parameterL;*/

#ifdef WIN32
typedef __int64 Long64T;
#define LONG64T_TEXT "%I64d"
extern "C" __declspec(dllexport) void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR = 50/*parameterR*/, double inputW = 4.0/*PARAMETER_W_DEFAULT*/, int inputK = 16/*parameterK*/, int inputL = 40/*parameterL*/);
extern "C" __declspec(dllexport) void query(char* queryFileStr, char* indexNameStr, char* outputFileStr);
extern "C" __declspec(dllexport) void addToIndex(char* dataFileStr, char* indexNameStr);
#else
typedef long long Long64T;
#define LONG64T_TEXT "%lld"
extern "C" void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR = 50/*parameterR*/, double inputW = 4.0/*PARAMETER_W_DEFAULT*/, int inputK = 16/*parameterK*/, int inputL = 40/*parameterL*/);
extern "C" void query(char* queryFileStr, char* indexNameStr, char* outputFileStr);
extern "C" void addToIndex(char* dataFileStr, char* indexNameStr);
#endif // WIN32
