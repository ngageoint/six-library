//#############################################################################
//
//    FILENAME:          SettableEllipsoid.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This file provides implementation for methods declared in the
//    SettableEllipsoid class.
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     06-Aug-2016   Krinsky  Initial Coding
//
//#############################################################################

#ifndef CSM_LIBRARY
#define CSM_LIBRARY
#endif

#include "SettableEllipsoid.h"
#include "Model.h"

namespace csm 
{
   //*****************************************************************************
   // SettableEllipsoid::SettableEllipsoid()
   //*****************************************************************************
   SettableEllipsoid::SettableEllipsoid()
   {
       csm::Ellipsoid wgs84Ellipsoid;
       setEllipsoid(wgs84Ellipsoid);
   }

   //**************************************************************************
   // SettableEllipsoid::SettableEllipsoid()
   //**************************************************************************
   SettableEllipsoid::SettableEllipsoid( const csm::Ellipsoid &ellipsoid )
   {
      setEllipsoid( ellipsoid );
   }

   csm::Ellipsoid SettableEllipsoid::getEllipsoid() const
   {
      return mEllipsoid;
   }

   void SettableEllipsoid::setEllipsoid(
      const csm::Ellipsoid &ellipsoid)
   {
      mEllipsoid = ellipsoid;
   }


   Ellipsoid SettableEllipsoid::getEllipsoid(
      const csm::Model *model)
   {
      Ellipsoid ellipse; // By default this is WGS-84

      const SettableEllipsoid *ellipseModel = dynamic_cast<const SettableEllipsoid *>(model);
      if (ellipseModel != NULL)
      {
         ellipse = ellipseModel->getEllipsoid();
      }

      return ellipse;
   }

} // namespace csm
