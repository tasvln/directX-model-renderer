#include "camera.h"

Camera::Camera(
    float fov, 
    float aspect, 
    float nearZ, 
    float farZ
) : 
    fov(fov),
    aspect(aspect),
    nearZ(nearZ),
    farZ(farZ),
    position{ 0.0f, 0.0f, -5.0f }, 
    target{ 0.0f, 0.0f, 0.0f } 
{
    LOG_INFO(L"Initializing Camera...");
    LOG_INFO(L"Camera position set to (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
    LOG_INFO(L"Camera target set to (%.2f, %.2f, %.2f)", target.x, target.y, target.z);

    setProjection(
        fov,
        aspect,
        nearZ,
        farZ
    );
    
    updateViewMatrix();
}

void Camera::setProjection(
    float fov, 
    float aspect, 
    float nearZ, 
    float farZ
) {
    projection = XMMatrixPerspectiveFovLH(
        fov,
        aspect,
        nearZ,
        farZ
    );

    LOG_INFO(L"Projection matrix updated. FOV: %.2f, Aspect: %.2f, NearZ: %.2f, FarZ: %.2f", fov, aspect, nearZ, farZ);
}

void Camera::updateViewMatrix() {
    XMVECTOR pos = XMLoadFloat3(&position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    view = XMMatrixLookAtLH(pos, tgt, up);
}

void Camera::update(float delta) {
    updateViewMatrix();
}

void Camera::updatePositionFromOrbit() {
    // convert spherical → Cartesian (x, y, z):
    position.x = target.x + radius * cosf(pitch) * sinf(yaw);
    position.y = target.y + radius * sinf(pitch);
    position.z = target.z + radius * cosf(pitch) * cosf(yaw);

    updateViewMatrix();
}

void Camera::orbit(float deltaYaw, float deltaPitch)
{
    yaw += deltaYaw;
    pitch += deltaPitch;

    // avoid flipping
    pitch = std::clamp(pitch, -XM_PIDIV2 + 0.01f, XM_PIDIV2 - 0.01f);

    updatePositionFromOrbit();
}

// Zoom by FOV (lens zoom) -> more FPS camera orientated
// The camera stays in place, but the projection matrix FOV changes. Perspective distortion changes (things stretch when wide)
void Camera::setFov(float newFov) {
    this->fov = std::clamp(newFov, 0.1f, XM_PIDIV2);
    setProjection(
        fov,
        aspect,
        nearZ,
        farZ
    );
}

// Zoom by radius (what you have now)
// Zooming moves the camera closer or farther from the target (changes radius). This is like physically walking toward/away from the cube.
void Camera::zoom(float wheelDelta) {
    radius -= wheelDelta;
    radius = std::clamp(radius, 1.0f, 90.0f);

    updatePositionFromOrbit();
}