#pragma once

#include "utils/pch.h"

class Camera {
public:
    Camera(
        float fov, 
        float aspect, 
        float nearZ, 
        float farZ
    );
    ~Camera() = default;

    void update(float delta);

    // Orbit
    void orbit(float deltaYaw, float deltaPitch);

    // onMouseWheel -> zoom controlas
    void setFov(float newFov);
    void zoom(float wheelDelta);

    // Pan
    void pan(float deltaX, float deltaY);

    void frameModel(const XMFLOAT3& center, float radius);

    void setProjection(
        float fov, 
        float aspect, 
        float nearZ, 
        float farZ
    );

    XMMATRIX getViewMatrix() const { 
        return view; 
    }

    XMMATRIX getProjectionMatrix() const { 
        return projection; 
    }

    XMMATRIX getViewProjectionMatrix() const {
        return XMMatrixMultiply(view, projection);
    }

    XMFLOAT3 getPosition() const {
        return position;
    }

    float getFov() const { 
        return fov; 
    }

    float getRadius() const { 
        return radius; 
    }

private:
    void updateViewMatrix();
    void updatePositionFromOrbit();

private:
    float fov;
    float aspect;
    float nearZ;
    float farZ;

    float yaw = 0.0f; // left/right angle around the Y-axis
    float pitch = 0.0f; // up/down angle
    
    float radius = 20.0f; // distance from target (how far the camera is)
    float targetRadius = 20.0f;

    float minRadius = 1.0f;
    float maxRadius = 100.0f;

    XMFLOAT3 position;
    XMFLOAT3 target;
    
    XMMATRIX projection;
    XMMATRIX view;
};
