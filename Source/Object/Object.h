#ifndef OBJECT_H
#define OBJECT_H

#include "Mesh/IMesh.h"
#include "Material/Material.h"
#include <string>

/**
* Object in scene. This has position, transform matrix, mesh and
* material vector.
*/

class Object
{
protected:
	D3DXVECTOR3 position;
	D3DXVECTOR3 localFront;
	D3DXVECTOR3 localLeft;
	D3DXVECTOR3 localUp;
	D3DXMATRIX transformMatrix;

	// Pointer to object who is the parent (for platforms)
	const Object *parentObject;
	D3DXMATRIX localTransformMatrix; // TODO Local transform matrix

	const IMesh *mesh;

	MaterialVector materialVector;

	// We store if the object is drawn to update or not the AnimationController
	bool drawn;

public:
	Object();

	virtual ~Object() {
		OutputDebugString("Destroying Object...\n");

		// Object does not destroy its Mesh, MeshFactory does!!.

		// Automaticaly material vector destroies itself
	};

	// VERY VERY VERY OPTIONAL!!!!
	void SETMATERIAL (const std::string &texture, const std::string &shader, const std::string &technique){
		Material *material = new Material(texture, shader, technique);
		materialVector.push_back(material);
	};

	// VERY VERY VERY OPTIONAL!!!!
	void SETMATERIAL (Texture texture, const std::string &shader, const std::string &technique){
		Material *material = new Material(texture, shader, technique);
		materialVector.push_back(material);
	};

	virtual void render() const;

	const IMesh * getMesh() const {
		return mesh;
	};

	void setMesh(IMesh *pMesh) {
		mesh=pMesh;
	};

	const MaterialVector & getMaterialVector() const {
		return materialVector;
	};

	// TODO This is just for one material in material vector
	void setTechnique(const std::string &technique) {
		materialVector[0]->setTechnique(technique);
	}

	void reloadShader();

	const D3DXMATRIX & getTransformMatrix() const {
		return transformMatrix;
	};

	virtual void setTransformMatrix(const D3DXMATRIX &pTransformMatrix) {
		transformMatrix = pTransformMatrix;
		updateVariablesFromTransformMatrix();
	};

	void setLocalTransformMatrix(const D3DXMATRIX &pTransformMatrix) {
		assert( parentObject || !"If no exist parentObject do not use setLocalTransformMatrix" );
		localTransformMatrix = pTransformMatrix;
	};

	void setParentObject(const Object *pParentObject) {
		parentObject = pParentObject;
	};

	void updateTransformMatrix() {
		assert(parentObject);
		if(parentObject) {
			const D3DXMATRIX &parentTransformMatrix = parentObject->getTransformMatrix();
			D3DXMATRIX newTransformMatrix;
			D3DXMatrixMultiply(&newTransformMatrix, &localTransformMatrix, &parentTransformMatrix);

			setTransformMatrix(newTransformMatrix);
		}
	};

	const D3DXVECTOR3 & getPosition() const {return position;};
	const D3DXVECTOR3 & getLocalLeftVector() const {return localLeft;};
	const D3DXVECTOR3 & getLocalUpVector() const {return localUp;};
	const D3DXVECTOR3 & getLocalFrontVector() const {return localFront;};
	virtual void setPosition(const D3DXVECTOR3 &pPosition);
	virtual void setAngles(float yaw, float pitch);
	float getYaw() const;
	float getPitch() const;

	bool getDrawn() const { return drawn; };
	void setDrawn(bool pDrawn) { drawn = pDrawn; };

private:
	void updateVariablesFromTransformMatrix();
};

#endif //OBJECT_H
