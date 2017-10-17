#ifndef _CAMERACLASS_
#define _CAMERACLASS_

#include <Windows.h>
#include <DirectXMath.h>

class CameraClass{
private:
	DirectX::XMFLOAT4X4	_viewMatrix;
	DirectX::XMFLOAT3	_lookTo, _up, _position;
	float		_pitch, _yaw, _roll;
	float		_amountX, _amountY, _amountZ, _amountPitch, _amountYaw, _amountRoll;

public:
	CameraClass();
	virtual ~CameraClass();

	DirectX::XMMATRIX	getViewMatrix();
	void		setPosition(float x, float y, float z);
	void		setRotation(float roll, float pitch, float yaw);
	
	void		moveX(float amount);
	void		moveY(float amount);
	void		moveZ(float amount);

	void		pitch(float amount);
	void		yaw(float amount);
	void		roll(float amount);

	void		renderFreeLookCamera();
	void		resetCamera();

	DirectX::XMFLOAT3	getPosition();
	DirectX::XMFLOAT3	getRotation();
	DirectX::XMFLOAT3	getUpVector();
	DirectX::XMFLOAT3	getLookToVector();

};

#endif