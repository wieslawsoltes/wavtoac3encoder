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
	/// EventArgs for frame related events
	/// </summary>
	public class FrameEventArgs : EventArgs
	{
		private readonly int m_nFrameNumber;
		private readonly int m_nSize;
		private readonly Status m_Status;


		/// <summary>
		/// Gets the status.
		/// </summary>
		/// <value>The status.</value>
		public Status Status
		{
			get { return m_Status; }
		}

		/// <summary>
		/// Gets the size of the encoded frame.
		/// </summary>
		/// <value>The size.</value>
		public int Size
		{
			get { return m_nSize; }
		}

		/// <summary>
		/// Gets the frame number.
		/// </summary>
		/// <value>The frame number.</value>
		public int FrameNumber
		{
			get { return m_nFrameNumber; }
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="FrameEventArgs"/> class.
		/// </summary>
		/// <param name="frameNumber">The frame number.</param>
		/// <param name="size">The size of the encoded frame.</param>
		/// <param name="status">The status.</param>
		public FrameEventArgs( int frameNumber, int size, Status status )
		{
			m_nFrameNumber = frameNumber;
			m_nSize = size;
			m_Status = status;
		}
	}
}
