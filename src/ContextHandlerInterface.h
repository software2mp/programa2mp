/*! \file ContextHandlerInterface.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#ifndef CONTEXTHANDLERINTERFACE_H_
#define CONTEXTHANDLERINTERFACE_H_

// Includes Estandar
// Includes Suri
#include "suri/DataViewManager.h"
// Includes Wx
// Defines
// forwards

namespace suri {

class ContextHandlerInterface {
public:
	/** Carga del archivo principal del proyecto todos los elementos, grupos y
	 * los acomoda por jerarquia*/
	virtual bool Load(wxXmlNode* pcontextnode,
			DataViewManager* pdataviewmanager, const std::string& RelativePath = "")=0;
	/** Persiste los elementos y grupos dentro del archivo de proyecto. */
	virtual wxXmlNode* Save(wxXmlNode* pcontextsnodes,
			DataViewManager* pdataviewmanager, wxString tipo,
			wxString nombre)=0;
	/** Destructor. */
	virtual ~ContextHandlerInterface() {
	}
};

} /** namespace suri */

#endif /* CONTEXTHANDLERINTERFACE_H_ */
