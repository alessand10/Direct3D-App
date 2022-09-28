#include "Camera.h"


/**
 * @brief Initializes a camera object with default settings.
 * @param defFOV The FOV to initialize the camera with.
 * @param defAspectRatio The aspect ratio to initialize the camera with.
 * @param defNearZ The distance to the near clipping plane to initialize the camera with.
 * @param defFarZ The distance to the far clipping plane to initialize the camera with.
 */
Camera::Camera(float defFOV, float defAspectRatio, float defNearZ, float defFarZ)
{
    location = XMVECTOR{ 10.f, 10.f, 10.f, 1.f };
    viewDirection = XMVECTOR{ 0.f, 0.f, 0.f, 1.f };
    upDirection = XMVECTOR{ 0.f, 1.f, 0.f, 1.f };
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
    return XMMatrixLookAtLH(location, viewDirection, upDirection);
}

/**
 * @brief Creates the projection matrix based on FOV, aspect ratio, nearZ and farZ.
 * @return The projection matrix.
 */
XMMATRIX Camera::getProjMatrix()
{
    return XMMatrixPerspectiveFovLH(fovInRadians, aspectRatio, nearClippingPlane, farClippingPlane);
}
