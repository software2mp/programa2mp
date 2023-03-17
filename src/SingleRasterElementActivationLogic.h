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

#ifndef SINGLERASTERELEMENTACTIVATIONLOGIC_H_
#define SINGLERASTERELEMENTACTIVATIONLOGIC_H_

//Includes standard

//Includes Suri
#include "suri/ActivationLogic.h"

//Includes Wx

//Includes App
#include "Context.h"

//Defines

//! Clase de seleccion de elementos del contexto.
/**
 * Esta clase proporciona la logica para activar/desactivar elementos del
 * contexto Raster.
 * Verifica que no se activen vectores si no hay un raster activo y tambien
 * verifica que no se activen 2 imagenes al mismo tiempo.
 */
class SingleRasterElementActivationLogic : public suri::ActivationLogic {
   //! Ctor. de Copia.
   SingleRasterElementActivationLogic(
         const SingleRasterElementActivationLogic &SingleRasterElementActivationLogic);

public:
   //! Ctor. default
   SingleRasterElementActivationLogic(suri::ViewerWidget *pViewer,
                                      const std::string &RequiredElement) :
         pViewer_(pViewer), requiredElement_(RequiredElement) {
   }
   //! Dtor.
   virtual ~SingleRasterElementActivationLogic() {
   }
   //! activado/desactiva elementos del contexto
   virtual void SetActivation(suri::Element *pElement,
                              suri::LayerList::ElementListType &ElementList,
                              suri::LayerList::ElementSelectionType &ElementData,
                              bool Status);
protected:
private:
   suri::LayerList::ElementListType elementList_;	///< Lista de elementos activos al momento de aplicar la logica
   suri::ViewerWidget *pViewer_;  ///< View asociado
   std::string requiredElement_;  ///< Texto para el mensaje de elemento requerido
};

#endif /*SINGLERASTERELEMENTACTIVATIONLOGIC_H_*/
