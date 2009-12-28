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
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace Aften
{
	#region FrameEncoder base class

	/// <summary>
	/// The Aften AC3 Encoder base class
	/// </summary>
	public abstract class FrameEncoder : IDisposable
	{
		/// <summary>
		/// Error message EqualAmountOfSamples
		/// </summary>
		protected const string _EqualAmountOfSamples = "Each channel must have an equal amount of samples.";

		private readonly int m_nChannels;
		private bool m_bDisposed;
		private bool m_bEncodingDone;

		[DllImport( "aften.dll" )]
		private static extern void aften_set_defaults( ref EncodingContext context );

		[DllImport( "aften.dll" )]
		protected static extern int aften_encode_init( ref EncodingContext context );

		[DllImport( "aften.dll" )]
		protected static extern void aften_encode_close( ref EncodingContext context );

		/// <summary>
		/// Releases unmanaged and - optionally - managed resources
		/// </summary>
		/// <param name="disposing"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
		private void Dispose( bool disposing )
		{
			// Check to see if Dispose has already been called.
			if ( !m_bDisposed ) {
				if ( disposing ) {
					// Dispose managed resources
				}
				// Release unmanaged resources
				DoCloseEncoder();

				m_bDisposed = true;
			}
		}

		/// <summary>
		/// Gets or sets a value indicating whether encoding is done.
		/// </summary>
		/// <value><c>true</c> if encoding is done; otherwise, <c>false</c>.</value>
		protected bool EncodingDone
		{
			get { return m_bEncodingDone; }
			set { m_bEncodingDone = value; }
		}

		/// <summary>
		/// Checks the length of the samples array.
		/// </summary>
		/// <param name="samples">The samples.</param>
		protected void CheckSamplesLength( Array samples )
		{
			if ( samples.Length % m_nChannels != 0 )
				throw new InvalidOperationException( _EqualAmountOfSamples );
		}

		/// <summary>
		/// Closes the encoder.
		/// </summary>
		protected abstract void DoCloseEncoder();

		/// <summary>
		/// Called when a frame has been encoded.
		/// </summary>
		/// <param name="sender">The sender.</param>
		/// <param name="e">The <see cref="Aften.FrameEventArgs"/> instance containing the event data.</param>
		protected virtual void OnFrameEncoded( object sender, FrameEventArgs e )
		{
			if ( FrameEncoded != null )
				FrameEncoded( sender, e );
		}

		/// <summary>
		/// Raised when a frame has been encoded.
		/// </summary>
		public event EventHandler<FrameEventArgs> FrameEncoded;

		/// <summary>
		/// Gets a default context.
		/// </summary>
		/// <returns></returns>
		public static EncodingContext GetDefaultsContext()
		{
			EncodingContext context = new EncodingContext();
			aften_set_defaults( ref context );

			return context;
		}

		/// <summary>
		/// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
		/// </summary>
		public void Dispose()
		{
			this.Dispose( true );
			GC.SuppressFinalize( this );
		}

		#region Encoding

		/// <summary>
		/// Encodes the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <returns>MemoryStream containing the encoded frames</returns>
		public MemoryStream Encode( Array samples )
		{
			this.CheckSamplesLength( samples );

			return this.Encode( samples, samples.Length / m_nChannels );
		}

		/// <summary>
		/// Encodes a part of the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="samplesPerChannelCount">The samples per channel count.</param>
		/// <returns>
		/// MemoryStream containing the encoded frames
		/// </returns>
		public MemoryStream Encode( Array samples, int samplesPerChannelCount )
		{
			MemoryStream stream = new MemoryStream();
			this.Encode( samples, samplesPerChannelCount, stream );

			return stream;
		}

		/// <summary>
		/// Encodes the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="frames">The frames.</param>
		public void Encode( Array samples, Stream frames )
		{
			this.CheckSamplesLength( samples );

			this.Encode( samples, samples.Length / m_nChannels, frames );
		}

		/// <summary>
		/// Encodes a part of the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="samplesPerChannelCount">The samples per channel count.</param>
		/// <param name="frames">The frames.</param>
		public abstract void Encode( Array samples, int samplesPerChannelCount, Stream frames );

		/// <summary>
		/// Encodes the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <returns>
		/// MemoryStream containing the encoded frames
		/// </returns>
		public MemoryStream Encode( Stream samples )
		{
			MemoryStream stream = new MemoryStream();
			this.Encode( samples, stream );

			return stream;
		}

		/// <summary>
		/// Encodes the specified interleaved samples stream.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="frames">The frames.</param>
		public abstract void Encode( Stream samples, Stream frames );

		/// <summary>
		/// Encodes the specified interleaved samples and flushes the encoder.  This instance needs to be disposed afterwards.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <returns>
		/// MemoryStream containing the encoded frames
		/// </returns>
		public MemoryStream EncodeAndFlush( Stream samples )
		{
			MemoryStream stream = new MemoryStream();
			this.EncodeAndFlush( samples, stream );

			return stream;
		}

		/// <summary>
		/// Encodes the specified interleaved samples stream and flushes the encoder.  This instance needs to be disposed afterwards.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="frames">The frames.</param>
		public void EncodeAndFlush( Stream samples, Stream frames )
		{
			this.Encode( samples, frames );
			if ( !this.EncodingDone )
				this.Flush( frames );
		}

		#endregion

		/// <summary>
		/// Flushes the encoder und returns the remaining frames. This instance needs to be disposed afterwards.
		/// </summary>
		/// <returns>MemoryStream containing the encoded frames</returns>
		public MemoryStream Flush()
		{
			MemoryStream stream = new MemoryStream();
			this.Flush( stream );

			return stream;
		}

		/// <summary>
		/// Flushes the encoder und returns the remaining frames.  This instance needs to be disposed afterwards.
		/// </summary>
		/// <param name="frames">The frames.</param>
		public abstract void Flush( Stream frames );

		/// <summary>
		/// Aborts the encoding. This instance needs to be disposed afterwards.
		/// </summary>
		public abstract void Abort();

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoder"/> class.
		/// </summary>
		/// <param name="channels">The channels.</param>
		protected FrameEncoder( int channels )
		{
			m_nChannels = channels;
		}

		/// <summary>
		/// Releases unmanaged resources and performs other cleanup operations before the
		/// <see cref="FrameEncoder"/> is reclaimed by garbage collection.
		/// </summary>
		~FrameEncoder()
		{
			this.Dispose( false );
		}
	}

	#endregion

	#region Strongly typed FrameEncoder class

	/// <summary>
	/// The Aften AC3 Encoder
	/// </summary>
	/// <typeparam name="TSample">The type of the sample.</typeparam>
	public abstract class FrameEncoder<TSample> : FrameEncoder
		where TSample : struct
	{
		private readonly byte[] m_FrameBuffer = new byte[A52Sizes.MaximumCodedFrameSize];
		private readonly byte[] m_StreamBuffer;
		private readonly TSample[] m_Samples;
		private readonly TSample[] m_StreamSamples;
		private readonly int m_nTotalSamplesPerFrame;
		private readonly int m_nTSampleSize = Marshal.SizeOf( typeof( TSample ) );
		private readonly RemappingDelegate m_Remap;
		private readonly EncodeFrameDelegate m_EncodeFrame;
		private readonly ToTSampleDelegate m_ToTSample;

		private EncodingContext m_Context;
		private int m_nRemainingSamplesCount;
		private int m_nFrameNumber;
		private bool m_bFedSamples;
		private bool m_bGotFrames;
		private int m_bAbort;


		internal delegate TSample ToTSampleDelegate( byte[] buffer, int startIndex );
		internal delegate int EncodeFrameDelegate(
			ref EncodingContext context, byte[] frameBuffer, TSample[] samples, int count );

		/// <summary>
		/// Delegate for a remapping function
		/// </summary>
		public delegate void RemappingDelegate(
			TSample[] samples, int channels, AudioCodingMode audioCodingMode );

		/// <summary>
		/// Checks the state of the encoding.
		/// </summary>
		private void CheckEncodingState()
		{
			if ( this.EncodingDone )
				throw new InvalidOperationException( "Encoding was already completed. Dispose this object." );
		}

		/// <summary>
		/// Closes the encoder.
		/// </summary>
		protected override void DoCloseEncoder()
		{
			// Context is value type, so I don't want to copy it to the base
			aften_encode_close( ref m_Context );
		}

		#region Encoding

		/// <summary>
		/// Encodes the frame.
		/// </summary>
		/// <param name="samplesPerChannelCount">The samples per channel count.</param>
		/// <returns></returns>
		private int EncodeFrame( int samplesPerChannelCount )
		{
			if ( samplesPerChannelCount > 0 ) {
				m_bFedSamples = true;
				if ( (m_Remap != null) )
					m_Remap( m_Samples, m_Context.Channels, m_Context.AudioCodingMode );
			}

			int nSize = m_EncodeFrame( ref m_Context, m_FrameBuffer, m_Samples, samplesPerChannelCount );
			if ( nSize > 0 ) {
				m_bGotFrames = true;
				this.OnFrameEncoded( this, new FrameEventArgs( ++m_nFrameNumber, nSize, m_Context.Status ) );
			} else if ( nSize < 0 )
				throw new InvalidOperationException( "Encoding error" );

			return nSize;
		}

		/// <summary>
		/// Encodes a part of the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="samplesPerChannelCount">The samples per channel count.</param>
		/// <param name="frames">The frames.</param>
		public override void Encode( Array samples, int samplesPerChannelCount, Stream frames )
		{
			TSample[] typedSamples = (TSample[]) samples;
			this.Encode( typedSamples, samplesPerChannelCount, frames );
		}

		/// <summary>
		/// Encodes the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <returns>MemoryStream containing the encoded frames</returns>
		public MemoryStream Encode( TSample[] samples )
		{
			this.CheckSamplesLength( samples );

			return this.Encode( samples, samples.Length / m_Context.Channels );
		}

		/// <summary>
		/// Encodes a part of the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="samplesPerChannelCount">The samples per channel count.</param>
		/// <returns>
		/// MemoryStream containing the encoded frames
		/// </returns>
		public MemoryStream Encode( TSample[] samples, int samplesPerChannelCount )
		{
			MemoryStream stream = new MemoryStream();
			this.Encode( samples, samplesPerChannelCount, stream );

			return stream;
		}

		/// <summary>
		/// Encodes the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="frames">The frames.</param>
		public void Encode( TSample[] samples, Stream frames )
		{
			this.CheckSamplesLength( samples );

			this.Encode( samples, samples.Length / m_Context.Channels, frames );
		}

		/// <summary>
		/// Encodes a part of the specified interleaved samples.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="samplesPerChannelCount">The samples per channel count.</param>
		/// <param name="frames">The frames.</param>
		public void Encode( TSample[] samples, int samplesPerChannelCount, Stream frames )
		{
			this.CheckEncodingState();
			int nSamplesCount = samplesPerChannelCount * m_Context.Channels;
			if ( nSamplesCount > samples.Length )
				throw new InvalidOperationException( "samples contains less data then specified." );

			int nOffset = m_nRemainingSamplesCount;
			int nSamplesNeeded = m_nTotalSamplesPerFrame - nOffset;
			int nSamplesDone = 0;
			while ( nSamplesCount - nSamplesDone + nOffset >= m_nTotalSamplesPerFrame ) {
				if ( Interlocked.Exchange( ref m_bAbort, 0 ) != 0 ) {
					this.EncodingDone = true;

					return;
				}

				Buffer.BlockCopy( samples, nSamplesDone * m_nTSampleSize, m_Samples, nOffset * m_nTSampleSize, nSamplesNeeded * m_nTSampleSize );
				int nSize = this.EncodeFrame( A52Sizes.SamplesPerFrame );
				frames.Write( m_FrameBuffer, 0, nSize );
				nSamplesDone += m_nTotalSamplesPerFrame - nOffset;
				nOffset = 0;
				nSamplesNeeded = m_nTotalSamplesPerFrame;
			}
			m_nRemainingSamplesCount = nSamplesCount - nSamplesDone;
			if ( m_nRemainingSamplesCount > 0 )
				Buffer.BlockCopy( samples, nSamplesDone * m_nTSampleSize, m_Samples, 0, m_nRemainingSamplesCount * m_nTSampleSize );
		}

		/// <summary>
		/// Encodes the specified interleaved samples stream and flushes the encoder.
		/// </summary>
		/// <param name="samples">The samples.</param>
		/// <param name="frames">The frames.</param>
		public override void Encode( Stream samples, Stream frames )
		{
			this.CheckEncodingState();
			if ( !samples.CanRead )
				throw new InvalidOperationException( "Samples stream must be readable." );

			int nOffset = 0;
			int nCurrentRead;
			while ( (nCurrentRead = samples.Read( m_StreamBuffer, 0, m_nTSampleSize )) == m_nTSampleSize ) {
				m_StreamSamples[nOffset] = m_ToTSample( m_StreamBuffer, 0 );
				++nOffset;
				if ( nOffset == m_nTotalSamplesPerFrame ) {
					this.Encode( m_StreamSamples, frames );
					nOffset = 0;
					if ( this.EncodingDone )
						return;
				}
			}
			if ( (nCurrentRead != 0) || (nOffset % m_Context.Channels != 0) )
				throw new InvalidOperationException( _EqualAmountOfSamples );

			if ( nOffset > 0 )
				this.Encode( m_StreamSamples, nOffset / m_Context.Channels, frames );
		}

		#endregion

		/// <summary>
		/// Flushes the encoder und returns the remaining frames.  This instance needs to be disposed afterwards.
		/// </summary>
		/// <param name="frames">The frames.</param>
		public override void Flush( Stream frames )
		{
			this.CheckEncodingState();
			int nSize;
			int nSamplesPerChannelCount = m_nRemainingSamplesCount / m_Context.Channels;
			do {
				nSize = this.EncodeFrame( nSamplesPerChannelCount );
				frames.Write( m_FrameBuffer, 0, nSize );
				nSamplesPerChannelCount = 0;
			} while ( nSize > 0 || (m_bFedSamples && !m_bGotFrames) );
			this.EncodingDone = true;
		}

		/// <summary>
		/// Aborts the encoding. This instance needs to be disposed afterwards.
		/// </summary>
		public override void Abort()
		{
			Interlocked.Exchange( ref m_bAbort, 1 );
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoder"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="remap">The remapping function.</param>
		/// <param name="encodeFrame">The encode frame function.</param>
		/// <param name="toTSample">The ToTSample function.</param>
		/// <param name="sampleFormat">The sample format.</param>
		internal FrameEncoder(
			ref EncodingContext context,
			RemappingDelegate remap,
			EncodeFrameDelegate encodeFrame,
			ToTSampleDelegate toTSample,
			A52SampleFormat sampleFormat )
			: this( ref context, encodeFrame, toTSample, sampleFormat )
		{
			m_Remap = remap;
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoder"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="encodeFrame">The encode frame function.</param>
		/// <param name="toTSample">The ToTSample function.</param>
		/// <param name="sampleFormat">The sample format.</param>
		internal FrameEncoder(
			ref EncodingContext context,
			EncodeFrameDelegate encodeFrame,
			ToTSampleDelegate toTSample,
			A52SampleFormat sampleFormat )
			: base( context.Channels )
		{
			if ( aften_encode_init( ref context ) != 0 )
				throw new InvalidOperationException( "Initialization failed" );

			context.SampleFormat = sampleFormat;
			m_Context = context;
			m_EncodeFrame = encodeFrame;
			m_ToTSample = toTSample;

			m_nTotalSamplesPerFrame = A52Sizes.SamplesPerFrame * m_Context.Channels;
			m_Samples = new TSample[m_nTotalSamplesPerFrame];
			m_StreamSamples = new TSample[m_nTotalSamplesPerFrame];

			m_StreamBuffer = new byte[m_nTSampleSize];
		}
	}

	#endregion

	#region Specific FrameEncoder classes

	/// <summary>
	/// The Aften AC3 Encoder for double precision floating point samples, normalized to [-1, 1]
	/// </summary>
	public class FrameEncoderDouble : FrameEncoder<double>
	{
		[DllImport( "aften.dll" )]
		private static extern int aften_encode_frame(
			ref EncodingContext context, byte[] frameBuffer, double[] samples, int count );

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderDouble"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="remap">The remapping function.</param>
		public FrameEncoderDouble( ref EncodingContext context, RemappingDelegate remap )
			: base( ref context, remap, aften_encode_frame, BitConverter.ToDouble, A52SampleFormat.Double ) { }

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderDouble"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		public FrameEncoderDouble( ref EncodingContext context )
			: base( ref context, aften_encode_frame, BitConverter.ToDouble, A52SampleFormat.Double ) { }
	}

	/// <summary>
	/// The Aften AC3 Encoder for single precision floating point samples, normalized to [-1, 1]
	/// </summary>
	public class FrameEncoderFloat : FrameEncoder<float>
	{
		[DllImport( "aften.dll" )]
		private static extern int aften_encode_frame(
			ref EncodingContext context, byte[] frameBuffer, float[] samples, int count );

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderFloat"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="remap">The remapping function.</param>
		public FrameEncoderFloat( ref EncodingContext context, RemappingDelegate remap )
			: base( ref context, remap, aften_encode_frame, BitConverter.ToSingle, A52SampleFormat.Float ) { }

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderFloat"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		public FrameEncoderFloat( ref EncodingContext context )
			: base( ref context, aften_encode_frame, BitConverter.ToSingle, A52SampleFormat.Float ) { }
	}

	/// <summary>
	/// The Aften AC3 Encoder for 8bit unsigned integer samples
	/// </summary>
	public class FrameEncoderUInt8 : FrameEncoder<byte>
	{
		[DllImport( "aften.dll" )]
		private static extern int aften_encode_frame(
			ref EncodingContext context, byte[] frameBuffer, byte[] samples, int count );

		/// <summary>
		/// Returns the byte at the startIndex
		/// </summary>
		/// <param name="value">The value.</param>
		/// <param name="startIndex">The start index.</param>
		/// <returns></returns>
		private static byte ToUInt8( byte[] value, int startIndex )
		{
			return value[startIndex];
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderUInt8"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="remap">The remapping function.</param>
		public FrameEncoderUInt8( ref EncodingContext context, RemappingDelegate remap )
			: base( ref context, remap, aften_encode_frame, ToUInt8, A52SampleFormat.UInt8 ) { }

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderUInt8"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		public FrameEncoderUInt8( ref EncodingContext context )
			: base( ref context, aften_encode_frame, ToUInt8, A52SampleFormat.UInt8 ) { }
	}

	/// <summary>
	/// The Aften AC3 Encoder for 8bit signed integer samples
	/// </summary>
	public class FrameEncoderInt8 : FrameEncoder<sbyte>
	{
		[DllImport( "aften.dll" )]
		private static extern int aften_encode_frame(
			ref EncodingContext context, byte[] frameBuffer, sbyte[] samples, int count );

		/// <summary>
		/// Returns the signed byte at the startIndex
		/// </summary>
		/// <param name="value">The value.</param>
		/// <param name="startIndex">The start index.</param>
		/// <returns></returns>
		private static sbyte ToInt8( byte[] value, int startIndex )
		{
			return (sbyte) value[startIndex];
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderInt8"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="remap">The remapping function.</param>
		public FrameEncoderInt8( ref EncodingContext context, RemappingDelegate remap )
			: base( ref context, remap, aften_encode_frame, ToInt8, A52SampleFormat.Int8 ) { }

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderInt8"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		public FrameEncoderInt8( ref EncodingContext context )
			: base( ref context, aften_encode_frame, ToInt8, A52SampleFormat.Int8 ) { }
	}

	/// <summary>
	/// The Aften AC3 Encoder for 16bit signed integer samples
	/// </summary>
	public class FrameEncoderInt16 : FrameEncoder<short>
	{
		[DllImport( "aften.dll" )]
		private static extern int aften_encode_frame(
			ref EncodingContext context, byte[] frameBuffer, short[] samples, int count );

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderInt16"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="remap">The remapping function.</param>
		public FrameEncoderInt16( ref EncodingContext context, RemappingDelegate remap )
			: base( ref context, remap, aften_encode_frame, BitConverter.ToInt16, A52SampleFormat.Int16 ) { }

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderInt16"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		public FrameEncoderInt16( ref EncodingContext context )
			: base( ref context, aften_encode_frame, BitConverter.ToInt16, A52SampleFormat.Int16 ) { }
	}

	/// <summary>
	/// The Aften AC3 Encoder for 32bit signed integer samples
	/// </summary>
	public class FrameEncoderInt32 : FrameEncoder<int>
	{
		[DllImport( "aften.dll" )]
		private static extern int aften_encode_frame(
			ref EncodingContext context, byte[] frameBuffer, int[] samples, int count );

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderInt32"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		/// <param name="remap">The remapping function.</param>
		public FrameEncoderInt32( ref EncodingContext context, RemappingDelegate remap )
			: base( ref context, remap, aften_encode_frame, BitConverter.ToInt32, A52SampleFormat.Int32 ) { }

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEncoderInt32"/> class.
		/// </summary>
		/// <param name="context">The context.</param>
		public FrameEncoderInt32( ref EncodingContext context )
			: base( ref context, aften_encode_frame, BitConverter.ToInt32, A52SampleFormat.Int32 ) { }
	}

	#endregion
}
