/*! \file MainToolbar.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2012-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#ifndef MAINTOOLBAR_H_
#define MAINTOOLBAR_H_

//Includes standard
#include <string>

//Includes Suri
#include "suri/Widget.h"
#include "Event.h"

//Defines
#ifdef __DEBUG__
#	define __ENABLE_CONFIG_BUTTON__
#endif	// __DEBUG__
// forwards
class wxCommandEvent;
class wxUpdateUIEvent;
class Context;

namespace suri {
class LayerList;
class DataViewManager;
}

class DeletedElement_Callback;

DECLARE_EVENT_OBJECT(MainToolbarEventHandler);

//! Clase/Herramienta con la barra de herramientas generica de los contextos
/**
 *  Esta herramienta representa una barra de herramientas con los controles
 * genericos del contexto.
 */
class MainToolbar : public suri::Widget {
   //! Ctor. de Copia.
   MainToolbar(const MainToolbar &MainToolbar);

public:
   //! Clase "amiga" para los eventos
   friend class MainToolbarEventHandler;
   //! Constructor
   MainToolbar(wxWindow *pParent, Context *pContext, suri::DataViewManager* pDataViewManager);
   //! Destructor
   virtual ~MainToolbar();
   //! Crea la ventana de herramienta
   virtual bool CreateToolWindow();

   void DeleteElement();

protected:
   /**
    * Muestra pantalla para que el usuario ingrese un nombre de grupo.
    * @param[out] GroupName nombre del grupo ingresado por el usuario
    * @return bool que que indica si tuvo exito.
    */
   bool GetGroupName(std::string &GroupName) const;
   //! Evento de los botones: Gestionar Pat
   void OnManagePatButtonClick(wxCommandEvent &Event);
   //! Evento de los botones: Agregar Elemento
   void OnAddElementButtonClick(wxCommandEvent &Event);
   /**
    * Evento que se dispara al presionar el boton Agregar Elemento URL
    * Despliega un control de tipo texto en donde se ingresa la url que se desea agregar
    * Una vez finalizada la operacion agrega el elemento url al pat
    */
   void OnAddUrlElementButton();
   /** Agrega un elemento wms a la lista de elementos */
   void OnAddWmsElementButton();
   /** Agrupa los elementos seleccionados */
   void OnGroupElementsButton();
   //! Evento de los botones: Nuevo Pat
   void OnNewPatButtonClick(wxCommandEvent &Event);
   //! Evento de los botones: Eliminar Elemento
   void OnDeleteElementButtonClick(wxCommandEvent &Event);
   //! Evento de los botones: Guardar Pat
   void OnSavePatButtonClick(wxCommandEvent &Event);
//#ifdef __ENABLE_CONFIG_BUTTON__
   //! Evento de los botones: Configurar
   void OnConfigureButtonClick(wxCommandEvent &Event);
//#endif	// __ENABLE_CONFIG_BUTTON__
   //! Evento UIUpdate
   void OnUIUpdate(wxUpdateUIEvent &Event);
private:
   MainToolbarEventHandler *pEventHandler_;	///< Maneja eventos
   Context *pContext_;	///< El contexto asociado
   suri::DataViewManager* pDataViewManager_;

   DeletedElement_Callback* pDeletedCb_;
};

/** */
class DeletedElement_Callback : public suri::Callback {
public:
   /**
    * Ctor.
    */
   DeletedElement_Callback(MainToolbar* pToolbar = NULL);

   /** Metodo a reimplementar en cada evento */
   virtual void Handle(suri::BaseParameter* pParam = NULL);

private:
   MainToolbar* pToolbar_;
};

#endif /*PATTOOLBAR_H_*/
