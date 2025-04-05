#ifndef _LIBNOISE_CORE_
#define _LIBNOISE_CORE_

#include "libnoise_const.inc"
#include "libnoise_objects.inc"


//
// Compatibility
//
float fmod(float x, float y)
{
   // https://stackoverflow.com/questions/7610631/glsl-mod-vs-hlsl-fmod
  return x - y * floor(x/y);
}

float fabs(float val){
   return abs(val);
}

//
// Modules
//
Module GetModule(float x, float y, float z)
{
   Module module;

   module.Value = 0;
   module.m_X = x;
   module.m_Y = y;
   module.m_Z = z;

   /* Base module settings for Perlin, Billow, and RidgedMulti*/
   module.m_frequency = DEFAULT_FREQUENCY;
   module.m_lacunarity = DEFAULT_LACUNARITY;
   module.m_noiseQuality = DEFAULT_QUALITY;
   module.m_octaveCount = DEFAULT_OCTAVE_COUNT;
   module.m_persistence = DEFAULT_PERSISTENCE;
   module.m_seed = DEFAULT_SEED;

   /* Specific to Rigdged Multi Facture*/
   module.m_ridged_offset = 1.0;
   module.m_ridged_gain = 2.0;
   module.m_ridged_H = 1.0;

   /* Specific to Voronoi */
   module.m_voronoi_displacement = DEFAULT_VORONOI_DISPLACEMENT;
   module.m_voronoi_enableDistance = false;

   /* Specific to Cache.*/
   module.m_cachedValue = 0;
   module.m_cache_isCached = false;
   module.m_cache_xCache = 0;
   module.m_cache_yCache = 0;
   module.m_cache_zCache = 0;

   /* Specific to Turbulence*/
   module.m_turb_power = DEFAULT_TURBULENCE_POWER;
   module.m_turb_seed = DEFAULT_TURBULENCE_SEED;
   module.m_turb_frequency = DEFAULT_TURBULENCE_FREQUENCY;
   module.m_turb_roughness = DEFAULT_TURBULENCE_ROUGHNESS;

   return module;
}

Module GetModule(Module m)
{
   Module module;

   module.Value = m.Value;
   module.m_X = m.m_X;
   module.m_Y = m.m_Y;
   module.m_Z = m.m_Z;

   /* Base module settings for Perlin, Billow, and RidgedMulti */
   module.m_frequency = m.m_frequency;
   module.m_lacunarity = m.m_lacunarity;
   module.m_noiseQuality = m.m_noiseQuality;
   module.m_octaveCount = m.m_octaveCount;
   module.m_persistence = m.m_persistence;
   module.m_seed = m.m_seed;

   /* Specific to Rigdged Multi Facture*/
   module.m_ridged_offset = m.m_ridged_offset;
   module.m_ridged_gain = m.m_ridged_gain;
   module.m_ridged_H = m.m_ridged_H;

   /* Specific to Voronoi*/
   module.m_voronoi_displacement = m.m_voronoi_displacement;
   module.m_voronoi_enableDistance = m.m_voronoi_enableDistance;

   /* Specific to Cache.*/
   module.m_cachedValue = m.m_cachedValue;
   module.m_cache_isCached = m.m_cache_isCached;
   module.m_cache_xCache = m.m_cache_xCache;
   module.m_cache_yCache = m.m_cache_yCache;
   module.m_cache_zCache = m.m_cache_zCache;

   /* Specific to RotatePoint*/
   module.m_x1Matrix = m.m_x1Matrix;
   module.m_x2Matrix = m.m_x2Matrix;
   module.m_x3Matrix = m.m_x3Matrix;
   module.m_xAngle = m.m_xAngle;
   module.m_y1Matrix = m.m_y1Matrix;
   module.m_y2Matrix = m.m_y2Matrix;
   module.m_y3Matrix = m.m_y3Matrix;
   module.m_yAngle = m.m_yAngle;
   module.m_z1Matrix = m.m_z1Matrix;
   module.m_z2Matrix = m.m_z2Matrix;
   module.m_z3Matrix = m.m_z3Matrix;
   module.m_zAngle = m.m_zAngle;

   /* Specific to Select*/
   module.m_edgeFalloff = m.m_edgeFalloff;
   module.m_lowerBound = m.m_lowerBound;
   module.m_upperBound = m.m_upperBound;

   /* Specific to Turbulence*/
   module.m_turb_power = m.m_turb_power;
   module.m_turb_seed = m.m_turb_seed;
   module.m_turb_frequency = m.m_turb_frequency;
   module.m_turb_roughness = m.m_turb_roughness;

   return module;
}

