#pragma once
#include "../../nclgl/OGLRenderer.h" 
#include "Camera.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);
	virtual void RenderScene();
	void SwitchToPerspective();
	void SwitchToOrthographic();
	inline void SetScale(float s) { scale = s; }
	inline void SetRotation(float r) { rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; } 
	virtual void UpdateScene(float msec);
protected:
	Mesh* triangle;
	Mesh* Quad;
	float scale;
	float rotation;
	Vector3 position;
	Camera* camera;
};

