# WAV to AC3 Encoder

[![Gitter](https://badges.gitter.im/wieslawsoltes/wavtoac3encoder.svg)](https://gitter.im/wieslawsoltes/wavtoac3encoder?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

[![Build status](https://ci.appveyor.com/api/projects/status/xgpr76v102iu86wr/branch/master?svg=true)](https://ci.appveyor.com/project/wieslawsoltes/wavtoac3encoder/branch/master)
[![Github All Releases](https://img.shields.io/github/downloads/wieslawsoltes/wavtoac3encoder/total.svg)](https://github.com/wieslawsoltes/wavtoac3encoder)
[![GitHub release](https://img.shields.io/github/release/wieslawsoltes/wavtoac3encoder.svg)](https://github.com/wieslawsoltes/wavtoac3encoder)
[![Github Releases](https://img.shields.io/github/downloads/wieslawsoltes/wavtoac3encoder/latest/total.svg)](https://github.com/wieslawsoltes/wavtoac3encoder)

WAV to AC3 Encoder an audio encoder which generates compressed audio streams based on [ATSC A/52 specification](https://en.wikipedia.org/wiki/Dolby_Digital).

### Support WAV to AC3 Encoder

[![PayPal - The safer, easier way to pay online!](https://www.paypalobjects.com/en_US/i/btn/btn_donate_SM.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GFQMG36HPYT6W)

### Download WAV to AC3 Encoder

| Platforn              | Type        | Version       | Download                                                                                                                               |
|-----------------------|-------------|---------------|----------------------------------------------------------------------------------------------------------------------------------------|
| Windows 32-bit        | Installer   | 5.5           | [EncWAVtoAC3-5.5.exe](https://github.com/wieslawsoltes/wavtoac3encoder/releases/download/5.5/EncWAVtoAC3-5.5.exe)                      |
| Windows 64-bit        | Installer   | 5.5           | [EncWAVtoAC3-5.5-AMD64.exe](https://github.com/wieslawsoltes/wavtoac3encoder/releases/download/5.5/EncWAVtoAC3-5.5-AMD64.exe)          |
| Windows 32-bit        | Portable    | 5.5           | [EncWAVtoAC3-5.5.zip](https://github.com/wieslawsoltes/wavtoac3encoder/releases/download/5.5/EncWAVtoAC3-5.5.zip)                      |
| Windows 64-bit        | Portable    | 5.5           | [EncWAVtoAC3-5.5-AMD64.zip](https://github.com/wieslawsoltes/wavtoac3encoder/releases/download/5.5/EncWAVtoAC3-5.5-AMD64.zip)          |

### Download Aften

| Platforn              | Type        | Version       | Download                                                                                                                                                               |
|-----------------------|-------------|---------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| -                     | Sources     | git-20091226  | [aften-git-20091226-icl11_1_054-src.zip](https://github.com/wieslawsoltes/wavtoac3encoder/releases/download/5.5/aften-git-20091226-icl11_1_054-src.zip)                |
| Windows 32-bit        | Binaries    | git-20091226  | [aften-git-20091226-icl11_1_054-win32-bin.zip](https://github.com/wieslawsoltes/wavtoac3encoder/releases/download/5.5/aften-git-20091226-icl11_1_054-win32-bin.zip)    |
| Windows 64-bit        | Binaries    | git-20091226  | [aften-git-20091226-icl11_1_054-win64-bin.zip](https://github.com/wieslawsoltes/wavtoac3encoder/releases/download/5.5/aften-git-20091226-icl11_1_054-win64-bin.zip)    |

### Mirrors

[![Softpedia Download Mirror](http://www.softpedia.com/_img/softpedia_100_free.png)](http://www.softpedia.com/get/Multimedia/Audio/Audio-CD-Rippers-Encoders/WAV-to-AC3-Encoder.shtml)

## About

WAV to AC3 Encoder is an audio encoder which generates compressed 
audio streams based on [ATSC A/52 specification](https://en.wikipedia.org/wiki/Dolby_Digital) specification. This type of audio 
is also known as AC-3 or Dolby® Digital and is one of the audio codecs 
used in DVD-Video content. The program is a simple GUI for `Aften` `A/52` 
audio encoder  created by `Justin Ruggle`s. WAV to AC3 Encoder 
project was started on `27th January 2007` by `Wiesław Šoltés`.

## Features

* High-quality output audio files.
* Vary fast encoding.
* Multi-threaded encoding.
* Milti-channel file input.
* Multiple-mono file input.
* Advanved MUX wizard.
* Optimized for modern CPUs.
* Small program size.
* Load and save program settings.
* Load and save encoder presets.
* Avisynth scripting support (32 bit & 64 bit).
* Full UNICODE support.
* Very simple to use.
* Options for advanced users.
* Sources available under GNU GPLv2 license.
* Support for multi-language user interface.
* Portable and installer packages available.

## Command-Line

    Usage: EncWAVtoAC3 [Options]
    Options:
    -h, --help                      Show command-line help message.
    -x, --encode-and-exit           Encode input files and exit.
    -r, --read-only                 Do not save configuration on exit.
    -i, --input-file <file.ext>     Set input files path (*.wav, *.bin, *.pcm, *.raw, *.avs).
    -o, --output-file <file.ext>    Set output file path (*.ac3).
    -d, --output-path <path>        Set default output path (default: input file path is used).
    -c, --load-config <file.ext>    Load program configation (*.config).
    -f, --load-files <file.ext>     Load list of files to encode (*.files, *.mux).
    -p, --load-presets <file.ext>   Load list of preconfigured presets (*.presets).
    -e, --load-engines <file.ext>   Load list of Aften engines (*.engines).
    -m, --mux-wizard                Show standalone MUX Wizard window.
    Notes: AviSynth is not supported under x64 platforms.

## Keyboard Shortcuts

### Main Menu

| Shortcut          | Description                   |
|-------------------|-------------------------------|
| Ctrl+F            | Add Files                     |
| Ctrl+D            | Add Directory                 |
| Ctrl+M            | MUX Wizard                    |
| Ctrl+O            | Load Files List               |
| Ctrl+S            | Save Files List               |
| Ctrl+Shitf+O      | Load Presets                  |
| Ctrl+Shitf+S      | Save Presets                  |
| Alt+F4            | Exit                          |
| Ctrl+Shift+W      | Disable All Warnings          |
| Ctrl+Shift+C      | Save Configuration On Exit    |
| F7                | Load Configuration            |
| F8                | Save Configuration            |
| Ctrl+F1           | Command-Line Help             |
| Ctrl+Shift+W      | Website                       |
| Ctrl+Shift+A      | About                         |

### Files List

| Shortcut          | Description                   |
|-------------------|-------------------------------|
| Ctrl+F            | Add Files                     |
| Ctrl+D            | Add Directory                 |
| Ctrl+M            | MUX Wizard                    |
| Ctrl+O            | Load Files List               |
| Ctrl+S            | Save Files List               |
| Ctrl+Up           | Move Up                       |
| Ctrl+Down         | Move Down                     |
| Del               | Remove Selected Files         |
| Ctrl+Del          | Remove All Files              |

### Main Dialog

| Shortcut          | Description                   |
|-------------------|-------------------------------|
| F5                | Encode                        |
| Ctrl+E            | Edit Engines                  |
| F9                | Add Preset                    |
| Ctrl+Shift+D      | Delete Preset                 |
| Ctrl+Shift+R      | Reset Preset                  |

## Configuration

All confguration files are in plain text format. Each configuration file 
has different structure. Edit the files and check the sources to find 
out more about how this files are created. Each configuration file is 
automaticaly loaded/saved by program every time the program is started/closed.

* Program configuration is stored in `EncWAVtoAC3.config` file.
* Presets for encoder are stored in `EncWAVtoAC3.presets` file.
* Paths to libaften.dll engines are stored in `EncWAVtoAC3.engines` file.
* User interface trasnlation files are located in Lang sub-directory.
* Translation files must be saved with `Unicode` charset and `BOM header`.

## AviSynth Support

To run `64-bit` version you need to install `avisynth64`:
* http://code.google.com/p/avisynth64/

Additional help and informations is available here: 
* http://forum.doom9.org/showthread.php?t=152800

Running `avisynth` requires properly setup Input Options (e.g. override wave header settings).

You can double-click in the file list on avisynth script to get e.g. number of audio channels.

## Troubleshooting

Please first check the `*.log` file in program directory for more information.

To report issues please use: 
* https://github.com/wieslawsoltes/wavtoac3encoder/issues

For more informations and help please visit:
* http://forum.doom9.org/showthread.php?t=113074

## Sources

Source code is available in Git repository.

https://github.com/wieslawsoltes/wavtoac3encoder

## Translations

* English (Wiesław Šoltés)
* German (A. Haack)
* Japanese (http://donkichirou.blog27.fc2.com/blog-entry-277.html)
* Polish (Wiesław Šoltés)

## Build

* Download latest `Aften` sources and binaries from https://github.com/wieslawsoltes/wavtoac3encoder.
* Extract them in `aften` sub-directory in project source tree.
* Use `Microsoft Visual Studio Community 2017` to build `Release` binaries.
* Distribute `Release` binaries with `libaften.dll`.
* You can also build installer using `Inno Setup`.

## Resources

* https://github.com/wieslawsoltes/wavtoac3encoder
* http://aften.sourceforge.net/
* http://sourceforge.net/projects/aften/

## Copyrights

WAV to AC3 Encoder, Copyright (C) 2007-2016 Wiesław Šoltés

Aften, Copyright (c) 2006-2007 Justin Ruggles <justinruggles@bellsouth.net>
                     2006-2007 Prakash Punnoor <prakash@punnoor.de>

## License

WAV to AC3 Encoder
Copyright (C) 2007-2016 Wiesław Šoltés <wieslaw.soltes@gmail.com>

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
