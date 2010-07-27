WAV to AC3 Encoder 4.7 (Using Aften A/52 Audio Encoder)

==ABOUT==

WAV to AC3 Encoder is an audio encoder which generates compressed 
audio streams based on ATSC A/52 specification. This type of audio 
is also known as AC-3 or Dolby® Digital and is one of the audio codecs 
used in DVD-Video content. The program is a simple GUI for Aften A/52 
audio encoder  created by Justin Ruggles. WAV to AC3 Encoder 
project was started on 27th January 2007 by Wiesław Šoltés and is still
maintained and actively developed.

[http://i47.tinypic.com/20qdkcx.png]

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
  * Avisynth scripting support (32 bit & 64 bit).
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

==AviSynth==

To run 64-bit version you need to install avisynth64:
  * http://code.google.com/p/avisynth64/

Additional help and informations is available here: 
  * http://forum.doom9.org/showthread.php?t=152800

Running avisynth requires to setup properly Input Options (at least overrides wave header settings).

You can double-click on avisynth script in the file list to get e.g. number of audio channels.

==Troubleshooting==

Please first check the *.log file in program directory for more information.

To report issues please use: 
  * https://code.google.com/p/wavtoac3encoder/issues/list

For more informations and help please visit:
  * http://forum.doom9.org/showthread.php?t=113074

==SOURCES==

Source code is managed by the Subversion repository.

svn checkout http://wavtoac3encoder.googlecode.com/svn/trunk/ wavtoac3encoder-read-only

==BUILD==

  * Download latest Aften sources from http://code.google.com/p/wavtoac3encoder/

  * Extract them in aften sub-directory in project source tree.

  * Use Miscroft Visual Studio 2008 to build Release binaries.

  * Distribute Release binaries with libaften.dll.

  * You can also build installer using Inno Setup.

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

==DOWNLOAD MIRRORS==

<a href="http://www.softpedia.com/get/Multimedia/Audio/Audio-CD-Rippers-Encoders/WAV-to-AC3-Encoder.shtml"><img border="0" src="http://www.softpedia.com/images/softpedia_download_small.gif"/></a>


$Id$