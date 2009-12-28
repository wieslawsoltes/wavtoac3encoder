/********************************************************************************
 * Copyright (C) 2007 by Prakash Punnoor                                        *
 * prakash@punnoor.de                                                           *
 *                                                                              *
 * This library is free software; you can redistribute it and/or                *
 * modify it under the terms of the GNU Lesser General Public                   *
 * License as published by the Free Software Foundation; either                 *
 * version 2 of the License                                                     *
 *                                                                              *
 * This library is distributed in the hope that it will be useful,              *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU            *
 * Lesser General Public License for more details.                              *
 *                                                                              *
 * You should have received a copy of the GNU Lesser General Public             *
 * License along with this library; if not, write to the Free Software          *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA *
 ********************************************************************************/
using System;

namespace Aften
{
	/// <summary>
	/// Some helpful size constants
	/// </summary>
	public static class A52Sizes
	{
		/// <summary>
		/// Maximum encoded ac3 frame size
		/// </summary>
		public const int MaximumCodedFrameSize = 3840;

		/// <summary>
		/// Samples per ac3 frame per channel
		/// </summary>
		public const int SamplesPerFrame = 1536;
	}

	/// <summary>
	/// Aften's mode of operation
	/// </summary>
	internal enum AftenOperationMode
	{
		AFTEN_ENCODE,
		AFTEN_TRANSCODE
	}

	/// <summary>
	/// Aften Encoding Mode
	/// </summary>
	public enum EncodingMode
	{
		/// <summary>
		/// CBR
		/// </summary>
		Cbr = 0,
		/// <summary>
		/// VBR
		/// </summary>
		Vbr
	}

	/// <summary>
	/// Floating-Point Data Types
	/// </summary>
	public enum FloatType
	{
		/// <summary>
		/// Double
		/// </summary>
		Double,
		/// <summary>
		/// Float
		/// </summary>
		Float
	}

	/// <summary>
	/// Audio Sample Formats
	/// </summary>
	public enum A52SampleFormat
	{
		/// <summary>
		/// Unsigned bytes
		/// </summary>
		UInt8 = 0,
		/// <summary>
		/// Signed 16bit Words
		/// </summary>
		Int16,
		/// <summary>
		/// Signed 20bit Words
		/// </summary>
		Int20,
		/// <summary>
		/// Signed 24bit Words
		/// </summary>
		Int24,
		/// <summary>
		/// Signed Doublewords
		/// </summary>
		Int32,
		/// <summary>
		/// Single precision floating point, normalized to [-1, 1]
		/// </summary>
		Float,
		/// <summary>
		/// Double precision floating point, normalized to [-1, 1]
		/// </summary>
		Double,
		/// <summary>
		/// Signed bytes
		/// </summary>
		Int8
	}

	/// <summary>
	/// Dynamic Range Profiles
	/// </summary>
	public enum DynamicRangeProfile
	{
		/// <summary>
		/// Light dynamic range compression for films
		/// </summary>
		FilmLight = 0,
		/// <summary>
		/// Standard dynamic range compression for films
		/// </summary>
		FilmStandard,
		/// <summary>
		/// Light dynamic range compression for music
		/// </summary>
		MusicLight,
		/// <summary>
		/// Standard dynamic range compression for music
		/// </summary>
		MusicStandard,
		/// <summary>
		/// Dynamic range compression for ppeech
		/// </summary>
		Speech,
		/// <summary>
		/// No dynamic range compression
		/// </summary>
		None
	}

	/// <summary>
	/// Audio Coding Mode (acmod) options
	/// </summary>
	public enum AudioCodingMode
	{
		/// <summary>
		/// Dual Mono
		/// </summary>
		DualMono = 0,
		/// <summary>
		/// Mono
		/// </summary>
		Mono,
		/// <summary>
		/// Stereo
		/// </summary>
		Stereo,
		/// <summary>
		/// 3 front channels, no rear channels
		/// </summary>
		Front3Rear0,
		/// <summary>
		/// 2 front channels, 1 rear channel
		/// </summary>
		Front2Rear1,
		/// <summary>
		/// 3 front channels, 1 rear channel
		/// </summary>
		Front3Rear1,
		/// <summary>
		/// 2 front channels, 2 rear channels
		/// </summary>
		Front2Rear2,
		/// <summary>
		/// 3 front channels, 2 rear channels
		/// </summary>
		Front3Rear2
	}

	/// <summary>
	/// SIMD instruction sets
	/// </summary>
	public struct SimdInstructions
	{
		/// <summary>
		/// MMX
		/// </summary>
		public bool Mmx;
		/// <summary>
		/// SSE
		/// </summary>
		public bool Sse;
		/// <summary>
		/// SSE2
		/// </summary>
		public bool Sse2;
		/// <summary>
		/// SSE3
		/// </summary>
		public bool Sse3;
		/// <summary>
		/// SSSE3
		/// </summary>
		public bool Ssse3;
		/// <summary>
		/// AMD 3DNow!
		/// </summary>
		public bool Amd3Dnow;
		/// <summary>
		/// AMD 3DNow! Extended
		/// </summary>
		public bool Amd3Dnowext;
		/// <summary>
		/// AMD SSE MMX Extensions
		/// </summary>
		public bool AmdSseMmx;
		/// <summary>
		/// PowerPC Altivec
		/// </summary>
		public bool Altivec;
	}

