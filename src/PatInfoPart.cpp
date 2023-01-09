//<COPYRIGHT> a reemplazar a futuro

#include "PatInfoPart.h"

// Includes standard
#include <string>
#include <list>

//Includes suri
#include "suri/AuxiliaryFunctions.h"
#include "logmacros.h"
#include "suri/Configuration.h"
#include "suri/FileManagementFunctions.h"
#include "suri/Uri.h"

//Includes de wx
#include "wx/xrc/xmlres.h"
#include "wx/textfile.h"
#include "wx/ffile.h"
#include "wx/dir.h"

//Includes app
#include "MainWindow.h"
#include "PatFile.h"
#include "HotLinkVectorElement.h"
#include "messages.h"
#include "suri/ParameterCollection.h"
#include "ContextHandlerInterface.h"
#include "ContextHandlerFactory.h"
#include "suri/ViewcontextInterface.h"

//Defines
#define EXISTS_CHANGES USE_CONTROL(*pToolWindow_, "ID_TXT_TITLE", wxTextCtrl,	 \
																		IsModified(), false)|| 	 \
							USE_CONTROL(*pToolWindow_, "ID_TXT_AUTHOR", wxTextCtrl,	 \
																		IsModified(), false)|| 	 \
							USE_CONTROL(*pToolWindow_, "ID_TXT_DESC", wxTextCtrl,		 \
																		IsModified(), false)|| 	 \
					USE_CONTROL(*pToolWindow_, "ID_TXT_COPYRIGHT", wxTextCtrl,		 \
																		IsModified(), false)|| 	 \
						USE_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl,		 \
																		IsModified(), false) 	 \

#define COPY_DIR(From, To) do{																\
	wxFileName *pfnfrom=NULL, *pfnto=NULL;													\
	pfnfrom = new wxFileName(From);															\
	pfnto = new wxFileName(To);																\
	if(!suri::CopyDir(*pfnfrom, *pfnto))															\
		REPORT_DEBUG("D:Error al intentar copiar HotLinks");							\
	delete pfnfrom;																				\
	delete pfnto;																					\
}while(0)

void InitPatInfoXmlResource();

START_IMPLEMENT_EVENT_OBJECT(PatInfoPartEvent, PatInfoPart)IMPLEMENT_EVENT_CALLBACK(OnButtonPreview,OnButtonPreview(event),wxCommandEvent)
END_IMPLEMENT_EVENT_OBJECT

/**
 * Constructor.
 * @param[in]	pNodeInfo: puntero al nodo Informacion. Puede ser null.
 * @param[in]	pContextList: puntero a la lista de contextos.
 * @param[in]	IsUserPat: indica si es invocado para trabajar con un PAT de usuario.
 * @param[in]	OldName: nombre actual del PAT.
 * @param[in]	IsEditing: indica si es invocado para editar las propiedades del PAT.
 * @param[in]	UpdateMainToolData: indica si deben actualizarse los datos del PAT guardados en el MainWindow.
 * \attention Cuando se crea un PatInfoPart y no se lo inserta en ningun
 * PartContainerWidget, el puntero debe ser eliminado.
 */
PatInfoPart::PatInfoPart(const wxXmlNode *pNodeInfo, std::list<Context*> *pContextList,
                         const bool &IsUserPat, suri::DataViewManager* pDataviewmanager, const wxString &OldName,
                         const bool &IsEditing, const bool &UpdateMainToolData) :
      NEW_EVENT_OBJECT(PatInfoPartEvent)
{
	pDataViewManager_= pDataviewmanager;
	windowTitle_ = _(caption_PAT_INFORMATION);
   pInfoNode_ = NULL;
   oldNamePat_ = OldName;
   isEditing_ = IsEditing;
   isNew_ = false;
   if (pNodeInfo)
      pInfoNode_ = new wxXmlNode(*pNodeInfo);
   else
      isNew_ = true;
   pContextList_ = pContextList;
   isUserPat_ = IsUserPat;
   updateMainToolData_ = UpdateMainToolData;
   wantedWidth_ = 600;  ///< Ancho prefereido de ventana
   wantedHeight_ = 300;  ///< Alto preferido de ventana
   // En GTK queda mejor con esta dimension
#ifdef __WXGTK__
   wantedWidth_ = 660;  ///< Ancho prefereido de ventana
   wantedHeight_ = 450;  ///< Alto preferido de ventana
#endif
}

//! Dtor.
PatInfoPart::~PatInfoPart() {
   DELETE_EVENT_OBJECT;
   delete pInfoNode_;
}

/**
 * Carga la ventana diseniada. Si tuvo exito, asocia evento de click al boton
 * que permite cargar una imagen previa(para el PAT).
 * Tambien crea un validador para la text donde se ingresa el nombre del PAT,
 * restringiendo los caracteres posibles para el nombre del mismo. Esta
 * restriccion se fundamenta en que el nombre del PAT ingresado sirve para
 * generar el directorio del PAT.
 * @return true si pudo crear la ventana, false C.O.C.
 */
bool PatInfoPart::CreateToolWindow() {
   pToolWindow_ = wxXmlResource::Get()->LoadPanel(pParentWindow_,
                                                  wxT("ID_PANEL_INFOPAT"));
   if (!pToolWindow_) return false;

   //agrego un validador a la text que contiene el nombre del PAT.
   wxArrayString validchars;
   GetValidChars(validchars);
   wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
   validator.SetIncludes(validchars);
   GET_CONTROL(*pToolWindow_, "ID_TXT_TITLE", wxTextCtrl)->SetValidator(validator);

   // Conecto los controles con sus eventos.
   GET_CONTROL(*pToolWindow_, "ID_BMPBTN_OPENPREVIEW", wxBitmapButton)
      ->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(PatInfoPartEvent::OnButtonPreview), NULL,
                pEventHandler_);
   wxSize size = pToolWindow_->GetSize();
   pToolWindow_->SetMinSize(
         wxSize(std::max(wantedWidth_, size.GetWidth()),
                std::max(wantedHeight_, size.GetHeight())));
   return true;
}

/**
 * Carga los controles con el contenido del nodo informacion del PAT.
 */
void PatInfoPart::SetInitialValues() {
   wxString name, author, desc, copy, preview;
   GetNodeValues(name, author, copy, desc, preview);
   GET_CONTROL(*pToolWindow_, "ID_TXT_TITLE", wxTextCtrl)->SetValue(name);
   GET_CONTROL(*pToolWindow_, "ID_TXT_AUTHOR", wxTextCtrl)->SetValue(author);
   GET_CONTROL(*pToolWindow_, "ID_TXT_DESC", wxTextCtrl)->SetValue(desc);
   GET_CONTROL(*pToolWindow_, "ID_TXT_COPYRIGHT", wxTextCtrl)->SetValue(copy);
   GET_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl)->SetValue(preview);
}

/**
 * Indica si el Part tiene cambios para salvar verificando el estado de las
 * textctrl y comparando su contenido con el del nodo informacion.
 * @return true en caso de haber cambios. false C.O.C.
 */
bool PatInfoPart::HasChanged() {
   if (modified_) return modified_;

   wxString name, author, desc, copy, preview, errormsg;
   GetNodeValues(name, author, copy, desc, preview);
   if (EXISTS_CHANGES)
      return (name.Cmp(USE_CONTROL(*pToolWindow_, "ID_TXT_TITLE", wxTextCtrl,
            GetValue(), "")) != 0
            || author.Cmp(USE_CONTROL(*pToolWindow_, "ID_TXT_AUTHOR", wxTextCtrl,
                  GetValue(), "")) != 0
            || desc.Cmp(
                  USE_CONTROL(*pToolWindow_, "ID_TXT_DESC", wxTextCtrl, GetValue(),
                        "")) != 0
            || copy.Cmp(USE_CONTROL(*pToolWindow_, "ID_TXT_COPYRIGHT", wxTextCtrl,
                  GetValue(), "")) != 0
            || preview.Cmp(USE_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl,
                  GetValue(), "")) != 0);
   return false;
}

