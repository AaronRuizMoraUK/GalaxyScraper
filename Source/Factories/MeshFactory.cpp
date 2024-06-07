#include "MeshFactory.h"
#include "Managers/DataLocatorManager.h"
#include "Global/VertexTypes.h"
#include "Global/Definitions.h"
#include <fstream>

MeshFactory * MeshFactory::meshFactory = NULL;

MeshFactory * MeshFactory::getMeshFactory() {
	if(meshFactory == NULL)
		meshFactory = new MeshFactory();

	return meshFactory;
}

IMesh * MeshFactory::createMesh(const std::string &name, CalCoreMesh *calCoreMesh) {
	MeshMap::const_iterator it = meshMap.find(name);

	if(it != meshMap.end())
		return it->second;
	else {
		IMesh * mesh = convertCalCoreMeshToMesh(calCoreMesh);
		assert( mesh );
		mesh->fixBoundingBoxAndSphereForCal3D( ); // This mesh comes from cal3d, so fix its Bounding Box and Shape
		meshMap[name]=mesh;
		return mesh;
	}
}

IMesh * MeshFactory::createMesh(const std::string &filename) {
	MeshMap::const_iterator it = meshMap.find(filename);

	if(it != meshMap.end())
		return it->second;
	else {
		DataLocatorManager *dataLocatorManager = DataLocatorManager::getDataLocatorManager();
		const std::string &pathToShader = dataLocatorManager->getPath("mesh");
		
		std::string pathFilename = pathToShader + filename;
		IMesh * mesh = readMesh(pathFilename.c_str());
		meshMap[filename]=mesh;
		return mesh;
	}
}


/*void readMesh(char *filename) {

leer fichero
leer cabecera
IMeshDef *mdef;
if( header.tvertex == "TVertexColoredTextured" ) {
mdef = new VBColoredTextured;
} else if( header.tvertex == "TVertexColoredDoubleTextured" ) {
mdef = new VBColoredDoubleTextured;
} else {
warn( "Invalid mesh type..");
}
hader.vertex_count
header.total_vertex_ 
read( fd, mdef->getRawVertx( ), mdef->getTotalSizeInVertex()  ) 
mdef->loadToVRAM();
mdef->freeSystemMemory( );
registrw( "pep", mdef );
}*/


