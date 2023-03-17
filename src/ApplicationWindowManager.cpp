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

#include "ApplicationWindowManager.h"

//Includes estandar
#include <algorithm>

// Tiene definido SVN_REVISION que indica la revision del codigo
// El contenido se genera durante la compilacion
#include "svn_version.h"

//Includes App (para que defina un mensaje utilizado en la clase suri::Application)
#include "messages.h"
//Includes Suri
#include "suri/Application.h"
#include "suri/messages.h"
#include "suri/XmlUrlManager.h"
#include "suri/FileManagementFunctions.h"
//Includes Wx
//Includes App
#include "MainWindow.h"
#include "resources.h"

//Defines
//! Permisos sobre el directorio temporal de la aplicacion.
#define CREATE_PERMISSIONS 0755

//forwards
// Forward para no tener que crear un .h
//! Inicializa recursos de la ventana.
#ifdef APPLICATION_XRC_INITIALIZATION_FUNCTION
APPLICATION_XRC_INITIALIZATION_FUNCTION_FORWARD;
#endif

//! Declara el objeto aplicacion
template<class MainClass>
DECLARE_APP(suri::Application<ApplicationWindowManager>)
//! Implementa el objeto aplicacion
IMPLEMENT_APP(suri::Application<ApplicationWindowManager>)

//! string que representa al bitmap del splash
const char ApplicationWindowManager::SplashBitmap_[] = image_SPLASH_APPLICATION;
//! la ruta del archivo de configuracion
const char ApplicationWindowManager::ConfigFile_[] = "configuracion.xml";

//! Constructor
ApplicationWindowManager::ApplicationWindowManager() :
      pHelpController_(NULL) {
}

//! Destructor.
ApplicationWindowManager::~ApplicationWindowManager() {
   delete pHelpController_;
}

/**
 * Retorna la unica instancia que deberia existir de ApplicationWIndowManager.
 * @return ApplicationWindowManager&: unica instancia de esta clase
 */
ApplicationWindowManager& ApplicationWindowManager::Instance() {
   static ApplicationWindowManager *psingleinstance = new ApplicationWindowManager;
   return *psingleinstance;
}

/**
 * Inicializa la ventana de la aplicacion y retorna el contenido.
 * @return wxTopLevelWindow* Ventana padre de la aplicacion
 */
wxTopLevelWindow *ApplicationWindowManager::GetWindow() {
   // si se vuelve a llamar, no creo nuevamente, retorno la ventana que existe
   MainWindow& mainwindow = MainWindow::Instance();
   mainwindow.GetFrame()->Maximize();
   return mainwindow.GetFrame();
}

/**
 * No hace nada, no reconoce ningun parametro
 * @param[in] Parameter string con parametro que quiero configurar
 */
void ApplicationWindowManager::SetParameter(const std::string &Parameter) {
}

/**
 * No hace nada, no reconoce ningun parametro
 * @param[in] Parameters multimap con los parametros que quiero configurar
 */
void ApplicationWindowManager::SetParameters(
      std::multimap<std::string, std::string> &Parameters) {
}

//! inicializa la aplicacion
/**
 * Elimina directorio temporal de la aplicacion y lo crea para que este vacio.
 * Inicializa clases de factorias que no se incluyen en ningun cpp para que se
 * agregue su .o al ejecutable.
 * \ todo por ahora el metodo devuelve true siempre, pero deberia fallar si
 * no se pudo crear el directorio temporal de la aplicacion.
 * @return true siempre
 */
bool ApplicationWindowManager::InitializeApplication() {
   // cargo variable con directorio temporal.
   wxFileName fn = wxFileName(
         wxStandardPaths().GetTempDir() + "/" + wxTheApp->GetAppName() + ".temp/temp/");
   //Elimino el directorio temporal y lo vuelvo a crear para que este vacio
   //cada vez que inicia la aplicacion
   suri::RemoveDir(fn.GetPath());
   wxFileName::Mkdir(fn.GetPath(), CREATE_PERMISSIONS, wxPATH_MKDIR_FULL);

   volatile suri::XmlUrlManager *p __attribute__ ((unused)) =
   new suri::XmlUrlManager();
   return true;
}

/**
 * Inicializa los resources de la ventana principal de la aplicacion.
 */
void ApplicationWindowManager::InitializeResources() {
#ifdef APPLICATION_XRC_INITIALIZATION_FUNCTION
   APPLICATION_XRC_INITIALIZATION_FUNCTION;
#endif // APPLICATION_XRC_INITIALIZATION_FUNCTION;
}

/**
 * Muestra ventana con la ayuda de la aplicacion. La pagina de ayuda desplegada
 *  se obtiene usando Index.
 * @param[in] Index indice de la pagina de ayuda.
 */
void ApplicationWindowManager::GetHelp(int Index) {
   if (!pHelpController_) {
      pHelpController_ = new wxHtmlHelpController();
      std::string helpdir = suri::Configuration::GetParameter("app_help_dir",
                                                              wxT("./"));
      helpdir += "ayuda.hhp";
      pHelpController_->AddBook(wxFileName(helpdir), false);
   }
   pHelpController_->Display(Index < 0 ? 0 : Index);
   pHelpController_->SetFrameParameters(_("Ayuda: %s"), wxSize(1024, 600));
}

//! Retorna objeto que permite imprimir
wxHtmlEasyPrinting* const ApplicationWindowManager::GetPrinter() {
   return wxGetApp().GetPrinter();
}