/**
 * Aplica los cambios al Nodo de informacion.
 * \pre el nombre del PAT no debe presentar conflictos con otros PATs.
 * \post En caso de que el PAT cuente con HotLinks copia los archivos asociados a
 *       los HotLinks al directorio del PAT.
 * @return true en caso de salvar el PAT con exito. false C.O.C.
 */
bool PatInfoPart::CommitChanges() {
   // Obtiene el nodo informacion
   pInfoNode_ = CreateInfoNode();
   if (!pInfoNode_) return false;

   // verifica que el nombre del PAT no presente conflictos(con otros PATs)
   if (!CheckPatName()) {
      DeletePatName();
      modified_ = true;
      REPORT_AND_FAIL_VALUE("D:Falla CheckPatName.", false);
   }

   //verifico la existencia del preview, si es que lo cargaron.
   wxString preview = USE_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl,
         GetValue(), "");
   if (!preview.IsEmpty()) {
      wxFileName fnpreview(preview);
      if (!fnpreview.FileExists()) {
         SHOW_ERROR(message_MISSING_PREVIEW);
         //No se traduce. Lo hace SHOW_
         modified_ = true;
         return false;
      }
      //verifico que el archivo cargado sea una imagen valida.
      wxImage img(fnpreview.GetFullPath());
      if (!img.IsOk()) {
         SHOW_ERROR(message_INVALID_IMAGE);
         //No se traduce. Lo hace SHOW_
         modified_ = true;
         return false;
      }
   }

   // obtiene el contenido del xml en string y luego genera el xml del PAT.
   wxString descxml = "";
   if (!CreatePat(descxml, pInfoNode_)) {
      SHOW_ERROR(message_PAT_XML_GENERATION_ERROR);
      //no se traduce, lo hace SHOW_
      return false;
   }
   wxFileName fnpat(
         MainWindow::Instance().GetPathUserPat() + GetPatName(pInfoNode_)
               + wxFileName::GetPathSeparator(wxPATH_UNIX));
   wxString pathpat = fnpat.GetVolume().c_str() + fnpat.GetVolumeSeparator()
         + fnpat.GetFullPath(wxPATH_UNIX);
   if (!SaveXml(pathpat, descxml)) {
      SHOW_ERROR(message_PAT_XML_SAVE_ERROR);
      //no se traduce, lo hace SHOW
      return false;
   }

   std::string hlpath = suri::Configuration::GetParameter("app_temp_dir", wxT(""));
   std::string common = suri::Configuration::GetParameter("pat_common_dir", wxT(""));

   //copio los hotlinks y los vectores al directorio del PATH
   COPY_DIR(hlpath + common, pathpat + common);
   //Modifico los nodo url del vector con la nueva ubicacion.
   ModifyUrlVector(pathpat.c_str(), pathpat.c_str() + common);

   /* Si estoy llamando al PatInfoPart dde. el MainWindow o si lo estoy llamando dde.
    * el PatManagerWidget para Editar el PAT que se encuentra cargado, entonces
    * actualizo los datos del MainWindow.*/
   wxString newnamepat = GetPatName(pInfoNode_);
   wxString oldnamepat = GetPatName(MainWindow::Instance().GetInfoNode());
   if (updateMainToolData_
         || (!updateMainToolData_ && !isNew_ && oldnamepat.Cmp(newnamepat) == 0)) {
      MainWindow::Instance().SetInfoNode(pInfoNode_);
      wxString pathaux = fnpat.GetVolume().c_str() + fnpat.GetVolumeSeparator()
            + fnpat.GetFullPath(wxPATH_UNIX).c_str();
      pathaux += "descripcion.xml";
      MainWindow::Instance().SetPathPat(pathaux.c_str());
      MainWindow::Instance().SetInitPatXml(descxml);
      MainWindow::Instance().SetWindowTitle(GetPatName(pInfoNode_).c_str());

   }

   GET_CONTROL(*pToolWindow_, "ID_TXT_TITLE", wxTextCtrl)->DiscardEdits();
   GET_CONTROL(*pToolWindow_, "ID_TXT_AUTHOR", wxTextCtrl)->DiscardEdits();
   GET_CONTROL(*pToolWindow_, "ID_TXT_DESC", wxTextCtrl)->DiscardEdits();
   GET_CONTROL(*pToolWindow_, "ID_TXT_COPYRIGHT", wxTextCtrl)->DiscardEdits();
   GET_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl)->DiscardEdits();
   SetModified(false);

   /* comento para que no borre el temporal cada vez que se salva para corregir
    * error que no muestra los hl despues de salvar el PAT.*/
#ifdef __UNUSED_CODE__
   MainWindow::Instance().ResetTempDir();
#endif
   return true;
}

/**
 * Establece los ultimos valores guardados en el nodo informacion, en los
 * controles del Part.
 */
bool PatInfoPart::RollbackChanges() {
   SetInitialValues();
   return true;
}

/**
 * Metodo que responde al evento de click sobre el boton de dialogo de apertura
 * de archivo. Permite seleccionar la imagen previa del PAT.
 */
void PatInfoPart::OnButtonPreview(wxCommandEvent &ButtonEvent) {
   wxFileDialog filedlg(pToolWindow_, _(caption_SELECT_PAT_PREVIEW), wxT(""), wxT(""),
                        filter_COMMON_IMAGE_FORMATS_LOAD);
   if (filedlg.ShowModal() == wxID_CANCEL) return;
   wxFileName fnpreview(filedlg.GetPath().c_str());
   wxString pathpreview = fnpreview.GetVolume().c_str()
         + (fnpreview.HasVolume() ? fnpreview.GetVolumeSeparator() : wxT(""))
         + fnpreview.GetFullPath(wxPATH_UNIX);
   GET_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl)->ChangeValue(pathpreview);
   GET_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl)->SetModified(true);
   ButtonEvent.Skip();
}

/**
 * Objetivo: Devuelve una copia del nodo Informacion.
 * @return nodo Informacion o null en caso de error.
 * \attention quien invoque a este metodo es responsable del puntero devuelto,
 * por lo tanto debe hacerse cargo de su eliminacion.
 */
wxXmlNode *PatInfoPart::GetInfoNode() {
   if (pInfoNode_) return new wxXmlNode(*pInfoNode_);

   return NULL;
}

/**
 * Objetivo:	Genera el archivo descripcion.xml y lo devuelve en un string.
 * @param[in]	pNodeInfo: nodo informacion. Puede ser NULL cuando no se disponen de la info.
 * @param[out]	DescXml: contenido del archivo descripcion xml.
 * @return true en caso de poder generar la info. false C.O.C.
 */
