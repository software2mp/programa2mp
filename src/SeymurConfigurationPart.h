/*! \file SeymurConfigurationPart.h */
/**
 *  Biblioteca de procesamiento GeoEspacial SuriLib.
 *  Copyright 2014-2007 CONAE - SUR Emprendimientos Tecnologicos S.R.L.
 *
 *  Este producto contiene software desarrollado por
 *  SUR Emprendimientos Tecnologicos S.R.L. (http://www.suremptec.com/)
 *
 */

#ifndef SEYMURCONFIGURATIONPART_H_
#define SEYMURCONFIGURATIONPART_H_

#include "suri/Part.h"
#include "suri/DataViewManager.h"
#include "suri/AppConfigPartsProviderInterface.h"

class SeymurConfigurationPart : public suri::AppConfigPartsProviderInterface {
public:
   SeymurConfigurationPart(suri::DataViewManager* pDataViewManager);
   virtual ~SeymurConfigurationPart();
   /** Devuelve los parts existentes */
   virtual std::vector<suri::Part *> GetConfigParts();

private:
   suri::DataViewManager* pDataViewManager_;
};

#endif /* SEYMURCONFIGURATIONPART_H_ */
