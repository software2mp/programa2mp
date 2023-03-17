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

#ifndef XMLCONTEXTHANDLER_H_
#define XMLCONTEXTHANDLER_H_

// Includes Estandar
// Includes Suri
#include "ContextHandlerInterface.h"
#include "suri/ParameterCollection.h"
#include "suri/MutableWorkGroupInterface.h"
#include "ContextGroupNodeFilter.h"

// Includes Wx
// Defines
// forwards

namespace suri {

class XmlContextHandler: public ContextHandlerInterface {
public:
    typedef enum {
     Imagenes = 1, Terrenos3D = 2, Fotografias = 3, Html = 4, Mapas = 5, Documentos = 6, Actividades = 7
	} ContextType;
	/** Ctor. */
	XmlContextHandler();
	/** Destructor */
	virtual ~XmlContextHandler();
	/** Create del factory */
	static ContextHandlerInterface* Create(suri::ParameterCollection& params);
	/** Carga desde el archivo XML del proyecto todos los elementos
	 *  y grupos en la aplicacion. */
   virtual bool Load(wxXmlNode* pcontextnode, DataViewManager* pdataviewmanager,
         const std::string& RelativePath = "");
   /** Levanta los elementos y sus grupos desde el archivo del proyecto. */
	virtual wxXmlNode* Save(wxXmlNode* pcontextsnodes,
			DataViewManager* pdataviewmanager, wxString tipo, wxString nombre);
	/** Agrega un elemento al datasource */
private:
	std::string AddElement(wxXmlNode* pNodeElement,
			suri::DataViewManager* pDataviewmanager, wxString contextname,
			const std::string& RelativePath = "");
	/** Carga los items en el arbol y los acomoda. */
   bool LoadItems(NodePath parent, wxXmlNode* pnode,
         MutableWorkGroupInterface* pwgroot,
         suri::DataViewManager* pDataviewmanager, wxString contextname,
         const std::string& RelativePath = "");
	/** Acomoda los rasters dentro de la jerarquia de grupos. */
	wxXmlNode* SaveItems(wxXmlNode*& pnode, wxXmlNode* psavednode,
			TreeIterator* treelements,
			ViewcontextInterface::ViewcontextType ctype,
			DataViewManager* pdataviewmanager,
			int rescursivecall);
	/** Devuelve un iterador del arbol */
	TreeIterator GetTreeElementsIterator(DataViewManager* pdataviewmanager,
                                        ViewcontextInterface::ViewcontextType ctype);
   /** Devuelve la cantidad de grupos que hay en un determinado contexto. */
   int ContextElementsCount(DataViewManager* pdataviewmanager,
                          ViewcontextInterface::ViewcontextType ctype);

};

} /** namespace suri */

#endif /* XMLCONTEXTHANDLER_H_ */
