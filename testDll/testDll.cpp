// testDll.cpp : main project file.
#pragma comment (lib,"E2LSH") 

#include "stdafx.h"
#include "Form1.h"
#include <Windows.h>
#include <iostream>
#include <TCHAR.H>
using namespace testDll;
using namespace std;


//extern "C" __declspec(dllexport) int test();

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	typedef void (*SETUPINDEX)(string ,string);//����ָ������
	typedef void (*QUERY)(string ,string, string);
	typedef int (*TEST)();//����ָ������
	HINSTANCE Hint = ::LoadLibrary( _T("E:\\projects\\DllTest\\DllTest\\PCASiftAPI.dll") ); //�������Ǹղ����ɵ�dll


	System::String ^ error = GetLastError().ToString();

	SETUPINDEX setUpIndex = (SETUPINDEX)GetProcAddress(Hint,"setUpIndex");//ȡ��dll������add����
	QUERY query = (QUERY)GetProcAddress(Hint,"query");

	error = GetLastError().ToString();
	TEST test = (TEST)GetProcAddress(Hint,"test");//ȡ��dll������add����

	int haha = test();
	//cout<<add(3,4)<<endl;

	setUpIndex("E:\\projects\\E2LSH\\E2LSH\\1100.txt", "E:\\projects\\E2LSH\\E2LSH\\asdfasf");
	query("E:\\projects\\E2LSH\\E2LSH\\transform1.txt", "E:\\projects\\E2LSH\\E2LSH\\asdfasf", "E:\\projects\\E2LSH\\E2LSH\\output.txt");
	
	return 0;
	typedef int  (* LPFNDLLFUNC)(int ,int);
	LPFNDLLFUNC lpfnDllFunc; 

	HINSTANCE hInst = LoadLibrary(LPCWSTR("E:\\projects\\E2LSH\\Debug\\E2LSHDll.dll"));

	/*
	if (hInst == NULL) 
	MessageBox("err", NULL, MB_OK);
	else
	MessageBox("ok", NULL, MB_OK);
	*/
	lpfnDllFunc = (LPFNDLLFUNC)GetProcAddress(hInst, "setUpIndex");

	if (lpfnDllFunc != 0) {
		cout << "OK" << endl;
	}
	FreeLibrary(hInst);


	// Enabling Windows XP visual effects before any controls are created
	//Application::EnableVisualStyles();
	//Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	//Application::Run(gcnew Form1());
	return 0;
}
