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
using System.Runtime.InteropServices;

namespace Aften
{
	/// <summary>
	/// Utility functions
	/// </summary>
	public static class Utility
	{
		[DllImport( "aften.dll" )]
		private static extern IntPtr aften_get_version(); // returns string not on heap!

		[DllImport( "aften.dll" )]
		private static extern int aften_wav_channels_to_acmod(
			int ch, uint chmask, out AudioCodingMode acmod, out bool lfe );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_wav_to_a52(
			double[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_wav_to_a52(
			float[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_wav_to_a52(
			byte[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_wav_to_a52(
			sbyte[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_wav_to_a52(
			short[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_wav_to_a52(
			int[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );
		
		[DllImport( "aften.dll" )]
		private static extern int aften_remap_mpeg_to_a52(
			double[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_mpeg_to_a52(
			float[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_mpeg_to_a52(
			byte[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_mpeg_to_a52(
			sbyte[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_mpeg_to_a52(
			short[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern int aften_remap_mpeg_to_a52(
			int[] samples, int n, int ch, A52SampleFormat fmt, AudioCodingMode acmod );

		[DllImport( "aften.dll" )]
		private static extern FloatType aften_get_float_type();


		/// <summary>
		/// Gets the version.
		/// </summary>
		/// <returns></returns>
		public static string GetVersion()
		{
			return Marshal.PtrToStringAuto( aften_get_version() );
		}

		/// <summary>
		/// Gets the audio coding mode from the specified channels and mask
		/// </summary>
		/// <param name="channels">The channels.</param>
		/// <param name="channelMask">The channel mask.</param>
		/// <param name="hasLfe">if set to <c>true</c> has lfe.</param>
		/// <returns></returns>
		public static AudioCodingMode GetAudioCodingMode(
			int channels, uint channelMask, out bool hasLfe )
		{
			AudioCodingMode audioCodingMode;
			if ( aften_wav_channels_to_acmod(
				channels, channelMask, out audioCodingMode, out hasLfe ) != 0 )
				throw new InvalidOperationException( "channels or channelMask are not valid" );

			return audioCodingMode;
		}

		/// <summary>
		/// Remaps the standard wave to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapWaveToA52(
			double[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_wav_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Double, audioCodingMode );
		}

		/// <summary>
		/// Remaps the standard wave to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapWaveToA52(
			float[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_wav_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Float, audioCodingMode );
		}

		/// <summary>
		/// Remaps the standard wave to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapWaveToA52(
			byte[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_wav_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.UInt8, audioCodingMode );
		}

		/// <summary>
		/// Remaps the standard wave to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapWaveToA52(
			sbyte[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_wav_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Int8, audioCodingMode );
		}

		/// <summary>
		/// Remaps the standard wave to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapWaveToA52(
			short[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_wav_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Int16, audioCodingMode );
		}

		/// <summary>
		/// Remaps the standard wave to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapWaveToA52(
			int[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_wav_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Int32, audioCodingMode );
		}

		/// <summary>
		/// Remaps the MPEG to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapMpegToA52(
			double[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_mpeg_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Double, audioCodingMode );
		}

		/// <summary>
		/// Remaps the MPEG to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapMpegToA52(
			float[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_mpeg_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Float, audioCodingMode );
		}

		/// <summary>
		/// Remaps the MPEG to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapMpegToA52(
			byte[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_mpeg_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.UInt8, audioCodingMode );
		}

		/// <summary>
		/// Remaps the MPEG to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapMpegToA52(
			sbyte[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_mpeg_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Int8, audioCodingMode );
		}

		/// <summary>
		/// Remaps the MPEG to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapMpegToA52(
			short[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_mpeg_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Int16, audioCodingMode );
		}

		/// <summary>
		/// Remaps the MPEG to a52 order.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="channels">The channels.</param>
		/// <param name="audioCodingMode">The audio coding mode.</param>
		public static void RemapMpegToA52(
			int[] samples, int channels, AudioCodingMode audioCodingMode )
		{
			aften_remap_mpeg_to_a52( samples, samples.Length / channels, channels, A52SampleFormat.Int32, audioCodingMode );
		}

		/// <summary>
		/// Gets the float precision of the compiled Aften library
		/// </summary>
		/// <returns></returns>
		public static FloatType GetFloatType()
		{
			return aften_get_float_type();
		}
	}
}