bool PatInfoPart::CreatePat(wxString &DescXml, const wxXmlNode *pNodeInfo) {
   DescXml.clear();
   if (!pNodeInfo) return true;

   wxXmlNode *pnode;

   // Si esta editando solo me interesa obtener el nodo informacion.
   if (isEditing_) {
      wxXmlNode *pnodeinfo = new wxXmlNode(
            pNodeInfo == NULL ? *pInfoNode_ : *pNodeInfo);
      pnode = CreatePatNode();  //creo el nodo pat
      if (!pnode)
         REPORT_AND_FAIL_VALUE(
               "D:Error al crear archivo descripcion.xml: no se pudo crear raiz.",
               false);
      pnode->AddChild(new wxXmlNode(*pnodeinfo));
      delete pnodeinfo;

      //Agrego los contextos que ya tenia
      wxFileName fnpat(
            MainWindow::Instance().GetPathUserPat() + oldNamePat_
                  + wxFileName::GetPathSeparator(wxPATH_UNIX));
      /** Toma el path del archivo de configuración. Si este no tiene valor alguno,
       * usa la ruta del archivo. */
      wxString pathpat = "";
      wxString configurationPath = suri::Configuration::GetParameter(
            "app_working_dir", "");
      if (!configurationPath.CompareTo("No definido")) {
         pathpat = configurationPath + "\\" + fnpat.GetFullName();
      } else {
         pathpat = fnpat.GetVolume().c_str() + fnpat.GetVolumeSeparator()
               + fnpat.GetFullPath(wxPATH_UNIX);
      }

      /* Cargo el xml existente, el que estoy modificando, para obtener los
       * contextos y agregarlos despues del nuevo nodo informacion */
      wxString pathpatold = pathpat + "descripcion.xml";
      wxXmlDocument doc;
      if (!doc.Load(pathpatold, suri::XmlElement::xmlEncoding_)) return false;

      wxXmlNode *pnodecontext = doc.GetRoot()->GetChildren();

      //obtengo nodo informacion
      wxXmlNode *ppreceding = pnode->GetChildren();
      while (ppreceding && ppreceding->GetName().Cmp(INFORMATION_NODE) != 0)
         ppreceding = ppreceding->GetNext();
      if (!ppreceding) {
         REPORT_DEBUG("D:No se pudo obtener nodo informacion.");
         return false;
      }

      //obtengo los contextos
      while (pnodecontext && pnodecontext->GetName().Cmp(CONTEXT_NODE) != 0)
         pnodecontext = pnodecontext->GetNext();
      while (pnodecontext) {
         //uso el ctor. de copia para eliminar el padre y el ste. Ver InsertChildAfter
         wxXmlNode *pchild = new wxXmlNode(*pnodecontext);

         #if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION == 8) && (wxRELEASE_NUMBER <8)
         pchild->SetParent(ppreceding->GetParent());
         pchild->SetNext(ppreceding->GetNext());
         ppreceding->SetNext(pchild);
#else
         if (!pnode->InsertChildAfter(pchild, ppreceding))
         REPORT_DEBUG("D:Falla InsertChildAfter");
         ppreceding = pchild;
#endif
         pnodecontext = pnodecontext->GetNext();
      }
   } else
      pnode = GetNewPat(pNodeInfo);

   wxXmlNode* pextranode = pnode->GetChildren();
   	wxXmlNode* pfinalnode = NULL;
   	wxXmlNode* pprevnode = NULL;
   	wxString proptype = "";
	wxString propname = "";
 	suri::ParameterCollection params;
 	suri::ContextHandlerInterface* pxmlhandler =
 			suri::ContextHandlerFactory::GetInstance()->Create(XMLMODE, params);

 	// Nos desplazamos al siguiente nodo para
 	// evitar que pase con el nodo informacion.
 	pprevnode = pextranode; // Info node.
 	pextranode = pextranode->GetNext();

 	while(pextranode != NULL) {
 		if( pextranode->GetName().CompareTo(wxT(CONTEXT_NODE)) == 0) {
 		 	pextranode->GetPropVal(wxT(TYPE_NODE), &proptype);
 		 	pextranode->GetPropVal(wxT(NAME_NODE), &propname);

 		 	/** Saver */
 		 	pfinalnode = pxmlhandler->Save(pextranode, pDataViewManager_, proptype, propname);
 		 	/**********/

 		 	if (pfinalnode != NULL) {
				wxXmlProperty *nameprop = new wxXmlProperty(wxT(NAME_NODE), propname);
				wxXmlProperty *typeprop = new wxXmlProperty(wxT(TYPE_NODE), proptype);
				typeprop->SetNext(nameprop);
				pfinalnode->SetProperties(typeprop);

				if ( pnode->RemoveChild( pextranode ) )
					if ( !pnode->InsertChildAfter(pfinalnode, pprevnode))
						return false; // No pudo insertar el nodo

 		 	}else
 		 		pfinalnode = pextranode;
 		}
 		pextranode = pprevnode->GetNext()->GetNext();
 		pprevnode = pfinalnode; // seteo como previo al recien insertado.
 	}


   //salvo el xml en un string.
   PatFile::SaveXml(DescXml, pnode);
   delete pnode;
   delete pxmlhandler;

   return true;
}

/**
 * Objetivo: Elimina el directorio pasado por parametro y todo su contenido.
 * @param[in]	DirPath: path absoluto del directorio a eliminar.
 * @param[out]	ErrorMsg: mensaje del error producido.
 * @return true en caso de poder eliminar todo el directorio. false C.O.C.
 */
bool PatInfoPart::DeleteFiles(const wxString &DirPath, wxString &ErrorMsg) {
   ErrorMsg.Empty();
   wxFileName *pfn = new wxFileName(DirPath);
   wxString pathdiraux = pfn->GetVolume().c_str()
         + (pfn->HasVolume() ? pfn->GetVolumeSeparator() : wxT(""));
   pathdiraux += pfn->GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX);
   wxDir *pdir = new wxDir(pathdiraux);
   if (!pdir->IsOpened()) {
      ErrorMsg = wxString::Format(_(message_PAT_UNINSTALL_ERROR_s),
                                  pfn->GetDirs().Item(pfn->GetDirCount() - 1).c_str());
      delete pfn;
      delete pdir;
      return false;
   }
   delete pdir;
   if (!suri::RemoveDir(*pfn)) {
      ErrorMsg = wxString::Format(_(message_DELETE_FILE_ERROR_s), DirPath.c_str());
      delete pfn;
      return false;
   }
   ErrorMsg.Empty();
   delete pfn;
   return true;
}

/**
 * Objetivo: Crea el directorio que contiene al PAT y salva el archivo
 * descripcion.xml
 * @param[in]	PathPat: path absoluto del directorio a crear.
 * @param[in]	XmlDesc: contenido del archivo descripcion.xml
 * @param[in]	OldNamePat: nombre anterior del PAT, para verificar si se
 * sobreescribe el PAT o no.
 * @return	Crea_Ok? true:false;
 */
bool PatInfoPart::SaveXml(const wxString &PathPat, const wxString &XmlDesc,
                          const wxString &OldNamePat) {
   wxFileName fn(PathPat);
   if (fn.IsDir()) {
      //se modifico el nombre del PAT
      if (OldNamePat != wxEmptyString) {
         fn.RemoveLastDir();
         wxString pathaux = fn.GetVolume().c_str()
               + (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""));
         pathaux += fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX);
         pathaux += OldNamePat;
         pathaux += fn.GetPathSeparator(wxPATH_UNIX);
         if (wxDirExists(pathaux)) {
            wxString errormsg;
            if (!DeleteFiles(pathaux, errormsg))
            SHOW_ERROR(errormsg);
         }
      }

      wxString pathdir = fn.GetVolume().c_str()
            + (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""));
      pathdir += fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX);
      if (!fn.Mkdir(pathdir, 0755, wxPATH_MKDIR_FULL))
      REPORT_AND_FAIL_VALUE("D:Error al crear directorio %s.", false);
   } else
      REPORT_AND_FAIL_VALUE("D:Error al crear directorio %s.", false, PathPat.c_str());

   wxString pathpat = fn.GetVolume().c_str()
         + (fn.HasVolume() ? fn.GetVolumeSeparator() : wxT(""));
   pathpat += fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX);
   pathpat += "descripcion.xml";

   return PatFile::SaveXml(pathpat, XmlDesc);
}

