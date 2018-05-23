#include "stdafx.h"
#include "Tools.h"
#include <Windows.h>

Tools::Tools()
{
}


Tools::~Tools()
{
}
/*

std::string platdiff_A2U8(const std::string &src)
{
const char * gb2312 = src.c_str();
unsigned int nInLen = (unsigned int)src.size();

char * utf8 = new char[2 * nInLen + 1 + 1];

int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);

wchar_t* wstr = new wchar_t[len + 1];
memset(wstr, 0, len + 1);
MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);

len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
memset(utf8, 0, len + 1);
WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8, len, NULL, NULL);

std::string ret = utf8;

delete[] utf8;
delete[] wstr;

return ret;
}
std::string platdiff_U82A(const std::string &src)
{
const char * utf8 = src.c_str();

int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
wchar_t* wstr = new wchar_t[len + 1];
memset(wstr, 0, len + 1);
MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
char* str = new char[len + 1];
memset(str, 0, len + 1);
WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);

std::string ret = str;

delete[] wstr;
delete[] str;

return ret;
}


*/
std::string Tools::strW2C(const std::wstring & str){
	char * utf8 = new char[4 * str.size() + 2 + 2];
	auto len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, utf8, len, NULL, NULL);
	std::string ret = utf8;
	delete[] utf8;
	return ret;
}
std::wstring Tools::strC2W(const std::string & str){
	const char * utf8 = str.c_str();
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	std::wstring ret = wstr;
	delete[] wstr;
	return ret;
}