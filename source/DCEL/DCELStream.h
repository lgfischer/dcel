///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DCELStream_h
#define DCELStream_h

#include <iostream>
#include "Mesh.h"

template <class MeshT>
class DCELStream
{
public:

    static void read(MeshT& mesh, std::istream &stream);

    /**
    	Format:

        version 1
        #this is a comment
        vc <number of vertices in the mesh>
        fc <number of faces in the mesh>
        ec <number of edges in the mesh>
        v <custom user data>
        ... # repeat v for vc times, one for each vertex
        f <number of vertices that define this face> <list of id of each vertex> <custom user data>
        ... # repeat f for fc times, one for each face
        e <id of the source vertex of the edge> <id of the source of the twin edge> <custom user data>
        ... # repeat e for ec times, one for each half edge
        end
    */
    static void write(const MeshT& mesh, std::ostream &stream);

    template<class MeshT, class MeshT2>
    static void copyDcelData(const MeshT& source, MeshT2& target )
    {
        const MeshT::Face* face = NULL;
        const unsigned int numVertices = source.getNumVertices();
        const unsigned int numFaces = source.getNumFaces();
        const unsigned int numEdges = source.getNumHalfEdges();

        target.clear();
        target.getVertices().reserve( numVertices );
        target.getFaces().reserve( numFaces );
        target.getHalfEdges().reserve( numEdges );

        for( unsigned int vertexId=0; vertexId<numVertices; ++vertexId )
        {
            target.createVertex();

            // convert here the data from the source to target vertex
        }
        for( unsigned int faceId=0; faceId<numFaces; ++faceId )
        {
            face = source.getFace( faceId );

            unsigned int vid[3];//generalizar
            unsigned int vertexCount=0;
            MeshT::EdgeIterator it(face);
            while( it.hasNext() )
            {
                vid[vertexCount] = source.getVertexId( it.getNext()->getOrigin() );
                ++vertexCount;
            }
            target.createTriangularFace( vid[0], vid[1], vid[2] );

            // convert here the data from the source to target faces
        }

        /*
        for( unsigned int edgeId=0; edgeId<numEdges; ++edgeId )
        {
            // convert here the data from the source to target half-edges
        }
        */

    };

protected:
private:

    static void read_v1(MeshT& mesh, std::istream &stream);

    static void consumeComment(const std::string& commentBegin, std::istream &stream);
};


//////////////////////////////////////////////////////////////////////////
//                            IMPLEMENTATION                            //
//////////////////////////////////////////////////////////////////////////


template <class MeshT>
void DCELStream<MeshT>::read(MeshT& mesh, std::istream &stream)
{
    while( stream.good() )
    {
        std::string str;
        stream >> str;
        if( str=="version" )
        {
            unsigned int version;
            stream >> version;
            switch(version)
            {
            case 1:
                read_v1(mesh, stream);
                break;
            default:
                std::cerr << "Warning: unknown version " << version << std::endl;
            }
        }
        else if( str.at(0)=='#' )
        {
            consumeComment(str, stream);
        }
        else
        {
            std::cerr << "Unknown line: " << str << std::endl;
        }
    }
}

template <class MeshT>
void DCELStream<MeshT>::write(const MeshT& mesh, std::ostream &stream)
{
    const MeshT::Vertex* vertex = NULL;
    const MeshT::Face* face = NULL;
    const unsigned int numVertices = mesh.getNumVertices();
    const unsigned int numFaces = mesh.getNumFaces();
    const unsigned int numEdges = mesh.getNumHalfEdges();

    stream << "version 1" << std::endl;

    stream << "vc " << numVertices << std::endl;
    stream << "fc " << numFaces << std::endl;
    stream << "ec " << numEdges << std::endl;

    for( unsigned int vertexId=0; vertexId<numVertices; ++vertexId )
    {
        vertex = mesh.getVertex( vertexId );
        stream << "v " << vertex->getData() << std::endl;
    }

    for( unsigned int faceId=0; faceId<numFaces; ++faceId )
    {
        face = mesh.getFace( faceId );
        stream << "f ";

        unsigned int vid[3];//generalizar
        unsigned int vertexCount=0;
        MeshT::EdgeIterator it(face);
        while( it.hasNext() )
        {
            vid[vertexCount] = mesh.getVertexId( it.getNext()->getOrigin() );
            ++vertexCount;
        }
        stream << vertexCount;
        for( unsigned int v=0; v<vertexCount; ++v )
        {
            stream << " " << vid[v];
        }
        stream << " " << face->getData();
        stream << std::endl;
    }

    for( unsigned int edgeId=0; edgeId<numEdges; ++edgeId )
    {
        const MeshT::HalfEdge* edge = mesh.getHalfEdge( edgeId );
        stream << "e " << mesh.getVertexId( edge->getOrigin() ) 
            << " " << mesh.getVertexId( edge->getTwin()->getOrigin() )
            << " " << edge->getData() << std::endl;
    }

    stream << "end";
}

template <class MeshT>
void DCELStream<MeshT>::read_v1(MeshT& mesh, std::istream &stream)
{
    std::string str;

    bool reading = true;
    while( reading && stream.good() )
    {
        stream >> str;

        if( str=="v" )
        {
            MeshT::Vertex* vertex = mesh.getVertex( mesh.createVertex() );
            stream >> (vertex->getData());
        }
        else if( str=="f" )
        {
            unsigned int vertexCount;
            stream >> vertexCount;
            unsigned int vid[3];//TODO generalize
            for( unsigned int i=0; i<vertexCount; ++i )
                stream >> (vid[i]);

            unsigned int fid = mesh.createTriangularFace( vid[0], vid[1], vid[2] );
            MeshT::Face* face = mesh.getFace( fid );
            stream >> face->getData();
        }
        else if( str=="e" )
        {
            unsigned int from, to;
            stream >> from >> to;
            MeshT::HalfEdge* edge = mesh.getHalfEdge( mesh.getVertex(from), mesh.getVertex(to) );
            if( edge )
            {
                stream >> edge->getData();
            }
            else
            {
                std::cerr << '?';
            }
        }

        else if( str=="vc" )
        {
            unsigned int vc = std::numeric_limits<unsigned int>::max();
            stream >> vc;
            mesh.getVertices().reserve( vc*5 );
        }
        else if( str=="fc" )
        {
            unsigned int fc = std::numeric_limits<unsigned int>::max();
            stream >> fc;
            mesh.getFaces().reserve( fc*5 );
        }
        else if( str=="ec" )
        {
            unsigned int ec = std::numeric_limits<unsigned int>::max();
            stream >> ec;
            mesh.getHalfEdges().reserve( ec*5 );
        }
        else if( str=="end" )
        {
            reading = false;
        }
        else if( str.at(0)=='#' )
        {
            consumeComment(str, stream);
        }
        else
        {
            std::cerr << "Unknown line: " << str;
        }
    }
    mesh.manageUnhandledTriangles();
}

template <class MeshT>
void DCELStream<MeshT>::consumeComment(const std::string& commentBegin, std::istream &stream)
{
    char buffer[512];
    stream.getline( buffer, sizeof(buffer) );
    std::cerr << commentBegin << " " << buffer << std::endl;
}

#endif//DCELStream_h
