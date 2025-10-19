#include "../../include/core/camera.h"

#include <algorithm>
#include <cmath>

namespace core {

    glm::mat4 OrbitCamera::view() const {
        glm::vec3 dir = {std::cos(pitch_) * std::cos(yaw_), std::sin(pitch_), std::cos(pitch_) * std::sin(yaw_)};
        glm::vec3 eye = target_ - dir * distance_;
        return glm::lookAt(eye, target_, {0.0f, 1.0f, 0.0f});
    }

    void OrbitCamera::orbitBy(float dx, float dy) {
        const float rotSpeed = 0.005f;
        yaw_ += dx * rotSpeed;
        pitch_ -= dy * rotSpeed;
        constexpr float limit = glm::radians(89.0f);
        pitch_ = std::clamp(pitch_, -limit, limit);
    }

    void OrbitCamera::zoomBy(float scrollY) {
        distance_ *= std::exp(-scrollY * 0.15f);
        distance_ = std::clamp(distance_, 1.0f, 150.0f);
    }

}
