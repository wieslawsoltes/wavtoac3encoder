//
// WAV to AC3 Encoder
// Copyright (C) 2007-2016 Wies³aw Šoltés <wieslaw.soltes@gmail.com>
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

#pragma once

typedef CMap<int, int, CString, CString&> LangMap_t;

typedef struct TLang
{
	CString szFileName;
	CString szEnglishName;
	CString szTargetName;
	LangMap_t *lm;
} Lang;

typedef CList<Lang, Lang&> LangList_t;

void SearchFolderForLang(CString szPath, const bool bRecurse, LangList_t& m_LangLst);
void CleanLangList(LangList_t& m_LangLst);
bool LoadLang(CString &szFileName, LangMap_t *lm);
