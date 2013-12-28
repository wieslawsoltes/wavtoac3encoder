//
// WAV to AC3 Encoder
// Copyright (C) 2007-2013 Wies³aw Šoltés <wieslaw.soltes@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/* $Id$ */

#include "stdafx.h"
#include "EncWAVtoAC3.h"
#include "Language.h"
#include "MyFile.h"

bool LoadLang(CString &szFileName, LangMap_t &lm)
{
	try
	{
		CMyFile fp;
		if (fp.FOpen(szFileName, false) == false)
			return false;

		// clear list
		lm.RemoveAll();

		TCHAR Buffer;
		const ULONGLONG nLength = fp.FSize();
		CString szBuffer = _T("");

		int key;
		CString szKey = _T("");
		CString szValue = _T("");

		while (fp.FRead(Buffer) == true)
		{
			if ((Buffer != '\r') && (Buffer != '\n'))
				szBuffer += Buffer;

			// we have full line if there is end of line mark or end of file
			if ((Buffer == '\n') || (fp.FPos() == nLength))
			{
				szBuffer += _T("\0");

				int nPos = szBuffer.Find('=', 0);
				if (nPos != -1)
				{
					szKey = szBuffer.Mid(0, nPos);
					szValue = szBuffer.Mid(nPos + 1, szBuffer.GetLength() - 1);

					szValue.Replace(_T("\\n"), _T("\n"));
					szValue.Replace(_T("\\t"), _T("\t"));

					_stscanf(szKey, _T("%x"), &key);

					lm[key] = szValue;
				}

				szBuffer = _T("");
			}
		}

		fp.FClose();
		return true;
	}
	catch (...)
	{
		return false;
	}
}