Module GetModule(float x, float y, float z, float value)
{
   Module module = GetModule(x, y, z);
   module.Value = value;
   return module;
}

Module GetModule(Module m, float value)
{
   Module module = GetModule(m);
   module.Value = value;
   return module;
}

vec3 LatLonToXYZ(float lat, float lon)
{
   float r = cos(DEG_TO_RAD * lat);
   float x = r * cos(DEG_TO_RAD * lon);
   float y = sin(DEG_TO_RAD * lat);
   float z = r * sin(DEG_TO_RAD * lon);

   return vec3(x, y, z);//(float3)(x, y, z);
}

int ClampValue(int value, int lowerBound, int upperBound)
{
   return clamp(value, lowerBound, upperBound);
   /*if (value < lowerBound) {
      return lowerBound;
   }
   else if (value > upperBound) {
      return upperBound;
   }
   else {
      return value;
   }*/
}

float CubicInterp(float n0, float n1, float n2, float n3, float a)
{
   float p = (n3 - n2) - (n0 - n1);
   float q = (n0 - n1) - p;
   float r = n2 - n0;
   float s = n1;
   return p * a * a * a + q * a * a + r * a + s;
}

float LinearInterp(float n0, float n1, float a)
{
   return ((1.0 - a) * n0) + (a * n1);
}

float SCurve3(float a)
{
   return (a * a * (3.0 - 2.0 * a));
}

float SCurve5(float a)
{
   float a3 = a * a * a;
   float a4 = a3 * a;
   float a5 = a4 * a;
   return (6.0 * a5) - (15.0 * a4) + (10.0 * a3);
}

float MakeInt32Range(float n)
{
   if (n >= 1073741824.0) {
      return (2.0 * fmod(n, float(1073741824.0))) - 1073741824.0;
   }
   else if (n <= -1073741824.0) {
      return (2.0 * fmod(n, float(1073741824.0))) + 1073741824.0;
   }
   else {
      return n;
   }
}

