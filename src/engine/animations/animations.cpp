#include "iostream"
#include "cmath"

#include "animations.hpp"

float Anim::easeInOutQuart(float x) {
    return x < 0.5f ? 8.f * x * x * x * x : 1.f - std::powf(-2.f * x + 2.f, 4.f) / 2.f;
}

float Anim::easeOutBounce(float x) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;

    if (x < 1 / d1) {
        return n1 * x * x;
    }
    else if (x < 2.f / d1) {
        return n1 * (x -= 1.5f / d1) * x + 0.75f;
    }
    else if (x < 2.5f / d1) {
        return n1 * (x -= 2.25f / d1) * x + 0.9375f;
    }
    else {
        return n1 * (x -= 2.625f / d1) * x + 0.984375f;
    }
}

float Anim::easeInBack(float x) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.f;

    return c3 * x * x * x - c1 * x * x;
}

float Anim::CircleX(float t) {
    float kakayata_peremennaya = t * 2.f * 3.14159f;
    float x = cosf(kakayata_peremennaya);
    return x;
}

float Anim::CircleY(float t) {
    float kakayata_peremennaya = t * 2.f * 3.14159f;
    float y = sinf(kakayata_peremennaya);
    return y;
}