// The dimension of the points.
int pointsDimension = 128;

double parameterR = 50;

int parameterK = 16;

int parameterL = 40;

extern "C" __declspec(dllexport) void setUpIndex(char* dataFileStr, char* indexNameStr, double inputR = parameterR, double inputW = 4.0/*PARAMETER_W_DEFAULT*/, int inputK = parameterK, int inputL = parameterL);
extern "C" __declspec(dllexport) void query(char* queryFileStr, char* indexNameStr, char* outputFileStr);