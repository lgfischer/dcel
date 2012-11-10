///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DCEL_EdgeIterator_h
#define DCEL_EdgeIterator_h

#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
//#include <cassert>

/**
	A class that helps the iteration of the edges of a vertice or face.
    
    In the case of a vertex, it runs over each edge that has the given
    edge as origin.

    In the case of a face, it runs over each edge that forms the boundary
    of that face.
*/
template<class VertexDataT, class HalfEdgeDataT, class FaceDataT>
class EdgeIteratorT
{
    typedef VertexT<VertexDataT, HalfEdgeDataT, FaceDataT> Vertex;
    typedef HalfEdgeT<VertexDataT, HalfEdgeDataT, FaceDataT> HalfEdge;
    typedef FaceT<VertexDataT, HalfEdgeDataT, FaceDataT> Face;

public:

    /**
    	Creates an EdgeIterator that runs over each edge that starts on the
        given vertex.
    */
    inline EdgeIteratorT(const Vertex* vertex)
    {
        reset(vertex);
    };

    /**
    	Creates an EdgeIterator that runs over each edge that forms the 
        boundary of the given face.
    */
    inline EdgeIteratorT(const Face* face)
    {
        reset(face);
    };

    /**
    	Returns the next edge in the sequence. 
        
        Returns NULL if all the sequence of edges has ended.
    */
    inline HalfEdge* getNext()
    {
        HalfEdge* next = nextEdge;

        if( nextEdge )
        {
            if( vertexIterator ) // edges starting at a vertex iterator
            {
                HalfEdge* twin = nextEdge->getTwin();
                //assert(twin);
                nextEdge = twin->getNext();
            }
            else // edges from face iterator
            {
                nextEdge = nextEdge->getNext();
            }
        }

        if( nextEdge==startEdge )
        {
            nextEdge = NULL;
        }

        return next;
    };

    /**
    	Check if there is any edge in the sequence of edges.
    */
    inline bool hasNext() const
    {
        return nextEdge!=NULL;
    };

    /**
    	Makes the iterator start again, from the first edge 
        of the sequence.
    */
    inline void reset()
    {
        nextEdge = startEdge;
    };

    inline void reset(const Vertex* vertex)
    {
        //assert(vertex);
        vertexIterator = true;
        startEdge = vertex->getIncidentEdge();
        reset();
    };

    inline void reset(const Face* face)
    {
        //assert(face);
        vertexIterator = false;
        startEdge = face->getBoundary();
        reset();
    };

protected:
private:
    HalfEdge* startEdge;
    HalfEdge* nextEdge;
    bool vertexIterator;
};

#endif//DCEL_EdgeIterator_h
