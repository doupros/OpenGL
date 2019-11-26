#include"Renderer.h"

double GetRandNum(int min, int max) {
	srand(seed);
	seed = rand();
	return (min + rand() % (max - min + 1));
}

int CalculateFrameRate()
{
	static float framesPerSecond = 0.0f;  // This will store our fps 
	static float lastTime = 0.0f;  // This will hold the time from the last frame 
	float currentTime = GetTickCount() * 0.001f;
	int temp = 0;
	++framesPerSecond;
	if (currentTime - lastTime > 1.0f)
	{
		lastTime = currentTime;
		//if (SHOW_FPS == 1) fprintf(stderr, "\nCurrent Frames Per Second: %d\n\n", (int)framesPerSecond);
		std::cout <<"Current FPS : "<<(int)framesPerSecond << std::endl;
		temp = framesPerSecond;
		framesPerSecond = 0;
	}
	return temp;
}

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	/*---------		CAMERA					-----------------*/
#pragma region camera

	camera = new Camera();
	camera->SetPosition(Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500.0f, RAW_WIDTH * HEIGHTMAP_X));

#pragma endregion
	/*---------			Skybox					-----------------*/
#pragma region Skybox
	skyboxMesh = Mesh::GenerateQuad();
	cubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	);
/*	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE0, cubeMap);*/
	//skyboxMesh->SetTexture(cubeMap);
	SetTextureRepeating(skyboxMesh->GetTexture(), true);
	skyboxShader = new Shader(SHADERDIR"skyboxVertex.glsl", SHADERDIR"skyboxFragment.glsl");
	if (!skyboxShader->LinkProgram())
	{
		return;
	}

#pragma endregion

	/*---------		HEIGHTMAP				-----------------*/
#pragma region HeightMap

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	//heightMapShader = new Shader(SHADERDIR"heightMapVertex.glsl", SHADERDIR"heightMapFrag.glsl");
	heightMapShader = new Shader(SHADERDIR"heightMapVertex_1.glsl", SHADERDIR"heightMapFrag_1.glsl");
	if (!heightMapShader->LinkProgram()) {
		return;
	}

	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!heightMap->GetTexture() || !heightMap->GetBumpMap()) {
		return;
	}
	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

#pragma endregion

	/*---------			WATER					-----------------*/
#pragma region Water

	waterQuad = Mesh::GenerateQuad();
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"reflectFragment.glsl");
	if (!reflectShader->LinkProgram())
	{
		return;
	}
	waterQuad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water.TGA",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!waterQuad->GetTexture()) {
		return;
	}
	SetTextureRepeating(waterQuad->GetTexture(), true);

	waterRotate = 0.0f;

#pragma endregion

	/*---------			Floor						-----------------*/
#pragma region Floor

	floor = Mesh::GenerateQuad();
	floor->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	floor->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

#pragma endregion

	/*---------			Hell						-----------------*/
#pragma region Hell

	hellData = new MD5FileData(MESHDIR"hellknight.md5mesh");
	hellNode = new MD5Node(*hellData);
	hellData->AddAnim(MESHDIR"idle2.md5anim");
	hellNode->PlayAnim(MESHDIR"idle2.md5anim");
	hellShader = new Shader(SHADERDIR"hellShaderVertex.glsl", SHADERDIR"hellShaderFrag 1.glsl");
	if (!hellShader->LinkProgram()) {
		return;
	}

#pragma endregion

	/*---------			Light						-----------------*/
#pragma region Light

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f),
		500.0f,
		(RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)),
		Vector4(0.9f, 0.9f, 1.0f, 1),
		(RAW_WIDTH * HEIGHTMAP_X) / 2.0f);

	lightTwo = new Light(Vector3(RAW_HEIGHT * HEIGHTMAP_X/3.0f, 700.0f, RAW_HEIGHT * HEIGHTMAP_Z/3.0f), Vector4(1, 0, 0, 1), 1500);

#pragma endregion	

	/*---------			Particle					-----------------*/
