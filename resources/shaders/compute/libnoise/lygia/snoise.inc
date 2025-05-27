#ifndef _LYGIA_SNOISE_
#define _LYGIA_SNOISE_

#include "math.inc"

float snoise(fvec3 v) {
    const fvec2  C = FVEC2(1.0f/6.0f, 1.0f/3.0f) ;
    const fvec4  D = FVEC4(0.0f, 0.5f, 1.0f, 2.0f);

    // First corner
    fvec3 i  = floor(v + dot(v, C.yyy) );
    fvec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    fvec3 g = step(x0.yzx, x0.xyz);
    fvec3 l = 1.0f - g;
    fvec3 i1 = min( g.xyz, l.zxy );
    fvec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    fvec3 x1 = x0 - i1 + C.xxx;
    fvec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    fvec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289_f3(i);
    fvec4 p = permute_f4( permute_f4( permute_f4(
              i.z + FVEC4(0.0f, i1.z, i2.z, 1.0f ))
            + i.y + FVEC4(0.0f, i1.y, i2.y, 1.0f ))
            + i.x + FVEC4(0.0f, i1.x, i2.x, 1.0f ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857f; // 1.0/7.0
    fvec3  ns = n_ * D.wyz - D.xzx;

    fvec4 j = p - 49.0f * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    fvec4 x_ = floor(j * ns.z);
    fvec4 y_ = floor(j - 7.0f * x_ );    // mod(j,N)

    fvec4 x = x_ *ns.x + ns.yyyy;
    fvec4 y = y_ *ns.x + ns.yyyy;
    fvec4 h = 1.0f - FABS(x) - FABS(y);

    fvec4 b0 = FVEC4( x.x, x.y, y.x, y.y );
    fvec4 b1 = FVEC4( x.z, x.w, y.z, y.w );

    //fvec4 s0 = FVEC4(lessThan(b0,0.0))*2.0 - 1.0;
    //fvec4 s1 = FVEC4(lessThan(b1,0.0))*2.0 - 1.0;
    fvec4 s0 = floor(b0)*2.0f + 1.0f;
    fvec4 s1 = floor(b1)*2.0f + 1.0f;
    fvec4 sh = -step(h, FVEC4(0.0f, 0.0f, 0.0f, 0.0f));

    fvec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    fvec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    fvec3 p0 = FVEC3(a0.x, a0.y, h.x);
    fvec3 p1 = FVEC3(a0.z, a0.w, h.y);
    fvec3 p2 = FVEC3(a1.x, a1.y, h.z);
    fvec3 p3 = FVEC3(a1.z, a1.w, h.w);

    //Normalise gradients
    fvec4 norm = taylorInvSqrt_f4(FVEC4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    fvec4 m = max(0.6f - FVEC4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0f);
    m = m * m;
    return 42.0f * dot( m*m, FVEC4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

#endif // _SYGIA_PNOISE_