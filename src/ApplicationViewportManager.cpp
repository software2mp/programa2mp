/*! \file ApplicationViewportManager.cpp */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

// Includes Estandar

// Includes Suri
#include "ApplicationViewportManager.h"
#include "MainWindow.h"

// Includes Wx
// Defines
// forwards

ApplicationViewportManager::ApplicationViewportManager(MainWindow* pMainWindow) :
      pMainWindow_(pMainWindow) {
}

ApplicationViewportManager::~ApplicationViewportManager() {
}

suri::SuriObject::UuidType ApplicationViewportManager::CreateViewport(
      const suri::SuriObject::ClassIdType &ViewportType) {
   return suri::SuriObject::NullUuid;

}
std::vector<suri::SuriObject::UuidType> ApplicationViewportManager::GetViewportIds() const {
   return pMainWindow_->GetActiveContext()->GetViewportIds();
}
void ApplicationViewportManager::SelectViewport(
      const suri::SuriObject::UuidType &ViewportId) {
   // pMainWindow_->GetActiveContext()->SelectViewport(ViewportId);
   // No es necesario seleccionar viewport ya que hay uno solo por contexto
}
suri::ViewportInterface* ApplicationViewportManager::GetSelectedViewport() const {
   return pMainWindow_->GetActiveContext()->GetSelectedViewport();
}

suri::ViewportInterface* ApplicationViewportManager::GetViewport(
      const suri::SuriObject::UuidType &ViewportId) const {
   return pMainWindow_->GetActiveContext()->GetViewport(ViewportId);
}

void ApplicationViewportManager::SetViewportActive(
      const suri::SuriObject::UuidType &ViewportId, bool Active) {
}

bool ApplicationViewportManager::IsViewportActive(
      const suri::SuriObject::UuidType &ViewportId) const {
   // No es necesario seleccionar viewport ya que hay uno solo por contexto
   return false;
}
