//<COPYRIGHT> a reemplazar a futuro

#ifndef CONTEXT3D_H_
#define CONTEXT3D_H_

//Includes standard

//Includes Suri

//Includes Wx
#include "wx/xml/xml.h"

//Includes App
#include "Context.h"

//Defines

//! Clase concreta del contexto "3D"
/**
 * Esta clase es utlizada para crear el Terrenos3D.
 * Permite abrir :
 *  - Imagen de 1 banda + imagen de 1 o 3 bandas como terreno y textura
 *    respectivamente.
 *  - Vectores de punto con Hotlink.
 */
class Context3D : public Context {
   //! Ctor. de Copia.
   Context3D(const Context3D &Context3D);
public:
   Context3D(wxWindow *pParent = NULL, suri::DataViewManager* pDataViewManager = NULL);
   virtual ~Context3D();
   //! Crea la ventana
   virtual bool CreateToolWindow();

protected:
	//! Clase derivada donde se agrega el elemento del contexto
	virtual suri::Element *AddElement(const wxString& Filename,
			const bool &ManuallyAdd, const wxXmlNode* pElementNode = NULL);
private:
};

#endif /*CONTEXT3DTOOL_H_*/
