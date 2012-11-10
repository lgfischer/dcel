///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WavefrontObjImporter_h
#define WavefrontObjImporter_h

#include "DCELStream.h"
#include "Vector3.h"

class WavefrontObjLoader
{
public:

    void load( const std::string& objFile );

    std::list<Vector3f> vertices;
    unsigned int verticeCount;
    std::list<unsigned int> faces;
    unsigned int faceCount;
protected:
private:
};

/**
    Class that imports a Wavefront OBJ file into a DCEL mesh.
    
    It assumes that the mesh is totally triangulated.
*/
template <class MeshT>
class WavefrontObjImporter
{
public:

    /**
        Loads from the given filename.

        It just calls the 'import' method with the opened stream.
    */
    void import( const std::string& objFile, MeshT& mesh);

    void import( std::list<Vector3f>& vertices, unsigned int verticeCount, std::list<unsigned int>& faces, unsigned int faceCount, MeshT& mesh);
};

template <class MeshT>
void WavefrontObjImporter<MeshT>::import( const std::string& objFilename, MeshT& mesh )
{
    std::cerr << "Starting importing the file '" << objFilename << "'" << std::endl;

    std::cerr << "- loading the OBJ file" << std::endl;
    WavefrontObjLoader loader;
    loader.load(objFilename);

    import(loader.vertices, loader.verticeCount, loader.faces, loader.faceCount, mesh);

}

template <class MeshT>
void WavefrontObjImporter<MeshT>::import( std::list<Vector3f>& vertices, unsigned int verticeCount, std::list<unsigned int>& faces, unsigned int faceCount, MeshT& mesh)
{
    // put it into the mesh
    std::cerr << "- loading the DCEL mesh: " << std::endl;
    std::cerr << "  + " << verticeCount << " vertices" << std::endl;
    mesh.clear();
    mesh.getVertices().reserve(verticeCount);
    mesh.getFaces().reserve(faceCount);
    //mesh.getEdges().reserve( 2*(verticeCount+faceCount) ); // Euler: V – A + F = 2 --> A = V + F - 2
    mesh.getHalfEdges().reserve( 2*(3*faceCount) ); // just a good expensive number of edges =)
    std::list<Vector3f>::iterator vit = vertices.begin();
    while( vit!=vertices.end() )
    {
        unsigned int id = mesh.createVertex();
        mesh.getVertex(id)->getData().position = *vit;
        vit++;
    }

    std::cerr << "  + " << faceCount << " faces" << std::endl; 
    std::list<unsigned int>::iterator fit = faces.begin();
    unsigned int count=0;
    while( fit!=faces.end() )
    {
        count++;
        unsigned int a = *fit; ++fit;
        unsigned int b = *fit; ++fit;
        unsigned int c = *fit; ++fit;
        unsigned int fid = mesh.createTriangularFace( a, b, c );
        if (fid!=-1)
        {
            //std::cout << a << ' ' << b << ' ' << c << std::endl;
            mesh.checkFace(fid);
            mesh.checkAllFaces();
        }
    }

    std::cerr << "  + " << mesh.getNumUnhandledTriangles() << " bad triangles" << std::endl;
    mesh.manageUnhandledTriangles();
    mesh.checkAllFaces();

    std::cerr << "Done!" << std::endl;
}

#endif//WavefrontObjImporter_h
