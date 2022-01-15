#pragma once
#include <string>
#include <direct.h>
#include <windows.h>
#include "export.h"
static std::string getResource(std::string fileName)
{
	char exeFullPath[MAX_PATH]; // Full path
	std::string strPath = "";

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (std::string)exeFullPath;    // Get full path of the file
	size_t pos = strPath.find_last_of('\\', strPath.length());
	strPath =  strPath.substr(0, pos);  // Return the directory without the file name
	return strPath + "//resource//" + fileName;
}