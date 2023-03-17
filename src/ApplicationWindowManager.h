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

#ifndef APPLICATIONWINDOWMANAGER_H_
#define APPLICATIONWINDOWMANAGER_H_

//Includes estandar
#include <map>
//Includes Suri
//Includes Wx
#include "wx/wx.h"
#include "wx/html/helpctrl.h"
//Includes App
//Defines

/**
 * Esta clase es el singleton que usa suri::Aplication para inicializar
 * la aplicación.
 */
class ApplicationWindowManager {
   //! ctor privado, clase singleton
   ApplicationWindowManager();
   //! ctor de copia.
   ApplicationWindowManager(const ApplicationWindowManager&);
public:
   //! dtor
   virtual ~ApplicationWindowManager();
   //! metodo de obtencion de la instancia
   static ApplicationWindowManager& Instance();
   //! retorna la ventana que la aplicacion considera principal
   wxTopLevelWindow *GetWindow();
   //! carga un parametro como string
   void SetParameter(const std::string &Parameter);
   //! carga los parametros completos
   void SetParameters(std::multimap<std::string, std::string> &Parameters);
   //! inicializa la aplicacion
   static bool InitializeApplication();
   //! inicializa los recursos de la aplicacion
   static void InitializeResources();
   //! Muestra el HelpController
   void GetHelp(int Index = 0);
   //! Retorna objeto que permite imprimir
   wxHtmlEasyPrinting* const GetPrinter();

   static const char SplashBitmap_[];  ///< string que representa al bitmap del splash
   static const char ConfigFile_[];  ///< ruta del archivo de configuracion
private:
   wxHtmlHelpController* pHelpController_;  ///< ayuda de la aplicacion
};

#endif /* APPLICATIONWINDOWMANAGER_H_ */
