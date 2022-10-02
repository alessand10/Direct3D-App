#pragma once
#include "Common.h"

class Camera {
	XMFLOAT4 location;
	XMFLOAT4 lookAt;
	XMFLOAT4 upDirection;
	float fovInRadians;
	float aspectRatio;
	float nearClippingPlane;
	float farClippingPlane;

public:
	Camera(float defFOV = XMConvertToRadians(45.f), float defAspectRatio = (1920.f/1080.f), float defNearZ = 0.1, float defFarZ = 100.f);
	void setFOV(float newFOV);
	void setAspectRatio(float newAspectRatio);
	void setNearAndFarClippingPlane(float nearZ, float farZ);
	XMMATRIX generateWorldViewProjMatrix(XMMATRIX world);
	XMMATRIX getViewMatrix();
	XMMATRIX getProjMatrix();
	void panCamera(XMFLOAT4 translationVector);
	float getDistanceFromTarget();
	XMFLOAT4 getLocation();
	void setLocation(float x, float y, float z);
	void setLocation(XMFLOAT4 loc);
	void setLookAtTarget(XMFLOAT4 target);
	XMFLOAT4 getLeftVector();
	XMFLOAT4 getRightVector();
	XMFLOAT4 getUpVector();
	XMFLOAT4 getForwardVector();
	XMFLOAT4 getLookAtTarget();
};
