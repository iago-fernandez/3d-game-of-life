/**
 * @file camera.cpp
 * @brief Implements the orbit camera logic, including input application and Cartesian conversion.
 */
#include "../../include/core/camera.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace core {

    glm::mat4 OrbitCamera::view() const {
        return glm::lookAt(position(), target_, { 0.0f, 1.0f, 0.0f });
    }

    glm::vec3 OrbitCamera::position() const {
        // Convert spherical coordinates to Cartesian direction
        // Note: We subtract from target because we want the position *away* from the center
        glm::vec3 offset;
        offset.x = std::cos(pitch_) * std::cos(yaw_);
        offset.y = std::sin(pitch_);
        offset.z = std::cos(pitch_) * std::sin(yaw_);

        return target_ - (offset * distance_);
    }

    void OrbitCamera::orbitBy(float dx, float dy) {
        constexpr float rotationSpeed = 0.005f;

        yaw_ += dx * rotationSpeed;
        pitch_ -= dy * rotationSpeed;

        // Clamp pitch to avoid gimbal lock at the poles (90 degrees)
        constexpr float pitchLimit = glm::radians(89.0f);
        pitch_ = std::clamp(pitch_, -pitchLimit, pitchLimit);
    }

    void OrbitCamera::zoomBy(float scrollY) {
        // Logarithmic zoom for smooth feeling at any distance
        constexpr float zoomSensitivity = 0.15f;
        distance_ *= std::exp(-scrollY * zoomSensitivity);

        // Prevent zooming through the target or too far away
        distance_ = std::clamp(distance_, 1.0f, 100.0f);
    }

}