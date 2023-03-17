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

// Includes Estandar
#include <list>
#include <string>
#include <vector>

// Includes Suri
#include "suri/AuxiliaryFunctions.h"
#include "DefaultViewcontextManager.h"

// Includes Wx

// Includes App
#include "ContextManager.h"
#include "ApplicationContextManager.h"

// Defines

// forwards

/** Constructor */
ApplicationContextManager::ApplicationContextManager() :
      pApplicationMenuGui_(NULL) {
}

/** Destructor */
ApplicationContextManager::~ApplicationContextManager() {
   delete pApplicationMenuGui_;
}

/**
 * Setea el Gui
 * @param[in] pGui gui a utilizar
 * \attention esta clase toma posesion del puntero.
 */
void ApplicationContextManager::SetGui(suri::GuiInterface *pGui) {
   delete pApplicationMenuGui_;
   pApplicationMenuGui_ = pGui;
}

suri::GuiInterface* ApplicationContextManager::GetGui() {
   return pApplicationMenuGui_;
}

// ----- INICIO METODOS FORWARD-MANAGER-INTERFACE ------------------------------
/** Crea un Viewport del tipo especificado y lo agrega a los manejados */
suri::SuriObject::UuidType ApplicationContextManager::CreateViewport(
      const suri::SuriObject::ClassIdType &ViewportType) {
   ContextManager &cm = ContextManager::Instance();
   return cm.GetContext(cm.GetActiveContext())->CreateViewport(ViewportType);}

/** Obtiene un listado con los Ids de Viewports que esta manejando */
std::vector<suri::SuriObject::UuidType> ApplicationContextManager::GetViewportIds() const {
   ContextManager &cm = ContextManager::Instance();
   return cm.GetContext(cm.GetActiveContext())->GetViewportIds();
}

/** Selecciona un Viewport segun su Id */
void ApplicationContextManager::SelectViewport(
      const suri::SuriObject::UuidType &ViewportId) {
   ContextManager &cm = ContextManager::Instance();
   cm.GetContext(cm.GetActiveContext())->SelectViewport(ViewportId);
}

/** Obtiene la instancia del Viewport previamente seleccionado */
suri::ViewportInterface* ApplicationContextManager::GetSelectedViewport() const {
   ContextManager &cm = ContextManager::Instance();
   return cm.GetContext(cm.GetActiveContext())->GetSelectedViewport();
}

/** Obtiene la instancia de un viewport en funcion de su Id */
suri::ViewportInterface* ApplicationContextManager::GetViewport(
      const suri::SuriObject::UuidType &ViewportId) const {
   ContextManager &cm = ContextManager::Instance();
   return cm.GetContext(cm.GetActiveContext())->GetViewport(ViewportId);
}

/** Activa/Desactiva un viewport */
void ApplicationContextManager::SetViewportActive(
      const suri::SuriObject::UuidType &ViewportId, bool Active) {
   ContextManager &cm = ContextManager::Instance();
   cm.GetContext(cm.GetActiveContext())->SetViewportActive(ViewportId, Active);
}

/** Indica si el Viewport esta activado */
bool ApplicationContextManager::IsViewportActive(
      const suri::SuriObject::UuidType &ViewportId) const {
   ContextManager &cm = ContextManager::Instance();
   return cm.GetContext(cm.GetActiveContext())->IsViewportActive(ViewportId);
}

/*!
 * \TODO(Javier - Sin TCK): Cada vez que agrego la misma herramienta se agrega veces al menu
 *                          Esto genera multiples herramientas conectadas
 * \TODO(Javier - Sin TCK): Implementar RemoveTool y eliminarla antes de agregarla
 * @param pTool
 */
void ApplicationContextManager::AddTool(suri::ToolInterface *pTool) {
   ContextManager &cm = ContextManager::Instance();
   // Agrego la herramienta en todos los contextos
   for (size_t i = 0; cm.GetContext(i); ++i) {
      cm.GetContext(i)->SetGuiManager(pApplicationMenuGui_);
      cm.GetContext(i)->AddTool(pTool);
   }
   if (pApplicationMenuGui_) {
      pApplicationMenuGui_->AddTool(pTool);
   }
}

/** Remueve una herramienta */
void ApplicationContextManager::RemoveTool() {
   ContextManager &cm = ContextManager::Instance();
   cm.GetContext(cm.GetActiveContext())->RemoveTool();
}

// ----- FIN METODOS FORWARD-MANAGER-INTERFACE ---------------------------------
