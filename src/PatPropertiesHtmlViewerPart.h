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

#ifndef PATPROPERTIES_H_
#define PATPROPERTIES_H_

//Includes standard

//Includes Suri
#include "suri/HtmlViewerPart.h"

//Includes Wx

//Includes App

//Defines

//Forwards

//! Esta clase permite visualizar la informacion del PAT.
/**
 * Permite visualizar el nodo informacion del PAT en un HTML.
 */
class PatPropertiesHtmlViewerPart : public suri::HtmlViewerPart {
   //! Ctor. de Copia.
   PatPropertiesHtmlViewerPart(
         const PatPropertiesHtmlViewerPart &PatPropertiesHtmlViewerPart);

public:
   //! ctor
   PatPropertiesHtmlViewerPart(const std::string &PatPath);
   //! dtor
   virtual ~PatPropertiesHtmlViewerPart();
   //! Se crea la ventana de herramienta en las clases derivadas
   virtual bool CreateToolWindow();
   //! Inicializa los controles
   virtual void SetInitialValues();
   //! Devuelve el estado del Part.
   virtual bool IsEnabled() {
      return true;
   }
   //! Indica si el Part tiene cambios para salvar.
   virtual bool HasChanged() {
      return modified_;
   }
   //! Salva los cambios realizados en el Part.
   virtual bool CommitChanges() {
      return true;
   }
   //! Restaura los valores originales del Part.
   virtual bool RollbackChanges() {
      return true;
   }
   //! Setea el nuevo contenido de la ventana.
   void SetHtml(const std::string &HtmlText = "");
protected:
   std::string patPath_;  ///< path del PAT(del archivo descripcion.xml)
private:
   //! Verifica que existan los templates html
   bool CheckDataDirectory(const wxString &Path = wxT(""));
   //! obtiene el template html
   bool GetTemplateHtml(const wxString &PathHtml, wxString &Html);
};

#endif /*PATPROPERTIES_H_*/
