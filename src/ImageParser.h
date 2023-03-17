/* Copyright (c) 2006-2023 SpaceSUR and CONAE

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This file is part of the SuriLib project and its derived programs. You must retain
this copyright notice and license text in all copies or substantial
portions of the software.

If you develop a program based on this software, you must provide a visible
notice to its users that it contains code from the SuriLib project and provide
a copy of this license. The notice should be displayed in a way that is easily
accessible to users, such as in the program's "About" box or documentation.

For more information about SpaceSUR, visit <https://www.spacesur.com>.
For more information about CONAE, visit <http://www.conae.gov.ar/>. */

//<COPYRIGHT> a reemplazar a futuro

#ifndef IMAGEPARSER_H_
#define IMAGEPARSER_H_

//Includes standard
#include <map>

//Includes Suri

//Includes Wx
#include <wx/wxprec.h>
#include <wx/html/htmlpars.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//Includes App

//Defines

//Forwards

//! Clase soporte para poder parsear un html
/**
 * Esta Clase permite parsear un fuente html, y obtener todos los url de las
 * imagenes que contiene.
 */
class ImageParser : public wxHtmlParser {
   wxSortedArrayString imgArray_;  ///< vector con los url de las imagenes del html.
   wxString htmlSrc_;  ///< fuente html a parsear.
public:
   //! Ctor. default.
   ImageParser(const wxString &HtmlSrc);
   //! Agrega un url de imagen al vector.
   void AddString(wxString Txt);
   //! Devuelve el producto del parseo del html.
   wxObject* GetProduct() {
      return NULL;
   }
   //! (Ver doc. wx)
   void AddText(const wxChar* pTxt);
   //! Devuelve los url de las imagenes.
   wxArrayString GetSources();
};

#endif /*IMAGEPARSER_H_*/
