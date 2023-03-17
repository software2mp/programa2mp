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

#ifndef APPLICATIONCONTEXTMANAGER_H_
#define APPLICATIONCONTEXTMANAGER_H_

// Includes Estandar
#include <vector>

// Includes Suri
#include "suri/ViewportManagerInterface.h"
#include "suri/ToolManagerInterface.h"
#include "suri/GuiInterface.h"
#include "suri/SuriObject.h"
#include "suri/ViewcontextInterface.h"
#include "suri/Subject.h"
#include "suri/NotificationInterface.h"
#include "suri/LayerInterface.h"

// Includes Wx

// Includes App

// Defines

// forwards
class Context;
class ContextManager;
class ViewerWidget;

/**
 * Manager de Contextos de la Aplicacion
 * \TODO(Sebastian - TCK #852): Analizar eliminacion de ApplicationContextManager
 */
class ApplicationContextManager : public suri::ViewportManagerInterface,
                                  public suri::ToolManagerInterface {
public:
   /** Constructor */
   ApplicationContextManager();
   /** Destructor */
   virtual ~ApplicationContextManager();
   /** Seter del GUI */
   void SetGui(suri::GuiInterface *pGui);

   suri::GuiInterface* GetGui();

   /** Crea un Viewport del tipo especificado y lo agrega a los manejados */
   virtual suri::SuriObject::UuidType CreateViewport(
         const suri::SuriObject::ClassIdType &ViewportType);
   /** Obtiene un listado con los Ids de Viewports que esta manejando */
   virtual std::vector<suri::SuriObject::UuidType> GetViewportIds() const;
   /** Selecciona un Viewport segun su Id */
   virtual void SelectViewport(const suri::SuriObject::UuidType &ViewportId);
   /** Obtiene la instancia del Viewport previamente seleccionado */
   virtual suri::ViewportInterface* GetSelectedViewport() const;
   /** Obtiene la instancia de un viewport en funcion de su Id */
   virtual suri::ViewportInterface* GetViewport(const suri::SuriObject::UuidType &ViewportId) const;
   /** Activa/Desactiva un viewport */
   virtual void SetViewportActive(const suri::SuriObject::UuidType &ViewportId, bool Active = true);
   /** Indica si el Viewport esta activado */
   virtual bool IsViewportActive(const suri::SuriObject::UuidType &ViewportId) const;
   /** Agrega una herramienta */
   virtual void AddTool(suri::ToolInterface *pTool);
   /** Remueve una herramienta */
   virtual void RemoveTool();

private:
   /** GUI del Menu de Aplicacion */
   suri::GuiInterface *pApplicationMenuGui_;
};

#endif /* APPLICATIONCONTEXTMANAGER_H_ */
