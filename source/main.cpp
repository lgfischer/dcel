///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#include "DCEL/Mesh.h"
#include "DCEL/DCELStream.h"
#include "DCEL/WavefrontObjImporter.h"
#include "DCEL/Vector3.h"
#include <fstream>

/**
    Here I'll define the three data structures that will be
	associated with Vertices, half-edges and faces. Put anything you want 
	inside of it.
*/
class VertexData
{
public:
	VertexData()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	float x, y, z;
	void setVerticeCoordinates(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	/**
	    If you need to save the stream to a file, rewrite the << operator for the 
		ostream class. The DCELStream class will use it to write to a file. More latter.
	*/
	friend std::ostream& operator <<(std::ostream &os, const VertexData& vertexData)
	{
		os << vertexData.x << " " << vertexData.y;
		return os;
	}

	/**
	    If you save the mesh to a file, rewrite the operator >> to be able to load
		the mesh latter using the DCELStream class.
	*/
	friend std::istream& operator >>(std::istream &is, VertexData& vertexData)
	{
		is >> vertexData.x >> vertexData.y;
		return is;
	}
};

/**
    Another possible class to use within a vertex. In the current implementation of
	the WavefrontObjImporter, I need the Vector3f class to load the vertices positions.

	Of course, there should be another way to do this, but I didn't figured out.
	If you have a suggestion, let me know =)
*/
class VertexDataWithPosition
{
public:
	Vector3f position;
protected:
private:
};

class HalfEdgeData
{
public:
	HalfEdgeData() {
		modified = false;
	}

	void modify() {
		modified = true;
	}

	bool isModified() const {
		return modified;
	}

	friend std::ostream& operator <<(std::ostream &os, const HalfEdgeData& halfEdgeData)
	{
		std::string modified = halfEdgeData.isModified()? "true" : "false";
		os << modified;
		return os;
	}

	friend std::istream& operator >>(std::istream &is, HalfEdgeData& halfEdgeData)
	{
		std::string modified;
		is >> modified;

		if (modified.compare("true")==0)
		{
			halfEdgeData.modify();
		}

		return is;
	}

private:
	bool modified;
};

class FaceData
{
public:
	std::string colorName;

	friend std::ostream& operator <<(std::ostream &os, const FaceData& faceData)
	{
		os << faceData.colorName;
		return os;
	}

