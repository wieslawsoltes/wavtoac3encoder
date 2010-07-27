WAV to AC3 Encoder 4.6 (Using Aften A/52 Audio Encoder)

==LICENSE==

WAV to AC3 Encoder
Copyright (C) 2007, 2008, 2009 Wiesław Šoltés <wieslaw.soltes@gmail.com>

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

==ABOUT==

WAV to AC3 Encoder is an audio encoder which generates compressed 
audio streams based on ATSC A/52 specification. This type of audio 
is also known as AC-3 or Dolby® Digital and is one of the audio 
codecs used in DVD-Video content.

WAV to AC3 Encoder is a GUI for Aften A/52 audio encoder 
created by Justin Ruggles. WAV to AC3 Encoder project was 
started at 27 January 2007 by Wiesław Šoltés. 

WAV to AC3 Encoder has full UNICODE and ANSI support. 
The latest sources can be obtained through Subversion. 
The main developer of WAV to AC3 Encoder is Wiesław Šoltés. 

===Main Program Features===

  * High-quality output audio files.
  * Vary fast encoder.
  * Multi-threaded encoding.
  * Milti-channel file input.
  * Multiple-mono file input.
  * Advanved mux wizard.
  * Optimized for modern CPUs.
  * Small program size.
  * Load and save all program settings.
  * Load and save encoder presets.
  * Avisynth scripting support (Win32 only).
  * Full UNICODE and ANSI support.
  * Very simple to use.
  * Options for advanced users.
  * Sources available under GNU GPLv2 license.

==CONFIGURATION==

All confguration files are in plain text format. Each configuration file 
has different structure. Edit the files and check the sources to find 
out more about how this files are created. Each configuration file is 
automaticaly loaded/saved by program every time the program is started/closed.

Program configuration is stored in EncWAVtoAC3.config file.

Presets for encoder are stored in EncWAVtoAC3.presets file.

Paths to libaften.dll engines are stored in EncWAVtoAC3.engines file.

==SOURCES==

Source code is managed by the Subversion repository.

svn checkout http://wavtoac3encoder.googlecode.com/svn/trunk/ wavtoac3encoder-read-only

==BUILD==

  * Download latest Aften sources from http://code.google.com/p/wavtoac3encoder/

  * Extract them in aften sub-directory in project source tree.

  * Use Miscroft Visual Studio 2010 Beta 1 to build Release binaries.

  * Distribute Release binaries with libaften.dll.

  * You can also build installer using Inno Setup.

==COPYRIGHTS==

WAV to AC3 Encoder, Copyright (C) 2007, 2008, 2009 Wiesław Šoltés

Aften, Copyright (c) 2006-2007 Justin Ruggles <justinruggles@bellsouth.net>
                     2006-2007 Prakash Punnoor <prakash@punnoor.de>

==WEBSITES==

===WAV to AC3 Encoder===

  * http://code.google.com/p/wavtoac3encoder/

===Aften===

  * http://aften.sourceforge.net/
  * http://sourceforge.net/projects/aften/


$Id$