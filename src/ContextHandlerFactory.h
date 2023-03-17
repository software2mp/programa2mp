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
