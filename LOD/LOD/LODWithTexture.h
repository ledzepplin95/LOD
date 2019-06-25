#pragma once

#include <string>
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric_tex.h>

using namespace std;

void  LODWithTexture(string fileName, string fileNameOut, size_t targetFaces, vcg::tri::TriEdgeCollapseQuadricTexParameter&qparams);