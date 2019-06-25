#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include "mdVector.h"

using namespace MeshDecimation;
using namespace std;

string getStringNChar(string str, size_t n);
char * wch2chr(LPCTSTR lpString);
int chr2wch(const char* buffer, wchar_t* &wBuf);

string NumberToString(long num);
void GetFileExtension(const string & fileName, string & fileExtension);
bool LoadOBJ(const string & fileName,
	vector< Vec3<Float> > & points,
	vector< Vec3<int> > & triangles,
	vector< Vec3<Float> > & uvs,
	vector< Vec3<int> > & fUVs,
	vector< Vec3<Float> > &normals,
	vector< Vec3<int> > &fNormals,
	vector<string> &mtls,
	vector<string> &texs);
bool IsOBJTextured(const string & fileName,size_t&faces);
void GetDirOBJFiles(string path, vector<string>&files);
vector<string> GetOutputFiles(const char*dir,const vector<string>&inputFiles);