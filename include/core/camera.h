#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace core {

    /**
     * @brief Simple orbit camera around a target point.
     */
    class OrbitCamera {
    public:
        glm::vec3 target_{0.0f, 0.0f, 0.0f};
        float distance_ = 15.0f;
        float yaw_ = 0.0f;
        float pitch_ = glm::radians(-20.0f);

        /**
         * @brief Build the view matrix.
         * @return 4x4 view matrix.
         */
        glm::mat4 view() const;

        /**
         * @brief Apply mouse drag deltas in pixels to orbit angles.
         * @param dx Mouse delta X in pixels.
         * @param dy Mouse delta Y in pixels.
         */
        void orbitBy(float dx, float dy);

        /**
         * @brief Apply scroll delta to zoom in/out.
         * @param scrollY Accumulated scroll amount.
         */
        void zoomBy(float scrollY);
    };

}
