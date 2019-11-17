#pragma once
#include "../../nclgl/SceneNode.h"
#include "..\..\nclgl\OBJMesh.h" 

class MidLakePavilion :public SceneNode{
public:
	MidLakePavilion(void);
	~MidLakePavilion(void) {};
	virtual void Update(float msec);
	static void CreatePivilion() {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"MidLakePavilion.obj");
		pavilion = m;
	}
	static void DeletePavilion() { delete pavilion; }
protected:
	static Mesh* pavilion;
	SceneNode* stone;
};

