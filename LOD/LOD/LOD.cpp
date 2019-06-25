// LOD.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS

#include "LODWithTexture.h"

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "ElapsedTime.h"
#include "strutil.h"
#include "mdVector.h"

using namespace MeshDecimation;

void print_help()
{	
	printf("EXAMPLE: LOD l input.obj\n");
	printf("Example: LOD s input.obj output.obj ratio\n");
	printf("Example: LOD s input.obj output.obj ratio [-n] [-bpw] [value]\n");
	printf("Example: LOD b input_dir output_dir ratio [-n] [-bpw] [value]\n");
	printf("-n preserve normal\n");
	printf("-bpw boundary preserve weight\n");
}

void GetLODParams(int argc, char** argv, vcg::tri::TriEdgeCollapseQuadricTexParameter&qparams)
{
	for (int i = 5; i < argc;)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'n':
			case 'N':
				qparams.NormalCheck = true;
				printf("Using Normal Deviation Checking\n");
				break;
			case 'b':
			case 'B':
				if ((argv[i][2] == 'p' && argv[i][3] == 'w') || (argv[i][2] == 'P' && argv[i][3] == 'W'))
				{
					qparams.PreserveBoundary = true;
					qparams.BoundaryWeight = atof(argv[i + 1]);
					printf("Using Quality Checking\n");
				}
				else
				{
					qparams.PreserveBoundary = false;
					printf("NOT Using Quality Checking\n");
				}
			default:  printf("Unknown option '%s'\n", argv[i]);
				exit(0);
			}
		}
		i++;
	}
}

void printOBJInfo(string fileName, Float ratio)
{
	vector< Vec3<Float> > points;
	vector< Vec3<int> > triangles;
	vector< Vec3<Float> > uvs;
	vector< Vec3<int> > fUVs;
	vector< Vec3<Float> > normals;
	vector< Vec3<int> > fNormals;
	vector<string> mtls;
	vector<string> texs;
	string fileExtension;
	GetFileExtension(fileName, fileExtension);
	size_t targetFaces=0;
	if (fileExtension == ".OBJ" || fileExtension == ".obj")
	{
		if (LoadOBJ(fileName, points, triangles, uvs, fUVs,
			normals, fNormals, mtls, texs))
		{
			cout << "# triangles      " << triangles.size() << endl;
			cout << "# vertices       " << points.size() << endl;
			cout << "# UVs            " << uvs.size() << endl;
			cout << "# face UVs       " << fUVs.size() << endl;
			cout << "# normals        " << normals.size() << endl;
			cout << "# face normals   " << fNormals.size() << endl;
			targetFaces = ratio*triangles.size();
			cout << "# target faces   " << targetFaces << endl;
			cout << "# materials      " << mtls.size() << endl;
		}		
	}	
}

int run(int argc, char** argv)
{
	const string  fileNameIn(argv[2]);	
	if (strcmp(argv[1], "l") == 0 ||
		strcmp(argv[1], "L") == 0)
	{
		printOBJInfo(fileNameIn, 1.0);
		return EXIT_SUCCESS;
	}
	else if (strcmp(argv[1], "s") == 0 || strcmp(argv[1], "S") == 0)
	{
		if (argc < 5)
		{
			cout << "Example: LOD s input.obj output.obj ratio [-n] [-bpw] [value]" << endl;
			return EXIT_FAILURE;
		}
		if (atoi(argv[4]) < 0.0 || atoi(argv[4]) > 1.0) return EXIT_FAILURE;

		cout << endl << "Quadric Error Metrics\n" << endl;
		cout << "Input            " << fileNameIn << endl;		
		size_t        targetFaces;
		printOBJInfo(fileNameIn, atof(argv[4]));
		const string  fileNameOut(argv[3]);
		cout << "Output           " << fileNameOut << endl;		
		if (IsOBJTextured(fileNameIn,targetFaces))
		{
			vcg::tri::TriEdgeCollapseQuadricTexParameter qparams;
			qparams.SetDefaultParams();
			qparams.QualityThr = 0.3;
			qparams.ExtraTCoordWeight = 1.0;
			GetLODParams(argc, argv, qparams);
			targetFaces = atof(argv[4])*targetFaces;
			LODWithTexture(fileNameIn, fileNameOut, targetFaces, qparams);

			cout << endl;
		}
	}
	else
		return EXIT_FAILURE;
}

int main(int argc, char** argv)
{
	printf("Copyright 2019 Sikong Ltd.\n");
	
	if (argc < 3)
	{	  
		printf("Author: Zhihai Liu\n\n");
		printf("error: no input file \n\n");
		print_help();
		return EXIT_FAILURE;
	}	
	cout << endl;

	cout << "LOD\n\n";	
	ElapsedTime et;
	et.Start();	
	if (strcmp(argv[1], "b") == 0 || strcmp(argv[1], "B") == 0)
	{
		if (argc < 5)
		{
			cout << "LOD b input_dir output_dir ratio [-n] [-bpw] [value]" << endl;
			return EXIT_FAILURE;
		}
		if (atoi(argv[4]) < 0.0 || atoi(argv[4]) > 1.0) return EXIT_FAILURE;
		
		vector<string> inputOBJFiles;
		string path(argv[2]);
		GetDirOBJFiles(path,inputOBJFiles);
		vector<string> outputOBJFiles = GetOutputFiles(argv[3], inputOBJFiles);
		for(size_t i=0; i<inputOBJFiles.size(); i++)
		{
			const string  fileNameIn(inputOBJFiles[i]);
			const string  fileNameOut(outputOBJFiles[i]);
			size_t targetFaces;
			printOBJInfo(fileNameIn, atof(argv[4]));
			cout << "Input            " << fileNameIn << endl;
			cout << "Output           " << fileNameOut << endl;
			if (IsOBJTextured(fileNameIn,targetFaces))
			{
				vcg::tri::TriEdgeCollapseQuadricTexParameter qparams;
				qparams.SetDefaultParams();
				qparams.QualityThr = 0.3;
				qparams.ExtraTCoordWeight = 1.0;
				GetLODParams(argc, argv, qparams);
				targetFaces = atof(argv[4])*targetFaces;
				LODWithTexture(fileNameIn, fileNameOut, targetFaces, qparams);				
			}
			cout << endl << endl;
		}

		et.End();
		cout << endl << et.ToString() << endl;
		return EXIT_SUCCESS;
	}
	run(argc, argv);	
	et.End();
	cout << endl << et.ToString() << endl;

	return EXIT_SUCCESS;
}