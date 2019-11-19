#pragma once
#include "..\..\nclgl\OGLRenderer.h"
#include "..\..\nclgl\Camera.h"
#include "..\..\nclgl\HeightMap.h"
#include "..\..\nclgl\MD5Mesh.h"
#include "..\..\nclgl\MD5Node.h"
# define SHADOWSIZE 2048 
class Renderer :public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	virtual void MoveLight();
protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawMesh(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* sceneShader;
	Shader* shadowShader;

	HeightMap* heightMap;
	Mesh* quad;

	GLuint shadowTex;
	GLuint shadowFBO;
	GLuint cubeMap;

	MD5FileData* hellData;
	MD5Node* hellNode;
	Light* light;
	Camera* camera;

	float waterRotate;

};

