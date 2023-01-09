/*! \file GroupsContextNodeFilter.cpp */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#include "ContextNodeFilter.h"
#include "suri/DatasourceInterface.h"
#include "DefaultTreeNode.h"
#include "suri/LayerList.h"
#include "Context.h"

// Includes Estandar
// Includes Suri
// Includes Wx
// Defines
// forwards

namespace suri {

/** Ctor. */
ContextNodeFilter::ContextNodeFilter(
      DataViewManager* pdataviewmanager, ViewcontextInterface::ViewcontextType context) :
      pdataviewmanager_(pdataviewmanager), context_(context) {
	// TODO Auto-generated constructor stub
}

ContextNodeFilter::~ContextNodeFilter() {
	// TODO Auto-generated destructor stub
}

/** Clona */
NodeFilterInterface* ContextNodeFilter::Clone() const {
	return new ContextNodeFilter(pdataviewmanager_, context_);
}

/** Filtra SOLAMENTE a aquellos grupos que no pertenezcan al contexto pasado
 * en el constructor. */
bool ContextNodeFilter::IsFiltred(TreeNodeInterface* pNode) const {
   if (pNode->GetContextType() == context_)
            return false;
   return true;
}

} /** namespace suri */
