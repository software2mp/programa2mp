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

#include "SingleRasterElementActivationLogic.h"

//Includes standard

//Includes Suri
#include "suri/ViewerWidget.h"

//Includes Wx

//Includes App
#include "Context2D.h"
#include "messages.h"

//Defines

// --------------- CLASE DE SELECCION DE ELEMENTOS DEL CONTEXTO ---------------
/**
 * Este metodo es el que proporciona la logica de activado/desactivado de
 * elementos del contexto.
 * Valida que no se puedan activar Vectores si no hay al menos 1 imagen activa.
 * Valida que no se esten activas mas de 1 imagen a la vez.
 * @param[in]	ElementList: lista de elementos del contexto.
 * @param[in]	ElementData: datos del estado y ubicacion en la lista del elemento.
 * @param[in]	Status: estado a aplicar al elemento(activo/desactivo).
 * @param[out]	pElement: puntero al elemento que se desea activar/desactivar.
 */
void SingleRasterElementActivationLogic::SetActivation(
      suri::Element *pElement, suri::LayerList::ElementListType &ElementList,
      suri::LayerList::ElementSelectionType &ElementData, bool Status) {
   const_cast<SingleRasterElementActivationLogic*>(this)->elementList_.clear();
   if (!pElement) return;
   switch (ElementData.find(pElement)->second.group_) {
      // Anotacion y vector se activan o desactivan sin modificaciones al resto
      case Context2D::AnotationPosition:
      case Context2D::VectorPosition: {
         bool canactivate = false;
         // recorro todos los elementos de la lista
         suri::LayerList::ElementListType::iterator it = ElementList.begin();
         while (it != ElementList.end()) {
            suri::Element *ptemp = *it;
            // si se esta activando algo que no es raster, y hay un raster activo, lo permito
            if (Status && ElementData[ptemp].group_ == Context2D::RasterPosition
                  && ptemp->IsActive()) canactivate = true;
            it++;
         }
         // si no habia un raster activo, falla
         if (!canactivate && Status)
            SHOW_ERROR(message_ELEMENT_MUST_BE_DISPLAYED_s, requiredElement_.c_str());
         //no se traduce, lo hace SHOW_
         else
            pElement->Activate(Status);
         break;
      }
      case Context2D::RasterPosition: {
         // recorro todos los elementos de la lista
         suri::LayerList::ElementListType::iterator it = ElementList.begin();
         while (it != ElementList.end()) {
            suri::Element *ptemp = *it;
            // Si estoy activando un raster
            // el elemento actual (del recorrido de la lista) es un raster
            // y esta activo, entonces lo desactivo
            if (Status) {
               // si esta activo y no es el actual
               if (ptemp->IsActive() && ptemp != pElement) {
                  // si no es raster, ademas me lo guardo para reactivarlo mas tarde
                  if (ElementData[ptemp].group_ == Context2D::RasterPosition) {
                     ptemp->PreventNextViewerUpdate();
                     ptemp->Activate(false);
                  }
               }
            } else	// Estoy desactivando
            {
               // si el raster esta activo, desactivo todos los elementos
               if (pElement->IsActive() && ptemp != pElement) {
                  ptemp->PreventNextViewerUpdate();
                  ptemp->Activate(false);
               }
            }
            it++;
         }
         // hago que el viewer se resetee para aceptar la imagen nueva
         pViewer_->ResetViewer();
         pElement->Activate(Status);
         break;
      }
      default:
         break;
   }
}
// --------------- CLASE DE SELECCION DE ELEMENTOS DEL CONTEXTO ---------------
