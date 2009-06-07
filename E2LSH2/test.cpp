#include "LSHMain.h"
#include <cstdio>
using namespace std;

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		printf("Not enough parameters.\n");
		return 0;
	}

	int opt = 0;
	sscanf(argv[1], "%d", &opt);

	FILE* fout = 0;
	long long id;
	int points;
	switch (opt)
	{
	case 1:
		printf("create index...\n");
		setUpIndex(argv[2], argv[3]);
		printf("finish.\n");
		break;
	case 2:
		printf("add to index...\n");
		addToIndex(argv[2], argv[3]);
		printf("finish.\n");
		break;
	default:
		printf("query...\n");
		query(argv[2], argv[3], argv[4]);
		fout = fopen(argv[4], "rb");
		if (fout == 0)
		{
			printf("Read result file error: %s\n.", argv[4]);
			break;
		}
		fread(&id, sizeof(long long), 1, fout);
		fread(&points, sizeof(int), 1, fout);
		while (id != -1)
		{
			printf ("%lld: %d\n", id, points);
			fread(&id, sizeof(long long), 1, fout);
			fread(&points, sizeof(int), 1, fout);
		}
		fclose(fout);
		
		break;
	}

	return 0;
}

