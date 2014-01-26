/**
 * Aften: A/52 audio encoder
 * Copyright (c) 2006 Justin Ruggles
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file window.h
 * A/52 Kaiser-Bessel Derived Window header
 */

#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"
#include "cpu_caps.h"

#if defined(HAVE_MMX) || defined(HAVE_SSE)
#include "x86/window.h"
#endif

extern FLOAT a52_window[512];

typedef struct A52WindowFunctions {
    /**
     * Apply the A/52 window function to 512 input samples.
     */
    void (*apply_a52_window)(FLOAT *samples);
} A52WindowFunctions;

extern void a52_window_init(A52WindowFunctions *winf);

#endif /* WINDOW_H */
