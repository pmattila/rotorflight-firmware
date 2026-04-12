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

#include "common/maths.h"
#include "common/trig.h"
#include "drivers/system.h"

#include "bench.h"

#ifdef USE_MATH_BENCH

#define MATH_BENCH_ITERATIONS 1000U

#define M_4PIf    12.56637061435917295352f
#define M_PI_16f  0.19634954084936207748f

float addF(float x)          { return x + (float)0.5; }
double addD(double x)        { return x + (double)0.5; }

float sinD(float x)          { return sin(x); }
float cosD(float x)          { return cos(x); }
float tanD(float x)          { return tan(x); }

float sincosA(float x)
{
    sincosf_t result = sincos_approx(x);
    UNUSED(result);
    return result.sin;
}

float sincosF(float x)
{
    sincosf_t result = sincos_fast(x);
    UNUSED(result);
    return result.sin;
}

float sqrtD(float x)         { return sqrt(x); }
float sqrtF(float x)         { return sqrtf(x); }

float logD(float x)          { return log(x); }
float expD(float x)          { return exp(x); }
float powD(float x)          { return pow(x, M_PIf); }

double powX(double x)        { return pow(x, M_PIf); }
float powfX(float x)         { return powf(x, M_PIf); }
float pow_approxX(float x)   { return pow_approx(x, M_PIf); }


// DWT->CYCCNT is the word at DWT_BASE + 4 (same on all Cortex-M3/M4/M7 here).
#define CYCCNT_ADDR   ((uint32_t)DWT_BASE + 4U)

// Clobbers for an unknown float(float) callee (AAPCS hard-float caller-saved).
#define BL_CLOBBERS \
    "memory", "cc", "r0", "r1", "r2", "r3", "r12", "lr", \
    "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15"

__attribute__((optimize("-O2")))
__attribute__((noinline))
float mathBenchCall(mathBenchTargetFn_t func, float x, uint32_t *out_cycles)
{
    const uint32_t cyccnt_addr = CYCCNT_ADDR;
    uint32_t dt = 0;
    uint32_t t0, t1;

    register float s0 asm("s0");

    for (uint32_t i = 0; i < 16; i++) {
        s0 = x;
        __asm__ __volatile__(
            "ldr %[t0], [%[cc]]\n\t"
            "blx %[fn]\n\t"
            "ldr %[t1], [%[cc]]\n\t"
            : [t0] "=&r" (t0), [t1] "=&r" (t1), [s0] "+t" (s0)
            : [cc] "r" (cyccnt_addr), [fn] "r" (func)
            : BL_CLOBBERS);
        dt += t1 - t0;
    }

    *out_cycles = dt >> 4;

    return s0;
}


