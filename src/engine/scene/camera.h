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

    void setPosition(const XMFLOAT3& pos) { 
        position = pos; 
        updateViewMatrix();
    }

    void setTarget(const XMFLOAT3& tgt) { 
        target = tgt; 
        updateViewMatrix();
    }

    void setProjection(
        float fov, 
        float aspect, 
        float nearZ, 
        float farZ
    );

    void update(float delta);

    // Orbit
    void updatePositionFromOrbit();
    void orbit(float deltaYaw, float deltaPitch);

    // onMouseWheel
    void setFov(float newFov);
    void zoom(float wheelDelta);

    XMMATRIX getViewMatrix() const { 
        return view; 
    }

    XMMATRIX getProjectionMatrix() const { 
        return projection; 
    }

    XMMATRIX getViewProjectionMatrix() const {
        return XMMatrixMultiply(view, projection);
    }

    float getFov() const { 
        return fov; 
    }

private:
    void updateViewMatrix();

private:
    float fov;
    float aspect;
    float nearZ;
    float farZ;

    float yaw = 0.0f; // left/right angle around the Y-axis
    float pitch = 0.0f; // up/down angle
    float radius = 5.0f; // distance from target (how far the camera is)

    XMFLOAT3 position;
    XMFLOAT3 target;
    XMMATRIX projection;
    XMMATRIX view;
};
