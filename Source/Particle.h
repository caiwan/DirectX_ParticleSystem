#ifndef _PARTICLE_
#define _PARTICLE_

#include <Windows.h>
#include <DirectXMath.h>

struct Particle{
	DirectX::XMFLOAT3	_currPosition;
	DirectX::XMFLOAT3	_prevPosition;

	Particle();
	Particle(float x, float y, float z);
	~Particle();

	void setPosition(DirectX::XMFLOAT3 &position);
	DirectX::XMFLOAT3 getPosition();
};


#endif