//------------------------------------------------------------------------------
//-----------------------------------PRIVADAS-----------------------------------
//------------------------------------------------------------------------------

/**
 * Objetivo: Crea un nodo informacion a partir de los datos ingresados por el usr.
 * @return nodo informacion creado.
 * \attention quien invoque a este metodo es responsable del puntero devuelto,
 * por lo tanto debe hacerse cargo de su eliminacion.
 */
wxXmlNode *PatInfoPart::CreateInfoNode() {
   wxString name, author, desc, day, month, year, copyright, preview;
   GetUsrValues(name, author, desc, copyright, preview, day, month, year);

   wxXmlNode *pnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(INFORMATION_NODE));  //No se traduce, elemento XML
   //grabo preview
   wxXmlNode *pchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(PREVIEW_NODE));  //No se traduce, elemento XML
   wxXmlNode *pchild2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(URL_NODE));  //No se traduce, elemento XML
   if (!preview.IsEmpty()) {
      wxFileName fnpreview(preview);
      wxString pathpreview = fnpreview.GetVolume().c_str()
            + fnpreview.GetVolumeSeparator();
      pathpreview += fnpreview.GetFullPath(wxPATH_UNIX);
      pchild2->AddChild(
            new wxXmlNode(pchild2, wxXML_TEXT_NODE, wxEmptyString, pathpreview));
   }
   pchild->AddChild(new wxXmlNode(*pchild2));
   pnode->InsertChild(new wxXmlNode(*pchild), NULL);
   delete pchild2;
   delete pchild;

   //grabo copyright
   pchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(COPYRIGHT_NODE));  //No se traduce, elemento XML
   pchild->AddChild(
         new wxXmlNode(pchild, wxXML_TEXT_NODE, wxEmptyString, wxT(copyright)));
   pnode->InsertChild(new wxXmlNode(*pchild), pnode->GetChildren());
   delete pchild;

   //grabo la fecha
   pchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(DATE_NODE));  //No se traduce, elemento XML
   pchild2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(YEAR_NODE));  //No se traduce, elemento XML
   pchild2->AddChild(new wxXmlNode(pchild2, wxXML_TEXT_NODE, wxEmptyString, year));
   pchild->InsertChild(new wxXmlNode(*pchild2), pchild->GetChildren());
   delete pchild2;
   pchild2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(MONTH_NODE));  //No se traduce, elemento XML
   pchild2->AddChild(new wxXmlNode(pchild2, wxXML_TEXT_NODE, wxEmptyString, month));
   pchild->InsertChild(new wxXmlNode(*pchild2), pchild->GetChildren());
   delete pchild2;
   pchild2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(DAY_NODE));  //No se traduce, elemento XML
   pchild2->AddChild(new wxXmlNode(pchild2, wxXML_TEXT_NODE, wxEmptyString, day));
   pchild->InsertChild(new wxXmlNode(*pchild2), pchild->GetChildren());
   delete pchild2;
   pnode->InsertChild(new wxXmlNode(*pchild), pnode->GetChildren());
   delete pchild;

   //grabo la descripcion
   pchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(DESCRIPTION_NODE));  //No se traduce, elemento XML
   pchild->AddChild(new wxXmlNode(pchild, wxXML_TEXT_NODE, wxEmptyString, wxT(desc)));
   pnode->InsertChild(new wxXmlNode(*pchild), pnode->GetChildren());
   delete pchild;

   //grabo el autor
   pchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(AUTHOR_NODE));
   pchild->AddChild(new wxXmlNode(pchild, wxXML_TEXT_NODE, wxEmptyString, wxT(author)));
   pnode->InsertChild(new wxXmlNode(*pchild), pnode->GetChildren());
   delete pchild;

   //grabo el nombre del PAT
   pchild = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(NAME_NODE));
   pchild->AddChild(new wxXmlNode(pchild, wxXML_TEXT_NODE, wxEmptyString, wxT(name)));
   pnode->InsertChild(new wxXmlNode(*pchild), pnode->GetChildren());
   delete pchild;

   //comento para eliminar copia innecesaria.
   //return new wxXmlNode(*pnode);
   return pnode;
}

/**
 * Objetivo: parsea el nodo informacion y devuelve sus valores.
 * @param[out]	Name: nombre del PAT.
 * @param[out]	Author: autor del PAT.
 * @param[out]	Copyright: copyright del PAT.
 * @param[out] Description: descripcion del PAT.
 * @param[out] Preview: imagen que se utiliza en la descripcion del PAT.
 */
void PatInfoPart::GetNodeValues(wxString &Name, wxString &Author, wxString &Copyright,
                                wxString &Description, wxString &Preview) {
   Name = wxT(""), Author = wxT(""),
   Description = wxT(""), Copyright = wxT(""),
   Preview = wxT("");

   //parseo el nodo.
   if (pInfoNode_) {
      wxXmlNode *pchild = pInfoNode_->GetChildren();
      while (pchild) {
         if (pchild->GetName().Cmp(NAME_NODE) == 0)
            Name = pchild->GetNodeContent().c_str();

         if (pchild->GetName().Cmp(AUTHOR_NODE) == 0)
            Author = pchild->GetNodeContent().c_str();

         if (pchild->GetName().Cmp(DESCRIPTION_NODE) == 0)
            Description = pchild->GetNodeContent().c_str();

         if (pchild->GetName().Cmp(COPYRIGHT_NODE) == 0)
            Copyright = pchild->GetNodeContent().c_str();

         if (pchild->GetName().Cmp(PREVIEW_NODE) == 0)
            Preview = pchild->GetChildren()->GetNodeContent().c_str();

         pchild = pchild->GetNext();
      }

      if (!isUserPat_ || Name.IsEmpty()) Name = wxT("");
   }
}

/**
 * Objetivo: Devuelve los valores cargados por el usr. en las text; y la fch.
 * @param[out]	Name: nombre del PAT.
 * @param[out]	Author: autor del PAT.
 * @param[out]	Copyright: copyright del PAT.
 * @param[out] Description: descripcion del PAT.
 * @param[out] Preview: imagen que se utiliza en la descripcion del PAT.
 * @param[out]	Day: dia actual.
 * @param[out]	Month: mes actual.
 * @param[out] Year: anio actual.
 */
void PatInfoPart::GetUsrValues(wxString &Name, wxString &Author, wxString &Description,
                               wxString &Copyright, wxString &Preview, wxString &Day,
                               wxString &Month, wxString &Year) {
   //obtengo url de la imagen de preview
   Preview = USE_CONTROL(*pToolWindow_, "ID_TXT_PREVIEW", wxTextCtrl,
         GetValue(), "");
   //obtengo copyright
   Copyright = USE_CONTROL(*pToolWindow_, "ID_TXT_COPYRIGHT", wxTextCtrl,
         GetValue(), "");
   //grabo la fecha
   Day = suri::LongToString(wxDateTime::Today().GetDay(), 2);
   /*TODO(16/09/2008 - Alejandro): Checkear valor de retorno de  GetMonth() en
    * linux. En win. devuelve el mes anterior por eso le sumo 1*/
   Month = suri::LongToString(wxDateTime::Today().GetMonth() + 1, 2);
   Year = suri::LongToString(wxDateTime::Today().GetYear());

   //obtengo la descripcion
   Description = USE_CONTROL(*pToolWindow_, "ID_TXT_DESC", wxTextCtrl,
         GetValue(),"");

   //obtengo el autor
   Author = USE_CONTROL(*pToolWindow_, "ID_TXT_AUTHOR", wxTextCtrl, GetValue(),
         "");
   //obtengo el nombre del PAT
   Name = USE_CONTROL(*pToolWindow_, "ID_TXT_TITLE", wxTextCtrl, GetValue(),"");
}

