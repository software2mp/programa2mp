//<COPYRIGHT> a reemplazar a futuro

#ifndef SINGLERASTERELEMENTACTIVATIONLOGIC_H_
#define SINGLERASTERELEMENTACTIVATIONLOGIC_H_

//Includes standard

//Includes Suri
#include "suri/ActivationLogic.h"

//Includes Wx

//Includes App
#include "Context.h"

//Defines

//! Clase de seleccion de elementos del contexto.
/**
 * Esta clase proporciona la logica para activar/desactivar elementos del
 * contexto Raster.
 * Verifica que no se activen vectores si no hay un raster activo y tambien
 * verifica que no se activen 2 imagenes al mismo tiempo.
 */
class SingleRasterElementActivationLogic : public suri::ActivationLogic {
   //! Ctor. de Copia.
   SingleRasterElementActivationLogic(
         const SingleRasterElementActivationLogic &SingleRasterElementActivationLogic);

public:
   //! Ctor. default
   SingleRasterElementActivationLogic(suri::ViewerWidget *pViewer,
                                      const std::string &RequiredElement) :
         pViewer_(pViewer), requiredElement_(RequiredElement) {
   }
   //! Dtor.
   virtual ~SingleRasterElementActivationLogic() {
   }
   //! activado/desactiva elementos del contexto
   virtual void SetActivation(suri::Element *pElement,
                              suri::LayerList::ElementListType &ElementList,
                              suri::LayerList::ElementSelectionType &ElementData,
                              bool Status);
protected:
private:
   suri::LayerList::ElementListType elementList_;	///< Lista de elementos activos al momento de aplicar la logica
   suri::ViewerWidget *pViewer_;  ///< View asociado
   std::string requiredElement_;  ///< Texto para el mensaje de elemento requerido
};

#endif /*SINGLERASTERELEMENTACTIVATIONLOGIC_H_*/