#pragma region Particle

	int numInEachRow = sqrt(NUM_OF_PARTICLES);
	for (int i = 0; i < numInEachRow; ++i) {
		for (int j = 0; j < numInEachRow; ++j) {
			double tmp_length = GetRandNum(0, 10);//vertical range
			Vector3 tmp_velocity(0, -GetRandNum(100, 600), 0);//gravity
			double unitWidth = double(HEIGHTMAP_X * RAW_WIDTH) / double(numInEachRow);
			int offset_x = unitWidth * i;
			int offset_z = unitWidth * j;
			ParticleSystem[i * numInEachRow + j] = new Particles(Vector3(offset_x, SKY_HEIGHT, offset_z), tmp_velocity, ParticleColour,
				tmp_length * 1000, ParticleScale);
		}
	}
	particleShader = new Shader(SHADERDIR"particleVertex.glsl", SHADERDIR"particleFragment.glsl");//partical shader
	if (!particleShader->LinkProgram())
	{
		return;
	}
	snowTex = SOIL_load_OGL_texture(TEXTUREDIR"SnowFlower2.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);//snow texture
	if (!snowTex) {
		return;
	}
#pragma endregion


	/*---------			PointLights					-----------------*/
#pragma region PointLights

	pointLights = new Light[LIGHTNUM * LIGHTNUM];
	for (int x = 0; x < LIGHTNUM; x++)
	{
		for (int z = 0; z < LIGHTNUM; z++)
		{
			Light& l = pointLights[(x * LIGHTNUM) + z];

			float xPos = (RAW_WIDTH * HEIGHTMAP_X / (LIGHTNUM - 1)) * x;
			float zPos = (RAW_HEIGHT * HEIGHTMAP_Z / (LIGHTNUM - 1)) * z;
			l.SetPosition(Vector3(xPos, 100.0f, zPos));

			float r = 0.5f + (float)(rand() % 129) / 128.0f;
			float g = 0.5f + (float)(rand() % 129) / 128.0f;
			float b = 0.5f + (float)(rand() % 129) / 128.0f;
			l.SetColour(Vector4(r, g, b, 1.0f));

			float radius = (RAW_WIDTH * HEIGHTMAP_X / LIGHTNUM);
			l.SetRadius(radius);
		}
	}

	sphere = new OBJMesh();
	if (!sphere->LoadOBJMesh(MESHDIR"ico.obj"))
	{
		return;
	}
	pointlightShader = new Shader(SHADERDIR"pointlightvertex.glsl", SHADERDIR"pointlightfragment.glsl");
	if (!pointlightShader->LinkProgram()) {
		return;
	}

#pragma endregion

	/*---------			Font					-----------------*/
#pragma region Font

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);
	if (!basicFont) {
		return;
	}
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	if (!textShader->LinkProgram())
	{
		return;
	}

#pragma endregion


	/*---------			UFO					-----------------*/
#pragma region UFO Ball

	OBJMesh* tempMesh = new OBJMesh;
	tempMesh->LoadOBJMesh(MESHDIR"ico.obj");
	eyeBall = tempMesh;

	//eyeBallNode = new SceneNode;
	SceneNode* tempNode = new SceneNode();
	tempNode->SetMesh(eyeBall);
	//eyeBallNode->AddChild(tempNode);

	eyeBallNode = new SceneNode(eyeBall, Vector4(1, 0, 0, 1)); //Red! 
	eyeBallNode->SetModelScale(Vector3(100, 100,100));
	eyeBallNode->SetTransform(Matrix4::Translation(Vector3(0, 150, 0)));
	eyeBallNode->AddChild(tempNode);

	SceneNode* head = new SceneNode(eyeBall, Vector4(0, 0, 1, 1)); //Green! 
	head->SetModelScale(Vector3(150, 30, 150));
	head->SetTransform(Matrix4::Translation(Vector3(0, 30, 0)));
//	head->SetCameraDistance(50);
	eyeBallNode->AddChild(head);

	//SceneNode* leftArm = new SceneNode(eyeBall, Vector4(0, 0, 1, 1)); //Blue! 
	//leftArm->SetModelScale(Vector3(3, -18, 3));
	//leftArm->SetTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	//eyeBallNode->AddChild(leftArm);

	//SceneNode* rightArm = new SceneNode(eyeBall, Vector4(0, 0, 1, 1)); //Blue! 
	//rightArm->SetModelScale(Vector3(3, -18, 3));
	//rightArm->SetTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	//eyeBallNode->AddChild(rightArm);
	

//	eyeBallNode->SetModelScale(Vector3(600, 600, 600));

	eyeBallShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	if (!eyeBallShader->LinkProgram()) { return; }

#pragma endregion

	//camera->SetPosition(Vector3(0, 30, 175));
	/*-------------------------------------------	SHADERS	------------------------------------------------------*/
