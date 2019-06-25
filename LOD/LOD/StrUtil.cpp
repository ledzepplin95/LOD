#define _CRT_SECURE_NO_WARNINGS

#include "StrUtil.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <io.h>
#include <iostream>
#include <shlwapi.h>

void GetFileExtension(const string & fileName, string & fileExtension)
{
	size_t lastDotPosition = fileName.find_last_of(".");
	if (lastDotPosition == string::npos)
	{
		fileExtension = "";
	}
	else
	{
		fileExtension = fileName.substr(lastDotPosition, fileName.size());
		transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::toupper);
	}
}

inline string getStringNChar(string str, size_t n)
{
	return str.substr(0, n);
}

char * wch2chr(LPCTSTR lpString)
{
	UINT len = wcslen(lpString) * 2;
	char *buf = (char *)malloc(len);
	UINT i = wcstombs(buf, lpString, len);

	return buf;
}

int chr2wch(const char* buffer, wchar_t* &wBuf)
{
	size_t len = strlen(buffer);

	size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), NULL, 0);
	wBuf = new wchar_t[wlen + 1];
	MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), wBuf, int(wlen));
	wBuf[wlen] = 0;

	return (int)wlen;
}

string NumberToString(long num)
{
	string result;
	char buffer[10];
	_itoa_s(num, buffer, 10);
	result = buffer;

	return result;
}

bool LoadOBJ(const string & fileName,
	vector< Vec3<Float> > & points,
	vector< Vec3<int> > & triangles,
	vector< Vec3<Float> > & uvs,
	vector< Vec3<int> > & fUVs,
	vector< Vec3<Float> > &normals,
	vector< Vec3<int> > &fNormals,
	vector<string> &mtls,
	vector<string> &texs)
{
	const char ObjDelimiters[] = " /";
	const unsigned int bufSize = 1024;
	FILE * fId = fopen(fileName.c_str(), "r");

	if (fId)
	{
		char buffer[bufSize];
		Vec3<int> ip;
		Vec3<int> in;
		Vec3<int> it;
		char * pch;
		char * str;
		size_t nn = 0;
		size_t nt = 0;
		Vec3<Float> x;
		while (!feof(fId))
		{
			if (!fgets(buffer, bufSize, fId))
			{
				break;
			}
			else if (buffer[0] == 'v')
			{
				if (buffer[1] == ' ')
				{
					str = buffer + 2;
					for (int k = 0; k < 3; ++k)
					{
						pch = strtok(str, " ");
						if (pch) x[k] = static_cast<Float>(atof(pch));
						else
						{
							return false;
						}
						str = NULL;
					}
					points.push_back(x);
				}
				else if (buffer[1] == 'n')
				{
					str = buffer + 3;
					for (int k = 0; k < 3; ++k)
					{
						pch = strtok(str, " ");
						if (pch) x[k] = static_cast<Float>(atof(pch));
						else
						{
							return false;
						}
						str = NULL;
					}
					normals.push_back(x);

					++nn;
				}
				else if (buffer[1] == 't')
				{
					str = buffer + 3;
					for (int k = 0; k < 3; ++k)
					{
						pch = strtok(str, " ");
						if (pch) x[k] = static_cast<Float>(atof(pch));
						else
						{
							return false;
						}
						str = NULL;
					}
					uvs.push_back(x);

					++nt;
				}
			}
			else if (buffer[0] == 'f')
			{
				str = buffer + 2;
				for (int k = 0; k < 3; ++k)
				{
					pch = strtok(str, ObjDelimiters);
					if (pch) ip[k] = atoi(pch) - 1;
					else
					{
						return false;
					}
					str = NULL;
					if (nt > 0)
					{
						pch = strtok(NULL, ObjDelimiters);
						if (pch)  it[k] = atoi(pch) - 1;
						else
						{
							return false;
						}
					}
					if (nn > 0)
					{
						pch = strtok(NULL, ObjDelimiters);
						if (pch)  in[k] = atoi(pch) - 1;
						else
						{
							return false;
						}
					}
				}

				triangles.push_back(ip);
				fUVs.push_back(it);
				fNormals.push_back(in);
			}
			else if (buffer[0] == 'm')
			{
				mtls.push_back(buffer);
			}
			else if (buffer[0] == 'u')
			{
				texs.push_back(buffer);
			}
		}
		fclose(fId);
	}
	else
	{
		cout << "File not found" << endl;
		return false;
	}

	return true;
}

bool IsOBJTextured(const string & fileName,size_t&faces)
{
	const char ObjDelimiters[] = " /";
	const unsigned int bufSize = 1024;
	FILE * fId = fopen(fileName.c_str(), "r");
	size_t count = 0;
	faces = 0;
	if (fId)
	{
		char buffer[bufSize];		
		while (!feof(fId))
		{
			if (!fgets(buffer, bufSize, fId))
			{
				break;
			}
			else if (buffer[0] == 'v')
			{				
				if (buffer[1] == 't')
				{
					count++;
				}
			}
			else if (buffer[0] == 'f')
			{
				faces++;
			}
		}
		fclose(fId);
	}
	else
	{
		cout << "File not found" << endl;
		return false;
	}

	return count>0;
}

void GetDirOBJFiles(string path, vector<string>&files)
{	
	intptr_t hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{			
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					GetDirOBJFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				string fileExtension;
				GetFileExtension(fileinfo.name, fileExtension);
				if (fileExtension == ".OBJ" || fileExtension == ".obj")				
				    files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

vector<string> GetOutputFiles(const char*dir, const vector<string>&inputFiles)
{
	vector<string> result;
	for(size_t i=0; i<inputFiles.size(); i++)
	{
		wchar_t *wch;
		chr2wch(inputFiles[i].c_str(), wch);
		string fileName= wch2chr(PathFindFileName(wch));
		string file = dir;
		file += "\\";
		file += fileName;
		result.push_back(file);
	}

	return result;
}