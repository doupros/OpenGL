#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "common.h"
#include "GameTimer.h"
#include "Mesh.h"
#include "HeightMap.h"
#include <Cmath>


class Particles
{
protected:
	Vector3 originPosition;
	Vector3 position;
	Vector3 velocity;
	Vector4 colour;
	float lifeLength;
	//float rotation;
	Vector3 scale;
	float elapsedTime; //The amount of time this particle has been exist



	void SetPosition(Vector3 position) { this->position = position; }
	void SetVelocity(Vector3 velocity) { this->velocity = velocity; }
	void SetColour(Vector4 colour) { this->colour = colour; }
	void SetLifeLength(float lifeLength) { this->lifeLength = lifeLength; }
	void SetScale(Vector3 scale) { this->scale = scale; }
	void SetOriginPosition(Vector3 position) { this->originPosition = position; }
	//float GetRotation() { return rotation; }


public:
	Mesh* shape; //shape of the particles
	Particles();
	Particles(Vector3 position, Vector3 velocity, Vector4 colour, float lifeLength, Vector3 scale);
	~Particles(){}
	void Update(float msec);
	void DrawP();
	Vector3 GetScale() { return scale; }
	Vector3 GetPosition() { return position; }
	Vector3 GetDirection() { return velocity; }
	Vector4 GetColour() { return colour; }
};
