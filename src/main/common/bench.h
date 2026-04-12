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

#pragma once

#include <stdint.h>

#ifdef USE_MATH_BENCH

typedef double (*mathBenchRefFn_t)(double x);

// Function under test: float kernel(float x).
typedef float (*mathBenchTargetFn_t)(float x);

typedef struct {
    const char             *name;   // NULL = blank-line separator in output
    mathBenchTargetFn_t    fn;
    mathBenchRefFn_t       ref;    // libm reference in double; NULL = no accuracy line
    float              x0;
    float              x1;
    // filled by mathBenchRun():
    uint32_t           cycles;
    uint32_t           error;
    uint8_t            bits;
} mathBenchEntry_t;

extern mathBenchEntry_t mathBenchEntries[];
extern const int        mathBenchEntryCount;

// Time one call to func(x): writes DWT cycle delta into *out_cycles, returns func(x).
float mathBenchCall(mathBenchTargetFn_t func, float x, uint32_t *out_cycles);

void mathBenchRun(void);

#endif
