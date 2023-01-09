/*! \file ApplicationViewportManager.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#ifndef APPLICATIONVIEWPORTMANAGER_H_
#define APPLICATIONVIEWPORTMANAGER_H_

// Includes Estandar
#include "suri/ViewportManagerInterface.h"
#include "suri/SuriObject.h"

// Includes Suri
// Includes Wx
// Defines
// forwards
class MainWindow;

/** Clase que representa un viewportmanager para la aplicacion
 *  Software 2Mp
 */
class ApplicationViewportManager : public suri::ViewportManagerInterface {
public:
   ApplicationViewportManager(MainWindow* pMainWindow);
   virtual ~ApplicationViewportManager();
   virtual suri::SuriObject::UuidType CreateViewport(
         const suri::SuriObject::ClassIdType &ViewportType);
   virtual std::vector<suri::SuriObject::UuidType> GetViewportIds() const;
   virtual void SelectViewport(const suri::SuriObject::UuidType &ViewportId);
   virtual suri::ViewportInterface* GetSelectedViewport() const;
   virtual suri::ViewportInterface* GetViewport(
         const suri::SuriObject::UuidType &ViewportId) const;
   virtual void SetViewportActive(const suri::SuriObject::UuidType &ViewportId,
                                  bool Active = true);
   virtual bool IsViewportActive(const suri::SuriObject::UuidType &ViewportId) const;

private:
   MainWindow* pMainWindow_;
};

#endif /* APPLICATIONVIEWPORTMANAGER_H_ */