	/// <summary>
	/// Performance related parameters
	/// </summary>
	public struct SystemParameters
	{
		/// <summary>
		/// Number of threads
		/// How many threads should be used.
		/// Default value is 0, which indicates detecting number of CPUs.
		/// Maximum value is AFTEN_MAX_THREADS.
		/// </summary>
		public int ThreadsCount;

		/// <summary>
		/// Available SIMD instruction sets; shouldn't be modified
		/// </summary>
		public SimdInstructions AvailableSimdInstructions;

		/// <summary>
		/// Wanted SIMD instruction sets
		/// </summary>
		public SimdInstructions WantedSimdInstructions;
	}

	/// <summary>
	/// Parameters which affect encoded audio output
	/// </summary>
	public struct EncodingParameters
	{
		/// <summary>
		/// Bitrate selection mode.
		/// AFTEN_ENC_MODE_CBR : constant bitrate
		/// AFTEN_ENC_MODE_VBR : variable bitrate
		/// default is CBR
		/// </summary>
		public EncodingMode EncodingMode;

		/// <summary>
		/// Stereo rematrixing option.
		/// Set to 0 to disable stereo rematrixing, 1 to enable it.
		/// default is 1
		/// </summary>
		public bool UseRematrixing;

		/// <summary>
		/// Block switching option.
		/// Set to 0 to disable block switching, 1 to enable it.
		/// default is 0
		/// </summary>
		public bool UseBlockSwitching;

		/// <summary>
		/// DC high-pass filter option.
		/// Set to 0 to disable the filter, 1 to enable it.
		/// default is 0
		/// </summary>
		public bool UseDCFilter;

		/// <summary>
		/// Bandwidth low-pass filter option.
		/// Set to 0 to disable the, 1 to enable it.
		/// This option cannot be enabled with variable bandwidth mode (bwcode=-2)
		/// default is 0
		/// </summary>
		public bool UseBandwithFilter;

		/// <summary>
		/// LFE low-pass filter option.
		/// Set to 0 to disable the filter, 1 to enable it.
		/// This limits the LFE bandwidth, and can only be used if the input audio
		/// has an LFE channel.
		/// default is 0
		/// </summary>
		public bool UseLfeFilter;

		/// <summary>
		/// Constant bitrate.
		/// This option sets the bitrate for CBR encoding mode.
		/// It can also be used to set the maximum bitrate for VBR mode.
		/// It is specified in kbps. Only certain bitrates are valid:
		///   0,  32,  40,  48,  56,  64,  80,  96, 112, 128,
		/// 160, 192, 224, 256, 320, 384, 448, 512, 576, 640
		/// default is 0
		/// For CBR mode, this selects bitrate based on the number of channels.
		/// For VBR mode, this sets the maximum bitrate to 640 kbps.
		/// </summary>
		public int Bitrate;

		/// <summary>
		/// VBR Quality.
		/// This option sets the target quality for VBR encoding mode.
		/// The range is 0 to 1023 and corresponds to the SNR offset.
		/// default is 240
		/// </summary>
		public int Quality;

		/// <summary>
		/// Bandwidth code.
		/// This option determines the cutoff frequency for encoded bandwidth.
		/// 0 to 60 corresponds to a cutoff of 28.5% to 98.8% of the full bandwidth.
		/// -1 is used for constant adaptive bandwidth. Aften selects a good value
		///    based on the quality or bitrate parameters.
		/// -2 is used for variable adaptive bandwidth. Aften selects a value for
		///    each frame based on the encoding quality level for that frame.
		/// default is -1
		/// </summary>
		public int BandwidthCode;

		/// <summary>
		/// Bit Allocation speed/accuracy
		/// This determines how accurate the bit allocation search method is.
		/// Set to 0 for better quality
		/// Set to 1 for faster encoding
		/// default is 0
		/// </summary>
		public bool UseFastBitAllocation;

		/// <summary>
		/// Exponent Strategy search size
		/// This determines how many exponent strategy sets to search through
		/// to find the best combination.
		/// minimum is 1 (fixed strategy, lower quality, faster encoding)
		/// maximum is 32 (higher quality, slower encoding)
		/// default is 8
		/// </summary>
		public int ExponentStrategySearchSize;

		/// <summary>
		/// Dynamic Range Compression profile
		/// This determines which DRC profile to use.
		/// Film Light:     DYNRNG_PROFILE_FILM_LIGHT
		/// Film Standard:  DYNRNG_PROFILE_FILM_STANDARD
		/// Music Light:    DYNRNG_PROFILE_MUSIC_LIGHT
		/// Music Standard: DYNRNG_PROFILE_MUSIC_STANDARD
		/// Speech:         DYNRNG_PROFILE_SPEECH,
		/// None:           DYNRNG_PROFILE_NONE
		/// default is None
		/// </summary>
		public DynamicRangeProfile DynamicRangeProfile;