/**
 * Objetivo: Devuelve el contenido del nodo nombre que se encuentra dentro del
 * nodo informacion.
 * @param[in]	pNode: puntero al nodo informacion.
 * @return wxString con el Nombre del PAT
 */
wxString PatInfoPart::GetPatName(const wxXmlNode *pNode) {
   if (!pNode) return wxEmptyString;

   //Verifico que sea el nodo informacion.
   wxString aux = pNode->GetName();
   if (aux.IsEmpty() || aux.Cmp(INFORMATION_NODE) != 0) return wxEmptyString;

   wxXmlNode *pchild = pNode->GetChildren();
   while (pchild && pchild->GetName().Cmp(NAME_NODE) != 0)
      pchild = pchild->GetNext();

   if (!pchild) return wxT("");  //esto es un error, no deberia suceder nunca.

   return pchild->GetNodeContent().c_str();
}

/**
 * Objetivo: Genera y devuelve un nodo xml que contiene toda la informacion
 * del PAT(nodo informacion y de cada contexto en particular).
 * @param[in]	pInfoNode: nodo informacion.
 * @return un nodo xml con toda la info. del PAT o null en caso de error.
 * \attention el puntero retornado es creado por el metodo CreatePat, por lo
 * tanto quien lo reciba debe encargarse de eliminar el puntero recibido.
 */
wxXmlNode * PatInfoPart::GetNewPat(const wxXmlNode *pInfoNode) {
   wxXmlNode *pnode = NULL;

   //reemplazo por linea ste.
#ifdef __UNUSED_CODE__
   wxXmlNode *pnodeinfo = NULL;

   if(!pInfoNode)
   pnodeinfo = new wxXmlNode(*pInfoNode_);
   else
   pnodeinfo = new wxXmlNode(*pInfoNode);
#endif

   wxXmlNode *pnodeinfo = new wxXmlNode(pInfoNode == NULL ? *pInfoNode_ : *pInfoNode);

   pnode = CreatePatNode();  //creo el nodo pat
   if (!pnode) {
      delete pnodeinfo;
      REPORT_AND_FAIL_VALUE(
            "D:Error al crear archivo descripcion.xml: no se pudo crear raiz.", NULL);
   }
   pnode->AddChild(new wxXmlNode(*pnodeinfo));
   delete pnodeinfo;

   /* agrego los contextos.(Solo cuando se esta creando un PAT nuevo dde el
    * PatManagerWidget cargo los contextos vacios)*/
   if (!AddContexts(pnode, (isNew_ && !updateMainToolData_))) {
      delete pnode;
      REPORT_AND_FAIL_VALUE(
            "D:Error al crear archivo descripcion.xml: no se pudo agregar contextos.",
            NULL);
   }
   return pnode;
}

/**
 * Objetivo: Obtiene los subdirectorios que existen dentro de un PAT.
 * @param[in]	pVecItems: vector de strings con los paths de los items dentro del PAT.
 * @param[in]	PathPat: path del PAT. ("C:\PATs_INSTALADOS\MyPat\")
 * @param[out]	pVecDirs: vector de strings con los paths de los subdirectorios.
 */
void PatInfoPart::GetSubDirectories(const wxArrayString *pVecItems,
                                    const wxString PathPat, wxArrayString *pVecDirs) {
   wxString pathaux = "";
   for (size_t i = 0; i < pVecItems->GetCount(); i++) {
      wxFileName fn(pVecItems->Item(i).c_str());
      fn.RemoveLastDir();
      pathaux = fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX);
      if (pathaux.Cmp(PathPat) != 0 && pathaux.Len() > PathPat.Len()
            && CanInsert(
                  fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX).c_str(),
                  pVecDirs))
         pVecDirs->Add(
               fn.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR, wxPATH_UNIX).c_str());
   }

   if (pathaux.Cmp(PathPat) != 0 && pathaux.Len() > PathPat.Len())
      GetSubDirectories(pVecDirs, PathPat, pVecDirs);
}

/**
 * Objetivo: Carga los caracteres validos para generar un nombre de directorio.
 * @param[out] Chars: vector donde se devuelven los caracteres.
 * Nota: se restringen las vocales acentuadas, enie(may, min), y los simbolos:
 * comilla doble(34), asterisco(42), barra(47), 2ptos(58), menor(60), mayor(62),
 * pregunta(63), barra inv.(92), pipe(124)"
 */
void PatInfoPart::GetValidChars(wxArrayString &Chars) {
   for (short i = 32; i < 128; i++)
      if (i != 34 && i != 42 && i != 47 && i != 58 && i != 60 && i != 62 && i != 63
            && i != 92 && i != 124) Chars.Add(wxString::Format("%c", i));
}

/**
 * Objetivo: Valida el nombre del PAT y emite un mensaje de error en caso de que
 * no se cumplan las siguientes validaciones:
 * 	- no exista un PAT propietario con el mismo nombre.
 * 	- no exista un PAT de usuario con el mismo nombre.
 * 	- nombre del PAT es vacio("").
 * En el 2do caso, no da un error sino que pregunta si desea sobreescribir.
 * @return pasa validacion? true : false;
 */
bool PatInfoPart::CheckPatName() {
   wxString errormsg = wxT("");
   switch (CheckPatName(GetPatName(pInfoNode_), oldNamePat_, errormsg)) {
      case 0:  //no hubo errores
         return true;
      case 1:  //existe un PAT propietario con el mismo nombre.
      {
         SHOW_ERROR(errormsg);
         break;
      }
      case 2:  //existe un PAT de usuario con el mismo nombre.
      {
         if (SHOWQUERY(GetWindow(), errormsg,
               wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION) == wxID_YES) return true;
         break;
      }
      case 3:  //nombre de PAT vacio.
      {
         SHOWQUERY(GetWindow(), errormsg, wxCANCEL|wxICON_ERROR);
         break;
      }
      default: {
         SHOW_WARNING(errormsg);
         break;
      }
   }
   return false;
}

/**
 * Objetivo:	Verifica que no exista un PAT(de usr. o propietario) con el mismo
 * nombre que se le esta dando al PAT que se va a grabar.
 * @param[in]	PatName: nombre del PAT, que se usa para crear el directorio que
 * va a contener al PAT.
 * @param[in]	OldName: nombre anterior del PAT(solo tiene contenido cuando se
 * modifica el nombre del PAT.)
 * @param[out]	ErrorMsg: sirve para devolver un mensaje de error.
 * @return 0 PAT de usuario
 * @return 1 PAT de usuario con el mismo nombre
 * @return 3 Nombre de PAT incorrecto
 */
