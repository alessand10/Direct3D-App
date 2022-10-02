#include "Camera.h"
#include "Utilities.h"

/**
 * @brief Initializes a camera object with default settings.
 * @param defFOV The FOV to initialize the camera with.
 * @param defAspectRatio The aspect ratio to initialize the camera with.
 * @param defNearZ The distance to the near clipping plane to initialize the camera with.
 * @param defFarZ The distance to the far clipping plane to initialize the camera with.
 */
Camera::Camera(float defFOV, float defAspectRatio, float defNearZ, float defFarZ)
{
    location = XMFLOAT4{ 10.f, 10.f, 10.f, 1.f };
    lookAt = XMFLOAT4{ 0.f, 0.f, 0.f, 1.f };
    upDirection = XMFLOAT4{ 0.f, 1.f, 0.f, 1.f };
    fovInRadians = defFOV;
    aspectRatio = defAspectRatio;
    nearClippingPlane = defNearZ;
    farClippingPlane = defFarZ;
}

void Camera::setFOV(float newFOV)
{
    fovInRadians = newFOV;
}

void Camera::setAspectRatio(float newAspectRatio)
{
    aspectRatio = newAspectRatio;
}

void Camera::setNearAndFarClippingPlane(float nearZ, float farZ)
{
    nearClippingPlane = nearZ;
    farClippingPlane = farZ;
}

/**
 * @brief Generates a transposed world-view-projection matrix, ready to be used in a constant buffer.
 * @param world The world matrix used to create the world-view-projection matrix.
 * @return The transposed world-view-projection matrix.
 */
XMMATRIX Camera::generateWorldViewProjMatrix(XMMATRIX world)
{
    // Compute the transpose so that the matrix is in the correct form to be used in hlsl.
    return XMMatrixTranspose(XMMatrixMultiply(XMMatrixMultiply(world, getViewMatrix()), getProjMatrix()));
}

/**
 * @brief Creates the view matrix based on camera location and direction.
 * @return The view matrix.
 */
XMMATRIX Camera::getViewMatrix()
{
    return XMMatrixLookAtLH(XMLoadFloat4(&location), XMLoadFloat4(&lookAt), XMLoadFloat4(&upDirection));
}

/**
 * @brief Creates the projection matrix based on FOV, aspect ratio, nearZ and farZ.
 * @return The projection matrix.
 */
XMMATRIX Camera::getProjMatrix()
{
    return XMMatrixPerspectiveFovLH(fovInRadians, aspectRatio, nearClippingPlane, farClippingPlane);
}

void Camera::panCamera(XMFLOAT4 translationVector)
{
    VEC_TO_F4(result1, (F4_TO_VEC(location) + F4_TO_VEC(translationVector)));
    location = result1;
    VEC_TO_F4(result2, (F4_TO_VEC(lookAt) + F4_TO_VEC(translationVector)));
    lookAt = result2;
}


float Camera::getDistanceFromTarget() {
    return XMVectorGetX(XMVector3Length(XMLoadFloat4(&location) - XMLoadFloat4(&lookAt)));
}

XMFLOAT4 Camera::getLocation() {
    return location;
}

void Camera::setLocation(float x, float y, float z) {
    location.x = x;
    location.y = y;
    location.z = z;
}

void Camera::setLocation(XMFLOAT4 loc) {
    location = loc;
    loc.w = 1.f;
}

void Camera::setLookAtTarget(XMFLOAT4 target) {
    lookAt = target;
}

XMFLOAT4 Camera::getLeftVector() {
    XMFLOAT4 forwardVec = getForwardVector();
    VEC_TO_F4(result, XMVector3Cross(F4_TO_VEC(upDirection), F4_TO_VEC(forwardVec)));
    return result;
}

XMFLOAT4 Camera::getRightVector() {
    XMFLOAT4 forwardVec = getForwardVector();
    VEC_TO_F4(result, -XMVector3Cross(F4_TO_VEC(upDirection), F4_TO_VEC(forwardVec)));
    return result;
}

XMFLOAT4 Camera::getForwardVector() {
    VEC_TO_F4(result, XMVector3Normalize(F4_TO_VEC(lookAt) - F4_TO_VEC(location)));
    return result;
}

XMFLOAT4 Camera::getUpVector() {
    XMFLOAT4 rightVec = getRightVector();
    XMFLOAT4 forwardVec = getForwardVector();
    VEC_TO_F4(result, XMVector3Normalize(XMVector3Cross(F4_TO_VEC(rightVec), F4_TO_VEC(forwardVec))));
    return result;

}

XMFLOAT4 Camera::getLookAtTarget() {
    return lookAt;
}
