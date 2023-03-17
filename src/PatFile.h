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

#ifndef PATFILE_H_
#define PATFILE_H_

//Includes standard
#include <string>
#include <vector>
#include <map>

//Includes Suri

//Includes Wx

//Includes App
#include "suri/ZipFile.h"
#include "suri/XmlElement.h"


//Defines

//Forwards
class wxXmlNode;

//------------------------------Funciones Soporte-------------------------------
//! Devuelve el tamanio de los archivos
wxULongLong GetFilesSize(std::vector<wxXmlNode *> VecElementNode);
//! Copia directorio From a To
bool CopyDir(const wxFileName &FnDirFrom, const wxFileName &FnDirTo);
//! Elimina del FS el directorio FnDir.
bool RemoveDir(const wxFileName &FnDir);
//! Devuelve un path unico para un elemento.
wxString GetNextPath(const wxString &BasePath, const wxString &DirPrefix = wxT("Elem_"),
                     bool CreateDir = true);
//! Verifica si el elemento es un HotLink.
bool IsHotLinkVectorElement(const wxXmlNode *pElementNode, const wxString &Path =
      wxT(""));
//! Verifica si el elemento es una Anotacion.
bool IsAnotationElement(const wxXmlNode *pElementNode, const wxString &Path = wxT(""));
//! Verifica si el elemento es un archivo creado en el temp de la app.
bool IsTempFile(const wxXmlNode *pElementNode);
//! Convierte en absoluto el url de un elemento.
bool MakeUrlAbsolute(wxXmlNode *pNode, std::string AbsolutePath);
//! Convierte en absoluto el url de cualquier elemento.
bool GenericMakeUrlAbsolute(wxXmlNode *pNode, std::string AbsolutePath);
//------------------------------------------------------------------------------

//! Clase soporte que permite crear e instalar un PAT.
/**
 * Representacion de un PAT. Posee los metodos de acceso y modificacion
 * para manipular PATs.
 * Funciones:
 * 	- Instalar un PAT Comprimido
 * 	- Verificar la consistencia del PAT.
 * 	- Obtener la descripcion de un PAT.
 *    - Crear un PAT comprimido.
 */
