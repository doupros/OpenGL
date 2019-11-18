#include "Renderer.h"
Renderer::Renderer(Window& parent) :OGLRenderer(parent) {
	MidLakePavilion::CreatePivilion();
	camera = new Camera();
	light = new Light(Vector3(1,1,1),Vector4(1,1,1,1),500.0f);
	quad = Mesh::GenerateQuad();

	//currentShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	currentShader = new Shader(SHADERDIR "PerPixelVertex.glsl", SHADERDIR "PerPixelFragment.glsl");
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");

	if (!currentShader->LinkProgram()|| !reflectShader->LinkProgram() ||	!skyboxShader->LinkProgram())
	{
		return;
	}
	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !quad->GetTexture()) {
		return;
	}

	SetTextureRepeating(quad->GetTexture(), true);
	
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	camera->SetPosition(Vector3(0, 30, 175));

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	waterRotate = 0.0f;
	init = true;
}

Renderer::~Renderer(void) {
	delete camera;
	//delete root;
	delete light;
	MidLakePavilion::DeletePavilion();
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	waterRotate += msec / 1000.0f;
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(currentShader->GetProgram());
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(),"cameraPos"), 1, (float*)&camera->GetPosition());
	SetShaderLight(*light);
	//DrawOBJMesh();
	DrawSkybox();
	DrawWater();
	glUseProgram(0);
	SwapBuffers();
}

void Renderer::DrawOBJMesh(OBJMesh* n) {

}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (2500.0f);

	float heightY = 1.0f;

	float heightZ = 1250.0f;

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));

	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));


	UpdateShaderMatrices();

	quad->Draw();

	glUseProgram(0);
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);

	UpdateShaderMatrices();
	quad->Draw();

	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::MoveLight() {
	Vector3 position = light->GetPosition();
	float temp = light->GetRadius();
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_I)) {
		position += Vector3(0, 0, -10);
		light->SetPosition(position);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
		position -= Vector3(0, 0, -10);
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
