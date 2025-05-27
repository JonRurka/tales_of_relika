#ifndef _LYGIA_MATH_
#define _LYGIA_MATH_

float mod289(const float x) { return x - floor(x * (1.0f / 289.0f)) * 289.0f; }
fvec2 mod289_f2(const fvec2 x) { return x - floor(x * (1.0f / 289.0f)) * 289.0f; }
fvec3 mod289_f3(const fvec3 x) { return x - floor(x * (1.0f / 289.0f)) * 289.0f; }
fvec4 mod289_f4(const fvec4 x) { return x - floor(x * (1.0f / 289.0f)) * 289.0f; }

float permute(const float v)   { return mod289(((v * 34.0f) + 1.0f) * v); }
fvec2 permute_f2(const fvec2 v) { return mod289_f2(((v * 34.0f) + 1.0f) * v); }
fvec3 permute_f3(const fvec3 v) { return mod289_f3(((v * 34.0f) + 1.0f) * v); }
fvec4 permute_f4(const fvec4 v) { return mod289_f4(((v * 34.0f) + 1.0f) * v); }

float taylorInvSqrt(float r)    { return 1.79284291400159f - 0.85373472095314f * r; }
fvec2 taylorInvSqrt_f2(fvec2 r) { return 1.79284291400159f - 0.85373472095314f * r; }
fvec3 taylorInvSqrt_f3(fvec3 r) { return 1.79284291400159f - 0.85373472095314f * r; }
fvec4 taylorInvSqrt_f4(fvec4 r) { return 1.79284291400159f - 0.85373472095314f * r; }

float quintic(const float v)      { return v*v*v*(v*(v*6.0f-15.0f)+10.0f); }
fvec2  quintic_f2(const fvec2 v)  { return v*v*v*(v*(v*6.0f-15.0f)+10.0f); }
fvec3  quintic_f3(const fvec3 v)  { return v*v*v*(v*(v*6.0f-15.0f)+10.0f); }
fvec4  quintic_f4(const fvec4 v)  { return v*v*v*(v*(v*6.0f-15.0f)+10.0f); }


#endif //_LYGIA_MATH_