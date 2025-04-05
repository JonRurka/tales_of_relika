#ifndef _LIBNOISE_MODEL_
#define _LIBNOISE_MODEL_

#include "libnoise_core.inc"

STRUCT Module Model_Cylinder_GetModule(STRUCT Module m_pModule, float angle, float height)
{
   m_pModule.m_X = cos(angle * DEG_TO_RAD);
   m_pModule.m_Y = height;
   m_pModule.m_Z = sin(angle * DEG_TO_RAD);
   return m_pModule;
}

STRUCT Module Model_Line_GetModule(STRUCT Module m_pModule, fvec3 start, fvec3 end, float p)
{
   m_pModule.m_X = (end.x - start.x) * p + start.x;
   m_pModule.m_Y = (end.y - start.y) * p + start.y;
   m_pModule.m_Z = (end.z - start.z) * p + start.z;

   return m_pModule;
}

float Model_Line_m_Attenuate(STRUCT Module m_pModule, float p)
{
   m_pModule.Value = p * (1.0 - p) * 4 * m_pModule.Value;
   return m_pModule.Value;
}

STRUCT Module Sphere_GetValue(STRUCT Module m_pModule, float lat, float lon)
{
   fvec3 res = LatLonToXYZ(lat, lon);

   m_pModule.m_X = res.x;
   m_pModule.m_Y = res.y;
   m_pModule.m_Z = res.z;

   return m_pModule;
}


#endif