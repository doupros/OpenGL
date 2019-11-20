#pragma once
#include "..\..\nclgl\OGLRenderer.h"
#include"../../nclgl/Camera.h"
#include"..\..\nclgl\SceneNode.h"
#include"../../nclgl/Light.h"
#include "..\..\nclgl\HeightMap.h"
#include "../../nclgl/MD5Mesh.h"
#include "..\..\nclgl\MD5Node.h"
#include"MidLakePavilion.h"

# define SHADOWSIZE 2048 // New !



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

	void DrawOBJMesh();
	void DrawNode(SceneNode* n);

	void DrawMesh(); // New !
	void DrawFloor(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !

	SceneNode* root;

	Shader* sceneShader;
	Shader* shadowShader;
	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;

	GLuint shadowTex;
	GLuint shadowFBO;

	MD5FileData* hellData;
	MD5Node* hellNode;
	Mesh* floor;
	HeightMap* heightMap;
	Light* light;
	Camera* camera;
	Mesh* quad;
	GLuint cubeMap;

	float waterRotate;
};