	friend std::istream& operator >>(std::istream &is, FaceData& faceData)
	{
		is >> faceData.colorName;
		return is;
	}
};


/**
    Now I will define the type MyMesh, which is a Mesh that each vertex, face and half-edge
	has one object given as parameter here.
*/
typedef Mesh<VertexData, HalfEdgeData, FaceData> MyMesh;

/**
	Another type of Mesh. In this type, each vertex will have an instance of
	VertexDataWithPosition.

	Although MyMesh and MyMeshUsingVector have the same structure, they are not
	interchangeable. They are different objects! But you can use 
	the DCELStream::copyDcelData() method to convert between different mesh types.
*/
typedef Mesh<VertexDataWithPosition, HalfEdgeData, FaceData> MyMeshUsingVector;


/**
    This method will create and initialize an hard-coded mesh.
*/
void createHardCodedMesh(MyMesh* mesh) {

	// reserve the needed space for the mesh. These numbers came from the hardcoded structure :)
	mesh->getVertices().reserve( 5 ); // we will put 5 vertices into the mesh
	mesh->getHalfEdges().reserve( 16 ); //there are 8 edges on the figure. Each edge has 2 half-edges. So, 16
	mesh->getFaces().reserve( 4 ); //4 faces

	/**
	    Now, create the vertices. Each time I call createGetVertex here, a new vertex is created 
		and a pointer to it is returned. The first vertex has its ID equals to 0, the second equals
		to 1, and so on.

		If you call getData() on that pointer, you get a reference for the VertexData 
		object that is inside this vertex. The getData() works in the same way for 
		Half-Edge and Face pointers, returning a HalfEdgeData and FaceData object, 
		respectively.
	*/
	mesh->createGetVertex()->getData().setVerticeCoordinates( 10, 5  ); //vertex 0
	mesh->createGetVertex()->getData().setVerticeCoordinates( 6,  10 ); //vertex 1
	mesh->createGetVertex()->getData().setVerticeCoordinates( 15, 9  ); //vertex 2
	mesh->createGetVertex()->getData().setVerticeCoordinates( 14, 1  ); //vertex 3
	mesh->createGetVertex()->getData().setVerticeCoordinates( 2,  4  ); //vertex 4

	/**
	    Now we will create the faces using the IDs from the vertices created before. I'll
		give the three vertices's IDs that represents each face. Again, the faces receive 
		an incremental ID starting from 0.
		
		I must ensure that all faces are described in the same orientation (clock-wise or 
		counter-clock-wise). If I fail in this, I will receive very strange runtime errors.
	*/
	mesh->createTriangularFace( 0, 1, 2 );
	mesh->createTriangularFace( 0, 2, 3 );
	mesh->createTriangularFace( 0, 3, 4 );
	mesh->createTriangularFace( 0, 4, 1 );

	/**
		This is just a helper method that checks that all pointers are 
		set correctly. If I did anything wrong before, an exception will
		be thrown.
	*/
	mesh->checkAllFaces();

	/**
	    Now I'll use the ID of each face to set a color on it.
	*/
	mesh->getFace(0)->getData().colorName = "red";
	mesh->getFace(1)->getData().colorName = "green";
	mesh->getFace(2)->getData().colorName = "blue";
	mesh->getFace(3)->getData().colorName = "white";

	/**
	    Now I'll modify some half edges using a iterator. This will iterate over 
		all half-edges that starts on the vertex 1. The iterator can run over all 
		the edges that starts on the given vertex or face.
	*/
	MyMesh::EdgeIterator iterator( mesh->getVertex(1) ); 
	while(iterator.hasNext()) {
		MyMesh::HalfEdge* halfEdge = iterator.getNext();
		halfEdge->getData().modify();
	}

	/**
	    And also, I'll modify the half-edge that starts on the vertex 0 and ends on 
		the vertex 3. The method getHalfEdge(unsigned int a, unsigned int b) returns the 
		half-edge that starts at 'a' and ends at 'b', or null if such connection does
		not exists.
	*/
	MyMesh::HalfEdge* edge = mesh->getHalfEdge(3, 0);
	if (edge!=NULL)
	{
		edge->getData().modify();
	}
}

void saveMesh(MyMesh* mesh, const char* filename)
{
	if (mesh!=NULL)
	{
		std::ofstream outputFile;
		outputFile.open(filename);
		if (outputFile)
		{
			/**
			    This is where I will write the mesh to a file.

				Note that the operator "<<" need to be rewrited on the VertexData, 
				HalfEdgeData and FaceData classes.
			*/
			DCELStream<MyMesh>::write(*mesh, outputFile);

			outputFile.close();
		}
	}
}

void loadMesh(MyMesh* mesh, const char* filename)
{
	if (mesh!=NULL)
	{
		std::ifstream inputFile;
		inputFile.open(filename);
		if (inputFile)
		{
			/**
			    This is where I load the mesh from a DCEL file, saved with the 
				DCELStream<MyMesh>::write() method.

				As for the DCELStream<MyMesh>::write(), here you need to rewrite the
				">>" operator on the VertexData, HalfEdgeData and FaceData classes.
			*/
			DCELStream<MyMesh>::read(*mesh, inputFile);

			inputFile.close();
		}
	}
}

void doSomeStuffWithMesh(MyMesh* mesh)
{
	/**
	    Print some basic information about the mesh.
	*/
	std::cout << "The mesh contain " << mesh->getNumFaces() << " faces." << std::endl;
	std::cout << "The mesh contain " << mesh->getNumVertices() << " vertices." << std::endl;
	std::cout << "The mesh contain " << mesh->getNumHalfEdges() << " half-edges. " << 
		"This number is 2 times the number of edges." << std::endl;
	std::cout << std::endl;

	/**
	    Show the ID of the neighbor vertices of the vertex 4. It will get each half-edge 
		that starts	at the vertex 4, get its twin half-edge, and get the origin of the twin
		half-edge.

		The method getVertexId() returns the id of a given vertex. There are similar vertices
		for half-edges and faces
	*/
	std::cout << "The neighbors of the vertex 4 are: ";
	MyMesh::Vertex * vertex = mesh->getVertex(4);
	MyMesh::EdgeIterator iterator(vertex);
	while(iterator.hasNext()) {
		MyMesh::Vertex* neighborVertex = iterator.getNext()->getTwin()->getOrigin();
		std::cout << mesh->getVertexId(neighborVertex) << ", ";
	}
	std::cout << std::endl << std::endl;

	/**
	    Iterate over the edges that composes the face 1 (the one composed by
		the vertices 0, 2, 3). For each edge, will show its start vertex ID,
		its ending vertex ID (which is the origin of its twin), and show if
		it was modified (remember that I called modify() on the half-edge that
		starts on vertex 3 and ends on vertex 0, see createHardCodedMesh() ).
	*/
	std::cout << "The half-edges of the face 1 are: ";
	MyMesh::Face* face = mesh->getFace(1);
	iterator.reset(face); //now the iterator will run over the edges of the face
	while ( iterator.hasNext() )
	{
		MyMesh::HalfEdge* halfEdge = iterator.getNext();
		std::cout << "[id="<< mesh->getHalfEdgeId(halfEdge) << 
			" origin=" << mesh->getVertexId( halfEdge->getOrigin() ) <<
			" end=" << mesh->getVertexId( halfEdge->getTwin()->getOrigin() ) << 
			" modified=" << (halfEdge->getData().isModified()?"true":"false") << "], ";
	}
	std::cout << std::endl << std::endl;

	/**
	    Iterate over all faces that the mesh have, showing its color. It is possible 
		to iterate over the half-edges and faces in the same way.
	*/
	std::cout << "The faces are: ";
	for (unsigned int i=0; i<mesh->getNumFaces(); ++i)
	{
		MyMesh::Face* face = mesh->getFace(i);
		std::cout << "[id=" << mesh->getFaceId(face) << " color=" << face->getData().colorName << "], ";
	}
	std::cout << std::endl << std::endl;
}

void importWavefrontObjFile(MyMeshUsingVector* mesh, const char* filename)
{
	WavefrontObjImporter<MyMeshUsingVector> importer;
	importer.import(filename, *mesh);
}

void convertBetweenMeshes(MyMeshUsingVector* meshUsingVector, MyMesh* mesh)
{
	/**
	    This will create on the 'mesh' the same structure of 'meshUsingVector'.
		Note that only the DCEL connectivity is copied.
	*/
	DCELStream<MyMeshUsingVector>::copyDcelData(*meshUsingVector, *mesh);

	/**
		As the IDs of faces, half-edges and vertices are the same in both meshes, 
		it is possible to use this fact to convert internal data from one structure 
		to the other.

		Of course, there should be a better way to do this automatically in the
		copyDcelData() method, but I had no time to implement it =(
	*/
	for (unsigned int i=0;i<meshUsingVector->getNumVertices(); ++i)
	{
		MyMeshUsingVector::Vertex* originVertex = meshUsingVector->getVertex(i);
		MyMesh::Vertex* targetVertex = mesh->getVertex(i);
		targetVertex->getData().x = originVertex->getData().position.x;
		targetVertex->getData().y = originVertex->getData().position.y;
		targetVertex->getData().z = originVertex->getData().position.z;
	}
	for (unsigned int i=0;i<meshUsingVector->getNumHalfEdges(); ++i)
	{
		MyMeshUsingVector::HalfEdge* originHalfEdge = meshUsingVector->getHalfEdge(i);
		MyMesh::HalfEdge* targetHalfEdge = mesh->getHalfEdge(i);
		if (originHalfEdge->getData().isModified())
		{
			targetHalfEdge->getData().modify();
		}
	}
	for (unsigned int i=0;i<meshUsingVector->getNumFaces(); ++i)
	{
		MyMesh::Face* targetFace = mesh->getFace(i);
		targetFace->getData().colorName = "yellow";
	}
}

int main(int argc, char* argv[])
{
	try
	{
		/**
		    Creates a new, hard coded mesh, and save it to a file.
		*/
		MyMesh*  mesh = new MyMesh();
		createHardCodedMesh(mesh);
		saveMesh(mesh, "hard-coded-mesh.dcel");
		delete mesh;

		/**
			Creates a new, empty mesh, and fill it with the data from 
			the file saved before. Also, print some stuff about it
			on the console.
		*/
		mesh = new MyMesh(); //create a new, empty mesh
		loadMesh(mesh, "hard-coded-mesh.dcel");
		doSomeStuffWithMesh(mesh);
		delete mesh;

		/**
			Creates another type of mesh (using my Vector3 class). Then, import a .OBJ
			file into it. Finally, convert that mesh to the MyMesh data structure.

			The method checkAllFaces() just ensures that everything is ok =)
		*/
		MyMeshUsingVector* meshUsingVect = new MyMeshUsingVector();
		mesh = new MyMesh();
		importWavefrontObjFile(meshUsingVect, "sphere.obj");
		convertBetweenMeshes(meshUsingVect, mesh);
		mesh->checkAllFaces();
		meshUsingVect->checkAllFaces();
		delete meshUsingVect;
		delete mesh;

		system("pause");
		return 0;
	}
	catch (std::exception& e)
	{
		std::cerr << "An exception occurred: " << e.what() << std::endl;
		system("pause");
		return 1;
	}
}
