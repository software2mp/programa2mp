/*! \file SeymurConfigurationPart.cpp */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2014-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#include "SeymurConfigurationPart.h"
#include "SetupGeneralPart.h"
#include "SetupVectorialEditionPart.h"
#include "SetupMeasurementPart.h"
#include "Setup3DPart.h"
#include "suri/MapToolSetupPart.h"

/** Constructor */
SeymurConfigurationPart::SeymurConfigurationPart(suri::DataViewManager* pDataViewManager) :
   pDataViewManager_(pDataViewManager) {
}

/** Destructor */
SeymurConfigurationPart::~SeymurConfigurationPart() {
}

/* Devuelve los parts necesarios para la configuracion de Seymur */
std::vector<suri::Part *> SeymurConfigurationPart::GetConfigParts() {
   std::vector<suri::Part*> parts;
   parts.push_back(new suri::SetupGeneralPart(true));
   parts.push_back(new suri::SetupVectorialEditionPart(pDataViewManager_));
   parts.push_back(new suri::SetupMeasurementPart(pDataViewManager_));
   parts.push_back(new suri::MapToolSetupPart(pDataViewManager_));
   parts.push_back(new suri::Setup3DPart);
   return parts;
}
