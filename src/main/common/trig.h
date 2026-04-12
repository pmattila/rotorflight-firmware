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

#define M_1_PIf   0.31830988618379067154f
#define M_2_PIf   0.63661977236758134308f
#define M_4_PIf   1.27323954473516268616f
#define M_PI_2f   1.57079632679489661923f

float sin_minimax(float x);
float cos_minimax(float x);

float sin_taylor(float x);
float cos_taylor(float x);

float sin_betaflight(float x);
float cos_betaflight(float x);

