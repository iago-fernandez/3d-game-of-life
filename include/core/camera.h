/**
 * @file camera.h
 * @brief Defines the orbit camera for 3D navigation, handling spherical coordinates and view matrix calculations.
 */
#pragma once

#include <glm/glm.hpp>

namespace core {

    /**
     * @brief Simple orbit camera that rotates around a specific target point.
     *
     * Handles view matrix calculation based on spherical coordinates (yaw, pitch, distance).
     */
    class OrbitCamera {
    public:
        OrbitCamera() = default;

        /**
         * @brief Calculate and retrieve the view matrix.
         * @return A 4x4 view matrix looking at the target.
         */
        glm::mat4 view() const;

        /**
         * @brief Apply mouse movement to orbit angles.
         * @param dx Mouse delta X in pixels (yaw).
         * @param dy Mouse delta Y in pixels (pitch).
         */
        void orbitBy(float dx, float dy);

        /**
         * @brief Apply scroll input to zoom distance.
         * @param scrollY Accumulated scroll delta.
         */
        void zoomBy(float scrollY);

        /**
         * @brief Get the current camera position in world space.
         * @return Cartesian coordinates of the camera.
         */
        glm::vec3 position() const;

    private:
        glm::vec3 target_{ 0.0f, 0.0f, 0.0f };

        // Spherical coordinates
        float distance_ = 15.0f;
        float yaw_ = 0.0f;
        float pitch_ = glm::radians(-20.0f);
    };

}