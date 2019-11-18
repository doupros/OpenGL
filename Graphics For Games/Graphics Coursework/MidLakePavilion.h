#pragma once
#include "../../nclgl/SceneNode.h"
#include "..\..\nclgl\OBJMesh.h" 

class MidLakePavilion :public SceneNode,public OBJMesh{
public:
	MidLakePavilion(void);
	~MidLakePavilion(void) {};
	virtual void Update(float msec);
	OBJMesh* GetPavilion() { return pavilion; }
	static void CreatePivilion() {
		OBJMesh* m = new OBJMesh();
		m->LoadOBJMesh(MESHDIR"�閾���ƌu�X�e�[�W.obj");
		pavilion = m;
	}
	static void DeletePavilion() { delete pavilion; }
protected:
	static OBJMesh* pavilion;
};

