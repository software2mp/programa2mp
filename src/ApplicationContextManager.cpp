/*! \file ApplicationContextManager.cpp */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2012-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

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
