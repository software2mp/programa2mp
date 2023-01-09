/*! \file ContextHandlerFactory.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#ifndef CONTEXTHANDLERFACTORY_H_
#define CONTEXTHANDLERFACTORY_H_

// Includes Estandar
// Includes Suri

#include "suri/ParameterCollection.h"
#include "ContextHandlerInterface.h"

#define REGISTER_CONTEXTHANDLERFACTORY(M_CHName) \
namespace { \
class _M_Register_ContextHandlerFactory_ { \
public: \
   _M_Register_ContextHandlerFactory_() { \
      suri::ContextHandlerFactory::GetInstance()->Register(#M_CHName, &M_CHName::Create); \
   } \
}; \
_M_Register_ContextHandlerFactory_ _m_register_contexthandlerfactory_; \
} \


// Includes Wx
// Defines
// forwards

namespace suri {

typedef ContextHandlerInterface* (*CREATECONTEXTHANDLERFACTORY)(suri::ParameterCollection& Params);

class ContextHandlerFactory {
public:
	/** Destructor. */
	virtual ~ContextHandlerFactory();

	/** Crea una la instancia correspondiente respecto al CHName que se le pase. */
	ContextHandlerInterface* Create(const std::string& CHName, ParameterCollection& Params);

	/** Devuelve una instancia de la factoria. */
	static ContextHandlerFactory* GetInstance();

	/** Registra la clase. */
	void Register(std::string CHName, CREATECONTEXTHANDLERFACTORY pCreateCb);
private:
	/** Ctor. */
	ContextHandlerFactory();
	/** members */
	std::map<std::string, CREATECONTEXTHANDLERFACTORY> factoryCbs_;
};

} /** namespace suri */

#endif /* CONTEXTHANDLERFACTORY_H_ */
