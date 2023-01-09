/*! \file MainWindow.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2012-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

//Includes standard
#include <list>

//Includes Suri
#include "suri/Widget.h"
#include "suri/Element.h"

//Includes Wx
#include "wx/string.h"
#include "wx/bitmap.h"
#include "wx/xrc/xmlres.h"
#include "wx/listbook.h"

//Includes App
#include "Context.h"

//Defines

// forwards
namespace suri{
   class DatasourceManagerInterface;
   class ViewcontextManagerInterface;
   class ViewportManagerInterface;
   class GroupManagerInterface;
   class TreeSelectionManagerInterface;
   class LibraryManager;
   class TaskManager;
   class FeatureSelection;
   class DataViewManager;
};

// forwards wx
class wxListbookEvent;

DECLARE_EVENT_OBJECT(MainWindowEvent);

//! Herramienta y ventana principal
/**
 * Esta clase representa la ventana principal de la App.
 * Se encarga de:
 *    - lanzar ventana de splash.
 *    - Agregar los contextos.
 *    - Obtener las propiedades de todos los contextos
 *    - Almacena informacion del PAT cargado.
 * \todo: Hacer que solo se pueda crear usando el Singleton (ctor privado)
 */
class MainWindow : public suri::Widget, public suri::XmlElement {
   //! Ctor. de Copia.
   MainWindow(const MainWindow &MainWindow);

public:
   FRIEND_EVENT_OBJECT(MainWindowEvent);
   //! Devuelve una instancia de la clase.
   static MainWindow& Instance();
   //! Crea la ventana
   virtual bool CreateToolWindow();
   //! Obtiene el titulo del frame que va a crear
   virtual const wxString GetWindowTitle() const;
   //! Setea el titulo del frame
   void SetWindowTitle(const wxString &Title = wxT(""));
   //! Retorna el icono de la herramienta
   virtual void GetWindowIcon(wxBitmap& ToolBitmap, int X = 0, int Y = 0) const;
   //! Agrega un contexto
   bool AddContext(Context *pContext, const wxString &ContextName,
                   const wxString& Label, const wxBitmap& Bitmap);
   //! Agrega un contexto
   bool AddContext(Context *pContext, const wxString &ContextName,
                   const wxString& Label = wxT(""), const wxString& Bitmap = wxT(""));
   //! Abre un PAT
   bool OpenPat();
   //! Guarda el archivo XML
   bool SavePat();
   //! Cierra el PAT cargado
   bool ClosePat();
   //! Obtiene el xml de todos los contextos.
   virtual wxXmlNode *GetProperties();
   //! Guarda una copia del nodo Informacion del PAT activo.
   void SetInfoNode(const wxXmlNode *pInfoNode);
   //! Guarda el path del PAT activo.
   void SetPathPat(const std::string &Path);
   //! Devuelve una copia del nodo informacion.
   wxXmlNode *GetInfoNode();
   //! Devuelve el path del PAT activo.
   wxString GetPathPat() {
      return pathPat_;
   }
   ;
   //! Setea el xml con el estado inicial del PAT.
   void SetInitPatXml(const wxString &DescXml);
   //! Devuelve el xml con el estado inicial del PAT.
   wxString GetInitPatXml() {
      return initPatXml_;
   }
   ;
   //! Devuelve el path de directorio de PAT's de usuario
   std::string GetPathUserPat();
   //! Devuelve el path de directorio de PAT's propietarios
   std::string GetPathRootPat();
   //! Establece el path de directorio de PAT's de usuario
   void SetPathUserPat(const std::string &Path) {
      pathUserPat_ = Path;
   }
   //! Establece el path de directorio de PAT's propietarios
   void SetPathRootPat(const std::string &Path) {
      pathRootPat_ = Path;
   }
   //! Cierra los elementos de todos los contextos.
   bool CloseElements();
   //! Indica si el PAT fue modificado.
   bool IsPatModified();
   //! Evento OnClose
   void OnClose(wxCloseEvent &Event);
   /** Evento de menu contextual sobre la lista */
   void OnContextMenu(wxMenuEvent &Event);
   //! Limpia el directorio temporal y toda su estructura.
   bool ResetTempDir();
   //! Determina si hay un PAT cargado o elementos en cualquier contexto.
   bool IsContextModified();
   //! Invoca al metodo InstallCompressedPat para instalar el PAT.
   void SetParameter(const wxString &CommpressedPatUrl);
   //! Instala el PAT.
   void InstallCommpressedPat(const wxString &CommpressedPatUrl);
   //! Informa si alguno de los contextos tiene un elemento en edicion
   bool GetEdited();
   //! Si hay elemento en edicion pregunta que quiere hacer el usuario
   bool PatCanBeSaved();
   //! Retorna el contexto activo.
   Context* GetActiveContext();
   /** Retorna el contexto por tipo */
   Context *GetContext(const Context::ContextType &Type) const;

protected:
private:
   //! Evento de cambio de pagina
   void OnListbookChanged(wxListbookEvent &Event);
   //! ctor
   MainWindow();
   //! Destructor
   virtual ~MainWindow();
   std::list<Context*> contextList_;   ///< lista de los contextos
   wxString initPatXml_;  ///< xml del estado inicial del PAT.
   wxListbook *pListBook_;  ///< Contiene los contextos
   wxXmlNode *pInfoNode_;  ///< nodo informacion del PAT Activo.
   wxString pathPat_;  ///< path del PAT activo.
   std::string pathRootPat_;  ///< path del directorio de los PAT instalados
   std::string pathUserPat_;  ///< path del directorio de los PAT creados por usuario.
   suri::DatasourceManagerInterface* pDatasourceManager_; ///< Fuente de datos.
   suri::ViewcontextManagerInterface* pViewcontextManager_; ///< Visualizador de contexto.
   suri::ViewportManagerInterface* pViewportManager_;
   suri::GroupManagerInterface* pGroupManager_; ///< Administrador de grupos.
   suri::TreeSelectionManagerInterface* pSelectionManager_; ///< Administrador de arbol de seleccion.
   suri::LibraryManager* pLibraryManager_; ///< Administrador de Bibliotecas.
   suri::TaskManager* pTaskManager_; ///< Administrador de tareas.
   suri::FeatureSelection* pFeatureSelection_; ///< Selector de características.
   suri::DataViewManager* pDataViewManager_; ///< Visualizador de datos.

   EVENT_OBJECT_PROPERTY(MainWindowEvent);
};

#endif /*MAINWINDOW_H_*/

