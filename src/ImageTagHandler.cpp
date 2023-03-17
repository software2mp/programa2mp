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

#include "ImageTagHandler.h"

//Includes standard

//Includes Suri

//Includes Wx

//Includes App

//Defines

//Forwards.

//--- TAGHANDLER ---
/**
 * Constructor.
 */
ImageTagHandler::ImageTagHandler() {
   pParser_ = NULL;
}

/**
 * Constructor de copia.
 * @param[in]	pImgParser: puntero a un objeto ImageParser.
 */
ImageTagHandler::ImageTagHandler(ImageParser *pImgParser) {
   pParser_ = pImgParser;
}

/**
 * Devuelve el tipo de tag a parsear.
 * @return wxString con el tipo de tag a parsear.
 */
wxString ImageTagHandler::GetSupportedTags() {
   return "IMG";
}

/**
 * Este metodo es el "kernel" de cada parseador. Es llamado por el parseador
 * cada vez que se detecta un tag soportado.
 * Si el tag contiene el parametro "SRC", agrega el url de la imagen al vector
 * de urls de imagenes del parseador.
 * @param[in]	Tag: tag html.
 * @return true si el tag contiene el parametro SRC. false C.O.C.
 */
bool ImageTagHandler::HandleTag(const wxHtmlTag &Tag) {
   if (Tag.HasParam(wxT("SRC"))) {
      wxString src(Tag.GetParam(wxT("SRC")));
      pParser_->AddString(src);
      ParseInner(Tag);
      return true;
   } else
      return false;
}
