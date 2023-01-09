/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2013-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#include "XmlContextHandler.h"
#include "ContextHandlerFactory.h"
#include "suri/DatasourceInterface.h"
#include "suri/DatasourceManagerInterface.h"
#include "suri/WorkGroupInterface.h"
#include "suri/MutableWorkGroupInterface.h"
#include "suri/SuriObject.h"
#include "suri/XmlFunctions.h"
#include "suri/NodePath.h"
#include "DefaultTreeNode.h"
#include "suri/ViewcontextInterface.h"
#include "suri/SuriObject.h"
#include "suri/LayerList.h"
#include "GroupsNodeFilter.h"
#include "DefaultTreeNode.h"
#include "suri/LayerList.h"
#include "Context.h"
#include "MultimediaElement.h"
#include "HtmlElement.h"
#include "ContextNodeFilter.h"
#include "resources.h"

// Includes Estandar
// Includes Suri
// Includes Wx
// Defines
// forwards

const char* ContextTypeNames[] =
      { "Generico", "Imagenes", "Terrenos 3D", "Fotografias", "Html", "Mapas",
        "Documentos", "Actividades", "Error" };

namespace suri {

REGISTER_CONTEXTHANDLERFACTORY(XmlContextHandler)
/** Constructor */
XmlContextHandler::XmlContextHandler() {
}
/** Destructor */
XmlContextHandler::~XmlContextHandler() {
   // TODO Auto-generated constructor stub
}

/** Create del factory */
ContextHandlerInterface* XmlContextHandler::Create(suri::ParameterCollection& params) {
   return new XmlContextHandler();
}

/** Carga desde el archivo XML del proyecto todos los elementos
 *  y grupos en la aplicacion. */
bool XmlContextHandler::Load(wxXmlNode* pcontextnode, DataViewManager* pdataviewmanager,
                             const std::string& RelativePath) {
   wxXmlNode* pnode = NULL;
   MutableWorkGroupInterface* wgroot =
         dynamic_cast<MutableWorkGroupInterface *>(pdataviewmanager->GetGroupManager()->GetRootWorkGroup());
   // Si estoy parado en context, pido los hijos
   wxString contextname = "";
   pcontextnode->GetPropVal(wxT(NAME_NODE), &contextname);

   if (pcontextnode->GetName().CompareTo(CONTEXT_NODE) == 0) {
      pnode = pcontextnode->GetChildren();
   }
   if (!LoadItems(wgroot->GetRootPath(), pnode, wgroot, pdataviewmanager, contextname,
                  RelativePath)) return false;

   return true;
}

/** Levanta los elementos y sus grupos desde el archivo del proyecto. */
wxXmlNode* XmlContextHandler::Save(wxXmlNode* pcontextsnodes,
                                   DataViewManager* pdataviewmanager, wxString tipo,
                                   wxString nombre) {

   wxXmlNode* pelementsnodes = pcontextsnodes->GetChildren();
   if (!pelementsnodes) return NULL;
   wxXmlNode* pmainxml = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT(CONTEXT_NODE));
   wxXmlProperty* pmailprop = new wxXmlProperty(wxT(TYPE_NODE), tipo);
   if (nombre.Len() > 0) {
      wxXmlProperty* pmailsecondprop = new wxXmlProperty(wxT(NAME_NODE), nombre);
      pmailprop->SetNext(pmailsecondprop);
   }
   pmainxml->SetProperties(pmailprop);

   // Determino el contexto que va a contener ctype.
   ViewcontextInterface::ViewcontextType ctype =
         (ViewcontextInterface::ViewcontextType) 0;
   for (int i = 0; i <= 8; i++)
      if (nombre.CompareTo(ContextTypeNames[i]) == 0) {
         ctype = (ViewcontextInterface::ViewcontextType) i;
         break;
      }
   if (nombre == "Multimedia") ctype = (ViewcontextInterface::ViewcontextType) 3;
   // Devuelve numero total de grupos/elementos en un contexto.
   int totalsize = ContextElementsCount(pdataviewmanager, ctype);

