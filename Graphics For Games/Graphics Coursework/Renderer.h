#pragma once
#include "..\..\nclgl\OGLRenderer.h"
#include "..\..\nclgl\Camera.h"
#include "..\..\nclgl\HeightMap.h"
#include "..\..\nclgl\MD5Mesh.h"
#include "..\..\nclgl\MD5Node.h"
# define SHADOWSIZE 2048 
# define POST_PASSES 10
class Renderer :public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float msec);
	virtual void MoveLight();
	void TogglePostProssing();
protected:
	void DrawHeightmap();
	void DrawWater();
	void DrawSkybox();
	void DrawMesh(); // New !
	void DrawShadowScene(); // New !
	void DrawCombinedScene(); // New !
	void DrawPostProcess();

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* processShader;

	HeightMap* heightMap;
	Mesh* quad;
	Mesh* quad1;

	GLuint shadowTex;
	GLuint shadowFBO;
	GLuint cubeMap;

	MD5FileData* hellData;
	MD5Node* hellNode;
	Light* light;
	Camera* camera;

	GLuint bufferFBO;
	GLuint processFBO;
	GLuint bufferColourTex[2];
	GLuint bufferDepthTex;
	bool usingpostProssing=false;
	float waterRotate;

};

