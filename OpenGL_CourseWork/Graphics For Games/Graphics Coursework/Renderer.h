#pragma once
#include "..\..\nclgl\OGLRenderer.h"
#include "..\..\nclgl\Camera.h"
#include "..\..\nclgl\HeightMap.h"
#include "..\..\nclgl\MD5Mesh.h"
#include "..\..\nclgl\MD5Node.h"
#include "../nclgl/Particles.h"
#include"../nclgl/Matrix4.h"
#include"time.h"
#include "..\..\nclgl\OBJMesh.h"
#include"..\..\nclgl\TextMesh.h"
#include <sstream>
# define SHADOWSIZE 2048 
# define POST_PASSES 10
#define LIGHTNUM 8

const float SKY_HEIGHT = 2000;
const int NUM_OF_PARTICLES = 2500;
const Vector3 velocity(0, -100, 0);
const Vector4 ParticleColour(1, 1, 1, 1);
const Vector3 ParticleScale(RAW_WIDTH* HEIGHTMAP_X / 500, RAW_HEIGHT* HEIGHTMAP_Z / 500, RAW_HEIGHT* HEIGHTMAP_Z / 500);
const int ParticeLength = 100000;
static int seed = time(NULL);
const int Distance = 100;

class Renderer :public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	void RenderSceneTwo();
	virtual void UpdateScene(float msec);
	void MoveLight();
	void TogglePostProssing();
	void ToggleAutoCamera();
protected:
	void DrawSkybox();
	void DrawHeightmap();
	void DrawWater();
	void DrawMesh(); // hellNight
	void DrawShadowScene(); // shadow?
	void DrawCombinedScene(); // combined?
	void DrawPostProcess();
	void DrawFloor();
	void DrawParticles();//snow
	void PresentScene();
	void DrawMiniMap();
	void DrawSceneTwo();

	void DrawGUI(float fps);
	void DrawText(const std::string& text, const Vector3& position, const float size, const bool perspective);
	void GenerateScreenTexture(GLuint& into, bool depth);
	void CombineBuffers();
	void DrawPointLights();
	void DrawEyeBall(SceneNode* n);

	Shader* lightShader;
	Shader* reflectShader;
	Shader* skyboxShader;
	Shader* sceneShader;
	Shader* shadowShader;
	Shader* processShader;
	Shader* particleShader;//snow shader
	Shader* textShader;
	Shader* hellShader;
	Shader* pointlightShader;
	Shader* combineShader;
	Shader* heightMapShader;
	Shader* eyeBallShader;
	Shader* miniMapShader;
	Shader* sceneTwoShader;

	Particles* ParticleSystem[NUM_OF_PARTICLES];//number of snow
	HeightMap* heightMap;
	Mesh* waterQuad;
	Mesh* quad;
	Mesh* postQuad;
	Mesh* floor;
	Mesh* skyboxMesh;

	Mesh* eyeBall;
	SceneNode* eyeBallNode;
	OBJMesh* eyeBallMesh;

	OBJMesh* sphere;
	MD5FileData* hellData;
	MD5Node* hellNode;
	Light* light;
	Light* lightTwo;
	Light* pointLights;
	Camera* camera;

	GLuint shadowTex;
	GLuint shadowFBO;
	GLuint cubeMap;
	GLuint bufferFBO;
	GLuint processFBO;
	GLuint miniMapFBO;
	GLuint bufferColourText[2];
	GLuint bufferColourTex;
	GLuint bufferDepthTex;
	GLuint miniMapColourTex;
	GLuint miniMapDepthTex;
	GLuint snowTex;// texture of snow
	GLuint bufferColourTex1;// colour texture of snow

	GLuint bufferNormalTex;

	GLuint pointLightFBO;
	GLuint lightEmissiveTex;
	GLuint lightSpecularTex;
	
	Matrix4 viewmatrix;

	Font* basicFont;	//A font! a basic one...

	bool usingpostProssing=false;
	bool autoCamera = true;
	float waterRotate;
	float rotation;
	float timeValue;
	int fps;
};