   return SaveItems(pelementsnodes, pmainxml, NULL, ctype, pdataviewmanager, totalsize);
}
/** Acomoda los rasters dentro de la jerarquia de grupos. */
wxXmlNode* XmlContextHandler::SaveItems(wxXmlNode*& pelementsnode,
                                        wxXmlNode* psavednode,
                                        TreeIterator* treelements,
                                        ViewcontextInterface::ViewcontextType ctype,
                                        DataViewManager* pdataviewmanager,
                                        int rescursivecall) {
   MutableWorkGroupInterface* pwgroot =
         dynamic_cast<MutableWorkGroupInterface *>(pdataviewmanager->GetGroupManager()->GetRootWorkGroup());

   TreeIterator tree = GetTreeElementsIterator(pdataviewmanager, ctype);

   // Si es una llamada recursiva, me muevo al siguiente nodo.
   if (treelements == NULL)
      treelements = &tree;
   else
      treelements->NextNode();

   for (; treelements->IsValid() && rescursivecall > 0;) {
      NodePath nodepath = treelements->GetPath();
      TreeNodeInterface* treea = nodepath.GetLastPathNode();
      // Grupo
      if (nodepath.AllowsChildren()) {
         wxXmlNode* groupnode = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,
         GROUP_NODE);
         wxString groupname = treea->GetName();

         int deep = treea->GetSize();
         if (deep > 0) {
            // Armo nodo hijos
            wxXmlNode* pchildren = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,
            CHILDREN_NODE);
            groupnode->AddChild(pchildren);
            // Agrego nombre del grupo en nodo
            wxXmlNode* pxmlname = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,
            NAME_NODE);
            wxXmlNode* pxmltext = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString,
                                                groupname);
            pxmlname->AddChild(pxmltext);
            groupnode->InsertChild(pxmlname, pchildren);

            //treelements->NextNode();

            SaveItems(pelementsnode, pchildren, treelements, ctype, pdataviewmanager,
                      treea->GetSize());
         } else {
            // No hay hijos, agrego el nombre y ya.
            wxXmlNode* pxmlname = new wxXmlNode(NULL, wxXML_ELEMENT_NODE,
            NAME_NODE);
            wxXmlNode* pxmltext = new wxXmlNode(NULL, wxXML_TEXT_NODE, wxEmptyString,
                                                groupname);

            pxmlname->AddChild(pxmltext);
            groupnode->AddChild(pxmlname);
         }
         // Inserta grupo
         wxXmlNode* pchild = psavednode->GetChildren();
         if (pchild != NULL) {
            while (pchild->GetNext() != NULL)
               pchild = pchild->GetNext();
            pchild->SetNext(groupnode);
         } else {
            wxXmlNode* tmpnode = new wxXmlNode(*groupnode);
            psavednode->AddChild(tmpnode);
         }
         //treelements->NextNode();
      } else { // Inserta raster
         wxString id = treea->GetId();
         nodepath = pwgroot->SearchNode(id.c_str());
         // Me muevo al siguiente en el xml.
         if (nodepath.IsValid()) {
            wxXmlNode* pchild = psavednode->GetChildren();
            if (pchild != NULL) {
               while (pchild->GetNext() != NULL)
                  pchild = pchild->GetNext();

               pchild->SetNext(new wxXmlNode(*pelementsnode));
            } else {
               wxXmlNode* tmpnode = new wxXmlNode(*pelementsnode);
               psavednode->AddChild(tmpnode);
            }

            if (pelementsnode->GetNext() != NULL) pelementsnode =
                  pelementsnode->GetNext();

            //treelements->NextNode();
         } else
            return NULL;
      }
      treelements->NextNode();
      rescursivecall--;
   }
   return psavednode;
}