#pragma region SHADERS

	sceneShader = new Shader(SHADERDIR"shadowscenevert.glsl", SHADERDIR"shadowscenefrag.glsl");
	if (!sceneShader->LinkProgram())
	{
		return;
	}
	shadowShader = new Shader(SHADERDIR"shadowVert.glsl", SHADERDIR"shadowFrag.glsl");
	if (!shadowShader->LinkProgram())
	{
		return;
	}
	sceneTwoShader = new Shader(SHADERDIR"TexturedVertex _T10.glsl", SHADERDIR"TexturedFragment _T10.glsl");
	if (!sceneTwoShader->LinkProgram())
	{
		return;
	}

	processShader = new Shader(SHADERDIR"TexturedVertex _T10.glsl", SHADERDIR"processfrag.glsl");
	if (!processShader->LinkProgram())
	{
		return;
	}

	combineShader = new Shader(SHADERDIR"combinevert.glsl", SHADERDIR"combinefrag.glsl");
	if (!combineShader->LinkProgram()) {
		return;
	}

	lightShader = new Shader(SHADERDIR "BumpVertex.glsl", SHADERDIR "BumpFragment.glsl");
	if (!lightShader->LinkProgram())
	{
		return;
	}
	miniMapShader = new Shader(SHADERDIR "CourseworkMiniMapVertex.glsl", SHADERDIR "CourseworkMiniMapFragment.glsl");
	if (!miniMapShader->LinkProgram())
	{
		return;
	}


	/*sceneShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"bufferFragment.glsl");
	if (!sceneShader->LinkProgram()) {
		return;
	}*/


#pragma endregion

	/*-------------------------------------------		FBO		------------------------------------------------------*/
#pragma region FBO

#pragma region GenTexture
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &bufferColourTex);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	glGenTextures(1, &miniMapDepthTex);
	glBindTexture(GL_TEXTURE_2D, miniMapDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height,0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &miniMapColourTex);
	glBindTexture(GL_TEXTURE_2D, miniMapColourTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &bufferColourText[i]);
		glBindTexture(GL_TEXTURE_2D, bufferColourText[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	//GenerateScreenTexture(bufferColourTex, false);
	//GenerateScreenTexture(bufferDepthTex, true);
	GenerateScreenTexture(bufferNormalTex, false);
	GenerateScreenTexture(lightEmissiveTex, false);
	GenerateScreenTexture(lightSpecularTex, false);
	glBindTexture(GL_TEXTURE_2D, 0);
#pragma endregion

	glGenFramebuffers(1, &processFBO);

	GLenum buffers[2];
	buffers[0] = GL_COLOR_ATTACHMENT0;
	buffers[1] = GL_COLOR_ATTACHMENT1;


	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !shadowTex)
	{
		return;
	}	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(1, &bufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourText[0], 0);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	//glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferDepthTex || !bufferColourText[0]) {
		return;
	}			
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glGenFramebuffers(1, &miniMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, miniMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, miniMapDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,GL_TEXTURE_2D, miniMapDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, miniMapColourTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !miniMapDepthTex || !miniMapColourTex) { 
		return; 
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glGenFramebuffers(1, &pointLightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightEmissiveTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !lightEmissiveTex || !lightSpecularTex)
	{
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion

	quad = Mesh::GenerateQuad();
	postQuad = Mesh::GenerateQuad();
	rotation = 0.0f;
	init = true;
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	 
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete waterQuad;
	delete quad;
	delete postQuad;
	delete light;
	delete lightTwo;
	delete hellData;
	delete hellNode;
	//delete sphere;
	//delete[] pointLights;
	delete skyboxMesh;

	currentShader = 0;
	delete lightShader;
	delete hellShader;
	delete reflectShader;
	delete skyboxShader;
	delete sceneShader;
	delete shadowShader;
	delete processShader;
	delete particleShader;//delete partical shader
	delete combineShader;
	//delete pointlightShader;
	delete heightMapShader;

	glDeleteTextures(1, &bufferColourTex);
	glDeleteTextures(1, &bufferNormalTex);
	glDeleteTextures(1, &bufferDepthTex);
	glDeleteTextures(1, &lightEmissiveTex);
	glDeleteTextures(1, &lightSpecularTex);
	glDeleteTextures(1, &miniMapColourTex);
	glDeleteTextures(1, &miniMapDepthTex);

	glDeleteFramebuffers(1, &miniMapFBO);
	glDeleteFramebuffers(1, &shadowFBO);
	glDeleteFramebuffers(1, &bufferFBO);
	glDeleteFramebuffers(1, &processFBO);
	glDeleteFramebuffers(1, &pointLightFBO);

}

void Renderer::DrawGUI(float fps)
{
	string temp1, temp = "FPS=";
	stringstream s;
	s << fps;
	s >> temp1;
	temp1.assign(temp1, 0, 5);
	temp += temp1;
	//SetCurrentShader(textShader);
	glUseProgram(textShader->GetProgram());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	DrawText(temp, Vector3(0, 0, 0), 40.0f, 0);
	//DrawText("This is perspective text!!!!", Vector3(0, 0, -1000), 64.0f, true);
	glUseProgram(0);
}

void Renderer::DrawText(const std::string& text, const Vector3& position, const float size, const bool perspective) {
	//glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	UpdateShaderMatrices();
	mesh->Draw();
	modelMatrix.ToIdentity();

	glEnable(GL_DEPTH_TEST);
	delete mesh;
}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth = false) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8, width, height, 0, depth ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
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