class PatFile {
   //! Ctor. de Copia
   PatFile(const PatFile &NewPatFile);
public:
   //! enumeracion para identificar el tipo de pat
   typedef enum {
      PatRelative, PatAbsolute, PatCompressed, PatInvalid
   } PatType;
   //! ctor
   PatFile(const std::string &FileName);
   //! dtor
   ~PatFile();
   //! Retorna el tipo de PAT del que se trata
   PatType GetPatType() const {
      return typePat_;
   }
   ;
   //! Instala el PAT en caso de tratarse de uno comprimido
   bool InstallPat(const std::string &InstallPath) const;
   //! Verifica la consistencia del PAT
   bool CheckConsistencyPat(const std::string &PathXml = "",
                            const suri::ZipFile::ZipListType &FileList =
                                  suri::ZipFile::ZipListType()) const;
   //! Verifica la consistencia del PAT
   bool CheckConsistencyPat(std::vector<wxXmlNode*> VecNodes,
                            const suri::ZipFile::ZipListType &FileList =
                                  suri::ZipFile::ZipListType()) const;
   //! Crea un archivo PAT comprimido
   bool CreatePatFile(const std::string &PatFileName) const;
   //! Retorna el arbol XML
   wxXmlNode* GetContexts(wxXmlDocument* docxml);
   //! Retorna el arbol XML
   std::map<std::string, std::vector<wxXmlNode*> > GetContextNodes(
         const std::string& PathXml = "") const;
   //! Devuelve el nombre del PAT.
   std::string GetPatName() const;
   //! Devuelve la Descripcion del PAT.
   bool GetPatDescription(std::map<std::string, std::string> &Params);
   //! Devuelve el nodo informacion
   wxXmlNode *GetInfoNode(const std::string &PathXml = "");
   //! Devuelve una copia del nodo raiz
   wxXmlNode *GetRootNode(const std::string &XmlPath = wxT(""));
   //! Devuelve el nodo dependencias de un elemento.
   wxXmlNode *GetDependencyNode(const wxXmlNode *pElementNode) const;
   //! Devuelve un mensaje de error.
   std::string GetErrorMsg() {
      return errorMsg_;
   }
   //! salva un string que representa un XML en un archivo
   static bool SaveXml(const wxString &Filename, const wxString &XmlString);
   //! salva un Nodo Xml a un string
   static bool SaveXml(wxString &XmlString, wxXmlNode *pRootNode);
   //! agrega la linea doctype a un string que representa un XML
   static bool AddDocType(wxString &XmlString);
   //! Indica si se puede instalar/comprimir un PAT verificando espacio en disco
   bool CheckPatSize(const std::string &DestinyPath,
                     const std::string &PathXml = "") const;
   //! Devuelve el tipo de PAT con el que se va a trabajar
   PatType GetType(const std::string &PathXml = "") const;
protected:
   //! Genera un xml temporal cambiando la propiedad tipo y haciendo los path relativos
   bool CreatePatTemp(const std::string &PathTempXml,
                      suri::ZipFile::ZipListType &FileList,
                      suri::ZipFile::ZipListType &DirList) const;
   //! Verifica si PathFile se encuentra en la lista FileList.
   bool FindFileList(const std::string &PathFile,
                     const suri::ZipFile::ZipListType &FileList) const;
   //! Obtiene la propiedad nombre de un nodo contexto
   std::string GetContextName(const wxXmlNode *pContextNode) const;
   //! Devuelve el path relativo, a la ubicacion del PAT, de un elemento
   std::string GetPathElement(const wxXmlNode *pElementNode, const int NroElem,
                              const std::string &ContextName, const bool IsDirectory =
                                    false) const;
   //! Verifica los nodos dependencia
   bool CheckDependency(const std::vector<wxXmlNode*> VecDependency,
                        const suri::ZipFile::ZipListType &List =
                              suri::ZipFile::ZipListType()) const;
   //! Devuelve un vector con los nodos dependencia de un elemento
   std::vector<wxXmlNode*> GetDependency(
         const std::vector<wxXmlNode*> VecElement) const;
   //! Modifca los path de las dependencias
   void ModifyDependency(wxXmlNode *pNode, const std::string &Path,
                         const std::string &PathUrl,
                         suri::ZipFile::ZipListType &FileList,
                         suri::ZipFile::ZipListType &DirList) const;
   //! parsea el contenido de un nodo en un string
   std::string ParseNode(const wxXmlNode *pNode, const std::string &Separator = "");
   //! Agrega la imagen del preview a la lista de archivos.
   void AddPreviewImg(wxXmlNode *pNodeInfo, suri::ZipFile::ZipListType &FileList,
                      suri::ZipFile::ZipListType &DirList) const;
   //! Devuelve los path de descripcion.xml y de la img previa del PAT.
   void GetPaths(const std::string &PathPat, std::string &PathXml,
                 std::string &PathImg);
   //! Modifica el contenido del nodo url del nodo textura en un elemento de terreno.
   void ModifyTextureNode(wxXmlNode *pNode, const std::string &Path,
                          const std::string &PathUrl) const;
   //! Obtiene los url de los hotlinks para poder incluirlos en el .pat.
   void AddHotlinks(const wxXmlNode *pNode, suri::ZipFile::ZipListType &FileList,
                    suri::ZipFile::ZipListType &DirList) const;
   //! modifica el campo SURHotlink en la base de datos de los vectores.
   bool ModifyHotLinks(const wxString &XmlUrl) const;
   //! Setea un mensaje de error de alguna operacion.
   void SetErrorMsg(const std::string &Msg);
private:
   //!
   bool VerifyDateNode(const wxXmlNode *pDateNode);
   //! Formatea la cadena pasada por parametro, en n lineas de tamanio fijo.
   std::string FormatDescription(const std::string &Description, const int &Width);
   //! Devuelve todos los nodos elemento de un PAT.
   void GetElementsNode(std::vector<wxXmlNode*> &VecElementsNode,
                        const std::string &PathXml = "",
                        const bool AbsolutePath = false) const;
   //! Obtiene el nombre del PAT y lo asigna a la propiedad fileName_
   void SetPatName(const wxString &PathXml);
   std::string filePath_;  ///< path del archivo .pat o del archivo descripcion.xml.
   std::string fileName_;  ///< nombre del PAT, es el contenido del nodo informacion/nombre
   PatType typePat_;  ///< Tipo del PAT
   std::string errorMsg_;  ///< Mensaje de error.
};

#endif  // PATFILE_H_
