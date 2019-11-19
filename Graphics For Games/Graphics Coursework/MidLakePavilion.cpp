#include "MidLakePavilion.h"

Mesh* MidLakePavilion::pavilion = NULL;

MidLakePavilion::MidLakePavilion(void) {
	//SceneNode* mainScene = new SceneNode(pavilion, Vector4(1, 0, 1, 1));
	//mainScene->SetModelScale(Vector3(1, 1, 1));
	//mainScene->SetTransform(Matrix4::Translation(Vector3(0, 0, 0)));
	//AddChild(mainScene);
	//mainScene->SetBoundingRadius(1.0f);
	//stone = new SceneNode(pavilion,Vector4(0,1,0,1));
	//stone->SetModelScale(Vector3(5, 5, 5));
	//stone->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	//mainScene->AddChild(stone);
	SetMesh(pavilion);
}

void MidLakePavilion::Update(float msec) {

}	
