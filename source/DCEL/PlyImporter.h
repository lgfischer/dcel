///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef PlyImporter_h
#define PlyImporter_h

#include "DCELStream.h"
#include "Vector3.h"

/**
	This class is used internally by the PlyImporter, and should not be used externally.

    It loads a set of vertices into the "vertices" list and the set of faces into the 
    "faces" list. Each 3 values into the "faces" lists stores one triangular face,
    composed by the vertices referenced by the 3 unsigned int values.
*/
class PlyLoader
{
public:
    void load(const std::string& plyFilename);

    std::list<Vector3f> vertices;
    unsigned int verticeCount;
    std::list<unsigned int> faces;
    unsigned int faceCount;
};

/**
    Class that imports a PLY file into a DCEL mesh.
    
    It assumes that the mesh is totally triangulated.
*/
template <class MeshT>
class PlyImporter
{
public:

    /**
        Loads from the given filename.

        It just calls the 'import' method with the opened stream.
    */
    void import( const std::string& plyFilename, MeshT& mesh );

private:
};


template <class MeshT>
void PlyImporter<MeshT>::import( const std::string& plyFilename, MeshT& mesh )
{
    std::cerr << "Starting importing the file '" << plyFilename << "'" << std::endl;

    std::cerr << "- loading the PLY file" << std::endl;
    //loads the ply file
    PlyLoader loader;
    loader.load( plyFilename );

    // put it into the mesh
    std::cerr << "- loading the DCEL mesh: " << std::endl;
    std::cerr << "  + " << loader.verticeCount << " vertices" << std::endl;
    mesh.clear();
    mesh.getVertices().reserve(loader.verticeCount);
    mesh.getFaces().reserve(loader.faceCount);
    mesh.getHalfEdges().reserve( 2*(loader.verticeCount+loader.faceCount-2) ); // Euler: V – A + F = 2 --> A = V + F - 2
    std::list<Vector3f>::iterator vit = loader.vertices.begin();
    while( vit!=loader.vertices.end() )
    {
        unsigned int id = mesh.createVertex();
        mesh.getVertex(id)->getData().position = *vit;
        vit++;
    }

    std::cerr << "  + " << loader.faceCount << " faces" << std::endl; 

    std::list<unsigned int>::iterator fit = loader.faces.begin();
    unsigned int count=0;
    while( fit!=loader.faces.end() )
    {
        count++;
        unsigned int a = *fit; ++fit;
        unsigned int b = *fit; ++fit;
        unsigned int c = *fit; ++fit;
        unsigned int fid = mesh.createTriangularFace( a, b, c );
        if (fid!=-1)
        {
            mesh.checkFace(fid);
            mesh.checkAllFaces();
        }
    }

    std::cerr << "  + " << mesh.getNumUnhandledTriangles() << " bad triangles" << std::endl;

    mesh.manageUnhandledTriangles();
    mesh.checkAllFaces();
    std::cerr << "Done!" << std::endl;
}

#endif//PlyImporter_h