		/// <summary>
		/// Minimum bandwidth code.
		/// For use with variable bandwidth mode, this option determines the
		/// minimum value for the bandwidth code.
		/// default is 0.
		/// </summary>
		public int MinimumBandwidthCode;

		/// <summary>
		/// Maximum bandwidth code.
		/// For use with variable bandwidth mode, this option determines the
		/// maximum value for the bandwidth code.
		/// default is 60.
		/// </summary>
		public int MaximumBandwidthCode;
	}

	/// <summary>
	/// Metadata parameters
	/// See the A/52 specification for details regarding the metadata.
	/// </summary>
	public struct Metadata
	{
		/// <summary>
		/// Center mix level
		///  </summary>
		public int CenterMixLevel;

		/// <summary> 
		/// Surround mix level
		/// </summary>
		public int SurroundMixLevel;

		/// <summary>
		/// Dolby(R) Surround mode
		/// </summary>
		public int DolbySurroundMode;

		/// <summary> 
		/// Dialog normalization
		/// </summary>
		public int DialogNormalization;

		/// <summary> 
		/// Extended bit stream info 1 exists 
		/// </summary>
		public bool HasExtendedBitstreamInfo1;

		/// <summary> 
		/// Preferred downmix mode
		/// </summary>
		public int DownMixMode;

		/// <summary> 
		/// LtRt center mix level 
		/// </summary>
		public int LtRtCenterMixLevel;

		/// <summary> 
		/// LtRt surround mix level
		/// </summary>
		public int LtRtSurroundMixLevel;

		/// <summary>
		/// LoRo center mix level
		/// </summary>
		public int LoRoCenterMixLevel;

		/// <summary>
		/// LoRo surround mix level 
		/// </summary>
		public int LoRoSurroundMixLevel;

		/// <summary>
		/// Extended bit stream info 2 exists 
		/// </summary>
		public bool HasExtendedBitstreamInfo2;

		/// <summary> 
		/// Dolby(R) Surround EX mode 
		/// </summary>
		public int DolbySurroundEXMode;

		/// <summary> 
		/// Dolby(R) Headphone mode 
		/// </summary>
		public int DolbyHeadphoneMode;

		/// <summary> 
		/// A/D converter type 
		/// </summary>
		public int ADConverterType;

	}

	/// <summary>
	/// Values in this structure are updated by Aften during encoding.
	/// They give information about the previously encoded frame.
	/// </summary>
	public struct Status
	{
		/// <summary>
		/// Quality
		/// </summary>
		public int Quality;

		/// <summary>
		/// BitRate
		/// </summary>
		public int BitRate;

		/// <summary>
		/// BandwidthCode
		/// </summary>
		public int BandwidthCode;
	}

	/// <summary>
	/// libaften public encoding context
	/// </summary>
	public struct EncodingContext
	{
		/// <summary>
		/// EncodingParameters
		/// </summary>
		public EncodingParameters EncodingParameters;

		/// <summary>
		/// Metadata
		/// </summary>
		public Metadata Metadata;

		/// <summary>
		/// Status
		/// </summary>
		public Status Status;

		/// <summary>
		/// SystemParameters
		/// </summary>
		public SystemParameters SystemParameters;

		/// <summary>
		/// Aften's mode of operation
		/// </summary>
		internal AftenOperationMode Mode;

		/// <summary>
		/// Verbosity level.
		/// 0 is quiet mode. 1 and 2 are more verbose.
		/// default is 1
		/// </summary>
		public int Verbosity;

		/// <summary>
		/// Total number of channels in the input stream.
		/// </summary>
		public int Channels;

		/// <summary>
		/// Audio coding mode (channel configuration).
		/// There are utility functions to set this if you don't know the proper
		/// value.
		/// </summary>
		public AudioCodingMode AudioCodingMode;

		/// <summary>
		/// Indicates that there is an LFE channel present.
		/// There are utility functions to set this if you don't know the proper
		/// value.
		/// </summary>
		public bool HasLfe;

		/// <summary>
		/// Audio sample rate in Hz
		/// </summary>
		public int SampleRate;

		/// <summary>
		/// Audio sample format
		/// default: Int16
		/// </summary>
		internal A52SampleFormat SampleFormat;

	#pragma warning disable 0169
		/// <summary>
		/// Initial samples
		/// To prevent padding und thus to get perfect sync,
		/// exactly 256 samples/channel can be provided here.
		/// This is not recommended, as without padding these samples can't be properly
		/// reconstructed anymore.
		/// </summary>
		private IntPtr InitialSamples;

		/// <summary>
		/// Used internally by the encoder. The user should leave this alone.
		/// It is allocated in aften_encode_init and free'd in aften_encode_close.
		/// </summary>
		private IntPtr m_Context;
	#pragma warning restore 0169
	}
}
