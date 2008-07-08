#include <string.h>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <direct.h>

#include "Magick++.h"

static char* picType = "bmp";

/**
 * fileName the pic file to be converted
 * reFileName the bmp file generated
 */
bool picToBmp(const char* fileName, const char* reFileName)
{

	Magick::Image image;
	
	try { 
		image.read(fileName);
		image.magick(picType);
		image.write(reFileName);
	} 
	catch(Magick::Exception &e) 
	{ 
		// do nothing
		printf("Caught exception: %s.\n", e.what()); 
		return false; 
	} 

	return true;
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: PicToBmp source_file_pattern destination_dir.\n");
		return 1;
	}

	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	printf("Source file pattern is %s.\n", argv[1]);
	printf("The destination directory is %s.\n", argv[2]);
	// if the destination directory not exist, create it
	hFind = FindFirstFile(argv[2], &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if(_mkdir(argv[2]) != 0)
		{
			printf("Create %s fail.\n", argv[2]);
			return -1;
		}
	}
	else if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		// exist but not a directory
		printf ("%s is a file, not a directory.\n", argv[2]);
		return 1;
	}

	if (argc > 3)
	{
		picType = argv[3];
	}

	int count = 0;
	// pick the directory from the source file pattern, i.e.
	// e:\a\* -> e:\a\, e:\a\*.jpg -> e:\a\.
	char sfp[256];
	strcpy(sfp, argv[1]);
	char* p = strrchr(sfp, '\\');
	if (p == 0)
	{
		p = strrchr(sfp, '/');
	}
	if (p != 0)
	{
		*(p+1) = '\0';
	}
	std::string scdir(sfp);
	if (p == 0)
	{
		scdir = "";
	}

	// append "\\" to destination directory if not exist, i.e.
	// e:\a -> e:\a\, e:\a\ -> e:\a\.
	std::string redir(argv[2]);
	if (redir[redir.length()-1] != '\\' && redir[redir.length()-1] != '/')
	{
		redir += '\\';
	}

	hFind = FindFirstFile(argv[1], &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		printf ("Invalid File Handle. GetLastError reports %d.\n", 
			GetLastError ());
		return 1;
	}

	char buf[20];

	if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		// it must NOT be a directory
		printf("The file to be converted is %s.\n", findFileData.cFileName);
		_itoa_s(++count, buf, 20, 10);
		if (!picToBmp((scdir + findFileData.cFileName).c_str(), (redir + buf + ".bmp").c_str()))
		{
			--count;
		}
	}
	
	while (FindNextFile(hFind, &findFileData))
	{
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			// it must NOT be a directory
			printf("The file to be converted is %s.\n", findFileData.cFileName);
			_itoa_s(++count, buf, 20, 10);
			if (!picToBmp((scdir + findFileData.cFileName).c_str(), (redir + buf + ".bmp").c_str()))
			{
				--count;
			}
		}
	}

	FindClose(hFind);
	printf("Convert complete, totally %d files.\n", count);
	
	return 0;
}
