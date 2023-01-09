/*! \file XmlContextHandler.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

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
