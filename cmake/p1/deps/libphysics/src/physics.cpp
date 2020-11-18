#include "physics.h"
#include "tinymath.h"

namespace physics{
    float velocity(float distance, float time) {
        return tinymath::square(distance) / time;
    }
}