/** Carga los items en el arbol y los acomoda. */
bool XmlContextHandler::LoadItems(NodePath parent, wxXmlNode* pnode,
                                  MutableWorkGroupInterface* pwgroot,
                                  suri::DataViewManager* pDataviewmanager,
                                  wxString contextname,
                                  const std::string& RelativePath) {
   int index = 0;
   wxString elementaddedId = "";
   while (pnode != NULL) {
      // Busco el nombre del grupo o elemento
      wxXmlNode* pextranode = pnode->GetChildren();
      while (pextranode->GetName().CompareTo(NAME_NODE) != 0 && pextranode != NULL)
         pextranode = pextranode->GetNext();
      wxString nodename = pextranode->GetNodeContent();

      // Elemento
      if (pnode->GetName().CompareTo(CONTEXT_ELEMENT_NODE) == 0) {
         elementaddedId = AddElement(pnode, pDataviewmanager, contextname,
                                     RelativePath);
         // Obtengo el id del elemento recien agregado y lo busco para reubicarlo en el arbol.
         NodePath nodepath = pwgroot->SearchContent(elementaddedId.c_str());
         // Lo reubico.

         if (!nodepath.IsValid()) return false;

         if (!parent.IsValid()) return false;

         pwgroot->MoveNode(nodepath, parent, index);
         index++;
         // Grupo
      } else if (pnode->GetName().CompareTo(GROUP_NODE) == 0) {
         suri::NodePath rootpath = pwgroot->GetRootPath();

         ViewcontextInterface::ViewcontextType ct = ViewcontextInterface::V2D;
         if (contextname.CompareTo("Imagenes") == 0)
            ct = ViewcontextInterface::V2D;
         else if (contextname.CompareTo("Terrenos 3D") == 0)
            ct = ViewcontextInterface::V3D;
         else if (contextname.CompareTo("Fotografias") == 0)
            ct = ViewcontextInterface::Multimedia;
         else if (contextname.CompareTo("Html") == 0)
            ct = ViewcontextInterface::Html;
         else if (contextname.CompareTo("Mapas") == 0)
            ct = ViewcontextInterface::Map;
         else if (contextname.CompareTo("Documentos") == 0)
            ct = ViewcontextInterface::Documents;
         else if (contextname.CompareTo("Actividades") == 0) ct =
               ViewcontextInterface::Exercises;

         suri::NodePath newgroup =
               pDataviewmanager->GetGroupManager()->InsertContextGroup(
                     pDataviewmanager->GetViewcontextManager()->GetViewcontextByType(ct)->GetViewcontextId(),
                     nodename.c_str(), rootpath, pwgroot->GetRootSize());

         index++;
         // Busco hijos
         pextranode = pnode->GetChildren();
         while (pextranode != NULL
               && pextranode->GetName().CompareTo(CHILDREN_NODE) != 0)
            pextranode = pextranode->GetNext();

         if (pextranode != NULL
               && pextranode->GetName().CompareTo(CHILDREN_NODE) == 0) {
            LoadItems(newgroup, pextranode->GetChildren(), pwgroot, pDataviewmanager,
                      contextname);
         }

      }
      pnode = pnode->GetNext();
   }
   return true;
}

/** Agrega un elemento al datasource */
std::string XmlContextHandler::AddElement(wxXmlNode* pNodeElement,
                                          suri::DataViewManager* pDataviewmanager,
                                          wxString contextname,
                                          const std::string& RelativePath) {
   if (!pNodeElement) {
      return NULL;
   }

   /*** si @params RelativePath trae datos, pisamos las rutas
    * de las dependencias */
   if (RelativePath.length() > 0) {
      wxXmlNode* pmainurl = pNodeElement->GetChildren();

      // Actualizamos url hijo de elemento.
      while (pmainurl->GetName().CompareTo(URL_NODE) != 0)
         pmainurl = pmainurl->GetNext();

      std::string lastpartofpath = pmainurl->GetNodeContent().c_str();
      std::string fullpath = RelativePath + lastpartofpath;
      pmainurl->GetChildren()->SetContent(fullpath);

      wxXmlNode* pdependencies = pNodeElement->GetChildren();
      // Actualizamos urls de dependencias
      while (pdependencies != NULL
            && pdependencies->GetName().CompareTo(DEPENDENCY_NODE) != 0)
         pdependencies = pdependencies->GetNext();

      if (pdependencies != NULL) {
         wxXmlNode* purl = pdependencies->GetChildren();

         lastpartofpath = "";
         fullpath = "";
         while (purl != NULL) {
            lastpartofpath = purl->GetNodeContent().c_str();
            fullpath = RelativePath + lastpartofpath;
            purl->GetChildren()->SetContent(fullpath);
            purl = purl->GetNext();
         }
      }
   }

   suri::DatasourceInterface* pdatasource = NULL;
   ContextType ct = Imagenes;
   wxXmlNode* purlnode = pNodeElement->GetChildren();

   /* Busco el nodo URL */
   while (purlnode->GetName().CompareTo(URL_NODE) != 0)
      purlnode = purlnode->GetNext();

   // actualizar el url si es relativo
   std::string strUrl = purlnode->GetNodeContent().c_str();

   if (contextname.CompareTo("Imagenes") == 0)
      ct = Imagenes;
   else if (contextname.CompareTo("Terrenos 3D") == 0)
      ct = Terrenos3D;
   else if (contextname.CompareTo("Fotografias") == 0)
      ct = Fotografias;
   else if (contextname.CompareTo("Html") == 0)
      ct = Html;
   else if (contextname.CompareTo("Mapas") == 0)
      ct = Mapas;
   else if (contextname.CompareTo("Documentos") == 0)
      ct = Documentos;
   else if (contextname.CompareTo("Actividades") == 0)
      ct = Actividades;
   else if (contextname.CompareTo("Multimedia") == 0) ct = Fotografias;
   if (ct == Documentos || ct == Fotografias || ct == Actividades) {
      wxFileName filename(strUrl);
      suri::Element *pelement = NULL;
      if (contextname.CompareTo("Documentos") == 0
            || contextname.CompareTo("Actividades") == 0)
         pelement = suri::HtmlElement::Create(strUrl);
      else if ((contextname.CompareTo("Fotografias") == 0)
            || (contextname.CompareTo("Multimedia") == 0)) {
         pelement = suri::MultimediaElement::Create(strUrl);
      }
      if (!pelement) {
         SHOW_ERROR("D:No se pudo crear el documento HTML");
         return NULL;
      }
      pelement->SetName(filename.GetName());

      if (pNodeElement) {
         //obtengo el elemento de la lista y le reemplazo el nodo raiz.
         pelement->AddNode(NULL, new wxXmlNode(*pNodeElement), true);
      }

      const char *pdstypes[] = { "MultimediaDatasource", "HtmlDatasource" };
      for (int i = 0; i < 2 && pdatasource == NULL; ++i) {
         pdatasource = suri::DatasourceInterface::Create(pdstypes[i], pelement);
      }
   } else {
      const char *pdatasourcetypes[] = { "RasterDatasource", "VectorDatasource" };
      for (int i = 0; i < 2 && pdatasource == NULL; ++i)
         pdatasource = suri::DatasourceInterface::Create(pdatasourcetypes[i],
                                                         pNodeElement);
      if (pdatasource && contextname.CompareTo("Mapas") == 0) {
         pdatasource->SetIcon(icon_MAP_ELEMENT);
      }
   }

   if (pdatasource == NULL) return NULL;

   ViewcontextInterface* pviewcontext =
         pDataviewmanager->GetViewcontextManager()->GetViewcontextByType(
               (suri::ViewcontextInterface::ViewcontextType) ct);
   DatasourceManagerInterface *dm = pDataviewmanager->GetDatasourceManager();

   if (!dm->AddDatasourceWithNotification(
         pdatasource, pviewcontext->GetViewcontextId(),
         static_cast<suri::ViewcontextInterface::ViewcontextType>(ct))) return NULL;

   return pdatasource->GetId();
}