int IntValueNoise3D(int x, int y, int z, int seed)
{
   /* All constants are primes and must remain prime in order for this noise
     function to work correctly.*/
   int n = (
      X_NOISE_GEN   * x
      + Y_NOISE_GEN * y
      + Z_NOISE_GEN * z
      + SEED_NOISE_GEN * seed)
      & 0x7fffffff;
   n = (n >> 13) ^ n;
   return (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
}

float ValueNoise3D(int x, int y, int z, int seed)
{
   return 1.0 - (float(IntValueNoise3D(x, y, z, seed)) / 1073741824.0);
}

float GradientNoise3D(float fx, float fy, float fz, int ix, int iy, int iz, int seed)
{
   /* Randomly generate a gradient vector given the integer coordinates of the
   input value.  This implementation generates a random number and uses it
   as an index into a normalized-vector lookup table. */
   int vectorIndex = (
        X_NOISE_GEN * ix
      + Y_NOISE_GEN * iy
      + Z_NOISE_GEN * iz
      + SEED_NOISE_GEN * seed)
      & 0xffffffff;
   vectorIndex ^= (vectorIndex >> SHIFT_NOISE_GEN);
   vectorIndex &= 0xff;

   float xvGradient = g_randomVectors[(vectorIndex << 2)];
   float yvGradient = g_randomVectors[(vectorIndex << 2) + 1];
   float zvGradient = g_randomVectors[(vectorIndex << 2) + 2];

   /* Set up us another vector equal to the distance between the two vectors
    passed to this function.*/
   float xvPoint = (fx - float(ix));
   float yvPoint = (fy - float(iy));
   float zvPoint = (fz - float(iz));

   /* Now compute the dot product of the gradient vector with the distance
    vector.  The resulting value is gradient noise.  Apply a scaling value
    so that this noise value ranges from -1.0 to 1.0.*/
   return ((xvGradient * xvPoint)
      + (yvGradient * yvPoint)
      + (zvGradient * zvPoint)) * 2.12;
}

float GradientCoherentNoise3D(float x, float y, float z, int seed, int noiseQuality)
{
   /* Create a unit-length cube aligned along an integer boundary.  This cube
    surrounds the input point.*/
   int x0 = (x > 0.0 ? int(x) : int(x) - 1);
   int x1 = x0 + 1;
   int y0 = (y > 0.0 ? int(y) : int(y) - 1);
   int y1 = y0 + 1;
   int z0 = (z > 0.0 ? int(z) : int(z) - 1);
   int z1 = z0 + 1;

   /* Map the difference between the coordinates of the input value and the
    coordinates of the cube's outer-lower-left vertex onto an S-curve.*/
   float xs = 0, ys = 0, zs = 0;
   switch (noiseQuality) {
   case QUALITY_FAST:
      xs = (x - float(x0));
      ys = (y - float(y0));
      zs = (z - float(z0));
      break;
   case QUALITY_STD:
      xs = SCurve3(x - float(x0));
      ys = SCurve3(y - float(y0));
      zs = SCurve3(z - float(z0));
      break;
   case QUALITY_BEST:
      xs = SCurve5(x - float(x0));
      ys = SCurve5(y - float(y0));
      zs = SCurve5(z - float(z0));
      break;
   }

   /* Now calculate the noise values at each vertex of the cube.  To generate
    the coherent-noise value at the input point, interpolate these eight
    noise values using the S-curve value as the interpolant (trilinear
    interpolation.)*/
   float n0, n1, ix0, ix1, iy0, iy1;
   n0 = GradientNoise3D(x, y, z, x0, y0, z0, seed);
   n1 = GradientNoise3D(x, y, z, x1, y0, z0, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = GradientNoise3D(x, y, z, x0, y1, z0, seed);
   n1 = GradientNoise3D(x, y, z, x1, y1, z0, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy0 = LinearInterp(ix0, ix1, ys);

   n0 = GradientNoise3D(x, y, z, x0, y0, z1, seed);
   n1 = GradientNoise3D(x, y, z, x1, y0, z1, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = GradientNoise3D(x, y, z, x0, y1, z1, seed);
   n1 = GradientNoise3D(x, y, z, x1, y1, z1, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy1 = LinearInterp(ix0, ix1, ys);

   return LinearInterp(iy0, iy1, zs);
}

float ValueCoherentNoise3D(float x, float y, float z, int seed, int noiseQuality)
{
   /* Create a unit-length cube aligned along an integer boundary.  This cube
    surrounds the input point.*/
   int x0 = (x > 0.0 ? int(x) : int(x) - 1);
   int x1 = x0 + 1;
   int y0 = (y > 0.0 ? int(y) : int(y) - 1);
   int y1 = y0 + 1;
   int z0 = (z > 0.0 ? int(z) : int(z) - 1);
   int z1 = z0 + 1;

   /* Map the difference between the coordinates of the input value and the
    coordinates of the cube's outer-lower-left vertex onto an S-curve.*/
   float xs = 0, ys = 0, zs = 0;
   switch (noiseQuality) {
   case QUALITY_FAST:
      xs = (x - float(x0));
      ys = (y - float(y0));
      zs = (z - float(z0));
      break;
   case QUALITY_STD:
      xs = SCurve3(x - float(x0));
      ys = SCurve3(y - float(y0));
      zs = SCurve3(z - float(z0));
      break;
   case QUALITY_BEST:
      xs = SCurve5(x - float(x0));
      ys = SCurve5(y - float(y0));
      zs = SCurve5(z - float(z0));
      break;
   }

   /* Now calculate the noise values at each vertex of the cube.  To generate
    the coherent-noise value at the input point, interpolate these eight
    noise values using the S-curve value as the interpolant (trilinear
    interpolation.)*/
   float n0, n1, ix0, ix1, iy0, iy1;
   n0 = ValueNoise3D(x0, y0, z0, seed);
   n1 = ValueNoise3D(x1, y0, z0, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = ValueNoise3D(x0, y1, z0, seed);
   n1 = ValueNoise3D(x1, y1, z0, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy0 = LinearInterp(ix0, ix1, ys);
   n0 = ValueNoise3D(x0, y0, z1, seed);
   n1 = ValueNoise3D(x1, y0, z1, seed);
   ix0 = LinearInterp(n0, n1, xs);
   n0 = ValueNoise3D(x0, y1, z1, seed);
   n1 = ValueNoise3D(x1, y1, z1, seed);
   ix1 = LinearInterp(n0, n1, xs);
   iy1 = LinearInterp(ix0, ix1, ys);
   return LinearInterp(iy0, iy1, zs);
}

#endif