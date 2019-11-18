#pragma once
#include "..\..\nclgl\OGLRenderer.h"
#include"../../nclgl/Camera.h"
#include"..\..\nclgl\SceneNode.h"
#include"../../nclgl/Light.h"
#include"MidLakePavilion.h"

class Renderer :public OGLRenderer{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void UpdateScene(float msec);
	virtual void RenderScene();
	virtual void MoveLight();
protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawOBJMesh(OBJMesh* n);

	Light* light;
	OBJMesh* root;
	Camera* camera;

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	GLuint cubeMap;
	Mesh* quad;

	float waterRotate;
};

