#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "vcg/complex/complex.h"
#include "wrap/io_trimesh/import.h"
#include "wrap/io_trimesh/export_obj.h"
#include "vcg/complex/algorithms/local_optimization.h"
#include "vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric.h"
#include <vcg/complex/algorithms/local_optimization/tri_edge_collapse_quadric_tex.h>

using namespace vcg;
using namespace tri;

typedef float MESHLAB_SCALAR;

typedef MESHLAB_SCALAR Scalarm;
typedef vcg::Point2<MESHLAB_SCALAR>   Point2m;
typedef vcg::Point3<MESHLAB_SCALAR>   Point3m;
typedef vcg::Point4<MESHLAB_SCALAR>   Point4m;
typedef vcg::Plane3<MESHLAB_SCALAR>   Plane3m;
typedef vcg::Segment2<MESHLAB_SCALAR> Segment2m;
typedef vcg::Segment3<MESHLAB_SCALAR> Segment3m;
typedef vcg::Box3<MESHLAB_SCALAR>     Box3m;
typedef vcg::Matrix44<MESHLAB_SCALAR> Matrix44m;
typedef vcg::Matrix33<MESHLAB_SCALAR> Matrix33m;
typedef vcg::Shot<MESHLAB_SCALAR>     Shotm;
typedef vcg::Similarity<MESHLAB_SCALAR> Similaritym;

namespace vcg
{
	namespace vertex
	{
		template <class T> class Coord3m : public Coord<vcg::Point3<MESHLAB_SCALAR>, T> {
		public:	static void Name(std::vector<std::string> & name) { name.push_back(std::string("Coord3m")); T::Name(name); }
		};

		template <class T> class Normal3m : public Normal<vcg::Point3<MESHLAB_SCALAR>, T> {
		public:	static void Name(std::vector<std::string> & name) { name.push_back(std::string("Normal3m")); T::Name(name); }
		};

		template <class T> class CurvatureDirmOcf : public CurvatureDirOcf<CurvatureDirTypeOcf<MESHLAB_SCALAR>, T> {
		public:	static void Name(std::vector<std::string> & name) { name.push_back(std::string("CurvatureDirmOcf")); T::Name(name); }
		};

		template <class T> class RadiusmOcf : public RadiusOcf<MESHLAB_SCALAR, T> {
		public:	static void Name(std::vector<std::string> & name) { name.push_back(std::string("RadiusmOcf")); T::Name(name); }
		};

	}
	namespace face
	{
		template <class T> class Normal3m : public NormalAbs<vcg::Point3<MESHLAB_SCALAR>, T> {
		public:  static void Name(std::vector<std::string> & name) { name.push_back(std::string("Normal3m")); T::Name(name); }
		};

		template <class T> class CurvatureDirmOcf : public CurvatureDirOcf<CurvatureDirOcfBaseType<MESHLAB_SCALAR>, T> {
		public:	static void Name(std::vector<std::string> & name) { name.push_back(std::string("CurvatureDirdOcf")); T::Name(name); }
		};

	}
}

class CVertexO;
class CEdgeO;
class CFaceO;

class CUsedTypesO : public vcg::UsedTypes < vcg::Use<CVertexO>::AsVertexType,
	vcg::Use<CEdgeO   >::AsEdgeType,
	vcg::Use<CFaceO  >::AsFaceType > {};


class CVertexO : public vcg::Vertex< CUsedTypesO,
	vcg::vertex::InfoOcf,           
	vcg::vertex::Coord3m,           
	vcg::vertex::BitFlags,          
	vcg::vertex::Normal3m,          
	vcg::vertex::Qualityf,          
	vcg::vertex::Color4b,           
	vcg::vertex::VFAdjOcf,          
	vcg::vertex::MarkOcf,           
	vcg::vertex::TexCoordfOcf,      
	vcg::vertex::CurvaturefOcf,     
	vcg::vertex::CurvatureDirmOcf,  
	vcg::vertex::RadiusmOcf         
> {
};

class CEdgeO : public vcg::Edge<CUsedTypesO,
	vcg::edge::BitFlags,          
	vcg::edge::EVAdj,
	vcg::edge::EEAdj
> {
};

class CFaceO : public vcg::Face<CUsedTypesO,
	vcg::face::InfoOcf,              
	vcg::face::VertexRef,            
	vcg::face::BitFlags,             
	vcg::face::Normal3m,             
	vcg::face::QualityfOcf,          
	vcg::face::MarkOcf,              
	vcg::face::Color4bOcf,           
	vcg::face::FFAdjOcf,             
	vcg::face::VFAdjOcf,            
	vcg::face::CurvatureDirmOcf,     
	vcg::face::WedgeTexCoordfOcf     
> {};

class CMeshO : public vcg::tri::TriMesh< vcg::vertex::vector_ocf<CVertexO>, vcg::face::vector_ocf<CFaceO> >
{
public:
	int sfn;
	int svn;

	int pvn;
	int pfn;

	Matrix44m Tr;

	const Box3m &trBB()
	{
		static Box3m bb;
		bb.SetNull();
		bb.Add(Tr, bbox);
		return bb;
	}
};

typedef BasicVertexPair<CVertexO> VertexPair;

class MyTriEdgeCollapseQTex : public TriEdgeCollapseQuadricTex< CMeshO, VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<CMeshO> > {
public:
	typedef  TriEdgeCollapseQuadricTex< CMeshO, VertexPair, MyTriEdgeCollapseQTex, QuadricTexHelper<CMeshO> > TECQ;
	inline MyTriEdgeCollapseQTex(const VertexPair &p, int i, BaseParameterClass *pp) :TECQ(p, i, pp) {}
};


enum MeshElement {
	MM_NONE = 0x00000000,
	MM_VERTCOORD = 0x00000001,
	MM_VERTNORMAL = 0x00000002,
	MM_VERTFLAG = 0x00000004,
	MM_VERTCOLOR = 0x00000008,
	MM_VERTQUALITY = 0x00000010,
	MM_VERTMARK = 0x00000020,
	MM_VERTFACETOPO = 0x00000040,
	MM_VERTCURV = 0x00000080,
	MM_VERTCURVDIR = 0x00000100,
	MM_VERTRADIUS = 0x00000200,
	MM_VERTTEXCOORD = 0x00000400,
	MM_VERTNUMBER = 0x00000800,

	MM_FACEVERT = 0x00001000,
	MM_FACENORMAL = 0x00002000,
	MM_FACEFLAG = 0x00004000,
	MM_FACECOLOR = 0x00008000,
	MM_FACEQUALITY = 0x00010000,
	MM_FACEMARK = 0x00020000,
	MM_FACEFACETOPO = 0x00040000,
	MM_FACENUMBER = 0x00080000,
	MM_FACECURVDIR = 0x00100000,

	MM_WEDGTEXCOORD = 0x00200000,
	MM_WEDGNORMAL = 0x00400000,
	MM_WEDGCOLOR = 0x00800000,

	MM_CAMERA = 0x08000000,
	MM_TRANSFMATRIX = 0x10000000,
	MM_COLOR = 0x20000000,
	MM_POLYGONAL = 0x40000000,
	MM_UNKNOWN = 0x80000000,

	MM_ALL = 0xffffffff
};
