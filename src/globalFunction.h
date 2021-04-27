//
// Created by Sth on 2021/4/24.
//

#ifndef OPENGLHELLOWORLD_GLOBALFUNCTION_H
#define OPENGLHELLOWORLD_GLOBALFUNCTION_H

#include "glm/glm.hpp"
#include <time.h>

namespace global {
    using namespace glm;
    const float Epsilon = 0.00001f;

    inline float getDis(const vec2 &v2_1, const vec2 &v2_2) {
        return glm::length(v2_2 - v2_1);
    }

    inline float clampFloat(const float &val, float min_val = -1.0f, float max_val = 1.0f) {
        return glm::max(min_val, glm::min(val, max_val));
    }

    // 角度一律使用弧度
    inline vec2 rotateVec2(const vec2 &ori, const vec2 &v2, const float &angle) {
        // 平移
        vec2 ret = v2 - ori;
        vec2 ret_temp = ret;

        ret.x = cos(angle) * ret_temp.x - sin(angle) * ret_temp.y;
        ret.y = sin(angle) * ret_temp.x + cos(angle) * ret_temp.y;

        // 平移回来
        ret = ret + ori;
        return ret;
    }

    inline float getRand() {
//        srand(time(0));
        return rand() / (double(RAND_MAX));
    }
}
#endif //OPENGLHELLOWORLD_GLOBALFUNCTION_H
