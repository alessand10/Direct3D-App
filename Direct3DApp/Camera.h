#pragma once
#include "Common.h"

class Camera {
	XMVECTOR location;
	XMVECTOR viewDirection;
	XMVECTOR upDirection;
	float fovInRadians;
	float aspectRatio;
	float nearClippingPlane;
	float farClippingPlane;

public:
	Camera(float defFOV = XMConvertToRadians(90.f), float defAspectRatio = (1920.f/1080.f), float defNearZ = 0.1, float defFarZ = 100.f);
	void setFOV(float newFOV);
	void setAspectRatio(float newAspectRatio);
	void setNearAndFarClippingPlane(float nearZ, float farZ);
	XMMATRIX generateWorldViewProjMatrix(XMMATRIX world);
	XMMATRIX getViewMatrix();
	XMMATRIX getProjMatrix();
};