void Renderer::ToggleAutoCamera() {
	autoCamera = !autoCamera;
}

void Renderer::TogglePostProssing() {
	usingpostProssing = !usingpostProssing;
}

void Renderer::UpdateScene(float msec) {
	timeValue = msec;
	timeValue += 0.005f;
	if (!autoCamera) {
		camera->UpdateCamera(msec);
	}
	else {
		float tempZ = camera->GetPosition().z - 0.3f;
		if (camera->GetYaw() <= 28) {
			camera->SetYaw(camera->GetYaw() + 0.05f);
		}
		//	std::cout << camera->GetYaw()<<std:: endl;
		if (camera->GetPitch() >= -28) {
			camera->SetPitch(camera->GetPitch() - 0.05f);
		}
		//	std::cout << camera->GetPitch() << std::endl;
		camera->SetPosition(Vector3(camera->GetPosition().x + 0.3f, camera->GetPosition().y + 0.3f, tempZ));
	}
	viewMatrix = camera->BuildViewMatrix();
	light->SetPosition(Vector3(light->GetPosition().x, light->GetPosition().y+0.1, light->GetPosition().z));
	hellNode->Update(msec);
	waterRotate += msec / 1000.0f;
	rotation = msec * 0.01f;
	for (int i = 0; i < NUM_OF_PARTICLES; ++i) {// update snow
		ParticleSystem[i]->Update(msec);
	}

	//double lastTime = msec;
	//int nbFrames = 0;
	//// Measure speed
	//double currentTime = msec;
	//nbFrames++;
	//if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
	//	// printf and reset timer
	////	printf("%f ms/frame\n", 1000.0 / double(nbFrames));
	//	std::cout << 1000.0 / (double)nbFrames << std::endl;
	//	nbFrames = 0;
	//	lastTime += 1.0;
	//}
	fps = CalculateFrameRate();
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	DrawSkybox();
	DrawEyeBall(eyeBallNode);
	DrawShadowScene();
	DrawCombinedScene();
	DrawHeightmap();
	DrawWater();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DrawMiniMap();
	DrawParticles();
	//DrawGUI(fps);
	SwapBuffers();
}

void Renderer::RenderSceneTwo() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	DrawSceneTwo();
	//DrawHeightmap();
	//DrawWater();

	if (usingpostProssing){
		DrawPostProcess();
	}
	PresentScene();
	SwapBuffers();
}

void Renderer::DrawSkybox() {
	glDepthMask(GL_FALSE);
	glUseProgram(skyboxShader->GetProgram());
	SetCurrentShader(skyboxShader);
	UpdateShaderMatrices();
	skyboxMesh->Draw();
	//waterQuad->Draw();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
}

void Renderer::DrawHeightmap() {
//	glUseProgram(lightShader->GetProgram());
	SetCurrentShader(sceneShader);
	SetShaderLight(*light);

	//glUniform1f(glGetUniformLocation(currentShader->GetProgram(),"time"),timeValue);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	//glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);

	modelMatrix =
		Matrix4::Translation(Vector3(0, 0, 0)) *
		Matrix4::Scale(Vector3(1, 1, 1)) *
		Matrix4::Rotation(0, Vector3(0.0f, 0.0f, 0.0f));
	UpdateShaderMatrices();
	heightMap->Draw();
	modelMatrix.ToIdentity();

	glUseProgram(0);
}

