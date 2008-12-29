;
; WAV to AC3 Encoder
; Copyright (C) 2007, 2008, 2009 Wieslaw Soltes <wieslaw.soltes@gmail.com>
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; version 2 of the License.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;

; $Id$

#define RELEASE_WIN64

;
; Include setup release version
;
#include "SetupScript.version"

;
; Set current installer name and binaries path
;
#define ReleaseName "(x64)"
#define ReleaseNameShort "x64"
#define ReleasePath "..\Release\x64"

;
; Include setup base srcript
;
#include "SetupScript.base"
