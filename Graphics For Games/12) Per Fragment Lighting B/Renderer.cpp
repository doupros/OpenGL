#include "Renderer.h"


Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	camera = new Camera(0.0f, 0.0f, Vector3(
		RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));

	heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
	currentShader = new Shader(SHADERDIR "BumpVertex.glsl",
		SHADERDIR "BumpFragment.glsl");

	heightMap->SetTexture(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(
		TEXTUREDIR "Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));


	if (!currentShader->LinkProgram() || !heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);

	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,
		(float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete light;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
}

void Renderer::MoveLight() {
	Vector3 position = light->GetPosition();
	float temp = light->GetRadius();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_I)) {
		position += Vector3(0, 0, -10) ;
		light->SetPosition(position);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
		position -= Vector3(0, 0, -10) ;
		light->SetPosition(position);
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_J)) {
		position += Vector3(-10, 0, 0);
		light->SetPosition(position);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L)) {
		position -= Vector3(-10, 0, 0);
		light->SetPosition(position);
	}

	if (Window::GetKeyboard()->KeyDown(KEYBOARD_O)) {
		position.y += 10;
		light->SetPosition(position);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_P)) {
		position.y -= 10;
		light->SetPosition(position);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_M)) {
		temp += 10;
		light->SetRadius(temp);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_N)) {
		temp -= 10;
		light->SetRadius(temp);
	}
	//position -= Vector3(-10, 0, 0);
	//light->SetPosition(position);
}


void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(),
		"diffuseTex"), 0);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),
		"cameraPos"), 1, (float*)&camera->GetPosition());


	UpdateShaderMatrices();
	SetShaderLight(*light);

	heightMap->Draw();

	glUseProgram(0);
	SwapBuffers();
}
