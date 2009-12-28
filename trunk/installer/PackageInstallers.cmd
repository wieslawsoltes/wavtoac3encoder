@echo off
rem
rem WAV to AC3 Encoder
rem Copyright (C) 2007, 2008, 2009 Wieslaw Soltes <wieslaw.soltes@gmail.com>
rem
rem This program is free software; you can redistribute it and/or modify
rem it under the terms of the GNU General Public License as published by
rem the Free Software Foundation; version 2 of the License.
rem
rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with this program; if not, write to the Free Software
rem Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
rem
rem $Id$
setlocal
set IssCompiler32=c:\Program Files (x86)\Inno Setup 5\Compil32.exe
"%IssCompiler32%" /cc "SetupScript.iss"
"%IssCompiler32%" /cc "SetupScriptAMD64.iss"
endlocal