mathBenchEntry_t mathBenchEntries[] =
{
    { .name = "add",             .fn = addF,                      .ref = addD, .x0 = -1.4f,    .x1 = 1.4f     },
    { .name = NULL },
    { .name = "sin",             .fn = sinD,                      .ref = sin,  .x0 = -M_4PIf,  .x1 = M_4PIf   },
    { .name = "cos",             .fn = cosD,                      .ref = cos,  .x0 = -M_4PIf,  .x1 = M_4PIf   },
    { .name = "tan",             .fn = tanD,                      .ref = tan,  .x0 = -1.4f,    .x1 = 1.4f     },
    { .name = NULL },
    { .name = "sinf",            .fn = sinf,                      .ref = sin,  .x0 = -M_2PIf,  .x1 = M_2PIf   },
    { .name = "cosf",            .fn = cosf,                      .ref = cos,  .x0 = -M_2PIf,  .x1 = M_2PIf   },
    { .name = "tanf",            .fn = tanf,                      .ref = tan,  .x0 = -1.4f,    .x1 = 1.4f     },
    { .name = NULL },
    { .name = "sin_fast",        .fn = sin_fast,                  .ref = sin,  .x0 = -M_PI_4f, .x1 = M_PI_4f  },
    { .name = "cos_fast",        .fn = cos_fast,                  .ref = cos,  .x0 = -M_PI_4f, .x1 = M_PI_4f  },
    { .name = "tan_fast",        .fn = tan_fast,                  .ref = tan,  .x0 = -M_PI_4f, .x1 = M_PI_4f  },
    { .name = "sincos_fast",     .fn = sincosF,                   .ref = sin,  .x0 = -M_PI_4f, .x1 = M_PI_4f  },
    { .name = NULL },
    { .name = "sin_approx",      .fn = sin_approx,                .ref = sin,  .x0 = -M_2PIf,  .x1 = M_2PIf   },
    { .name = "cos_approx",      .fn = cos_approx,                .ref = cos,  .x0 = -M_2PIf,  .x1 = M_2PIf   },
    { .name = "tan_approx",      .fn = tan_approx,                .ref = tan,  .x0 = -1.4f,    .x1 = 1.4f     },
    { .name = "tan_approx2",     .fn = tan_approx2,               .ref = tan,  .x0 = -1.4f,    .x1 = 1.4f     },
    { .name = "sincos_approx",   .fn = sincosA,                   .ref = sin,  .x0 = -M_2PIf,  .x1 = M_2PIf   },
    { .name = NULL },
    { .name = "sin_betaflight",  .fn = sin_betaflight,            .ref = sin,  .x0 = -M_2PIf,  .x1 = M_2PIf   },
    { .name = "cos_betaflight",  .fn = cos_betaflight,            .ref = cos,  .x0 = -M_2PIf,  .x1 = M_2PIf   },
    { .name = NULL },
    { .name = "sqrt",            .fn = sqrtD,                     .ref = sqrt, .x0 = 0.001f,   .x1 = 1000.0f  },
    { .name = "sqrtf",           .fn = sqrtF,                     .ref = sqrt, .x0 = 0.001f,   .x1 = 1000.0f  },
    { .name = NULL },
    { .name = "log",             .fn = logD,                      .ref = log,  .x0 = 0.001f,   .x1 = 1000.0f  },
    { .name = "logf",            .fn = logf,                      .ref = log,  .x0 = 0.001f,   .x1 = 1000.0f  },
    { .name = "log_approx",      .fn = log_approx,                .ref = log,  .x0 = 0.001f,   .x1 = 1000.0f  },
    { .name = NULL },
    { .name = "exp",             .fn = expD,                      .ref = exp,  .x0 = -10.0f,   .x1 = 10.0f    },
    { .name = "expf",            .fn = expf,                      .ref = exp,  .x0 = -10.0f,   .x1 = 10.0f    },
    { .name = "exp_approx<0",    .fn = exp_approx,                .ref = exp,  .x0 = -10.0f,   .x1 = 0.0f     },
    { .name = "exp_approx>0",    .fn = exp_approx,                .ref = exp,  .x0 = 0.0f,     .x1 = 10.0f    },
    { .name = NULL },
    { .name = "pow*",            .fn = powD,                      .ref = powX, .x0 = 0.001f,   .x1 = 10.0f    },
    { .name = "powf*",           .fn = powfX,                     .ref = powX, .x0 = 0.001f,   .x1 = 10.0f    },
    { .name = "pow_approx*",     .fn = pow_approxX,               .ref = powX, .x0 = 0.001f,   .x1 = 10.0f    },
    { .name = NULL },
};

const int mathBenchEntryCount = sizeof(mathBenchEntries) / sizeof(mathBenchEntries[0]);

static inline float getSample(float x0, float x1, uint32_t K, uint32_t N)
{
    const float t = (float)K / (float)(N - 1U);
    return x0 + t * (x1 - x0);
}

void mathBenchRun(void)
{
    for (int j = 0; j < mathBenchEntryCount; j++) {
        mathBenchEntry_t *e = &mathBenchEntries[j];

        if (e->name && e->fn && e->ref) {
            double maxRel = 0;
            double maxErr = 0;

            uint32_t cycles = 0;
            uint32_t count = 0;
            uint32_t dt = 0;

            for (uint32_t k = 0; k < MATH_BENCH_ITERATIONS; k++) {
                float x = getSample(e->x0, e->x1, k, MATH_BENCH_ITERATIONS);

                double refv = e->ref((double)x);
                float funv = mathBenchCall(e->fn, x, &dt);

                cycles += dt;
                count += 1U;

                if (isfinite(refv) && isfinite(funv)) {
                    double err = fabs((double)funv - refv);
                    double ref = fabs(refv);
                    double rel = (ref > 1) ? (err / ref) : err;
                    maxRel = fmax(maxRel, rel);
                    maxErr = fmax(maxErr, err);
                }
            }

            e->bits = (maxRel > 0) ? (uint8_t)(-log2(maxRel)) : 0;
            e->error = maxErr * (double)1e9;
            e->cycles = count ? (cycles / count) : 0;
        }
        else {
            e->bits = 0;
            e->error = 0;
            e->cycles = 0;
        }
    }
}

#endif
