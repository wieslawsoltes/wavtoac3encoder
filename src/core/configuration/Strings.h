#pragma once

#include <map>
#include <string>

namespace config
{
    const std::map<int, const std::wstring> m_Strings =
    {
        //
        // Translation Language Name: in English
        //

        { 0x00000001, L"English" },

        //
        // Translation Language Name: in target language
        //

        { 0x00000002, L"English" },

        //
        // Main Dialog: Main Menu
        //

        { 0x00101001, L"&File" },
        { 0x00101002, L"Add &Files...\tCtrl+F" },
        { 0x00101003, L"Add &Directory...\tCtrl+D" },
        { 0x00101004, L"&MUX Wizard...\tCtrl+M" },
        { 0x00101005, L"L&oad Files List...\tCtrl+O" },
        { 0x00101006, L"Sa&ve Files List...\tCtrl+S" },
        { 0x00101007, L"&Load Presets...\tCtrl+Shitf+O" },
        { 0x00101008, L"&Save Presets...\tCtrl+Shitf+S" },
        { 0x00101009, L"E&xit\tAlt+F4" },

        { 0x00102001, L"&Options" },
        { 0x00102002, L"Disable All &Warnings\tCtrl+Shift+W" },
        { 0x00102003, L"Save Configuration On E&xit\tCtrl+Shift+C" },
        { 0x00102004, L"Loa&d Configuration\tF7" },
        { 0x00102005, L"Sav&e Configuration\tF8" },

        { 0x00103001, L"&Language" },
        { 0x00103002, L"&Default (English)" },

        { 0x00104001, L"&Help" },
        { 0x00104002, L"&Website\tCtrl+Shift+W" },
        { 0x00104003, L"&About...\tCtrl+Shift+A" },

        //
        // Main Dialog: Buttons
        //

        { 0x00201001, L"&Encode" },
        { 0x00201002, L"Rese&t" },
        { 0x00201003, L"&Remove" },
        { 0x00201004, L"A&dd" },
        { 0x00201005, L"&Add files..." },
        { 0x00201006, L"&..." },
        { 0x00201007, L"MU&X Wizard..." },
        { 0x00201008, L"Ed&it" },

        //
        // Main Dialog: Static Text
        //

        { 0x00202001, L"Preset:" },
        { 0x00202002, L"Quality:" },
        { 0x00202003, L"Bitrate:" },
        { 0x00202004, L"Value:" },
        { 0x00202005, L"Sample format:" },
        { 0x00202006, L"Sample rate:" },
        { 0x00202007, L"Channels:" },
        { 0x00202008, L"Engine:" },
        { 0x00202009, L"Threads:" },
        { 0x0020200A, L"M&ultiple mono input" },
        { 0x0020200B, L"Output file:" },
        { 0x0020200C, L"Output path:" },

        //
        // Main Dialog: Files List
        //

        { 0x00203001, L"File path" },
        { 0x00203002, L"File size (bytes)" },

        //
        // Main Dialog: Files List Context-Menu
        //

        { 0x00204001, L"&List" },
        { 0x00204002, L"Add &Files...\tCtrl+F" },
        { 0x00204003, L"Add &Directory...\tCtrl+D" },
        { 0x00204004, L"&MUX Wizard...\tCtrl+M" },
        { 0x00204005, L"L&oad Files List...\tCtrl+O" },
        { 0x00204006, L"Sa&ve Files List...\tCtrl+S" },
        { 0x00204007, L"Move &Up\tCtrl+Up" },
        { 0x00204008, L"Move Dow&n\tCtrl+Down" },
        { 0x00204009, L"Remove &Selected Files\tDel" },
        { 0x0020400A, L"Remove &All Files\tCtrl+Del" },

        //
        // Main Dialog: Settings List
        //

        { 0x00205001, L" Option" },
        { 0x00205002, L" Value" },

        //
        // Main Dialog: Tooltips
        //

        { 0x00206001, L"CBR bitrate in kbps:\n\nCBR mode is selected by default. This option allows for\nsetting the fixed bitrate. The default bitrate depends\non the number of channels (not including LFE).\n1 = 96 kbps\n2 = 192 kbps\n3 = 256 kbps\n4 = 384 kbps\n5 = 448 kbps\n\nVBR quality:\n\nA value 0 to 1023 which corresponds to SNR offset, where\nq=240 equates to an SNR offset of 0. 240 is the default\nvalue. This scale will most likely be replaced in the\nfuture with a better quality measurement." },
        { 0x00206002, L"Enable VBR mode. If unchecked the CBR mode is used instead." },
        { 0x00206003, L"Set currently used encoder preset. You can load/save presets from/to file\nfrom File menu. All presets are automatically loaded/saved from/to text file." },
        { 0x00206004, L"Aften will auto-detect available SIMD instruction sets\nfor your CPU, so you shouldn't need to disable sets\nexplicitly - unless for speed or debugging reasons." },
        { 0x00206005, L"This option enables MMX optimizations (if supported by CPU).\n\n" },
        { 0x00206006, L"This option enables SSE optimizations (if supported by CPU).\n\n" },
        { 0x00206007, L"This option enables SSE2 optimizations (if supported by CPU).\n\n" },
        { 0x00206008, L"This option enables SSE3 optimizations (if supported by CPU).\n\n" },
        { 0x00206009, L"Raw audio input sample format specifies the sample format\nwhen using raw audio input. Using this option forces Aften to\ntreat the input as raw audio. The choices for the\npre-defined sample formats are (default: s16_le):\nu8, s16_le, s16_be, s20_le, s20_be, s24_le, s24_be,\ns32_le, s32_be, float_le, float_be, double_le, double_be" },
        { 0x0020600A, L"Raw audio input sample rate option forces Aften to\ntreat the input as raw audio (default: 48000)." },
        { 0x0020600B, L"Raw audio input channels forces Aften to treat the input as\nraw audio (default: 2)." },
        { 0x0020600C, L"Aften can use multiple threads to speed up encoding.\nBy default, Aften uses one thread for each logical CPU\nyour system has, but you can override this value. A\nvalue of 0 is the default and indicates that Aften\nshould try to detect the number of CPUs." },
        { 0x0020600D, L"Set currently used Aften library. By selecting optimized Aften\nlibrary you can speedup the encoding process." },
        { 0x0020600E, L"Set default output path for encoded files. By default files\nare encoded to the same directory as input files. When using\nmultiple mono input than here is set the output file path." },
        { 0x0020600F, L"Enable multiple mono input mode. By adding multiple mono input\nfiles to the files list (minimum 2, maximum 6) in correct channel\norder the mono input files will be encoded into single ac3 file." },
        { 0x00206010, L"Edit currently available Aften engines." },

        //
        // Main Dialog: Misc.
        //

        { 0x00207001, L"Default" },
        { 0x00207002, L"<Auto>" },
        { 0x00207003, L"<Ignored>" },
        { 0x00207004, L"<Same as input file path>" },
        { 0x00207005, L"<Same as first input file path + output.ac3>" },
        { 0x00207006, L"Supported Files" },
        { 0x00207007, L"Files" },
        { 0x00207008, L"All Files" },
        { 0x00207009, L"Failed to allocate memory for filenames buffer!" },
        { 0x0020700A, L"Fatal Error" },
        { 0x0020700B, L"Add directory with supported input files:" },
        { 0x0020700C, L"Remove all files from list and add files selected in MUX Wizard?\n\nNote: The channel configuration for current preset will be adjusted." },
        { 0x0020700D, L"MUX Wizard" },
        { 0x0020700E, L"Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||" },
        { 0x0020700F, L"Preconfigured Presets (*.presets)|*.presets|All Files (*.*)|*.*||" },
        { 0x00207010, L"Error" },
        { 0x00207011, L"Add at least one file to the file list!" },
        { 0x00207012, L"Supported are minimum 1 and maximum 6 mono input files!" },
        { 0x00207013, L"Failed to load libaften.dll dynamic library!" },
        { 0x00207014, L"Disable 'Multiple mono input' mode in order to use Avisynth scripts!" },
        { 0x00207015, L"Invalid output path!" },
        { 0x00207016, L"Invalid output file!" },
        { 0x00207017, L"Failed to create output path!" },
        { 0x00207018, L"Encoded %d mono files in %s (%0.3lf sec)" },
        { 0x00207019, L"Encoded %d file%s in %s (%0.3lf sec)" },
        { 0x0020701A, L"s" },
        { 0x0020701B, L"New preset" },
        { 0x0020701C, L"AC3 Files (*.ac3)|*.ac3|All Files (*.*)|*.*||" },
        { 0x0020701D, L"Select default output path:" },
        { 0x0020701E, L"Failed to load" },
        { 0x0020701F, L"library" },
        { 0x00207020, L"Loaded" },
        { 0x00207021, L"version" },
        { 0x00207022, L"Failed to initialize Avisynth" },
        { 0x00207023, L"Sample format" },
        { 0x00207024, L"unknown" },
        { 0x00207025, L"Sample rate" },
        { 0x00207026, L"Channels" },
        { 0x00207027, L"Audio samples" },
        { 0x00207028, L"Decoded size" },
        { 0x00207029, L"AVS File Properties" },
        { 0x0020702A, L"Error while searching for files!" },

        //
        // Main Dialog: Advanced Encoder Options Groups
        //

        { 0x00208001, L"Encoding options" },
        { 0x00208002, L"Bitstream info metadata" },
        { 0x00208003, L"Dynamic range compression and dialog normalization" },
        { 0x00208004, L"Input options" },
        { 0x00208005, L"Input filters" },
        { 0x00208006, L"Alternate bit stream syntax" },

        //
        // Main Dialog: (1) Encoding options
        //

        { 0x00301001, L"Fast bit allocation" },
        { 0x00301002, L"Fast bit allocation is a less-accurate search method\nfor CBR bit allocation. It only narrows down the SNR\nvalue to within 16 of the optimal value. The result\nis lower overall quality, but faster encoding. This\nmay not give the same results each time when using\nparallel encoding." },
        { 0x00301003, L"More accurate encoding (default)" },
        { 0x00301004, L"Faster encoding" },

        { 0x00302001, L"Exponent strategy search size" },
        { 0x00302002, L"The encoder determines the best combination of\nexponent strategies for a frame by searching through\na list of pre-defined exponent strategies. This option\ncontrols the size of the list to be searched. The\nvalue can range from 1 (lower quality but faster) to\n32 (higher quality but slower). The default value is 8." },
        { 0x00302003, L"1 (lower quality but faster)" },
        { 0x00302004, L"8 (default)" },
        { 0x00302005, L"32 (higher quality but slower)" },

        { 0x00303001, L"Start-of-stream padding" },
        { 0x00303002, L"The AC-3 format uses an overlap/add cycle for encoding\neach block. By default, Aften pads the delay buffer\nwith a block of silence to avoid inaccurate encoding\nof the first frame of audio. If this behavior is not\nwanted, it can be disabled. The pad value can be a\n1 (default) to use padding or 0 to not use padding." },
        { 0x00303003, L"No padding" },
        { 0x00303004, L"256 samples of padding (default)" },

        { 0x00304001, L"Bandwidth" },
        { 0x00304002, L"The bandwidth setting corresponds to the high-frequency\ncutoff. Specifically, it sets the highest frequency bin\nwhich is encoded. The AC-3 format uses a 512-point MDCT\nwhich gives 256 frequency levels from 0 to 1/2 of the\nsamplerate. The formula to give the number of coded\nfrequency bins from bandwidth setting is:\n(w * 3) + 73, which gives a range of 73 to 253\nThere are 2 special values, -1 and -2.\nWhen -1 is used, Aften automatically selects what\nit thinks is an appropriate bandwidth. This is the\ndefault setting.\nWhen -2 is used, a bandwidth is chosen for each frame\nbased on CBR frame size and a target quality of 240.\nVariable bandwidth cannot be used with VBR mode." },
        { 0x00304003, L"Fixed adaptive bandwidth (default)" },
        { 0x00304004, L"Variable adaptive bandwidth" },
        { 0x00304005, L"0 (28% of full bandwidth)" },
        { 0x00304006, L"60 (99% of full bandwidth)" },

        { 0x00305001, L"Minimum bandwidth" },
        { 0x00305002, L"For variable bandwidth mode (-2), this option sets the\nminimum value for the bandwidth code. This allows the\nuser to avoid a harsh cutoff frequency by sacrificing\ngeneral audio quality. The default value is 0." },
        { 0x00305003, L"0 (default)" },

        { 0x00306001, L"Maximum bandwidth" },
        { 0x00306002, L"For variable bandwidth mode (-2), this option sets the\nmaximum value for the bandwidth code. This can be used\nto speed up encoding by using a lower value than 60,\nwhich is the default." },
        { 0x00306003, L"60 (default)" },

        { 0x00307001, L"Stereo rematrixing" },
        { 0x00307002, L"Using stereo rematrixing can increase quality by\nremoving redundant information between the left and\nright channels. This technique is common in audio\nencoding, and is sometimes called mid/side encoding.\nWhen this setting is turned on, Aften adaptively turns\nrematrixing on or off for each of 4 frequency bands for\neach block. When this setting is turned off,\nrematrixing is not used for any blocks. The default\nvalue is 1." },
        { 0x00307003, L"Independent L+R channels" },
        { 0x00307004, L"Mid/side rematrixing (default)" },

        { 0x00308001, L"Block switching" },
        { 0x00308002, L"The AC-3 format allows for 2 different types of MDCT\ntransformations to translate from time-domain to\nfrequency-domain. The default is a 512-point transform,\nwhich gives better frequency resolution. There is also\na 256-point transform, which gives better time\nresolution. The specification gives a suggested method\nfor determining when to use the 256-point transform.\nWhen block switching is turned on, Aften uses the spec\nmethod for selecting the 256-point MDCT. When it is\nturned off, only the 512-point MDCT is used, which is\nfaster. Block switching is turned off by default." },
        { 0x00308003, L"Use only 512-point MDCT (default)" },
        { 0x00308004, L"Selectively use 256-point MDCT" },

        //
        // Main Dialog: (2) Bitstream info metadata
        //

        { 0x00401001, L"Center mix level" },
        { 0x00401002, L"When three front channels are in use, this code\nindicates the nominal down mix level of the center\nchannel with respect to the left and right channels.\n0 = -3.0 dB (default)\n1 = -4.5 dB\n2 = -6.0 dB" },
        { 0x00401003, L"-3.0 dB (default)" },

        { 0x00402001, L"Surround mix level" },
        { 0x00402002, L"If surround channels are in use, this code indicates\nthe nominal down mix level of the surround channels.\n0 = -3 dB (default)\n1 = -6 dB\n2 = 0" },
        { 0x00402003, L"-3 dB (default)" },

        { 0x00403001, L"Dolby Surround mode" },
        { 0x00403002, L"When operating in the two channel mode, this code\nindicates whether or not the program has been encoded in\nDolby Surround. This information is not used by the\nAC-3 decoder, but may be used by other portions of the\naudio reproduction equipment.\n0 = not indicated (default)\n1 = not Dolby surround encoded\n2 = Dolby surround encoded" },
        { 0x00403003, L"Not indicated (default)" },
        { 0x00403004, L"Not Dolby surround encoded" },
        { 0x00403005, L"Dolby surround encoded" },

        //
        // Main Dialog: (3) Dynamic range compression and dialog normalization
        //

        { 0x00501001, L"Dialog normalization" },
        { 0x00501002, L"The dialog normalization value sets the average dialog\nlevel. The value is typically constant for a particular\naudio program. The decoder has a target output dialog\nlevel of -31dB, so if the dialog level is specified as\nbeing -31dB already (default), the output volume is not\naltered. Otherwise, the overall output volume is\ndecreased so that the dialog level is adjusted down to\n-31dB." },
        { 0x00501003, L"31 (default)" },

        { 0x00502001, L"Dynamic Range Compression profile" },
        { 0x00502002, L"Dynamic Range Compression allows for the final output\ndynamic range to be limited without sacrificing quality.\nThe full dynamic range audio is still encoded, but a\ncode is given for each block which tells the decoder to\nadjust the output volume for that block. The encoder\nmust analyze the input audio to determine the best way\nto compress the dynamic range based on the loudness and\ntype of input (film, music, speech).\n0 = Film Light\n1 = Film Standard\n2 = Music Light\n3 = Music Standard\n4 = Speech\n5 = None (default)" },
        { 0x00502003, L"Film Light" },
        { 0x00502004, L"Film Standard" },
        { 0x00502005, L"Music Light" },
        { 0x00502006, L"Music Standard" },
        { 0x00502007, L"Speech" },
        { 0x00502008, L"None (default)" },

        //
        // Main Dialog: (4) Input options
        //

        { 0x00601001, L"Audio coding mode (overrides wav header)" },
        { 0x00601002, L"The acmod and lfe options allow the user to explicitly select the\ndesired channel layout. This only controls the interpretation\nof the input, so no downmixing or upmixing is done.\n 0 = 1+1 (Ch1,Ch2)\n1 = 1/0 (C)\n2 = 2/0 (L,R)\n3 = 3/0 (L,R,C)\n4 = 2/1 (L,R,S)\n5 = 3/1 (L,R,C,S)\n6 = 2/2 (L,R,SL,SR)\n7 = 3/2 (L,R,C,SL,SR)" },

        { 0x00602001, L"Specify use of LFE channel (overrides wav header)" },
        { 0x00602002, L"The acmod and lfe options allow the user to explicitly select the\ndesired channel layout. This only controls the interpretation\nof the input, so no downmixing or upmixing is done.\n 0 = LFE channel is not present\n1 = LFE channel is present" },
        { 0x00602003, L"LFE channel is not present" },
        { 0x00602004, L"LFE channel is present" },

        { 0x00603001, L"Specify channel configuration (overrides wav header)" },
        { 0x00603002, L"The chconfig option allow the user to explicitly select the\ndesired channel layout. This only controls the interpretation\nof the input, so no downmixing or upmixing is done.\n 1+1 = (Ch1,Ch2)\n1/0 = (C)\n2/0 = (L,R)\n3/0 = (L,R,C)\n2/1 = (L,R,S)\n3/1 = (L,R,C,S)\n2/2 = (L,R,SL,SR)\n3/2 = (L,R,C,SL,SR)\nAdding +LFE indicates use of the LFE channel" },

        { 0x00604001, L"Channel mapping order of input audio" },
        { 0x00604002, L"Some programs create WAVE files which use a channel\nmapping other than the standard WAVE mapping. This\noption allows the user to specify if the input file\nuses WAVE, AC-3, or MPEG channel mapping. The MPEG\nchannel mapping is used by DTS and by MPEG-2/4 formats\nsuch as MP2 and AAC.\n0 = WAVE mapping (default)\n1 = AC-3 mapping\n2 = MPEG mapping" },
        { 0x00604003, L"WAV mapping (default)" },
        { 0x00604004, L"AC-3 mapping" },
        { 0x00604005, L"MPEG mapping" },

        { 0x00605001, L"Read input WAVE audio data until the end-of-file" },
        { 0x00605002, L"This overrides the data size in the WAVE header, and\ncan be useful for streaming input or files larger than\n4 GB.\n0 = use data size in header (default)\n1 = read data until end-of-file" },
        { 0x00605003, L"Use data size in header (default)" },
        { 0x00605004, L"Read data until end-of-file" },

        //
        // Main Dialog: (5) Input filters
        //

        { 0x00701001, L"Specify use of the bandwidth low-pass filter" },
        { 0x00701002, L"The bandwidth low-pass filter pre-filters the input\naudio before converting to frequency-domain. This\nsmooths the cutoff frequency transition for slightly\nbetter quality.\n0 = do not apply filter (default)\n1 = apply filter" },
        { 0x00701003, L"Do not apply filter (default)" },
        { 0x00701004, L"Apply filter" },

        { 0x00702001, L"Specify use of the DC high-pass filter" },
        { 0x00702002, L"The DC high-pass filter is listed as optional by the\nAC-3 specification. The implementation, as suggested,\nis a single pole filter at 3 Hz.\n0 = do not apply filter (default)\n1 = apply filter" },
        { 0x00702003, L"Do not apply filter (default)" },
        { 0x00702004, L"Apply filter" },

        { 0x00703001, L"Specify use of the LFE low-pass filter" },
        { 0x00703002, L"The LFE low-pass filter is recommended by the AC-3\nspecification. The cutoff is 120 Hz. The specification\nrecommends an 8th order elliptic filter, but instead,\nAften uses a Butterworth 2nd order cascaded direct\nform II filter.\n0 = do not apply filter (default)\n1 = apply filter" },
        { 0x00703003, L"Do not apply filter (default)" },
        { 0x00703004, L"Apply filter" },

        //
        // Main Dialog: (6) Alternate bit stream syntax
        //

        { 0x00801001, L"Specify use of extended bitstream info 1" },
        { 0x00801002, L"Extended bitstream info 1 contains the dmixmod,\nltrtcmix, ltrtsmix, lorocmix, and lorosmix fields. If\nthis option is turned on, all these values are written\nto the output stream.\n0 = do not write xbsi1\n1 = write xbsi1" },
        { 0x00801003, L"Do not write xbsi1" },
        { 0x00801004, L"Write xbsi1" },

        { 0x00802001, L"Preferred stereo downmix mode" },
        { 0x00802002, L"This code indicates the type of stereo downmix preferred\nby the mastering engineer, and can be optionally used,\noverridden, or ignored by the decoder.\n0 = not indicated (default)\n1 = Lt/Rt downmix preferred\n2 = Lo/Ro downmix preferred" },
        { 0x00802003, L"Not indicated (default)" },
        { 0x00802004, L"Lt/Rt downmix preferred" },
        { 0x00802005, L"Lo/Ro downmix preferred" },

        { 0x00803001, L"Lt/Rt center mix level" },
        { 0x00803002, L"This code indicates the nominal down mix level of the\ncenter channel with respect to the left and right\nchannels in an Lt/Rt downmix.\n0 = +3.0 dB\n1 = +1.5 dB\n2 =  0.0 dB\n3 = -1.5 dB\n4 = -3.0 dB (default)\n5 = -4.5 dB\n6 = -6.0 dB\n7 = -inf dB" },
        { 0x00803003, L"-3.0 dB (default)" },

        { 0x00804001, L"Lt/Rt surround mix level" },
        { 0x00804002, L"This code indicates the nominal down mix level of the\nsurround channels with respect to the left and right\nchannels in an Lt/Rt downmix.\n0 = +3.0 dB\n1 = +1.5 dB\n2 =  0.0 dB\n3 = -1.5 dB\n4 = -3.0 dB (default)\n5 = -4.5 dB\n6 = -6.0 dB\n7 = -inf dB" },
        { 0x00804003, L"-3.0 dB (default)" },

        { 0x00805001, L"Lo/Ro center mix level" },
        { 0x00805002, L"This code indicates the nominal down mix level of the\ncenter channel with respect to the left and right\nchannels in an Lo/Ro downmix.\n0 = +3.0 dB\n1 = +1.5 dB\n2 =  0.0 dB\n3 = -1.5 dB\n4 = -3.0 dB (default)\n5 = -4.5 dB\n6 = -6.0 dB\n7 = -inf dB" },
        { 0x00805003, L"-3.0 dB (default)" },

        { 0x00806001, L"Lo/Ro surround mix level" },
        { 0x00806002, L"This code indicates the nominal down mix level of the\nsurround channels with respect to the left and right\nchannels in an Lo/Ro downmix.\n0 = +3.0 dB\n1 = +1.5 dB\n2 =  0.0 dB\n3 = -1.5 dB\n4 = -3.0 dB (default)\n5 = -4.5 dB\n6 = -6.0 dB\n7 = -inf dB" },
        { 0x00806003, L"-3.0 dB (default)" },

        { 0x00807001, L"Specify use of extended bitstream info 2" },
        { 0x00807002, L"Extended bitstream info 2 contains the dsurexmod,\ndheadphon, and adconvtyp fields. If this option is\nturned on, all these values are written to the output\nstream. These options are not used by the AC-3 decoder,\nbut may be used by other portions of the audio\nreproduction equipment.\n0 = do not write xbsi2\n1 = write xbsi2" },
        { 0x00807003, L"Do not write xbsi2" },
        { 0x00807004, L"Write xbsi2" },

        { 0x00808001, L"Dolby Surround EX mode" },
        { 0x00808002, L"This code indicates whether or not the program has been\nencoded in Dolby Surround EX.\n0 = not indicated (default)\n1 = Not Dolby Surround EX encoded\n2 = Dolby Surround EX encoded" },
        { 0x00808003, L"Not indicated (default)" },
        { 0x00808004, L"Not Dolby Surround EX encoded" },
        { 0x00808005, L"Dolby Surround EX encoded" },

        { 0x00809001, L"Dolby Headphone mode" },
        { 0x00809002, L"This code indicates whether or not the program has been\nDolby Headphone-encoded.\n0 = not indicated (default)\n1 = Not Dolby Headphone encoded\n2 = Dolby Headphone encoded" },
        { 0x00809003, L"Not indicated (default)" },
        { 0x00809004, L"Not Dolby Headphone encoded" },
        { 0x00809005, L"Dolby Headphone encoded" },

        { 0x0080A001, L"A/D converter type" },
        { 0x0080A002, L"This code indicates the type of A/D converter technology\nused to capture the PCM audio.\n0 = Standard (default)\n1 = HDCD" },
        { 0x0080A003, L"Standard (default)" },
        { 0x0080A004, L"HDCD" },

        //
        // About Dialog
        //

        { 0x00901001, L"&OK" },
        { 0x00901002, L"About WAV to AC3 Encoder" },
        { 0x00901003, L"This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; version 2 of the License." },
        { 0x00901004, L"Version" },

        //
        // Work Dialog
        //

        { 0x00A01001, L"Encoding..." },
        { 0x00A01002, L"&Cancel" },
        { 0x00A01003, L"From:" },
        { 0x00A01004, L"To:" },
        { 0x00A01005, L"Elapsed time:" },
        { 0x00A01006, L"Total elapsed time:" },
        { 0x00A0100A, L"Fatal Error" },
        { 0x00A0100B, L"Failed to create worker thread!" },
        { 0x00A0100C, L"Encoding file %d of %d" },
        { 0x00A0100D, L"Encoding %d mono files" },

        //
        // Work Dialog: Current Job Info
        //

        { 0x00A02001, L"?" },
        { 0x00A02002, L"?-channel" },
        { 0x00A02003, L"Unsigned" },
        { 0x00A02004, L"Signed" },
        { 0x00A02005, L"Floating-point" },
        { 0x00A02006, L"[unsupported type]" },
        { 0x00A02007, L"1.1-channel" },
        { 0x00A02008, L"2.1-channel" },
        { 0x00A02009, L"3.1-channel" },
        { 0x00A0200A, L"4.1-channel" },
        { 0x00A0200B, L"5.1-channel" },
        { 0x00A0200C, L"multi-channel with LFE" },
        { 0x00A0200D, L"mono" },
        { 0x00A0200E, L"stereo" },
        { 0x00A0200F, L"3-channel" },
        { 0x00A02010, L"4-channel" },
        { 0x00A02011, L"5-channel" },
        { 0x00A02012, L"6-channel" },
        { 0x00A02013, L"multi-channel" },
        { 0x00A02014, L"unknown" },
        { 0x00A02015, L"little-endian" },
        { 0x00A02016, L"big-endian" },
        { 0x00A02017, L"Avisynth: Raw PCM Floating-point 32-bit little-endian" },
        { 0x00A02018, L"dual mono (1+1)" },
        { 0x00A02019, L"mono (1/0)" },
        { 0x00A0201A, L"stereo (2/0)" },
        { 0x00A0201B, L"NONE" },
        { 0x00A0201C, L"Threads: Auto" },
        { 0x00A0201D, L"Threads:" },

        //
        // Engines Dialog
        //

        { 0x00B01001, L"Engines Editor" },
        { 0x00B01002, L"Engine" },
        { 0x00B01003, L"Name:" },
        { 0x00B01004, L"Path:" },
        { 0x00B01005, L"&..." },
        { 0x00B01006, L"&Import..." },
        { 0x00B01007, L"&Export..." },
        { 0x00B01008, L"&Add" },
        { 0x00B01009, L"&Remove" },
        { 0x00B0100A, L"&OK" },
        { 0x00B0100B, L"&Cancel" },
        { 0x00B0100C, L"Name" },
        { 0x00B0100D, L"Path" },
        { 0x00B0100E, L"DLL Files (*.dll)|*.dll|All Files (*.*)|*.*||" },
        { 0x00B0100F, L"Aften Engines (*.engines)|*.engines|All Files (*.*)|*.*||" },

        //
        // MUX Dialog
        //

        { 0x00C01001, L"MUX Wizard" },
        { 0x00C01002, L"Channel config:" },
        { 0x00C01003, L"&Import MUX..." },
        { 0x00C01004, L"&Export MUX..." },
        { 0x00C01005, L"&OK" },
        { 0x00C01006, L"&Cancel" },
        { 0x00C01007, L"CLEAR" },
        { 0x00C01008, L"Specify channel configuration:\n1+1 = (Ch1,Ch2)\n1/0 = (C)\n2/0 = (L,R)\n3/0 = (L,R,C)\n2/1 = (L,R,S)\n3/1 = (L,R,C,S)\n2/2 = (L,R,SL,SR)\n3/2 = (L,R,C,SL,SR)" },
        { 0x00C01009, L"Indicates use of the LFE channel." },
        { 0x00C0100A, L"Front Left Channel" },
        { 0x00C0100B, L"Front Right Channel" },
        { 0x00C0100C, L"Front Center Channel" },
        { 0x00C0100D, L"Low Frequency Effect Channel" },
        { 0x00C0100E, L"Surround Left Channel" },
        { 0x00C0100F, L"Surround Right Channel" },
        { 0x00C01010, L"Surround Channel" },
        { 0x00C01011, L"Supported Files (*.files;*.mux)|*.files;*.mux|Files List (*.files)|*.files|MUX Files (*.mux)|*.mux|All Files (*.*)|*.*||" }
    };
}
