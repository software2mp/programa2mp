//<COPYRIGHT> a reemplazar a futuro

#ifndef CONTEXT2D_H_
#define CONTEXT2D_H_

//Includes standard

//Includes Suri
#include "DefaultViewcontext.h"

//Includes Wx
#include "wx/xml/xml.h"

//Includes App
#include "Context.h"

//Defines

class wxMouseEvent;
namespace suri {
class VectorEditor;
class DataViewManager;
class DefaultViewcontext;
}

DECLARE_EVENT_OBJECT(Context2DEvent);

//! Clase concreta del contexto "2D"
/**
 * Esta clase es utlizada para crear los contextos Imagenes y Mapas.
 * Ambos contextos permiten cargar :
 * - Imagenes (Raster).
 * - Vectores (Vector, Anotacion, Hotlink).
 * Tiene posibilidad de agregar las siguientes herramientas :
 * - Ajustes: modificacion de brillo y contraste.
 * - Animacion: animacion de imagenes.
 * - Dibujo: Creacion de vectores de anotacion, punto, linea y poligono.
 * - Medicion: De areas y distancias con cambio de unidades.
 * - Posicion de pixel: Indica la posicion del cursor en coordenadas de mundo.
 * - Mapa: Overlay de leyenda, grilla, escala y norte (rosa de los vientos)
 * Asigna la logica de activacion SingleRasterElementActivationLogic, para
 * permitir la activacion de 1 solo raster y vectores solo en caso de que
 * haya raster activo.
 * Esta clase define posiciones para los distintos tipos de elementos que
 * admite y en base a dicha posicion se ordena la lista de los mismos.
 */
class Context2D : public Context {
   //! Ctor. de Copia.
   Context2D(const Context2D &Context2D);

public:

   typedef enum {
      DefaultPosition = 0, AnotationPosition = 2, VectorPosition = 4, RasterPosition = 6
   } LayerPositionType;
   friend class Context2DEvent;
   Context2D(wxWindow *pParent = NULL, suri::DataViewManager* pDataViewManager = NULL, ContextType Type = Context2DType);
   virtual ~Context2D();
   //! Crea la ventana
   virtual bool CreateToolWindow();
   //! Retorna el icono de la herramienta
   virtual void GetWindowIcon(wxBitmap& ToolBitmap, int X = 0, int Y = 0) const;
protected:
   //! Clase derivada donde se agrega el elemento del contexto
	virtual suri::Element *AddElement(const wxString& Filename,
			const bool &ManuallyAdd, const wxXmlNode* pElementNode = NULL);
	//! Actualiza el nodo estilo
	void UpdateXMLStyleNode(suri::Element *pelement);
	//! Actualiza el nodo cache
	void UpdateXMLCacheNode(suri::Element *pelement);
private:
   Context2DEvent *pEventHandler_;	///< Maneja eventos de la herramienta
   suri::VectorEditor* pVectorEditor_;  ///< VectorEditor que se utiliza para agregar puntos a vectores que se editan en el contexto
   suri::DataViewManager* pDataViewManager_; ///< Visualizador de contexto.
   std::string path; ///< Path de nodos XML
};

#endif /*CONTEXT2DTOOL_H_*/