/** Devuelve un iterador del arbol */
TreeIterator XmlContextHandler::GetTreeElementsIterator(
      DataViewManager* pdataviewmanager, ViewcontextInterface::ViewcontextType ctype) {
   WorkGroupInterface* pworkgroup =
         pdataviewmanager->GetGroupManager()->GetRootWorkGroup();

   /*** testing ***/
#ifdef __DEBUG__
   TreeIterator tit = pworkgroup->GetIterator(new ContextNodeFilter(pdataviewmanager, ctype));

   while (tit.IsValid()) {
//      NodePath npath = tit.GetPath();
//
//      int count = npath.GetPathCount();
//      count = count;
//
//      TreeNodeInterface* tn = npath.GetLastPathNode();
//      std::string name = tn->GetName();
//      name = name;

      TreeNodeInterface* pcurrent = tit.GetCurrent();
      if (pcurrent != NULL) {
         TreeNodeInterface* pparent = pcurrent->GetParent();
         pparent = pparent;

         std::string name = pcurrent->GetName();
         name = name;

         SuriObject::UuidType content = pcurrent->GetContent();
         content = content;

         bool allowschildren = pcurrent->AllowsChildren();
         allowschildren = allowschildren;
      }

//      if ( npath.AllowsChildren() ) {
//         TreeNodeInterface* tn = npath.GetLastPathNode();
//         std::string name = tn->GetName();
//         name = name ;
//      }
      tit.NextNode();
   }
#endif

   /****************/

   return pworkgroup->GetIterator(new ContextNodeFilter(pdataviewmanager, ctype));
}

/** Devuelve la cantidad de grupos que hay en un determinado contexto. */
int XmlContextHandler::ContextElementsCount(
      DataViewManager* pdataviewmanager, ViewcontextInterface::ViewcontextType ctype) {
   int num = 0;
   WorkGroupInterface* pwg = pdataviewmanager->GetGroupManager()->GetRootWorkGroup();

   TreeIterator it = pwg->GetIterator();

   while (it.IsValid()) {
      NodePath nodepath = it.GetPath();
      TreeNodeInterface* ptn = nodepath.GetLastPathNode();
      if (ptn->GetContextType() == ctype) num++;
      it.NextNode();
   }
   return num;
}
} /** namespace suri */
