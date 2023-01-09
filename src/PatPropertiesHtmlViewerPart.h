//<COPYRIGHT> a reemplazar a futuro

#ifndef PATPROPERTIES_H_
#define PATPROPERTIES_H_

//Includes standard

//Includes Suri
#include "suri/HtmlViewerPart.h"

//Includes Wx

//Includes App

//Defines

//Forwards

//! Esta clase permite visualizar la informacion del PAT.
/**
 * Permite visualizar el nodo informacion del PAT en un HTML.
 */
class PatPropertiesHtmlViewerPart : public suri::HtmlViewerPart {
   //! Ctor. de Copia.
   PatPropertiesHtmlViewerPart(
         const PatPropertiesHtmlViewerPart &PatPropertiesHtmlViewerPart);

public:
   //! ctor
   PatPropertiesHtmlViewerPart(const std::string &PatPath);
   //! dtor
   virtual ~PatPropertiesHtmlViewerPart();
   //! Se crea la ventana de herramienta en las clases derivadas
   virtual bool CreateToolWindow();
   //! Inicializa los controles
   virtual void SetInitialValues();
   //! Devuelve el estado del Part.
   virtual bool IsEnabled() {
      return true;
   }
   //! Indica si el Part tiene cambios para salvar.
   virtual bool HasChanged() {
      return modified_;
   }
   //! Salva los cambios realizados en el Part.
   virtual bool CommitChanges() {
      return true;
   }
   //! Restaura los valores originales del Part.
   virtual bool RollbackChanges() {
      return true;
   }
   //! Setea el nuevo contenido de la ventana.
   void SetHtml(const std::string &HtmlText = "");
protected:
   std::string patPath_;  ///< path del PAT(del archivo descripcion.xml)
private:
   //! Verifica que existan los templates html
   bool CheckDataDirectory(const wxString &Path = wxT(""));
   //! obtiene el template html
   bool GetTemplateHtml(const wxString &PathHtml, wxString &Html);
};

#endif /*PATPROPERTIES_H_*/
