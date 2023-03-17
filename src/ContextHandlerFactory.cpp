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
