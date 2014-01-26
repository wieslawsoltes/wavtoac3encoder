/**
 * Aften: A/52 audio encoder
 *
 * x86 window functions header
 * Copyright (C) 2007 by Prakash Punnoor
 * prakash@punnoor.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation;
 * version 2 of the License
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

/**
 * @file window.h
 * A/52 Kaiser-Bessel Derived Window x86 header
 */

#ifndef X86_WINDOW_H
#define X86_WINDOW_H

#include "common.h"

extern void apply_a52_window_sse(FLOAT *samples);

#endif /* X86_WINDOW_H */