int PatInfoPart::CheckPatName(const wxString &PatName, const wxString &OldName,
                              wxString &ErrorMsg) {
   ErrorMsg = wxT("");
   wxString patnameaux = PatName;
   patnameaux = patnameaux.Trim(false);
   patnameaux = patnameaux.Trim();
   if (patnameaux.IsEmpty()) {
      ErrorMsg = _(message_PAT_NAME_MISSING);
      return 3;
   }

   /* si el nombre nuevo y el ant. son iguales y se trata de un PAT de usuario,
    * no pregunto si quiere sobreescribir.*/
   if (OldName != wxT("") && patnameaux.Cmp(OldName) == 0
         && (MainWindow::Instance().GetPathPat().StartsWith(
               MainWindow::Instance().GetPathUserPat().c_str()))) return 0;

// Se elimina la validacion de PAT instalado con el mismo nombre
#ifdef __UNUSED_CODE__
   //Valido que no haya un PAT instalado con el mismo nombre.
   wxDir dir(MainWindow::Instance().GetPathRootPat());
   if (!wxDir::Exists(MainWindow::Instance().GetPathRootPat()))
   wxFileName::Mkdir(MainWindow::Instance().GetPathRootPat(), 0755, wxPATH_MKDIR_FULL);
   if(!dir.IsOpened())
   {
      ErrorMsg = wxString::Format(_(message_OPEN_DIR_ERROR_s),
            MainWindow::Instance().GetPathRootPat().c_str());
      return 1;
   }

   wxString pathaux = MainWindow::Instance().GetPathRootPat() + patnameaux;
   if(dir.Exists(pathaux))
   {
      ErrorMsg = _(message_PORPIETARY_PAT_EXISTS);
      REPORT_AND_FAIL_VALUE("D:Error al intentar crear PAT. Ya existe un PAT propietario con el mismo nombre",1);
   }
   pathaux.Clear();
#else
   wxString pathaux = wxT("");
#endif

   // Valido que no haya un PAT de usuario con el mismo nombre.
   wxDir usrdir(MainWindow::Instance().GetPathUserPat());
   if (!wxDir::Exists(MainWindow::Instance().GetPathUserPat()))
      wxFileName::Mkdir(MainWindow::Instance().GetPathUserPat(), 0755,
                        wxPATH_MKDIR_FULL);
   if (!usrdir.IsOpened()) {
      ErrorMsg = wxString::Format(_(message_OPEN_DIR_ERROR_s),
                                  MainWindow::Instance().GetPathUserPat().c_str());
      return 1;
   }

   pathaux = MainWindow::Instance().GetPathUserPat() + patnameaux;
   if (usrdir.Exists(pathaux)) {
      ErrorMsg = _(question_USER_PAT_EXISTS);
      REPORT_AND_FAIL_VALUE("D:Ya existe un PAT de usuario con ese nombre.", 2);
   }

   return 0;
}

/**
 * Objetivo: Agrega los nodos contextos al xml.
 * @param[in]	pNode: nodo pat.(la raiz)
 * @param[in]	EmptyContexts: indica si hay que agregar los contextos sin sus
 * elementos.
 * @return true Pudo agregar los contextos
 * @return false Error al agregar contextos
 */
bool PatInfoPart::AddContexts(wxXmlNode *pNode, const bool &EmptyContexts) {
   //busco el nodo informacion para agregar a continuacion los nodos contextos
   wxXmlNode *pnodeinfo = pNode->GetChildren();
   while (pnodeinfo && pnodeinfo->GetName().Cmp(INFORMATION_NODE) != 0)
      pnodeinfo = pnodeinfo->GetNext();

   if (!pnodeinfo)
   REPORT_AND_FAIL_VALUE("D:Error al agregar contextos: nodo PAT vacio.", false);
   if (!pContextList_)
      REPORT_AND_FAIL_VALUE("D:Error al agregar contextos: lista de contextos vacia.",
                            false);
   //Agrego los contextos.
   std::list<Context*>::const_iterator contextit = pContextList_->begin();
   for (; contextit != pContextList_->end(); contextit++) {
      wxXmlNode *pcontextnode;
      wxXmlNode *pnodeaux = NULL;
      if (EmptyContexts) {
         /* agrego los contextos vacios. Si hay un PAT cargado, antes de insertar
          * el nodo, elimino los hijos para obtener el contexto vacio.*/
         if (pnodeaux) {
            delete pnodeaux;
            pnodeaux = NULL;
         }
         pnodeaux = new wxXmlNode(*((*contextit)->GetProperties()));
         while (pnodeaux->GetChildren())
            pnodeaux->RemoveChild(pnodeaux->GetChildren());
         pcontextnode = pnodeaux;
      } else {
         pcontextnode = (*contextit)->GetProperties();
         /* Modifico el path del elemento para que el separador de directorios
          * sea la barra de unix "/" y no la de win "\" */
         wxXmlNode *pnodeaux = pcontextnode->GetChildren();
         //recorro todos los elementos del contexto
         while (pnodeaux) {
            //Obtengo el 1er hijo del nodo elemento
            wxXmlNode *pchild = pnodeaux->GetChildren();
            while (pchild && pchild->GetName().Cmp(wxT(URL_NODE)) != 0)
               pchild = pchild->GetNext();
            if (pchild && IsFileSystemPath(pchild->GetNodeContent().c_str()))  //nodo url
                  {
               wxFileName fnurl(pchild->GetNodeContent().c_str());
               wxString urlaux = fnurl.GetVolume().c_str()
                     + (fnurl.HasVolume() ? fnurl.GetVolumeSeparator() : wxT(""));
               urlaux += fnurl.GetFullPath(wxPATH_UNIX);
               wxXmlNode *pnodeurl = pchild->GetChildren();
               if (pnodeurl) pnodeurl->SetContent(urlaux);
            }

            //Busco nodo dependencias
            pchild = pchild->GetNext();
            while (pchild && pchild->GetName().Cmp(wxT(DEPENDENCY_NODE)) != 0)
               pchild = pchild->GetNext();
            if (pchild && IsFileSystemPath(pchild->GetNodeContent().c_str()))  //nodo dependencias
                  {
               wxXmlNode *pnodeurldep = pchild->GetChildren();
               while (pnodeurldep) {
                  wxFileName fnurl(pnodeurldep->GetNodeContent().c_str());
                  wxString urlaux = fnurl.GetVolume().c_str()
                        + (fnurl.HasVolume() ? fnurl.GetVolumeSeparator() : wxT(""));
                  urlaux += fnurl.GetFullPath(wxPATH_UNIX);
                  wxXmlNode *pnodeurl = pnodeurldep->GetChildren();
                  if (pnodeurl) pnodeurl->SetContent(urlaux);

                  pnodeurldep = pnodeurldep->GetNext();
               }
            }

            pnodeaux = pnodeaux->GetNext();
         }
#ifdef __CODIGO_ANTERIOR__
         if(pnodeaux)  //nodo elemento
         {
            //Obtengo el 1er hijo del nodo elemento
            wxXmlNode *pchild = pnodeaux->GetChildren();
            while(pchild && pchild->GetName().Cmp(wxT(URL_NODE))!=0)
            pchild = pchild->GetNext();
            if(pchild)//nodo url
            {
               wxFileName fnurl(pchild->GetNodeContent().c_str());
               wxString urlaux = fnurl.GetVolume().c_str() +
               (fnurl.HasVolume()?fnurl.GetVolumeSeparator():wxT(""));
               urlaux += fnurl.GetFullPath(wxPATH_UNIX);
               wxXmlNode *pnodeurl = pchild->GetChildren();
               if(pnodeurl)
               pnodeurl->SetContent(urlaux);
            }

            //Busco nodo dependencias
            pchild = pchild->GetNext();
            while(pchild && pchild->GetName().Cmp(wxT(DEPENDENCY_NODE))!=0)
            pchild = pchild->GetNext();
            if(pchild)//nodo dependencias
            {
               wxXmlNode *pnodeurldep = pchild->GetChildren();
               while(pnodeurldep)
               {
                  wxFileName fnurl(pnodeurldep->GetNodeContent().c_str());
                  wxString urlaux = fnurl.GetVolume().c_str() +
                  (fnurl.HasVolume()?fnurl.GetVolumeSeparator():wxT(""));
                  urlaux += fnurl.GetFullPath(wxPATH_UNIX);
                  wxXmlNode *pnodeurl = pnodeurldep->GetChildren();
                  if(pnodeurl)
                  pnodeurl->SetContent(urlaux);

                  pnodeurldep = pnodeurldep->GetNext();
               }
            }
         }
#endif //__CODIGO_ANTERIOR__
         //Si es un elemento de Terreno, tengo que modificar la textura
         if (pcontextnode->GetPropVal(wxT(NAME_PROPERTY), wxT("")).Cmp(
               wxT(TYPE_PROPERTY_VALUE_TERRAIN)) == 0)  //No se traduce, elemento XML
               {
            ModifyTextureNode(pcontextnode);
#ifdef __CODIGO_ANTERIOR__
            if(pnodeaux)  //nodo elemento
            {
               //Obtengo el 1er hijo del nodo elemento
               wxXmlNode *pchild = pnodeaux->GetChildren();
               while(pchild && pchild->GetName().Cmp(wxT(RENDERIZATION_NODE)))//No se traduce, elemento XML
               pchild = pchild->GetNext();
               if(pchild)
               {
                  //Busco nodo textura
                  pchild = pchild->GetChildren();
                  while(pchild && pchild->GetName().Cmp(wxT(TEXTURE_NODE)))//No se traduce, elemento XML
                  pchild = pchild->GetNext();

                  if(pchild)
                  {
                     //Busco nodo elemento
                     pchild = pchild->GetChildren();
                     while(pchild && pchild->GetName().Cmp(wxT(CONTEXT_ELEMENT_NODE)))//No se traduce, elemento XML
                     pchild = pchild->GetNext();

                     if(pchild)
                     {
                        //Busco nodo url
                        pchild = pchild->GetChildren();
                        while(pchild && pchild->GetName()Cmp(wxT(URL_NODE)))//No se traduce, elemento XML
                        pchild = pchild->GetNext();

                        if(pchild)//nodo url
                        {
                           wxFileName fnurl(pchild->GetNodeContent().c_str());
                           wxString urlaux = fnurl.GetVolume().c_str() +
                           (fnurl.HasVolume()?fnurl.GetVolumeSeparator():wxT(""));
                           urlaux += fnurl.GetFullPath(wxPATH_UNIX);
                           wxXmlNode *pnodeurl = pchild->GetChildren();
                           if(pnodeurl)
                           pnodeurl->SetContent(urlaux);
                        }
                     }
                  }
               }
            }
#endif //__CODIGO_ANTERIOR__
         }
      }

#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION == 8) && (wxRELEASE_NUMBER <8)
      pcontextnode->SetParent(pnodeinfo->GetParent());
      pcontextnode->SetNext(pnodeinfo->GetNext());
      pnodeinfo->SetNext(pcontextnode);
#else
      pNode->InsertChildAfter(pcontextnode, pnodeinfo);
      pnodeinfo = pcontextnode;
#endif
      if (contextit == pContextList_->end() && pnodeaux) delete pnodeaux;
   }
   return true;
}