void Renderer::DrawEyeBall(SceneNode* n) {
	//glUseProgram(lightShader->GetProgram());
	SetCurrentShader(eyeBallShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	modelMatrix= 
		Matrix4::Translation(Vector3(1500, 320, 1300)) *
		Matrix4::Scale(Vector3(1, 1, 1)) *
		Matrix4::Rotation(90, Vector3(.0f, 1.0f, 0.0f));
	UpdateShaderMatrices();

	if (n->GetMesh()) {
		Matrix4 transform = n->GetWorldTransform() *Matrix4::Scale(n->GetModelScale());
		glUniformMatrix4fv(
			glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float*)&transform);

		glUniform4fv(
			glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float*)&n->GetColour());

		glUniform1i(
			glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());
		n->Draw(*this);
	}
	for (vector <SceneNode*>::const_iterator i = n->GetChildIteratorStart();i != n->GetChildIteratorEnd(); ++i) {
		DrawEyeBall(*i);
	}
	modelMatrix.ToIdentity();
	glUseProgram(0);
}

void Renderer::DrawMesh() {
	SetCurrentShader(hellShader);
	SetShaderLight(*light);
	SetShaderLight(*lightTwo);

	modelMatrix =
		Matrix4::Translation(Vector3(1100, 310, 1300)) *
		Matrix4::Scale(Vector3(2, 2, 2)) *
		Matrix4::Rotation(90, Vector3(.0f, 1.0f, 0.0f));
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);

	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	hellNode->Draw(*this);
	modelMatrix.ToIdentity();	
	//textureMatrix.ToIdentity();
	//viewMatrix.ToIdentity();	

	glUseProgram(0);
}

//
//void Renderer::PresentScene() {
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//
//	SetCurrentShader(sceneShader);
//	UpdateShaderMatrices();
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, bufferColourTex);
//
//	glActiveTexture(GL_TEXTURE3);
//	glBindTexture(GL_TEXTURE_2D, lightEmissiveTex);
//
//	glActiveTexture(GL_TEXTURE4);
//	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);
//	quad->Draw();
//	glUseProgram(0);
//}

void Renderer::PresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(sceneTwoShader);
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	postQuad->SetTexture(bufferColourText[0]);
	postQuad->Draw();
	glUseProgram(0);
}

void Renderer::DrawParticles()//draw snow
{
	//glDisable(GL_DEPTH_TEST);
	SetCurrentShader(particleShader);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f,(float)width / (float)height, 45.0f);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "snowTex"), 0);
	for (int i = 0; i < NUM_OF_PARTICLES; ++i) {
		modelMatrix = Matrix4::Translation(ParticleSystem[i]->GetPosition()) * Matrix4::Scale(ParticleSystem[i]->GetScale());
		modelMatrix.SetAsUp3Transpose(viewMatrix);
		UpdateShaderMatrices();
		ParticleSystem[i]->shape->SetTexture(snowTex);
		ParticleSystem[i]->DrawP();
	}
	projMatrix.ToIdentity();
	modelMatrix.ToIdentity();
	//textureMatrix.ToIdentity();
	//glDisable(GL_BLEND);
	glUseProgram(0);
}

void Renderer::DrawWater() {
	SetCurrentShader(reflectShader);
	//SetShaderLight(*lightTwo);
	SetShaderLight(*light);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);

	float heightY = 256 * HEIGHTMAP_Y / 3.0f;

	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);

	modelMatrix =
		Matrix4::Translation(Vector3(heightX, heightY, heightZ)) *
		Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
		Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	UpdateShaderMatrices();
	waterQuad->Draw();
	modelMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	glUseProgram(0);
}

void Renderer::DrawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	SetCurrentShader(shadowShader);
	//SetShaderLight(*light);
	SetShaderLight(*lightTwo);
	viewMatrix = Matrix4::BuildViewMatrix(light->GetPosition(), Vector3(0, 0, 0));
	textureMatrix = biasMatrix * (projMatrix * viewMatrix);
	UpdateShaderMatrices();
	//DrawHeightmap();
	DrawFloor();
	DrawMesh();
	glUseProgram(0);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawCombinedScene() {
	SetCurrentShader(sceneShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "shadowTex"), 2);
	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	//SetShaderLight(*light);
	SetShaderLight(*lightTwo);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	viewMatrix = camera->BuildViewMatrix();
	UpdateShaderMatrices();
	DrawFloor();
	DrawMesh();
	glUseProgram(0);
}

