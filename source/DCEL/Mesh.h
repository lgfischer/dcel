///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef DCEL_Mesh_h
#define DCEL_Mesh_h

#include <vector>
#include <list>
#include <limits>

#include "Vertex.h"
#include "Face.h"
#include "HalfEdge.h"
#include "EdgeIterator.h"

#include "Exception.h"

#define MESH_NULL_ID (std::numeric_limits<unsigned int>::max())

/**
	A class that stores the mesh structure. It the list of the vertices, edges and faces.
    Also, it has some methods for simplify the mesh manipulation.
*/
template<class VertexDataT, class HalfEdgeDataT, class FaceDataT>
class Mesh
{
	typedef Mesh<VertexDataT, HalfEdgeDataT, FaceDataT> MeshT;
public:

    typedef VertexT<VertexDataT, HalfEdgeDataT, FaceDataT> Vertex;
    typedef HalfEdgeT<VertexDataT, HalfEdgeDataT, FaceDataT> HalfEdge;
    typedef FaceT<VertexDataT, HalfEdgeDataT, FaceDataT> Face;
    typedef EdgeIteratorT<VertexDataT, HalfEdgeDataT, FaceDataT> EdgeIterator;

    typedef VertexDataT VertexData;
    typedef HalfEdgeDataT HalfEdgeData;
    typedef FaceDataT FaceData;

    Mesh( );

    ~Mesh( );

    /**
        Creates a new, unused vertex on the mesh.

    	Returns the id of the created vertex.
    */
    unsigned int createVertex( );

    /**
    	Creates a new, unused vertex on the mesh.

        Returns the pointer to the vertex itself.
    */
    Vertex* createGetVertex();

    /**
    	Create a face, that has its boundary pointing to bound.
    */
    unsigned int createFace( HalfEdge* bound );

    /**
        Create an edge between the vertex origin and twinOrigin. Return the
        ID of the half-edge that has origin as its origin vertex.

        Actually, this method will create two half edges, and will set their 
        origin, set their face pointers and each one will have the other as 
        their.

        The two half-edges will be placed in the edge's list sequentially.
    */
    unsigned int createEdge( Vertex* origin, Face* face, Vertex* twinOrigin, Face* twinFace );

    /**
        Adds a new face to the mesh. The face is bounded by previously
        added vertices, that are identified by the given IDs.

        The IDs must be given in a CCW order. No checking is done about this.
        If the vertices are not in a CCW order, the structure may become corrupted.

    	Returns the id of the created face.
    */
    unsigned int createTriangularFace(unsigned int vId1, unsigned int vId2, unsigned int vId3);

    /**
    	Returns a pointer to the given vertex ID.
    */
    inline Vertex* getVertex( unsigned int id ) const;

    /**
    	Return the list of vertices.
    */
    inline std::vector<Vertex>& getVertices();

    /**
    	Returns the list of vertices, for const operations.
    */
    inline const std::vector<Vertex>& getVertices() const;

    /**
    	Returns the current number of vertices stored on the mesh.

        The first vertex has ID=0 and the last one has ID=<NUM_VERTICES>-1
    */
    inline unsigned int getNumVertices() const;

    /**
    	Computes the ID of a given vertex.
    */
    inline unsigned int getVertexId( const Vertex* vertex ) const;

    /**
    	Returns the list of faces.
    */
    inline std::vector<Face>& getFaces( );

    /**
    	Returns the number of faces that this mesh has.

        The first face has ID=0 and the last one has ID=<NUM_FACES>-1
    */
    inline unsigned int getNumFaces() const;

    /**
    	Returns the list of faces, for const operations
    */
    inline const std::vector<Face>& getFaces( ) const;

    /**
    	Returns the face with the given ID.
    */
    inline Face* getFace( unsigned int faceId ) const;

    /**
    	Returns the ID of a given pointer face. The 
        ID can be used in the method getFace(id) to 
        retrieve the same pointer.
    */
    inline unsigned int getFaceId( const Face* face) const;

