#include "physics.h"
#include "math.h"

namespace physics{
    float velocity(float distance, float time) {
        return math::square(distance) / time;
    }
}