void Renderer::DrawFloor() {
	//	modelMatrix = Matrix4::Rotation(90, Vector3(1, 0, 0)) *Matrix4::Scale(Vector3(450, 450, 1));
	modelMatrix =
		Matrix4::Translation(Vector3(1100, 310, 1300)) *
		Matrix4::Scale(Vector3(800, 5, 800)) *
		Matrix4::Rotation(90, Vector3(1.0f, 0, 0.0f));
	Matrix4 tempMatrix = textureMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "textureMatrix"), 1, false, *&tempMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, *&modelMatrix.values);
	floor->Draw();
}

void Renderer::DrawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourText[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	SetCurrentShader(processShader);
	/*viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	modelMatrix.ToIdentity();*/
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	viewMatrix.ToIdentity();
	UpdateShaderMatrices();
	glDisable(GL_DEPTH_TEST);
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);
	for (int i = 0; i < POST_PASSES; ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourText[1], 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 0);
		postQuad->SetTexture(bufferColourText[0]);
		postQuad->Draw();
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourText[0], 0);
		postQuad->SetTexture(bufferColourText[1]);
		postQuad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawPointLights() {
	SetCurrentShader(pointlightShader);
	glBindFramebuffer(GL_FRAMEBUFFER, pointLightFBO);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "depthTex"), 3);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "normTex"), 4);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Vector3 translate = Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f));

	Matrix4 pushMatrix = Matrix4::Translation(translate);
	Matrix4 popMatrix = Matrix4::Translation(-translate);

	for (int x = 0; x < LIGHTNUM; ++x)
	{
		for (int z = 0; z < LIGHTNUM; ++z)
		{
			Light& l = pointLights[(x * LIGHTNUM) + z];
			float radius = l.GetRadius();

			modelMatrix = pushMatrix * Matrix4::Rotation(rotation, Vector3(0, 1, 0)) * popMatrix * Matrix4::Translation(l.GetPosition()) * Matrix4::Scale(Vector3(radius, radius, radius));

			l.SetPosition(modelMatrix.GetPositionVector());
			SetShaderLight(l);
			UpdateShaderMatrices();

			float dist = (l.GetPosition() - camera->GetPosition()).Length();

			if (dist < radius)// camera is inside the light volume !
			{
				glCullFace(GL_FRONT);
			}
			else
			{
				glCullFace(GL_BACK);
			}

			sphere->Draw();
		}
	}

	glCullFace(GL_BACK);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.2f, 0.2f, 0.2f, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}

void Renderer::DrawMiniMap() {
//	glUseProgram(lightShader->GetProgram());
	viewMatrix = camera->BuildViewMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER, miniMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	DrawSkybox();
	DrawHeightmap();
	//DrawEyeBall(eyeBallNode);
	DrawWater();
	//DrawShadowScene();
	//DrawCombinedScene();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	SetCurrentShader(miniMapShader);
	viewMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	projMatrix = Matrix4::Orthographic(-1, 1, 1, -1, -1, 1);
	modelMatrix = Matrix4::Scale(Vector3(0.3, 0.3, 0.3));
	UpdateShaderMatrices();
	quad->SetTexture(miniMapColourTex);
	quad->Draw();

	modelMatrix.ToIdentity();
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
}

void Renderer::DrawSceneTwo(){
	glBindFramebuffer(GL_FRAMEBUFFER, bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);
	glDepthMask(GL_FALSE);
	SetCurrentShader(skyboxShader);
	skyboxMesh->Draw();
	glDepthMask(GL_TRUE);
	SetCurrentShader(sceneShader);
	heightMap->Draw();
	SetCurrentShader(reflectShader);
	waterQuad->Draw();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DrawSomething() {
	//bind program
	//bind shader

	//update light 1 
	//update light ...

	//set uniform						" glUniform(				)"
	//set texture						"glUniform1i(				,n)"
	//										"glActiveTexture(		n	)"
	//										"glBindTexture(				)"

	//set Matrix	one by one
	//UpdateShaderMatrices();
	//draw();

	//reset Matrix						" * projMatrix     * viewMatrix"

	//unbind shader
	//unbind program

}