    /**
    	Return the list of half-edges. 
        
        Note that this list contains the two half-edges of each edge of the
        mesh. When this mesh is created using the create*Face methods,
        each half edge and its twin is adjacent in the list.

        In other words, in a normal case, to iterate over the edges only once
        you must iterate over the even or odd items of this list.
    */
    inline std::vector<HalfEdge>& getHalfEdges( );

    /**
    	Return a list of edges. Note that in this list the twins of the edges
        does not appear. In other words, this list represents the connections
        between two unordered vertices. Used for const operations.
    */
    inline const std::vector<HalfEdge>& getHalfEdges( ) const;

    /**
    	Returns the Half edge with the given ID.
    */
    HalfEdge* getHalfEdge(unsigned int id);

    /**
    	Returns the Half edge with the given ID.
    */
    const HalfEdge* getHalfEdge(unsigned int id) const;
    
    /**
    	Returns a pointer to the edge that has vertexA as origin and
        has vertexB as 'target' (edge->twin->origin).

        Returns NULL if this edge doesn't exist.
    */
    HalfEdge* getHalfEdge( Vertex* vertexA, Vertex* vertexB ) const;
    HalfEdge* getHalfEdge( unsigned int vertexIdA, unsigned int vertexIdB) const;

    /**
    	Returns the ID of a given halfEdge.
    */
    unsigned int getHalfEdgeId( HalfEdge* halfEdge) const;

    /**
    	Returns the number of half-edges of the mesh.

        Note that the actual number of edges is <NUM_HALF-EDGES> / 2.
    */
    unsigned int getNumHalfEdges() const;

    /**
    	Iterate through all faces, calling the checkFace() method
        to check if that face is consistent.
    */
    void checkAllFaces() const;

    /**
        A helper method used to test some assumed conditions about the DCEL. 
        
        It should be used after each face "add*Face" method.

        It checkFace for the following conditions on the last added face:
        - Every face must have a boundary (1)
    	- The sequence of vertices pointed by face->boundary must point to the same face (2)
        - Every edge must have an origin vertex (3)
        - Every vertex must have a incident pointer (4)
        - Every edge must have an twin edge (5)
        - Every edge must have a next pointer (6)
        - The pointer edge->next->origin must be equal to edge->twin->origin (7)
        - Every edge must have a prev pointer (8)
        - The pointer edge->prev->twin->origin must be equal to edge->origin (9)
    */
    void checkFace( unsigned int faceId ) const;

    /**
    	During the insertion of triangular faces, some times it can be 
        ambiguous how to insert the triangle into the structure, due to
        how the pointers connects the vertices, edges and faces.
        In these cases, these triangles are not inserted into the mesh,
        but are delayed to be inserted when there is no ambiguity in its
        insertion. This method tries to insert these triangles.
    */
    void manageUnhandledTriangles();

    int getNumUnhandledTriangles() const;

    /**
        Finds the half edge that does not have an associated face yet, and its twin
        starts at the given vertex.
    */
    HalfEdge* findIncidentHalfEdge(Vertex* vertex);

    /**
    	Clear the entire mesh, releasing the vertices, faces and half-edges.
    */
    void clear();

    const std::list<int>& getUnhandledTriangles() const
    {
        return this->unhandledTriangles;
    }

protected:
private:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<HalfEdge> edges;

    std::list<int> unhandledTriangles;
    unsigned int unhandledTrianglesCount;
};


//////////////////////////////////////////////////////////////////////////
//                            IMPLEMENTATION                            //
//////////////////////////////////////////////////////////////////////////


template<class Vdt, class Hdt, class Fdt>
Mesh<Vdt,Hdt,Fdt>::Mesh():
    unhandledTrianglesCount(0)
{
};

