/*
 * This file is part of Rotorflight.
 *
 * Rotorflight is free software. You can redistribute this software
 * and/or modify this software under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 * version.
 *
 * Rotorflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <math.h>
#include <stdint.h>

#include "platform.h"

#include "build/build_config.h"

#include "trig.h"


// Fast sin approximation for rad ∈ [-π/4, π/4].  Minimax-optimised degree-7 odd polynomial.
float sin_minimax(float x)
{
    const float c3 = -0.16666650669294222f;
    const float c5 =  0.00833197866315977f;
    const float c7 = -0.00019495636237996f;
    const float x2 = x * x;
    return x * (1.0f + x2 * (c3 + x2 * (c5 + x2 * c7)));
}

// Fast cos approximation for rad ∈ [-π/4, π/4].  Minimax-optimised degree-6 even polynomial.
float cos_minimax(float x)
{
    const float c2 = -0.49999894781370191f;
    const float c4 =  0.04165629457842692f;
    const float c6 = -0.00135978231111122f;
    const float x2 = x * x;
    return (1.0f + x2 * (c2 + x2 * (c4 + x2 * c6)));
}

// Fast sin approximation for rad ∈ [-π/8, π/8].  Taylor series degree-7 odd polynomial.
float sin_taylor(float x)
{
    const float c3 = -0.16666666666666667f;         // -1/6
    const float c5 =  0.0083333333333333332f;       //  1/120
    const float c7 = -0.00019841269841269841f;      // -1/5040
    const float x2 = x * x;
    return x * (1.0f + x2 * (c3 + x2 * (c5 + x2 * c7)));
}

// Fast cos approximation for rad ∈ [-π/8, π/8].  Taylor series degree-6 even polynomial.
FAST_CODE float cos_taylor(float x)
{
    const float c2 = -0.5f;                         // -1/2
    const float c4 =  0.041666666666666664f;        //  1/24
    const float c6 = -0.001388888888888889f;        // -1/720
    const float x2 = x * x;
    return (1.0f + x2 * (c2 + x2 * (c4 + x2 * c6)));
}


#define INV_PIO2    M_2_PIf

static inline float sin_poly5_bf(float r)
{
    // Pre-scaled for u = r*(π/2)
    const float c0 =  0x1.921f1cp0f; // 1.5707871913909912109375
    const float c1 = -0x1.4a974p-1f; // -0.6456851959228515625
    const float c2 =  0x1.3db294p-4f; // 7.756288349628448486328125e-2
    float s = r * r;
    return r * ((c2 * s + c1) * s + c0);
}

static inline float cos_poly6_bf(float r)
{
    const float d1 = -0x1.3bd39cp0f; // -1.2336976528167724609375
    const float d2 =  0x1.03bp-2f; // 0.25360107421875
    const float d3 = -0x1.4e5eecp-6f; // -2.04083733260631561279296875e-2
    float s = r * r;
    return ((d3 * s + d2) * s + d1) * s + 1.0f;
}

// ---- Quadrant mapping helpers ----
// r ∈ [-0.5, 0.5], q is quadrant index (…,-1,0,1,2,3,4,…).
static inline float sinf_quadrant_bf(float r, int q)
{
    //q &= 3;
    if (q & 1) { // odd: use cos, sign handled below
        float v = cos_poly6_bf(r);
        return (q & 2) ? -v : v;
    } else {     // even: use sin
        float v = sin_poly5_bf(r);
        return (q & 2) ? -v : v;
    }
}

static inline float cosf_quadrant_bf(float r, int q)
{
    //q &= 3;
    if (q & 1) { // odd: -sin, sign handled below
        float v = -sin_poly5_bf(r);
        return (q & 2) ? -v : v;   // q=1 -> -sin, q=3 -> +sin
    } else {     // even: cos
        float v = cos_poly6_bf(r);
        return (q & 2) ? -v : v;   // q=2 -> -cos
    }
}


float sin_betaflight(float x)
{
    float t = x * INV_PIO2;     // in quadrant units
    float qf = roundf(t);       // nearest quadrant as float
    int   q  = (int)qf;
    float r  = t - qf;          // remainder in [-0.5, 0.5]
    return sinf_quadrant_bf(r, q);
}

float cos_betaflight(float x)
{
    float t = x * INV_PIO2;
    float qf = roundf(t);
    int   q  = (int)qf;
    float r  = t - qf;          // [-0.5, 0.5]
    return cosf_quadrant_bf(r, q);
}