IMesh * MeshFactory::readMesh(const char *filename)
{
	std::ifstream myFile;
	myFile.open(filename, std::ios::in | std::ios::binary);

	if (myFile.is_open()) {

		//Read number of vertices
		int num_vertices;
		if (!myFile.read((char *)&num_vertices, 4)) {
			return NULL;
		}

		//Number of data to read of vertices (not used)
		int dataToRead;
		if (!myFile.read ((char *)&dataToRead, 4)) {
			return NULL;
		}

		//Read number of faces
		int num_triangles;
		if (!myFile.read ((char *)&num_triangles, 4)) {
			return NULL;
		}

		IMesh *mesh = new CoreMeshNormalTanTextured(num_vertices, num_triangles);

		// Vertices del objeto con coordenadas y textura
		CUSTOMVERTEX_NORMAL_TAN_TEXTURE * flatvertices = (CUSTOMVERTEX_NORMAL_TAN_TEXTURE *)mesh->getRawVertices();

		//Read Vertices
		for (int i=0; i<num_vertices; ++i) {
			if (!myFile.read ((char *)&(flatvertices[i].x), 4)) {
				return NULL;
			}

			//Read number of faces
			if (!myFile.read ((char *)&(flatvertices[i].y), 4)) {
				return NULL;
			}

			//Read number of faces
			if (!myFile.read ((char *)&(flatvertices[i].z), 4)) {
				return NULL;
			}


			if (!myFile.read ((char *)&(flatvertices[i].nx), 4)) {
				return NULL;
			}

			//Read number of faces
			if (!myFile.read ((char *)&(flatvertices[i].ny), 4)) {
				return NULL;
			}


			//Read number of faces
			if (!myFile.read ((char *)&(flatvertices[i].nz), 4)) {
				return NULL;
			}

			//Read number of faces
			if (!myFile.read ((char *)&(flatvertices[i].u1), 4)) {
				return NULL;
			}
			//Read number of faces
			if (!myFile.read ((char *)&(flatvertices[i].v1), 4)) {
				return NULL;
			}

			//Read number of faces
			/*if (!myFile.read ((char *)&(flatvertices[i].u2), 4)) {
			return NULL;
			}
			//Read number of faces
			if (!myFile.read ((char *)&(flatvertices[i].v2), 4)) {
			return NULL;
			}*/
		}

		// Vertices del objeto con coordenadas y textura
		unsigned short * flatFaces = (unsigned short *)mesh->getRawIndexes();
		int temp;

		//Read Faces
		for (int i=0; i<num_triangles*3; ++i) {
			//Read number of faces
			if (!myFile.read ((char *)&(temp), 4)) {
				return NULL;
			}
			flatFaces[i]=temp;
			flatFaces[i]--;
		}

		D3DXVECTOR3 min;
		//Read Faces
		for (int i=0; i<3; ++i) {
			//Read number of faces
			if (!myFile.read ((char *)&(min[i]), 4)) {
				return NULL;
			}
		}

		D3DXVECTOR3 max;
		//Read Faces
		for (int i=0; i<3; ++i) {
			//Read number of faces
			if (!myFile.read ((char *)&(max[i]), 4)) {
				return NULL;
			}
		}

		myFile.close();


		// CALCULATING TANGENTS
		const int numVertices = mesh->getNumTotalVertices();
		const int numTriangles = mesh->getNumTotalTriangles();
		CUSTOMVERTEX_NORMAL_TAN_TEXTURE *vertices = (CUSTOMVERTEX_NORMAL_TAN_TEXTURE *) mesh->getRawVertices();
		unsigned short *indexes = (unsigned short *) mesh->getRawIndexes();
		D3DXVECTOR3 *positions = new D3DXVECTOR3[ numVertices ];
		D3DXVECTOR3 *normals   = new D3DXVECTOR3[ numVertices ];
		D3DXVECTOR2 *uvs       = new D3DXVECTOR2[ numVertices ];
		D3DXVECTOR3 *tangents  = new D3DXVECTOR3[ numVertices ];
		for( int i=0; i<numVertices; ++i ) {
			CUSTOMVERTEX_NORMAL_TAN_TEXTURE &v = vertices[ i ];
			positions[ i ] = D3DXVECTOR3( v.x, v.y, v.z );
			normals[ i ] = D3DXVECTOR3( v.nx, v.ny, v.nz );
			uvs[ i ] = D3DXVECTOR2( v.u1, v.v1 );
		}
		CalculateTangentArray( numVertices, positions, normals, 
			uvs, numTriangles, (const Triangle *) indexes, tangents );
		for( int i=0; i<numVertices; ++i ) {
			vertices[ i ].tx = tangents[ i ].x;
			vertices[ i ].ty = tangents[ i ].y;
			vertices[ i ].tz = tangents[ i ].z;
		}
		delete [] positions;
		delete [] normals;
		delete [] uvs;
		delete [] tangents;


		mesh->loadToVRAM();
		mesh->constructBoundingBox(min, max);
		mesh->constructBoundingSphere();
		mesh->freeSystemMemory();

		return mesh;
	}

	return NULL;
}


