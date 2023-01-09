/*! \file ContextHandlerFactory.cpp */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#include "ContextHandlerFactory.h"

// Includes Estandar
// Includes Suri
// Includes Wx
// Defines
// forwards


namespace suri {

/** Ctor. */
ContextHandlerFactory::ContextHandlerFactory() {
	// TODO Auto-generated constructor stub

}

/** Destructor. */
ContextHandlerFactory::~ContextHandlerFactory() {
	// TODO Auto-generated destructor stub
}

/** Crea una la instancia correspondiente respecto al CHName que se le pase. */
ContextHandlerInterface* ContextHandlerFactory::Create( const std::string& CHName,
												ParameterCollection& Params) {
	std::map<std::string, CREATECONTEXTHANDLERFACTORY>::iterator it = factoryCbs_.find(CHName);

	if ( it != factoryCbs_.end())
		return it->second(Params);

	return NULL;
}

/**
 * Devuelve la unica instancia.
 */
ContextHandlerFactory* ContextHandlerFactory::GetInstance() {
	static ContextHandlerFactory instance;
	return &instance;
}
/** Registra la clase. */
void ContextHandlerFactory::Register(std::string CHName, CREATECONTEXTHANDLERFACTORY pCreateCb) {
	factoryCbs_[CHName] = pCreateCb;
}

} /** namespace suri */
