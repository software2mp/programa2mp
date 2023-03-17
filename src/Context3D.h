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

#ifndef CONTEXT3D_H_
#define CONTEXT3D_H_

//Includes standard

//Includes Suri

//Includes Wx
#include "wx/xml/xml.h"

//Includes App
#include "Context.h"

//Defines

//! Clase concreta del contexto "3D"
/**
 * Esta clase es utlizada para crear el Terrenos3D.
 * Permite abrir :
 *  - Imagen de 1 banda + imagen de 1 o 3 bandas como terreno y textura
 *    respectivamente.
 *  - Vectores de punto con Hotlink.
 */
class Context3D : public Context {
   //! Ctor. de Copia.
   Context3D(const Context3D &Context3D);
public:
   Context3D(wxWindow *pParent = NULL, suri::DataViewManager* pDataViewManager = NULL);
   virtual ~Context3D();
   //! Crea la ventana
   virtual bool CreateToolWindow();

protected:
	//! Clase derivada donde se agrega el elemento del contexto
	virtual suri::Element *AddElement(const wxString& Filename,
			const bool &ManuallyAdd, const wxXmlNode* pElementNode = NULL);
private:
};

#endif /*CONTEXT3DTOOL_H_*/
