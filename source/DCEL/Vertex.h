///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DCEL_Vertex_h
#define DCEL_Vertex_h

//#include <cassert>

template<class VertexDataT, class HalfEdgeDataT, class FaceDataT>
class HalfEdgeT;

template<class VertexDataT, class HalfEdgeDataT, class FaceDataT>
class VertexT
{
    typedef VertexT<VertexDataT, HalfEdgeDataT, FaceDataT> Vertex;
    typedef HalfEdgeT<VertexDataT, HalfEdgeDataT, FaceDataT> HalfEdge;

public:

    VertexT():
        data(),
        incidentEdge(NULL)
    {
    };

    inline HalfEdge* getIncidentEdge() const
    {
        //assert(this);
        return this->incidentEdge;
    };

    inline void setIncidentEdge(HalfEdge* edge)
    {
        //assert(this);
        this->incidentEdge = edge;
    };

    inline VertexDataT& getData()
    {
        return data;
    };

    inline const VertexDataT& getData() const
    {
        return data;
    };

protected:
private:
    HalfEdge* incidentEdge;
    VertexDataT data;
};

#endif//DCEL_Vertex_h
