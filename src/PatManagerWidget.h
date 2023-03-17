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

#ifndef PATMANAGERWIDGET_H_
#define PATMANAGERWIDGET_H_

//Includes standard

//Includes Suri
#include "suri/PartContainerWidget.h"

//Includes Wx
#include "wx/listctrl.h"

//Includes App
#include "PatPropertiesHtmlViewerPart.h"
#include "PatInfoPart.h"
#include "Context.h"

// forwards
namespace suri {
   class DataViewManager;
}

DECLARE_EVENT_OBJECT(PatManagerWidgetEvent);
class PatFile;

//! Esta clase es el Administrador de PATs
/**
 * Permite administrar los PATs.
 * Presenta una ventana con una lista donde se muestran los PATs creados e
 * instalados y (a izquierda de la lista) un panel en el cual se muestra una
 * descripcion del PAT seleccionado en la lista(PatPropertiesHtmlViewerPart) o
 * una ventana para poder editar las propiedades del PAT(PatInfoPart).
 * Esta clase permite:
 *    - crear un PAT(sin elementos)
 *    - editar las propiedades de un PAT de usuario.
 *    - abrir un PAT(cargar sus elementos en los contextos).
 *    - eliminar un PAT.
 *    - instalar un PAT comprimido.
 *    - generar un PAT comprimido a partir de un PAT de usuario o propietario.
 */
class PatManagerWidget : public suri::PartContainerWidget {
   //! Ctor. de Copia.
   PatManagerWidget(const PatManagerWidget &PatManagerWidget);

public:

   FRIEND_EVENT_OBJECT(PatManagerWidgetEvent);
   //! Ctor.
   PatManagerWidget(wxTopLevelWindow *pParentFrame, std::list<Context*> *pContextList,
                    wxXmlNode *pNodeInfo, suri::DataViewManager* pDataViewManager);
   //! Dtor.
   virtual ~PatManagerWidget();

   //! Se crea la ventana de herramienta en las clases derivadas
   virtual bool CreateToolWindow();
   //! Evento click sobre boton Ok.
   virtual void OnButtonOk(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Nuevo Cancel.
   virtual void OnButtonCancel(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Nuevo PAT.
   void OnButtonNew(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Nuevo PAT.
   void OnButtonEdit(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Cargar PAT.
   void OnButtonLoad(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Instalar PAT.
   void OnButtonInstall(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Desinstalar PAT.
   void OnButtonDelete(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Comprimir PAT.
   void OnButtonCompress(wxCommandEvent &ButtonEvent);
   //! Evento click sobre boton Help.
   void OnButtonHelp(wxCommandEvent &ButtonEvent);
   //! Evento de seleccion de algun item de la lista.
   void OnItemSelected(wxListEvent &ListEvent);
   //! Evento de deseleccion de algun item de la lista.
   void OnItemDeselected(wxListEvent &ListEvent);
   //! Evento de activacion de algun item de la lista.
   void OnItemActivated(wxListEvent &ListEvent);
   //! Evento que responde a la tecla ESC.
   void OnKeyPress(wxKeyEvent &KeyEvent);
   //! Evento UIUpdate
   void OnUIUpdate(wxUpdateUIEvent &Event);
   //! Evento Close
   void OnClose(wxCloseEvent &Event);
   //! Retorna la ventana padre de las partes.
   virtual wxWindow *GetPartParent();
   //! Retorna el icono de la herramienta
   virtual void GetWindowIcon(wxBitmap& ToolBitmap, int X = 0, int Y = 0) const;
   //! Instala un PAT.
   void Install(const wxString &CommpressedPatUrl);
protected:
   //! Inserta un Part en el contenedor.
   virtual bool DoInsert(suri::Part *pPart);
   //! Elimina el Part pasado por parametro.
   virtual bool DoRemove(suri::Part *pPart);
private:
   //! Genera los items de la lista de PAT's.
   void AddItems(wxArrayString Paths, const std::string &Path);
   //! Carga los elementos del PAT en los contextos.
   bool LoadElements(std::vector<std::string> &NotLoadedElements);
   //! Habilita/Deshabilita los controles.
   void EnableElements(const bool &Enable);
   //! Muestra la ventana del PatPropertiesHtmlViewerPart.
   void ShowPatProperties(const std::string &PathPat = "");
   //! Muestra la ventana del PatInfoPart.
   void ShowPatInfo(const bool &IsEditing = false);
   //! Indica si es un PAT de usuario o no.
   bool IsUserPat(const wxString &PathXml);
   //! Genera los directorios de trabajo de los PAT
   void CreatePatDirs();
   //! Efectua un refresh sobre la lista de Pat's.
   void RefreshPatList();
   //! Elimina todo el directorio del PAT.
   void DeletePat();
   //! Genera un archivo xml temporal.
   std::string CreateXmlTemp(const std::string &PathPat);
   //! Verifica que coincida el xml con los archivos dentro del PAT.
   bool VerifyCompressedPat(const std::string &PathPat);
   //! Genera directorios vacios
   bool CreateTempEmptyElement(const wxXmlNode *pElementNode);
   //! Genera directorios vacios
   bool DoCreateTempEmptyElement(const wxString &UrlElement,
                                 const wxString &DirTempPath);
   //! Carga un PAT en los contextos.
   void DoLoad();
   void LoadContextElements(const wxString& PatVersion, PatFile& Pat,
                            std::vector<std::string> &NotLoadedElements);
   void LoadContextElementsLegacyVersion(PatFile& Pat,
                                         std::vector<std::string> &NotLoadedElements);
   PatPropertiesHtmlViewerPart *pPatProp_;  ///< Widget para mostrar propiedades del PAT, se elimina por eventos de wx
   PatInfoPart *pPatInfo_;  ///< Widget para editar nodo informacion del PAT.
   std::list<Context*> *pListContext_;  ///< lista de los contextos
   wxImageList *pImageList_;  ///< Lista de imagenes.
   std::string selectedPat_;  ///< path del PAT seleccionado (ver OnItemSelected)
   wxXmlNode * pNodeInfo_;  ///< Nodo Informacion del PAT.
   EVENT_OBJECT_PROPERTY(PatManagerWidgetEvent);
   suri::DataViewManager* pDataViewManager_;
};

#endif /*PATMANAGERWIDGET_H_*/
