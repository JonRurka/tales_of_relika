#pragma once

#include "Plane.h"


struct Frustum
{
    BoundsVolume::Plane topFace;
    BoundsVolume::Plane bottomFace;

    BoundsVolume::Plane rightFace;
    BoundsVolume::Plane leftFace;

    BoundsVolume::Plane farFace;
    BoundsVolume::Plane nearFace;
};