#include "Particle.h"

using namespace DirectX;

Particle::Particle(float x, float y, float z){
	_currPosition = XMFLOAT3(x, y, z);
}

Particle::Particle(){
	_currPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Particle::~Particle(){}

void Particle::setPosition(XMFLOAT3 &position){
	_currPosition = position;
}

XMFLOAT3 Particle::getPosition(){
	return _currPosition;
}

