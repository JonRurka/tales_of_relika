#pragma once

#include "Transform.h"
#include "Frustum.h"

namespace BoundsVolume
{

    struct BoundingVolume
    {
        virtual bool isOnOrForwardPlane(const Plane& plane) const = 0;

        virtual bool isOnFrustum(const Frustum& camFrustum) const = 0;
    };

}