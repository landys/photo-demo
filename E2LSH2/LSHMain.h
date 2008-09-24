// The dimension of the points.
extern int pointsDimension;

extern double parameterR;

extern int parameterK;

extern int parameterL;

#ifdef WIN32
extern "C" __declspec(dllexport) void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR = parameterR, double inputW = 4.0/*PARAMETER_W_DEFAULT*/, int inputK = parameterK, int inputL = parameterL);
extern "C" __declspec(dllexport) void query(char* queryFileStr, char* indexNameStr, char* outputFileStr);
#else
extern "C" void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR = parameterR, double inputW = 4.0/*PARAMETER_W_DEFAULT*/, int inputK = parameterK, int inputL = parameterL);
extern "C" void query(char* queryFileStr, char* indexNameStr, char* outputFileStr);
#endif // WIN32
