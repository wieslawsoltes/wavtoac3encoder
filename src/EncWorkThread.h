//
// WAV to AC3 Encoder
// Copyright (C) 2007-2014 Wiesław Šoltés <wieslaw.soltes@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/* $Id$ */

#pragma once

typedef struct TAftenOpt
{
    int pad_start;
    int chmap;
    int read_to_eof;
    int raw_input;
    int raw_sr;
    int raw_ch;
    enum PcmSampleFormat raw_fmt;
    int raw_order;
} AftenOpt;

typedef struct TSingleWorkerData
{
    // input and output files
    CString szInPath;
    CString szOutPath;
    // aften related data
    AftenOpt opt;
    AftenContext s;
    // work stats
    int nProgress;
    double fEncTime;
    // termination flag
    bool bTerminate;
    // file id
    int nID;
    // success flag
    bool bSuccess;
} SingleWorkerData;

#define WM_WORKER_PROGRESS WM_USER + 0x10

DWORD WINAPI EncWorkThread(LPVOID pParam);