/**
 * Objetivo: Elimina el contenido del nodo Nombre del nodo informacion.
 * @return	modifica ok? true: false;
 */
bool PatInfoPart::DeletePatName() {
   if (!pInfoNode_) return false;

   wxXmlNode *pchild = pInfoNode_->GetChildren();
   while (pchild && pchild->GetName().Cmp(NAME_NODE) != 0)
      pchild = pchild->GetNext();

   if (!pchild) return false;

   wxXmlNode *pchild2 = pchild->GetChildren();
   pchild2->SetContent(wxEmptyString);
   return true;
}

/**
 * Devuelve la version de la aplicacion con la profundidad indicada.
 */
std::string PatInfoPart::GetAppVersion(int Depth) const {
   std::string retstr;

   std::string appversion = suri::Configuration::GetParameter("app_version", wxT("0.0"));
   std::vector<std::string> numbers = SplitEx(appversion, ".");

   for (int ix = 0, ixlen = numbers.size(); ix < ixlen && ix < Depth; ++ix) {
      if (retstr.length() > 0)
         retstr.append(".");
      retstr.append(numbers[ix]);
   }

   return retstr;
}

/**
 * Objetivo: Crea y devuelve el nodo PAT
 * @return un puntero de tipo wxXmlNode cuyo contenido es el nodo PAT.
 * \attention quien invoque a este metodo es responsable del puntero devuelto,
 * por lo tanto debe hacerse cargo de su eliminacion.
 */
wxXmlNode *PatInfoPart::CreatePatNode() {
   suri::XmlElement *pxml = new suri::XmlElement(
         wxT(PAT_NODE), wxT(TYPE_PROPERTY_VALUE_PAT_ABSOLUTE));  //No se traduce, elemento de XML
   wxXmlNode *pnode = pxml->GetProperties();
   std::string appversion = GetAppVersion(2);

   pnode->AddProperty(wxT(APP_VERSION_PROPERTY), appversion);  //No se traduce, elemento de XML
   pnode->AddProperty(wxT(PAT_VERSION_PROPERTY), wxT(PAT_VERSION_PROPERTY_VALUE));  //No se traduce, elemento de XML
   pnode->AddProperty(wxT(OS_VERSION_PROPERTY),
                      wxPlatformInfo().GetOperatingSystemIdName());
   return pnode;
}

/**
 * Este metodo es utilizado como soporte para el metodo GetSubDirectories, para
 * no tener subdirectorio repetidos.
 * Este metodo verifica que la Cadena pasada por parametro no este en el Vector.
 * @param[in] 	Cad: cadena a buscar dentro del vector.
 * @param[in]	Vec: vector de cadenas.
 * @return (no se encontro)? true:false;
 */
bool PatInfoPart::CanInsert(const wxString Cad, const wxArrayString *Vec) {
   for (size_t i = 0; i < Vec->GetCount(); i++)
      if (Vec->Item(i).Cmp(Cad) == 0) return false;

   return true;
}

/**
 * Este metodo modifica el contenido del xml del PAT, cambiando los nodos url de
 * los elementos vector para que tengan el mismo path que el PAT.
 * Esto es asi ya que los vectores cuando se crean a travez de la herramienta de
 * dibujo, se generan en la carpeta temporal de la App y luego cuando el PAT
 * es salvado se los copia al directorio del mismo.
 * @param[in]	PathPat: Path del PAT("C:\xxx\mypat\")
 */
