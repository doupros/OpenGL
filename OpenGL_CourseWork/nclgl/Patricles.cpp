#include "Particles.h"

Particles::Particles()
{

}

Particles::Particles(Vector3 position, Vector3 velocity, Vector4 colour, float lifeLength,  Vector3 scale){
	shape = Mesh::GenerateParticleQuad();
	//shape->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"SnowFlower.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	this->originPosition = position;
	this->position = position;
	this->velocity = velocity;
	this->colour = colour;
	this->lifeLength = lifeLength;
	//this->rotation = rotation;
	this->scale = scale;
	elapsedTime = 0;
}

void Particles::Update(float msec)
{
	position += velocity * msec / 1000;
	elapsedTime += msec;
	if (elapsedTime > lifeLength) {
		elapsedTime = 0;
		position = originPosition;
	};
}

void Particles::DrawP()
{
	shape->Draw();
}

