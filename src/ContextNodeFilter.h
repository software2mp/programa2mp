/*! \file GroupsContextNodeFilter.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */
#ifndef CONTEXTNODEFILTER_H_
#define CONTEXTNODEFILTER_H_

#include "suri/NodeFilterInterface.h"
#include "suri/DataViewManager.h"


// Includes Estandar
// Includes Suri
// Includes Wx
// Defines
// forwards

namespace suri {

class ContextNodeFilter : public suri::NodeFilterInterface {
public:
	/* Ctor. */
   ContextNodeFilter(DataViewManager* pdataviewmanage,
                           ViewcontextInterface::ViewcontextType context);
	/* Destructor */
   virtual ~ContextNodeFilter();
	/** Clona */
	virtual NodeFilterInterface* Clone() const;
	/** Filtra SOLAMENTE a aquellos grupos que no pertenezcan al contexto pasado
	 * en el constructor. */
	virtual bool IsFiltred(TreeNodeInterface* pNode) const;
private:
	DataViewManager* pdataviewmanager_;
	ViewcontextInterface::ViewcontextType context_; /* contexto */
};

} /** namespace suri */

#endif /* GROUPSCONTEXTNODEFILTER_H_ */
