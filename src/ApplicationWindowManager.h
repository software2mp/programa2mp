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
 * la aplicaci�n.
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
