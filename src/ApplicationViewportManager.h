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