template<class Vdt, class Hdt, class Fdt>
Mesh<Vdt,Hdt,Fdt>::~Mesh()
{
    this->clear();
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::createVertex( )
{
    this->vertices.push_back( Vertex() );
    return this->vertices.size()-1;
};

template<class Vdt, class Hdt, class Fdt>
VertexT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::createGetVertex( )
{
    this->vertices.push_back( Vertex() );
    return &this->vertices.back();
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::createFace( HalfEdge* bound )
{
    unsigned int faceId = this->faces.size();
    this->faces.push_back( Face() );
    Face* face = &(faces[faceId]);
    face->setBoundary( bound );
    return faceId;
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::createEdge( Vertex* origin, Face* face, Vertex* twinOrigin, Face* twinFace )
{
    unsigned int edgeId = this->edges.size();
    this->edges.push_back( HalfEdge() );
    HalfEdge* e1 = &this->edges[edgeId];
    e1->setOrigin( origin );
    e1->setFace( face );

    this->edges.push_back( HalfEdge() );
    HalfEdge* e2 = &this->edges[edgeId+1];
    e2->setOrigin( twinOrigin );
    e2->setFace( twinFace );

    e1->setTwin(e2);

    return edgeId;
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::createTriangularFace(unsigned int vId1, unsigned int vId2, unsigned int vId3)
{
    Vertex* v1 = this->getVertex(vId1);
    Vertex* v2 = this->getVertex(vId2);
    Vertex* v3 = this->getVertex(vId3);
    HalfEdge* e1 = this->getHalfEdge(v1, v2);
    HalfEdge* e2 = this->getHalfEdge(v2, v3);
    HalfEdge* e3 = this->getHalfEdge(v3, v1);

    int unusedVertices = 0;
    if (v1->getIncidentEdge()==NULL) unusedVertices++;
    if (v2->getIncidentEdge()==NULL) unusedVertices++;
    if (v3->getIncidentEdge()==NULL) unusedVertices++;

    int readyEdges = 0;
    if (e1!=NULL) readyEdges++;
    if (e2!=NULL) readyEdges++;
    if (e3!=NULL) readyEdges++;

    unsigned int nullFaceId = MESH_NULL_ID;
    unsigned int faceId = nullFaceId;
    Face* face = NULL;

    //the most simple case, all vertices has degree 0
    //create 3 edges, and link then
    if ( unusedVertices==3 && readyEdges==0 )
    {
        faceId = createFace( NULL );
        face = getFace( faceId );

        e1 = getHalfEdge( createEdge( v1, face, v2, NULL ) );
        v1->setIncidentEdge( e1 );
        face->setBoundary( e1 );

        e2 = getHalfEdge( createEdge( v2, face, v3, NULL ) );
        v2->setIncidentEdge( e2 );

        e3 = getHalfEdge(createEdge( v3, face, v1, NULL ) );
        v3->setIncidentEdge( e3 );

        e1->setNext( e2 );
        e2->setNext( e3 );
        e3->setNext( e1 );

        e1->getTwin()->setNext( e3->getTwin() );
        e3->getTwin()->setNext( e2->getTwin() );
        e2->getTwin()->setNext( e1->getTwin() );
    }

    //there are one vertex that has been used by another triangle.
    //Create the 3 edges, and link to the old triangle.
    else if( unusedVertices==2 && readyEdges==0 )
    {
        if( v2->getIncidentEdge()!=NULL )
        {
            Vertex* vt = v1;
            v1 = v2;
            v2 = v3;
            v3 = vt;
        }
        else if( v3->getIncidentEdge()!=NULL )
        {
            Vertex* vt = v1;
            v1 = v3;
            v3 = v2;
            v2 = vt;
        }
        HalfEdge* ei1 = findIncidentHalfEdge( v1 );
        if( ei1!=NULL )
        {
            HalfEdge* ei1n = ei1->getNext();

            faceId = createFace( NULL );
            face = getFace( faceId );

            e1 = getHalfEdge( createEdge( v1, face, v2, ei1->getFace() ) );
            face->setBoundary( e1 );

            e2 = getHalfEdge( createEdge( v2, face, v3, ei1->getFace() ) );
            v2->setIncidentEdge( e2 );

            e3 = getHalfEdge(createEdge( v3, face, v1, ei1->getFace() ) );
            v3->setIncidentEdge( e3 );

            e1->setNext( e2 );
            e2->setNext( e3 );
            e3->setNext( e1 );

            ei1->setNext( e3->getTwin() );
            e3->getTwin()->setNext( e2->getTwin() );
            e2->getTwin()->setNext( e1->getTwin() );
            e1->getTwin()->setNext( ei1n );
        }
    }

    //2 vertices are already in use by 2 distinct triangles, and
    //one vertex doesn't have been used yet.
    else if( unusedVertices==1 && readyEdges==0 )
    {
        if( v3->getIncidentEdge()==NULL )
        {
            Vertex* vt = v1;
            v1 = v2;
            v2 = v3;
            v3 = vt;
        }
        else if( v1->getIncidentEdge()==NULL )
        {
            Vertex* vt = v1;
            v1 = v3;
            v3 = v2;
            v2 = vt;
        }

        HalfEdge* ei1 = findIncidentHalfEdge( v1 );
        HalfEdge* ei3 = findIncidentHalfEdge( v3 );
        if( ei1!=NULL && ei3!=NULL )
        {
            HalfEdge* ei1n = ei1->getNext();
            HalfEdge* ei3n = ei3->getNext();

            faceId = createFace( NULL );
            face = getFace( faceId );

            e1 = getHalfEdge( createEdge( v1, face, v2, ei1n->getFace() ) );
            e2 = getHalfEdge( createEdge( v2, face, v3, ei3->getFace() ) );
            e3 = getHalfEdge(createEdge( v3, face, v1, ei1->getFace() ) );

            face->setBoundary( e1 );
            v2->setIncidentEdge( e2 );

            e1->setNext( e2 );
            e2->setNext( e3 );
            e3->setNext( e1 );

            ei1->setNext( e3->getTwin() );
            e3->getTwin()->setNext( ei3n );

            ei3->setNext( e2->getTwin() );
            e2->getTwin()->setNext( e1->getTwin() );
            e1->getTwin()->setNext( ei1n );
        }
    }

    //two of the vertices already have one edge connected between then,
    //and one vertex has not been used yet.
    else if (unusedVertices==1 && readyEdges==1)
    {
        //rotate pointers, so the v1 and v2 are connected, and v3 is unused
        if (e2!=NULL)
        {
            Vertex* vt = v1;
            v1 = v2;
            v2 = v3;
            v3 = vt;
            HalfEdge *et = e1;
            e1 = e2;
            e2 = e3;
            e3 = et;
        }
        else if (e3!=NULL)
        {
            Vertex* vt = v1;
            v1 = v3;
            v3 = v2;
            v2 = vt;
            HalfEdge* et = e1;
            e1 = e3;
            e3 = e2;
            e2 = et;
        }

        if (e1->getFace()==NULL)
        {
            HalfEdge* e1n = e1->getNext();
            HalfEdge* e1p = e1->getPrev();

            faceId = createFace( e1 );
            face = getFace( faceId );

            e2 = getHalfEdge(createEdge( v2, face, v3, e1n->getFace() ) );
            e3 = getHalfEdge(createEdge( v3, face, v1, e1p->getFace() ) );

            e1->setFace( face );
            v3->setIncidentEdge( e3 );

            e1->setNext( e2 );
            e2->setNext( e3 );
            e3->setNext( e1 );

            e1p->setNext( e3->getTwin() );
            e3->getTwin()->setNext( e2->getTwin() );
            e2->getTwin()->setNext( e1n );
        }
    }

    //all the tree vertices are used by different triangles, no 
    //ready edge available.
    else if ( unusedVertices==0 && readyEdges==0 )
    {
        HalfEdge* ei1 = findIncidentHalfEdge( v1 );
        HalfEdge* ei2 = findIncidentHalfEdge( v2 );
        HalfEdge* ei3 = findIncidentHalfEdge( v3 );

        if( ei1!=NULL && ei2!=NULL && ei3!=NULL )
        {

            HalfEdge* ei1n = ei1->getNext();
            HalfEdge* ei2n = ei2->getNext();
            HalfEdge* ei3n = ei3->getNext();

            faceId = createFace( NULL );
            face = getFace( faceId );

            e1 = getHalfEdge( createEdge( v1, face, v2, ei2->getFace() ) );
            e2 = getHalfEdge( createEdge( v2, face, v3, ei3->getFace() ) );
            e3 = getHalfEdge( createEdge( v3, face, v1, ei1->getFace() ) );

            face->setBoundary( e1 );

            e1->setNext( e2 );
            e2->setNext( e3 );
            e3->setNext( e1 );

            ei1->setNext( e3->getTwin() );
            e3->getTwin()->setNext( ei3n );

            ei3->setNext( e2->getTwin() );
            e2->getTwin()->setNext( ei2n );

            ei2->setNext( e1->getTwin() );
            e1->getTwin()->setNext( ei1n );
        }
    }

    //all vertices are used, and two of then are used by the 
    //same triangle. In this case, one edge will be shared with
    //other triangle.
    else if (unusedVertices==0 && readyEdges==1)
    {
        if( e2!=NULL )
        {
            Vertex* vt = v1;
            v1 = v2;
            v2 = v3;
            v3 = vt;
            HalfEdge *et = e1;
            e1 = e2;
            e2 = e3;
            e3 = et;
        }
        else if( e3!=NULL )
        {
            Vertex* vt = v1;
            v1 = v3;
            v3 = v2;
            v2 = vt;
            HalfEdge* et = e1;
            e1 = e3;
            e3 = e2;
            e2 = et;
        }

        HalfEdge* ei3 = findIncidentHalfEdge( v3 );
        if( ei3 && e1->getFace()==NULL )
        {
            HalfEdge* e1p = e1->getPrev();
            HalfEdge* e1n = e1->getNext();
            HalfEdge* ei3n = ei3->getNext();

            faceId = createFace( e1 );
            face = getFace( faceId );

            e2 = getHalfEdge( createEdge( v2, face, v3, ei3->getFace() ) );
            e3 = getHalfEdge( createEdge( v3, face, v1, e1p->getFace() ) );

            e1->setFace( face );
            e2->setFace( face );
            e3->setFace( face );

            e1->setNext( e2 );
            e2->setNext( e3 );
            e3->setNext( e1 );

            e1p->setNext( e3->getTwin() );
            e3->getTwin()->setNext( ei3n );

            ei3->setNext( e2->getTwin() );
            e2->getTwin()->setNext( e1n );
        }
    }

    //The three vertices are used, but there is an edge missing.
    //will create one edge to 'fill' the hole and expand the mesh
    else if( unusedVertices==0 && readyEdges==2 )
    {
        if (e1==NULL)
        {
            Vertex* vt = v1;
            v1 = v3;
            v3 = v2;
            v2 = vt;
            HalfEdge* et = e1;
            e1 = e3;
            e3 = e2;
            e2 = et;
        }
        else if (e3==NULL)
        {
            Vertex* vt = v1;
            v1 = v2;
            v2 = v3;
            v3 = vt;
            HalfEdge *et = e1;
            e1 = e2;
            e2 = e3;
            e3 = et;
        }

        if (e1->getFace()==NULL && e3->getFace()==NULL)
        {
            HalfEdge* e1n = e1->getNext();
            HalfEdge* e3p = e3->getPrev();

            if (e3->getNext()==e1 && e1->getPrev()==e3)
            {
                faceId = createFace( e1 );
                face = getFace( faceId );

                e2 = getHalfEdge( createEdge( v2, face, v3, e1n->getFace() ) );

                e1->setFace( face );
                e3->setFace( face );

                e1->setNext( e2 );
                e2->setNext( e3 );
                e3->setNext( e1 );

                e3p->setNext( e2->getTwin() );
                e2->getTwin()->setNext( e1n );
            }
            else
            {
                std::cerr << "outro erro" << std::endl;
            }
        }
    }

    //A practically closed face, need only that the internal edges
    //point to a new face. A easy case =)
    else if ( unusedVertices==0 && readyEdges==3 )
    {
        if(e1->getFace()==NULL && e2->getFace()==NULL && e3->getFace()==NULL)
        {
            if (e1->getNext()==e2 && e2->getNext()==e3 && e3->getNext()==e1)
            {
                faceId = createFace( e1 );
                face = getFace( faceId );
                e1->setFace( face );
                e2->setFace( face );
                e3->setFace( face );
            }
            else
            {
                std::cerr << "mais outro erro" << std::endl;
            }
        }
    }

    // if there is no face pointer, then a triangle should be inserted in
    // a difficult case to handle. For now, the most simple solution is
    // handle this triangle latter, after other triangles have been added
    // and there is no ambiguity
    if( face==NULL )
    {
        this->unhandledTriangles.push_back( vId1 );
        this->unhandledTriangles.push_back( vId2 );
        this->unhandledTriangles.push_back( vId3 );
        unhandledTrianglesCount++;
    }

    return faceId;
};

template<class Vdt, class Hdt, class Fdt>
VertexT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::getVertex( unsigned int id ) const
{
    const MeshT::Vertex* v = &(this->vertices[id]);
    return const_cast<MeshT::Vertex*>(v);
};

template<class Vdt, class Hdt, class Fdt>
std::vector<VertexT<Vdt,Hdt,Fdt>>& Mesh<Vdt,Hdt,Fdt>::getVertices()
{
    return this->vertices;
};

template<class Vdt, class Hdt, class Fdt>
const std::vector<VertexT<Vdt,Hdt,Fdt>>& Mesh<Vdt,Hdt,Fdt>::getVertices() const
{
    return this->vertices;
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::getNumVertices() const
{
    return this->vertices.size();
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::getVertexId(const Vertex* vertex) const
{
    const Vertex* firstVertex = &(this->vertices[0]);
    unsigned int id = vertex - firstVertex;
    if( id>=0 && id < this->vertices.size() )
    {
        if( &(this->vertices[id]) == vertex )
            return id;
    }
    return MESH_NULL_ID;
}

template<class Vdt, class Hdt, class Fdt>
std::vector<FaceT<Vdt,Hdt,Fdt>>& Mesh<Vdt,Hdt,Fdt>::getFaces( )
{
    return this->faces;
};

template<class Vdt, class Hdt, class Fdt>
const std::vector<FaceT<Vdt,Hdt,Fdt>>& Mesh<Vdt,Hdt,Fdt>::getFaces( ) const
{
    return this->faces;
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::getNumFaces( ) const
{
    return this->faces.size();
}

template<class Vdt, class Hdt, class Fdt>
FaceT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::getFace(unsigned int faceId) const
{
    const Face* f = &(this->faces[faceId]);
    return const_cast<Face*>(f);
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::getFaceId(const Face* face) const
{
    const Face* firstFace = &(this->faces[0]);
    unsigned int id = face - firstFace;
    if( id>=0 && id < this->faces.size() )
    {
        if( &(this->faces[id]) == face )
            return id;
    }
    return MESH_NULL_ID;
};

template<class Vdt, class Hdt, class Fdt>
inline std::vector<HalfEdgeT<Vdt,Hdt,Fdt>>& Mesh<Vdt,Hdt,Fdt>::getHalfEdges( )
{
    return this->edges;
};

template<class Vdt, class Hdt, class Fdt>
inline const std::vector<HalfEdgeT<Vdt,Hdt,Fdt>>& Mesh<Vdt,Hdt,Fdt>::getHalfEdges( ) const
{
    return this->edges;
};

template<class Vdt, class Hdt, class Fdt>
HalfEdgeT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::getHalfEdge(unsigned int id)
{
    HalfEdge* e = &(this->edges[id]);
    return e;
}

template<class Vdt, class Hdt, class Fdt>
const HalfEdgeT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::getHalfEdge(unsigned int id) const
{
    const HalfEdge* e = &(this->edges[id]);
    return e;
}

template<class Vdt, class Hdt, class Fdt>
HalfEdgeT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::getHalfEdge(Vertex* vertexA, Vertex* vertexB) const
{
    EdgeIterator it( vertexA );
    while( it.hasNext() )
    {
        HalfEdge* e = it.getNext();
        if( e->getTwin()->getOrigin()==vertexB )
        {
            return e;
        }
    }
    return NULL;
};

template<class Vdt, class Hdt, class Fdt>
HalfEdgeT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::getHalfEdge( unsigned int vertexIdA, unsigned int vertexIdB) const
{
    return getHalfEdge( getVertex(vertexIdA), getVertex(vertexIdB) );
};

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::getHalfEdgeId( HalfEdge* halfEdge) const
{
    const HalfEdge* firstEdge = &(this->edges[0]);
    unsigned int id = halfEdge - firstEdge;
    if( id>=0 && id < this->edges.size() )
    {
        if( &(this->edges[id]) == halfEdge )
            return id;
    }
    return MESH_NULL_ID;
}

template<class Vdt, class Hdt, class Fdt>
unsigned int Mesh<Vdt,Hdt,Fdt>::getNumHalfEdges() const
{
    return this->edges.size();
}

template<class Vdt, class Hdt, class Fdt>
void Mesh<Vdt,Hdt,Fdt>::checkAllFaces() const
{
    const unsigned int numFaces = this->getNumFaces();
    for( unsigned int f=0; f<numFaces; ++f )
    {
        this->checkFace(f);
    }
}

template<class Vdt, class Hdt, class Fdt>
void Mesh<Vdt,Hdt,Fdt>::checkFace(unsigned int faceId) const
{
    Face* face = this->getFace(faceId);

    if( face->getBoundary()==NULL )
    {
        throw cpp::Exception("Every face must have a boundary (1)");
    }

    HalfEdge* edge = NULL;
    EdgeIterator it(face);
    while( it.hasNext() )
    {
        edge = it.getNext();

        if( edge->getFace()!=face )
        {
            throw cpp::Exception("The sequence of half-edges pointed by face->boundary must point to the same face (2)");
        }

        if( edge->getOrigin()==NULL )
        {
            throw cpp::Exception("Every edge must have an origin vertex (3)");
        }

        if( edge->getOrigin()->getIncidentEdge()==NULL )
        {
            throw cpp::Exception("Every vertex must have a incident pointer (4)");
        }

        if( edge->getTwin()==NULL )
        {
            throw cpp::Exception("Every edge must have an twin edge (5)");
        }

        if( edge->getNext()==NULL )
        {
            throw cpp::Exception("Every edge must have a next half-edge pointer (6)");
        }
        else if( edge->getNext()->getOrigin()!=edge->getTwin()->getOrigin() )
        {
            throw cpp::Exception("The pointer edge->next->origin must be equal to edge->twin->origin (7)");
        }

        if( edge->getPrev()==NULL )
        {
            throw cpp::Exception("Every edge must have a prev pointer (8)");
        }
        else if( edge->getPrev()->getTwin()->getOrigin()!=edge->getOrigin() )
        {
            throw cpp::Exception("The pointer edge->prev->twin->origin must be equal to edge->origin (9)");
        }

        bool edgeFound = false;
        unsigned int count = 0;
        EdgeIterator vit( edge->getOrigin() );
        while(vit.hasNext())
        {
            if (vit.getNext() == edge)
            {
                edgeFound = true;
                break;
            }
            else
            {
                // if a "infinite" loop happens here, also there is an undetectable error!
                // lets say that a vertex will not have more than 1000 incident edges.
                count++;
                if( count>1000 )
                {
                    throw cpp::Exception("There is an error iterating through the edges of a vertex - possible infinite loop - (10)");
                }
            }
        }
        if ( !edgeFound )
        {
            throw cpp::Exception("The edge cannot be reachable iterating thought its origin (11)");
        }

        if (edge->getTwin()->getFace()==NULL)
        {
            const HalfEdge* initialBorder = edge->getTwin();
            const HalfEdge* cEdge = initialBorder->getNext();
            while( cEdge!=initialBorder  )
            {
                if (cEdge->getFace()!=NULL)
                {
                    throw cpp::Exception("The sequence of half edges on the border does not points to a null face (12)");
                }
                cEdge = cEdge->getNext();
            }
        }
    }

    if( edge->getNext()!=face->getBoundary() )
    {
        throw cpp::Exception("The pointer edge->next of the last edge in a face border must be equals to the face->boundary (12)");
    }
};

template<class Vdt, class Hdt, class Fdt>
void Mesh<Vdt,Hdt,Fdt>::manageUnhandledTriangles()
{
    if( unhandledTrianglesCount*3 != unhandledTriangles.size() )
    {
        throw cpp::Exception("The number of unhandled triangles should be equal to the number of unhandled vertices*3!");
    }

    unsigned int curTriangle = 0;
    while( !this->unhandledTriangles.empty() )
    {
        if( curTriangle==unhandledTrianglesCount )
        {
            unsigned int remainingTriangles = this->unhandledTriangles.size() / 3;
            if( remainingTriangles<unhandledTrianglesCount )
            {
                unhandledTrianglesCount = remainingTriangles;
                curTriangle = 0;
            }
            else
            {
                throw cpp::Exception("There are triangles that cannot be added to the mesh!");
            }
        }

        int v1 = unhandledTriangles.front();
        unhandledTriangles.pop_front();
        int v2 = unhandledTriangles.front();
        unhandledTriangles.pop_front();
        int v3 = unhandledTriangles.front();
        unhandledTriangles.pop_front();
        unsigned int fid = this->createTriangularFace(v1,v2,v3);
        if( fid==-1 ) //ajust the number of triangles in case of the triangle was not added to the mesh
        {
            unhandledTrianglesCount--;
        }

        curTriangle++;
    }
}

template<class Vdt, class Hdt, class Fdt>
int Mesh<Vdt,Hdt,Fdt>::getNumUnhandledTriangles() const
{
    return this->unhandledTrianglesCount;
}

template<class Vdt, class Hdt, class Fdt>
HalfEdgeT<Vdt,Hdt,Fdt>* Mesh<Vdt,Hdt,Fdt>::findIncidentHalfEdge(Vertex* vertex)
{
    HalfEdge* edgeTmp = NULL;
    HalfEdge* result = NULL;
    int nullFacesCount=0;
    EdgeIterator it(vertex);
    while( it.hasNext() )
    {
        edgeTmp = it.getNext();
        if( edgeTmp->getTwin()->getFace()==NULL )
        {
            result = edgeTmp->getTwin();
            nullFacesCount++;
        }
    }
    if( nullFacesCount!=1 )
    {
        result = NULL;
    }
    return result;
}

template<class Vdt, class Hdt, class Fdt>
void Mesh<Vdt,Hdt,Fdt>::clear()
{
    this->vertices.clear();
    this->edges.clear();
    this->faces.clear();
    this->unhandledTriangles.clear();
    this->unhandledTrianglesCount = 0;
}

#endif//DCEL_Mesh_h
