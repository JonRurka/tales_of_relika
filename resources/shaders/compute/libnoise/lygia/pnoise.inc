#ifndef _LYGIA_PNOISE_
#define _LYGIA_PNOISE_

#include "math.inc"

float pnoise(fvec3 P, fvec3 rep) {
    fvec3 Pi0 = FMOD(floor(P), rep); // Integer part, modulo period
    fvec3 Pi1 = FMOD(Pi0 + FVEC3(1.0f, 1.0f, 1.0f), rep); // Integer part + 1, mod period
    Pi0 = mod289_f3(Pi0);
    Pi1 = mod289_f3(Pi1);
    fvec3 Pf0 = FRAC(P); // Fractional part for interpolation
    fvec3 Pf1 = Pf0 - FVEC3(1.0f, 1.0f, 1.0f); // Fractional part - 1.0
    fvec4 ix = FVEC4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    fvec4 iy = FVEC4(Pi0.y, Pi0.y, Pi1.y, Pi1.y);
    fvec4 iz0 = Pi0.zzzz;
    fvec4 iz1 = Pi1.zzzz;

    fvec4 ixy = permute_f4(permute_f4(ix) + iy);
    fvec4 ixy0 = permute_f4(ixy + iz0);
    fvec4 ixy1 = permute_f4(ixy + iz1);

    fvec4 gx0 = ixy0 * (1.0f / 7.0f);
    fvec4 gy0 = FRAC(floor(gx0) * (1.0f / 7.0f)) - 0.5f;
    gx0 = FRAC(gx0);
    fvec4 gz0 = FVEC4(0.5f, 0.5f, 0.5f, 0.5f) - FABS(gx0) - FABS(gy0);
    fvec4 sz0 = step(gz0, FVEC4(0.0f, 0.0f, 0.0f, 0.0f));
    gx0 -= sz0 * (step(0.0f, gx0) - 0.5f);
    gy0 -= sz0 * (step(0.0f, gy0) - 0.5f);

    fvec4 gx1 = ixy1 * (1.0f / 7.0f);
    fvec4 gy1 = FRAC(floor(gx1) * (1.0f / 7.0f)) - 0.5f;
    gx1 = FRAC(gx1);
    fvec4 gz1 = FVEC4(0.5f, 0.5f, 0.5f, 0.5f) - FABS(gx1) - FABS(gy1);
    fvec4 sz1 = step(gz1, FVEC4(0.0f, 0.0f, 0.0f, 0.0f));
    gx1 -= sz1 * (step(0.0f, gx1) - 0.5f);
    gy1 -= sz1 * (step(0.0f, gy1) - 0.5f);

    fvec3 g000 = FVEC3(gx0.x, gy0.x, gz0.x);
    fvec3 g100 = FVEC3(gx0.y, gy0.y, gz0.y);
    fvec3 g010 = FVEC3(gx0.z, gy0.z, gz0.z);
    fvec3 g110 = FVEC3(gx0.w, gy0.w, gz0.w);
    fvec3 g001 = FVEC3(gx1.x, gy1.x, gz1.x);
    fvec3 g101 = FVEC3(gx1.y, gy1.y, gz1.y);
    fvec3 g011 = FVEC3(gx1.z, gy1.z, gz1.z);
    fvec3 g111 = FVEC3(gx1.w, gy1.w, gz1.w);

    fvec4 norm0 = taylorInvSqrt_f4(FVEC4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    fvec4 norm1 = taylorInvSqrt_f4(FVEC4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, FVEC3(Pf1.x, Pf0.y, Pf0.z));
    float n010 = dot(g010, FVEC3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, FVEC3(Pf1.x, Pf1.y, Pf0.z));
    float n001 = dot(g001, FVEC3(Pf0.x, Pf0.y, Pf1.z));
    float n101 = dot(g101, FVEC3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, FVEC3(Pf0.x, Pf1.y, Pf1.z));
    float n111 = dot(g111, Pf1);

    fvec3 fade_xyz = quintic_f3(Pf0);
    fvec4 n_z = mix(FVEC4(n000, n100, n010, n110), FVEC4(n001, n101, n011, n111), fade_xyz.z);
    fvec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return 2.2 * n_xyz;
}

#endif // _LYGIA_PNOISE_