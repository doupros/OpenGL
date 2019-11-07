#pragma once
#include "../../nclgl/OGLRenderer.h"
#include "Camera.h"

class Renderer:public OGLRenderer
{
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();

	void ToggleObject();
	void ToggleDepth();
	void ToggleAlphaBlend();
	void ToggleBlendMode();
	void MoveObject(float by);
	void UpdateScene(float msec);

protected:
	GLuint textures[2];
	Mesh* meshes[2];
	Vector3 positions[2];

	Matrix4 textureMatrix;
	Matrix4 modelMatrix;
	Matrix4 projMatrix;
	Matrix4 viewMatrix;

	bool modifyObject;
	bool usingDepth;
	bool usingAlpha;
	int blendMode;
	Camera* camera;
};

