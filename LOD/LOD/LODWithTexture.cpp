#define _CRT_SECURE_NO_WARNINGS

#include "LODWithTexture.h"
#include "LODWithTextureInterface.h"

void QuadricTexSimplification(CMeshO &m, int  TargetFaceNum, tri::TriEdgeCollapseQuadricTexParameter &pp)
{
	tri::UpdateNormal<CMeshO>::PerFace(m);
	math::Quadric<double> QZero;
	QZero.SetZero();
	tri::QuadricTexHelper<CMeshO>::QuadricTemp TD3(m.vert, QZero);
	tri::QuadricTexHelper<CMeshO>::TDp3() = &TD3;
	std::vector<std::pair<vcg::TexCoord2<float>, Quadric5<double> > > qv;
	tri::QuadricTexHelper<CMeshO>::Quadric5Temp TD(m.vert, qv);
	tri::QuadricTexHelper<CMeshO>::TDp() = &TD;
	vcg::LocalOptimization<CMeshO> DeciSession(m, &pp);
	DeciSession.Init<MyTriEdgeCollapseQTex>();

	DeciSession.SetTargetSimplices(TargetFaceNum);
	DeciSession.SetTimeBudget(0.1f);
	//	int startFn=m.fn;
	int faceToDel = m.fn - TargetFaceNum;
	while (DeciSession.DoOptimization() && m.fn > TargetFaceNum)
	{
		//char buf[256];
		//sprintf(buf, "Simplifing: heap size %i ops %i\n", int(DeciSession.h.size()), DeciSession.nPerfmormedOps);
		printf("LODing faces: %d\n", 100 - 100 * (m.fn - TargetFaceNum) / (faceToDel));
	};
	DeciSession.Finalize<MyTriEdgeCollapseQTex>();
}

void updateDataMask(CMeshO&cm, int neededDataMask)
{
	if ((neededDataMask & MM_FACEFACETOPO) != 0)
	{
		cm.face.EnableFFAdjacency();
		tri::UpdateTopology<CMeshO>::FaceFace(cm);
	}
	if ((neededDataMask & MM_VERTFACETOPO) != 0)
	{
		cm.vert.EnableVFAdjacency();
		cm.face.EnableVFAdjacency();
		tri::UpdateTopology<CMeshO>::VertexFace(cm);
	}

	if ((neededDataMask & MM_WEDGTEXCOORD) != 0)
		cm.face.EnableWedgeTexCoord();
	if ((neededDataMask & MM_FACECOLOR) != 0)
		cm.face.EnableColor();
	if ((neededDataMask & MM_FACEQUALITY) != 0)
		cm.face.EnableQuality();
	if ((neededDataMask & MM_FACECURVDIR) != 0)
		cm.face.EnableCurvatureDir();
	if ((neededDataMask & MM_FACEMARK) != 0)
		cm.face.EnableMark();
	if ((neededDataMask & MM_VERTMARK) != 0)
		cm.vert.EnableMark();
	if ((neededDataMask & MM_VERTCURV) != 0)
		cm.vert.EnableCurvature();
	if ((neededDataMask & MM_VERTCURVDIR) != 0)
		cm.vert.EnableCurvatureDir();
	if ((neededDataMask & MM_VERTRADIUS) != 0)
		cm.vert.EnableRadius();
	if ((neededDataMask & MM_VERTTEXCOORD) != 0)
		cm.vert.EnableTexCoord();
}

void Enable(CMeshO&cm, int openingFileMask)
{
	if (openingFileMask & tri::io::Mask::IOM_VERTTEXCOORD)
		updateDataMask(cm, MM_VERTTEXCOORD);
	if (openingFileMask & tri::io::Mask::IOM_WEDGTEXCOORD)
		updateDataMask(cm, MM_WEDGTEXCOORD);
	if (openingFileMask & tri::io::Mask::IOM_VERTCOLOR)
		updateDataMask(cm, MM_VERTCOLOR);
	if (openingFileMask & tri::io::Mask::IOM_FACECOLOR)
		updateDataMask(cm, MM_FACECOLOR);
	if (openingFileMask & tri::io::Mask::IOM_VERTRADIUS) updateDataMask(cm, MM_VERTRADIUS);
	if (openingFileMask & tri::io::Mask::IOM_CAMERA) updateDataMask(cm, MM_CAMERA);
	if (openingFileMask & tri::io::Mask::IOM_VERTQUALITY) updateDataMask(cm, MM_VERTQUALITY);
	if (openingFileMask & tri::io::Mask::IOM_FACEQUALITY) updateDataMask(cm, MM_FACEQUALITY);
	if (openingFileMask & tri::io::Mask::IOM_BITPOLYGONAL) updateDataMask(cm, MM_POLYGONAL);
}


void  LODWithTexture(string fileName, string fileNameOut, size_t targetFaces, tri::TriEdgeCollapseQuadricTexParameter&qparams)
{
	tri::io::ImporterOBJ<CMeshO>::Info oi;
	if (!tri::io::ImporterOBJ<CMeshO>::LoadMask(fileName.c_str(), oi))
		return;
	CMeshO m;
	Enable(m, oi.mask);
	int err = vcg::tri::io::ImporterOBJ<CMeshO>::Open(m, fileName.c_str(), oi);
	if (err)
	{
		printf("Unable to open mesh %s : '%s'\n", fileName.c_str(), vcg::tri::io::Importer<CMeshO>::ErrorMsg(err));
		exit(-1);
	}
	printf("mesh loaded\nvertices: %d faces: %d \n", m.vn, m.fn);
	bool CleaningFlag = false;
	if (CleaningFlag) {
		int dup = tri::Clean<CMeshO>::RemoveDuplicateVertex(m);
		int unref = tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
		printf("Removed %i duplicate and %i unreferenced vertices from mesh \n", dup, unref);
	}
	vcg::tri::UpdateBounding<CMeshO>::Box(m);

	updateDataMask(m, MM_VERTFACETOPO | MM_VERTMARK);
	tri::UpdateFlags<CMeshO>::FaceBorderFromVF(m);
	int TargetFaceNum = targetFaces;
	QuadricTexSimplification(m, TargetFaceNum, qparams);

	tri::UpdateBounding<CMeshO>::Box(m);
	if (m.fn > 0) {
		tri::UpdateNormal<CMeshO>::PerFaceNormalized(m);
		tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(m);
	}
	tri::UpdateNormal<CMeshO>::NormalizePerFace(m);
	tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(m);
	tri::UpdateNormal<CMeshO>::NormalizePerVertex(m);
	int mask = tri::io::Mask::IOM_ALL;
	vcg::tri::io::ExporterOBJ<CMeshO>::Save(m, fileNameOut.c_str(), mask);
}