void MeshFactory::CalculateTangentArray(int vertexCount, const D3DXVECTOR3 *vertex, const D3DXVECTOR3 *normal,
										const D3DXVECTOR2 *texcoord, int triangleCount, const Triangle *triangle, 
										D3DXVECTOR3 *tangent)
{
	D3DXVECTOR3 *tan1 = new D3DXVECTOR3[vertexCount * 2];
	D3DXVECTOR3 *tan2 = tan1 + vertexCount;
	memset(tan1, 0, vertexCount * sizeof(D3DXVECTOR3) * 2);

	for (int a = 0; a < triangleCount; a++)
	{
		unsigned short i1 = triangle->index[0];
		unsigned short i2 = triangle->index[1];
		unsigned short i3 = triangle->index[2];

		const D3DXVECTOR3& v1 = vertex[i1];
		const D3DXVECTOR3& v2 = vertex[i2];
		const D3DXVECTOR3& v3 = vertex[i3];

		const D3DXVECTOR2& w1 = texcoord[i1];
		const D3DXVECTOR2& w2 = texcoord[i2];
		const D3DXVECTOR2& w3 = texcoord[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		D3DXVECTOR3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		D3DXVECTOR3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;

		triangle++;
	}

	for (int a = 0; a < vertexCount; a++)
	{
		const D3DXVECTOR3& n = normal[a];
		const D3DXVECTOR3& t = tan1[a];

		// Gram-Schmidt orthogonalize
		D3DXVECTOR3 tm = t - n * D3DXVec3Dot(&n, &t);

		D3DXVec3Normalize( &tm, &tm );
		tangent[a].x = tm.x;
		tangent[a].y = tm.y;
		tangent[a].z = tm.z;

		// Calculate handedness
		//D3DXVECTOR3 vcross;
		//D3DXVec3Cross( &vcross, &n, &t );
		//tangent[a].w = (D3DXVec3Dot( &vcross, &tan2[a]) < 0.0f) ? -1.0f : 1.0f;
	}

	delete[] tan1;
}

IMesh * MeshFactory::convertCalCoreMeshToMesh(CalCoreMesh *calCoreMesh) {
	CalCoreSubmesh *calCoreSubmesh;

	if (calCoreMesh->getVectorCoreSubmesh().size() == 1)
		calCoreSubmesh = calCoreMesh->getCoreSubmesh(0);
	else
		calCoreSubmesh = calCoreMesh->getCoreSubmesh(1);

	if (calCoreSubmesh == NULL)
		return NULL;

	int skin_vertex_count = calCoreSubmesh->getVertexCount( );
	int skin_faces_count = calCoreSubmesh->getFaceCount();

	IMesh *mesh = new CoreMeshCal3DNormalTextured(skin_vertex_count, skin_faces_count);

	CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE * flatVertices = (CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE *)mesh->getRawVertices();

	const std::vector<CalCoreSubmesh::Vertex> &calVertexVector =  calCoreSubmesh->getVectorVertex( );
	const std::vector<std::vector<CalCoreSubmesh::TextureCoordinate> > &calTextureCoordinateVectors =  calCoreSubmesh->getVectorVectorTextureCoordinate();

	int sizeVector = calTextureCoordinateVectors.size();
	assert(sizeVector == 1);
	const std::vector<CalCoreSubmesh::TextureCoordinate> &calTextureCoordinateVector = calTextureCoordinateVectors[0];

	const CalCoreSubmesh::Vertex &calVertexFirst = calVertexVector[ 0 ];
	D3DXVECTOR3 minVertex(calVertexFirst.position.x, calVertexFirst.position.y, calVertexFirst.position.z);
	D3DXVECTOR3 maxVertex(minVertex);

	for (int i=0; i<skin_vertex_count; ++i) {
		const CalCoreSubmesh::Vertex &calVertex = calVertexVector[ i ];

		flatVertices[i].x = calVertex.position.x;
		flatVertices[i].y = calVertex.position.y;
		flatVertices[i].z = calVertex.position.z;

		if (flatVertices[i].x < minVertex.x)
			minVertex.x = flatVertices[i].x;

		if (flatVertices[i].y < minVertex.y)
			minVertex.y = flatVertices[i].y;

		if (flatVertices[i].z < minVertex.z)
			minVertex.z = flatVertices[i].z;

		if (flatVertices[i].x > maxVertex.x)
			maxVertex.x = flatVertices[i].x;

		if (flatVertices[i].y > maxVertex.y)
			maxVertex.y = flatVertices[i].y;

		if (flatVertices[i].z > maxVertex.z)
			maxVertex.z = flatVertices[i].z;

		flatVertices[i].nx = calVertex.normal.x;
		flatVertices[i].ny = calVertex.normal.y;
		flatVertices[i].nz = calVertex.normal.z;

		flatVertices[i].u1 = calTextureCoordinateVector[i].u;
		flatVertices[i].v1 = calTextureCoordinateVector[i].v;

		for( int j=0; j<4; ++j ) {
			flatVertices[i].weights[ j ] = 0.0f;
			flatVertices[i].indices[ j ] = 0.0f;
		}

		assert( calVertex.vectorInfluence.size( ) <= MAX_INFLUENCES );
		for( unsigned int j=0; j<calVertex.vectorInfluence.size( ); ++j ) {
			const CalCoreSubmesh::Influence &influence = calVertex.vectorInfluence[ j ];
			flatVertices[i].weights[ j ] = influence.weight;
			flatVertices[i].indices[ j ] = influence.boneId;
		}

		//char msg[ 128 ];
		//sprintf_s(msg, sizeof(msg), "Vertice %d le influye el hueso %1.0f %1.0f %1.0f %1.0f con pesos %f %f %f %f\n", i, flatVertices[i].indices[0], flatVertices[i].indices[1], flatVertices[i].indices[2], flatVertices[i].indices[3], flatVertices[i].weights[0], flatVertices[i].weights[1], flatVertices[i].weights[2], flatVertices[i].weights[3]);
		//OutputDebugString(msg);

		// AARRGGBB -> weights[0] = BB ; weights[1] = GG ; weights[2] = RR ; weights[3] = AA 
		// So swat [0] <-> [2]
		/*float tmpVtx = flatVertices[i].weights[ 0 ];
		flatVertices[i].weights[ 0 ] = flatVertices[i].weights[ 2 ];
		flatVertices[i].weights[ 2 ] = tmpVtx;

		float tmpIdx = flatVertices[i].indices[ 0 ];
		flatVertices[i].indices[ 0 ] = flatVertices[i].indices[ 2 ];
		flatVertices[i].indices[ 2 ] = tmpIdx;*/
	}

	unsigned short * flatFaces = (unsigned short *)mesh->getRawIndexes();

	const std::vector<CalCoreSubmesh::Face> &calFaceVector =  calCoreSubmesh->getVectorFace( );

	for (int i=0; i<skin_faces_count; ++i) {
		const CalCoreSubmesh::Face &calFace = calFaceVector[i];

		// This way the indices because of the normals of the face
		// In cal3d are reversed
		flatFaces[i*3] = calFace.vertexId[2];
		flatFaces[i*3+1] = calFace.vertexId[1];
		flatFaces[i*3+2] = calFace.vertexId[0];
	}

	// CALCULATING TANGENTS
	const int numVertices = mesh->getNumTotalVertices();
	const int numTriangles = mesh->getNumTotalTriangles();
	CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE *vertices = (CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE *) mesh->getRawVertices();
	unsigned short *indexes = (unsigned short *) mesh->getRawIndexes();
	D3DXVECTOR3 *positions = new D3DXVECTOR3[ numVertices ];
	D3DXVECTOR3 *normals   = new D3DXVECTOR3[ numVertices ];
	D3DXVECTOR2 *uvs       = new D3DXVECTOR2[ numVertices ];
	D3DXVECTOR3 *tangents  = new D3DXVECTOR3[ numVertices ];
	for( int i=0; i<numVertices; ++i ) {
		CUSTOMVERTEX_CAL3D_NORMAL_TAN_TEXTURE &v = vertices[ i ];
		positions[ i ] = D3DXVECTOR3( v.x, v.y, v.z );
		normals[ i ] = D3DXVECTOR3( v.nx, v.ny, v.nz );
		uvs[ i ] = D3DXVECTOR2( v.u1, v.v1 );
	}
	CalculateTangentArray( numVertices, positions, normals, 
		uvs, numTriangles, (const Triangle *) indexes, tangents );
	for( int i=0; i<numVertices; ++i ) {
		vertices[ i ].tx = tangents[ i ].x;
		vertices[ i ].ty = tangents[ i ].y;
		vertices[ i ].tz = tangents[ i ].z;
	}
	delete [] positions;
	delete [] normals;
	delete [] uvs;
	delete [] tangents;

	mesh->loadToVRAM();
	mesh->constructBoundingBox(minVertex, maxVertex);
	mesh->constructBoundingSphere();
	mesh->freeSystemMemory();

	return mesh;
}


/*IMesh * MeshFactory::readNoNormalMesh(const char *filename)
{
std::ifstream myFile;
myFile.open(filename, std::ios::in | std::ios::binary);

if (myFile.is_open()) {

//Read number of vertices
int num_vertices;
if (!myFile.read((char *)&num_vertices, 4)) {
return NULL;
}

//Number of data to read of vertices (not used)
int dataToRead;
if (!myFile.read ((char *)&dataToRead, 4)) {
return NULL;
}

//Read number of faces
int num_triangles;
if (!myFile.read ((char *)&num_triangles, 4)) {
return NULL;
}

IMesh *mesh = new CoreMeshTextured(num_vertices, num_triangles);

// Vertices del objeto con coordenadas y textura
CUSTOMVERTEX_TEXTURE * flatvertices = (CUSTOMVERTEX_TEXTURE *)mesh->getRawVertices();

//Read Vertices
for (int i=0; i<num_vertices; ++i) {
if (!myFile.read ((char *)&(flatvertices[i].x), 4)) {
return NULL;
}

//Read number of faces
if (!myFile.read ((char *)&(flatvertices[i].y), 4)) {
return NULL;
}


//Read number of faces
if (!myFile.read ((char *)&(flatvertices[i].z), 4)) {
return NULL;
}

//Read number of faces
if (!myFile.read ((char *)&(flatvertices[i].u1), 4)) {
return NULL;
}
//Read number of faces
if (!myFile.read ((char *)&(flatvertices[i].v1), 4)) {
return NULL;
}

//Read number of faces
//if (!myFile.read ((char *)&(flatvertices[i].u2), 4)) {
//return NULL;
//}
//Read number of faces
//if (!myFile.read ((char *)&(flatvertices[i].v2), 4)) {
//return NULL;
//}
}

// Vertices del objeto con coordenadas y textura
unsigned short * flatFaces = (unsigned short *)mesh->getRawIndexes();
int temp;
//Read Faces
for (int i=0; i<num_triangles*3; ++i) {
//Read number of faces
if (!myFile.read ((char *)&(temp), 4)) {
return NULL;
}
flatFaces[i]=temp;
flatFaces[i]--;
}

D3DXVECTOR3 min;
//Read Faces
for (int i=0; i<3; ++i) {
//Read number of faces
if (!myFile.read ((char *)&(min[i]), 4)) {
return NULL;
}
}

D3DXVECTOR3 max;
//Read Faces
for (int i=0; i<3; ++i) {
//Read number of faces
if (!myFile.read ((char *)&(max[i]), 4)) {
return NULL;
}
}

myFile.close();

mesh->loadToVRAM();
mesh->constructBoundingBox(min, max);
mesh->constructBoundingSphere();
mesh->freeSystemMemory();

return mesh;
}

return NULL;
}*/