void PatInfoPart::ModifyUrlVector(const std::string& PathPat,
                                  const std::string &CommonPath) {
   /* Cargo el xml existente salvado, recorro los vectores y modifico los url*/
   wxXmlDocument doc;
   wxString xmlpath = PathPat + wxT("descripcion.xml");
   if (!doc.Load(xmlpath, suri::XmlElement::xmlEncoding_)) return;

   // obtengo el nodo raiz para despues reemplazarlo ya que modifico los nodos hijo.
   wxXmlNode *prootnode = doc.DetachRoot();

   // obtengo el nodo contexto y busco el contexto raster
   wxXmlNode *pcontextnode = prootnode->GetChildren();
   while (pcontextnode) {
      if (pcontextnode->GetPropVal(wxT(TYPE_PROPERTY), "") == wxT(TYPE_PROPERTY_VALUE_RASTER)
            || pcontextnode->GetPropVal(wxT(TYPE_PROPERTY), "")
               == wxT(TYPE_PROPERTY_VALUE_TERRAIN)) {
         // recorro los elemento vector
         wxXmlNode *pelementnode = pcontextnode->GetChildren();
         for (; pelementnode; pelementnode = pelementnode->GetNext()) {
            if ((pelementnode->GetPropVal(wxT(TYPE_PROPERTY), "").Cmp(
                  wxT(TYPE_PROPERTY_VALUE_VECTOR)) != 0) || !IsTempFile(pelementnode)) //No se traduce, elemento XML
            continue;

            // reemplazo el url del elemento y de las dependencias.
            wxXmlNode *pchildnode = pelementnode->GetChildren();
            while (pchildnode) {
               if (pchildnode->GetName().Cmp(wxT(URL_NODE)) == 0) //No se traduce, elemento XML
                     {
                  wxString newurl = ModifyUrlNode(pchildnode, CommonPath);
                  //Modifico el campo de la base de datos que apunta al hotlink.
                  ModifyHotLinkFieldValue(CommonPath, newurl);
               }

               if (pchildnode->GetName().Cmp(wxT(DEPENDENCY_NODE)) == 0) //No se traduce, elemento XML
                  ModifyDependency(pchildnode, CommonPath);

               pchildnode = pchildnode->GetNext();
            }
         }
      }
      pcontextnode = pcontextnode->GetNext();
   }

   wxString xmlstring;
   PatFile::SaveXml(xmlstring, prootnode);
   PatFile::SaveXml(xmlpath, xmlstring);
   delete prootnode;
}

/**
 * Metodo soporte para el metodo ModifyUrlVector, cambia el contenido del nodo
 * url de un elemento.
 * @param[in]	CommonPath: path del directorio "...MyPat/comun/"
 * @param[out]	pUrlNode: nodo url modificado.
 * @return string con el contenido del nodo url.
 */
wxString PatInfoPart::ModifyUrlNode(wxXmlNode *pUrlNode, const wxString &CommonPath) {
   if (!pUrlNode) return wxT("");  //no deberia pasar nunca.

   wxFileName fn(pUrlNode->GetNodeContent());
   wxString url = CommonPath;
   if (fn.GetDirCount() > 0) {
      if (fn.HasExt())
         url += fn.GetDirs().Item(fn.GetDirCount() - 2)
               + fn.GetPathSeparator(wxPATH_UNIX);

      url += fn.GetDirs().Item(fn.GetDirCount() - 1) + fn.GetPathSeparator(wxPATH_UNIX);
   }

   url += fn.GetFullName();
   pUrlNode->GetChildren()->SetContent(url);
   return url;
}

/**
 * Metodo soporte para el metodo ModifyUrlVector, cambia el contenido de los
 * nodos url del nodo dependencias de un elemento.
 * @param[in]	ElementPath: path del elemento dentro del PAT.
 * @param[out]	pNodeDep: nodo dependencias modificado.
 */
void PatInfoPart::ModifyDependency(wxXmlNode *pNodeDep,
      const wxString &CommonPath) {
   if (!pNodeDep) return;

   wxXmlNode *purlnode = pNodeDep->GetChildren();
   for (; purlnode; purlnode = purlnode->GetNext()) {
      if (!purlnode->GetNodeContent().StartsWith("memory:"))
         ModifyUrlNode(purlnode, CommonPath);
   }
}
/**
 * Metodo soporte para el metodo ModifyUrlVector, modifica el contenido de la
 * base de datos de un vector, indicando el nuevo path del HotLink. Al igual que
 * cuando se crean vectores con la herramienta de dibujo, cuando se genera un
 * HotLink a un vector, se genera en la carpeta temporal de la App y luego al
 * grabar el PAT se lo copia a la carpeta del mismo.
 * @param[in]	ElementPath: path del elemento. (C:\...\MyPat\comun\)
 * @param[in]	HlUrl: url del HotLinkVectorElement.
 */
void PatInfoPart::ModifyHotLinkFieldValue(const wxString &CommonPath,
                                          const wxString &HlUrl) const {
   HotLinkVectorElement *phlelement = HotLinkVectorElement::Create(HlUrl.c_str());
   if (!phlelement) return;

   wxFileName fnhotlink(phlelement->GetHotLinkFieldValue());
   std::string newhlpath = CommonPath.c_str();
   newhlpath += suri::Configuration::GetParameter("app_hotlinks_dir", wxT(""));
   if (fnhotlink.GetDirCount() > 0) {
      newhlpath += fnhotlink.GetDirs().Item(fnhotlink.GetDirCount() - 1);
      newhlpath += fnhotlink.GetPathSeparator(wxPATH_UNIX);
   }
   newhlpath += fnhotlink.GetFullName();

   phlelement->SetHotLinkFieldValue(newhlpath);
   delete phlelement;
}

/**
 * Modifica el contenido del nodo textura|elemento|url con las barras de unix.
 * @param[in]	pContextNode: puntero al nodo contexto.
 */
void PatInfoPart::ModifyTextureNode(const wxXmlNode *pContextNode) {
   if (!pContextNode)  //no deberia suceder.
      return;

   wxXmlNode *pelementnode = pContextNode->GetChildren();
   if (!pelementnode)  //nodo elemento
      return;

   //Busco el nodo Renderizacion.
   wxXmlNode *pchild = pelementnode->GetChildren();
   while (pchild && pchild->GetName().Cmp(wxT(RENDERIZATION_NODE)))
      pchild = pchild->GetNext();

   if (!pchild) return;

   //Busco nodo textura
   pchild = pchild->GetChildren();
   while (pchild && pchild->GetName().Cmp(wxT(TEXTURE_NODE)))
      pchild = pchild->GetNext();

   if (!pchild) return;

   //Busco nodo elemento
   pchild = pchild->GetChildren();
   while (pchild && pchild->GetName().Cmp(wxT(CONTEXT_ELEMENT_NODE)))
      pchild = pchild->GetNext();

   if (!pchild) return;

   //Busco nodo url
   pchild = pchild->GetChildren();
   while (pchild && pchild->GetName().Cmp(wxT(URL_NODE)))
      pchild = pchild->GetNext();

   if (!pchild)  //nodo url
      return;

   //modifico el contenido del nodo url con la barra de unix.
   wxFileName fnurl(pchild->GetNodeContent().c_str());
   wxString urlaux = fnurl.GetVolume().c_str()
         + (fnurl.HasVolume() ? fnurl.GetVolumeSeparator() : wxT(""));
   urlaux += fnurl.GetFullPath(wxPATH_UNIX);
   wxXmlNode *pnodeurl = pchild->GetChildren();
   if (pnodeurl) pnodeurl->SetContent(urlaux);
}

/**
 * Analiza si el Uri corresponde a ruta de sistema de archivos
 * @param[in] Uri string con uri a analizar
 * @return bool que indica si el url corresponde a ruta de sistema de archivos
 */
bool PatInfoPart::IsFileSystemPath(std::string NodeUri) {
   // Para analizar si es path de SO analiza el tamanio del scheme,
   // longitud 0(unix) o longitud 1(windows)

   suri::Uri uri(NodeUri);
   std::string schema = uri.GetScheme();
   return schema.size() == 0 || schema.size() == 1;
}

