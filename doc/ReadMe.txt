WAV to AC3 Encoder 2.3 (Using Aften A/52 Audio Encoder)

1) LICENSE

WAV to AC3 Encoder
Copyright (C) 2007-2008 Wiesław Šoltés <wieslaw.soltes@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

2) ABOUT

WAV to AC3 Encoder is an audio encoder which generates compressed 
audio streams based on ATSC A/52 specification. This type of audio 
is also known as AC-3 or Dolby® Digital and is one of the audio 
codecs used in DVD-Video content.

WAV to AC3 Encoder is a GUI for Aften A/52 audio encoder 
created by Justin Ruggles. WAV to AC3 Encoder project was 
started at 27 January 2007 by Wiesław Šoltés. There are 
Win32 (x86) and Win64 (x86_64, AMD64) binaries available. 

WAV to AC3 Encoder has full UNICODE and ANSI support. 
The latest sources can be obtained through Subversion. 
The main developer of WAV to AC3 Encoder is Wiesław Šoltés. 

Main features:

* Multi-Threaded encoding.
* Loading and saving of all program settings.
* Encoder presets.
* Vary fast.
* Optimized for modern CPU's.
* High-quality of output audio files.
* Full UNICODE and ANSI support.
* Natively supports x86 and AMD64 architectures.
* Options for advanced users.
* Small size.
* Sources available under GNU GPLv2 license.
* Very simple to use. 

Program actively developed. Currently still in beta version.

3) CONFIGURATION

All confguration files are in plain text format. Each configuration file 
has different structure. Edit the files and check the sources to find 
out more about how this files are created. Each configuration file is 
automaticaly loaded/saved by program every time the program is started/closed.

Program configuration is stored in EncWAVtoAC3.config file.

Presets for enoder are stored in EncWAVtoAC3.presets file.

Paths and names of different libaften.dll engines are stored in 
EncWAVtoAC3.win32 (Win32) and EncWAVtoAC3.win64 (Win64) files.

4) SUBVERSION

svn checkout http://wavtoac3encoder.googlecode.com/svn/trunk/ wavtoac3encoder-read-only

5) BUILDING

Download latest Aften sources from http://code.google.com/p/wavtoac3encoder/
and extract them in aften sub-directory in project source tree.

Use Miscroft Visual Studio 2008 (C++ with MFC) to build Release binaries.

Distribute Release binaries with libaften.dll downloaded from the same 
site as aften sources. You can also build installer using Inno Setup
you need to put libaften.dll's to 'bin' sub-directories.

7) COPYRIGHTS

WAV to AC3 Encoder, Copyright (C) 2007-2008 Wiesław Šoltés
http://code.google.com/p/wavtoac3encoder/

Aften, Copyright (c) 2006-2007 Justin Ruggles <justinruggles@bellsouth.net>
                     2006-2007 Prakash Punnoor <prakash@punnoor.de>
http://aften.sourceforge.net/
http://sourceforge.net/projects/aften/

8) WEBSITE

http://code.google.com/p/wavtoac3encoder/

$Id$