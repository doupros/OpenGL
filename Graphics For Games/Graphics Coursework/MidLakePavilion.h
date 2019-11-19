#pragma once
#include "../../nclgl/SceneNode.h"
#include "..\..\nclgl\OBJMesh.h" 

class MidLakePavilion :public SceneNode,public OBJMesh{
public:
	MidLakePavilion(void);
	~MidLakePavilion(void) {};
	virtual void Update(float msec);
	Mesh* GetPavilion() { return pavilion; }
	static void CreatePivilion() {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"ÉÇÉuèdèÑómäÕ1943(”Ãﬁ√®”±).obj");
		pavilion = m;
	}
	static void DeletePavilion() { delete pavilion; }	
	static Mesh* pavilion;
protected:

};

