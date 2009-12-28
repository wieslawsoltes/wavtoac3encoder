/**
 * Aften: A/52 audio encoder
 *
 * ASM support header
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

#ifndef ASM_SUPPORT_H
#define ASM_SUPPORT_H

#ifdef __GNUC__
#include "gas_support.h"
#else
#include "intelas_support.h"
#endif

#endif /* ASM_SUPPORT_H */
