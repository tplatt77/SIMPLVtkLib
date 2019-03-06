/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "VSFilterFactory.h"

#include <array>

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/FilterParameters/FloatVec3.h"

VSFilterFactory* VSFilterFactory::s_Self = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterFactory::VSFilterFactory(QWidget* parent)
: QObject(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterFactory::~VSFilterFactory() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterFactory* VSFilterFactory::Instance()
{
  if(s_Self == nullptr)
  {
    s_Self = new VSFilterFactory();
  }
  return s_Self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImportFijiMontageFilter(const QString& fijiFile, const QString& dcPrefix, const QString& amName, const QString& daName)
{
  QString filterName = "ITKImportFijiMontage";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer importFijiMontageFilter = factory->create();
    if(importFijiMontageFilter.get() != nullptr)
    {
      QVariant var;

      // Set the path for the Fiji Configuration File
      var.setValue(fijiFile);
      if(!setFilterProperty(importFijiMontageFilter, "FijiConfigFilePath", var))
      {
        return;
      }

      // Set the Data Container Prefix
      var.setValue(dcPrefix);
      if(!setFilterProperty(importFijiMontageFilter, "DataContainerPrefix", var))
      {
        return;
      }

      // Set the Cell Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(importFijiMontageFilter, "CellAttributeMatrixName", var))
      {
        return;
      }

      // Set the Image Array Name
      var.setValue(daName);
      if(!setFilterProperty(importFijiMontageFilter, "AttributeArrayName", var))
      {
        return;
      }

      return importFijiMontageFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImportRobometMontageFilter(const QString& robometFile, const QString& dcPrefix, const QString& amName, const QString& daName, int sliceNumber,
                                                                        const QString &imageFilePrefix, const QString &imageFileExtension)
{
  QString filterName = "ITKImportRoboMetMontage";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer importRoboMetMontageFilter = factory->create();
    if(importRoboMetMontageFilter.get() != nullptr)
    {
      QVariant var;

      // Set the path for the Robomet Configuration File
      var.setValue(robometFile);
      if(!setFilterProperty(importRoboMetMontageFilter, "RegistrationFile", var))
      {
        return;
      }

      // Set the Data Container Prefix
      var.setValue(dcPrefix);
      if(!setFilterProperty(importRoboMetMontageFilter, "DataContainerPrefix", var))
      {
        return;
      }

      // Set the Cell Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(importRoboMetMontageFilter, "CellAttributeMatrixName", var))
      {
        return;
      }

      // Set the Image Array Name
      var.setValue(daName);
      if(!setFilterProperty(importRoboMetMontageFilter, "AttributeArrayName", var))
      {
        return;
      }

      // Slice number
      var.setValue(sliceNumber);
      if(!setFilterProperty(importRoboMetMontageFilter, "SliceNumber", var))
      {
        return;
      }

      // Image file prefix
      var.setValue(imageFilePrefix);
      if(!setFilterProperty(importRoboMetMontageFilter, "ImageFilePrefix", var))
      {
        return;
      }

      // Image file extension
      var.setValue(imageFileExtension);
      if(!setFilterProperty(importRoboMetMontageFilter, "ImageFileExtension", var))
      {
        return;
      }

      return importRoboMetMontageFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImportZeissMontageFilter(const QString& zeissFile, const QString& dcPrefix, const QString& amName, const QString& daName, const QString metaAmName,
                                                                        bool importAllMetadata, bool convertToGrayscale, bool changeOrigin, bool changeSpacing, float* colorWeights, float* origin,
                                                                        float* spacing)
{
  // Instantiate Import AxioVision V4 Montage filter
  QString filterName = "ImportAxioVisionV4Montage";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer importZeissMontageFilter = factory->create();
    if(importZeissMontageFilter.get() != nullptr)
    {
      QVariant var;

      // Set the path for the Robomet Configuration File
      var.setValue(zeissFile);
      if(!setFilterProperty(importZeissMontageFilter, "InputFile", var))
      {
        return;
      }

      // Set the Data Container Prefix
      var.setValue(dcPrefix);
      if(!setFilterProperty(importZeissMontageFilter, "DataContainerName", var))
      {
        return;
      }

      // Set the Cell Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(importZeissMontageFilter, "CellAttributeMatrixName", var))
      {
        return;
      }

      // Set the Image Array Name
      var.setValue(daName);
      if(!setFilterProperty(importZeissMontageFilter, "ImageDataArrayName", var))
      {
        return;
      }

      // Set the Metadata Attribute MatrixName
      var.setValue(metaAmName);
      if(!setFilterProperty(importZeissMontageFilter, "MetaDataAttributeMatrixName", var))
      {
        return;
      }

      // Set Import All Metadata
      var.setValue(importAllMetadata);
      if(!setFilterProperty(importZeissMontageFilter, "ImportAllMetaData", var))
      {
        return;
      }

      // Set Convert to Grayscale
      var.setValue(convertToGrayscale);
      if(!setFilterProperty(importZeissMontageFilter, "ConvertToGrayScale", var))
      {
        return;
      }

      if(convertToGrayscale)
      {
        float colorWeightR = colorWeights[0];
        float colorWeightG = colorWeights[1];
        float colorWeightB = colorWeights[2];
        FloatVec3_t colorWeights = {colorWeightR, colorWeightG, colorWeightB};
        var.setValue(colorWeights);
        if(!setFilterProperty(importZeissMontageFilter, "ColorWeights", var))
        {
          return;
        }
      }

      // Set Origin
      var.setValue(changeOrigin);
      if(!setFilterProperty(importZeissMontageFilter, "ChangeOrigin", var))
      {
        return;
      }

      if(changeOrigin)
      {
        float originX = origin[0];
        float originY = origin[1];
        float originZ = origin[2];
        FloatVec3_t newOrigin = {originX, originY, originZ};
        var.setValue(newOrigin);
        if(!setFilterProperty(importZeissMontageFilter, "Origin", var))
        {
          return;
        }
      }

      // Set Spacing
      var.setValue(changeSpacing);
      if(!setFilterProperty(importZeissMontageFilter, "ChangeSpacing", var))
      {
        return;
      }

      if(changeSpacing)
      {
        float spacingX = spacing[0];
        float spacingY = spacing[1];
        float spacingZ = spacing[2];
        FloatVec3_t newSpacing = {spacingX, spacingY, spacingZ};
        var.setValue(newSpacing);
        if(!setFilterProperty(importZeissMontageFilter, "Spacing", var))
        {
          return;
        }
      }

      return importZeissMontageFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createPCMTileRegistrationFilter(IntVec3_t montageSize, QStringList dcNames, const QString& amName, const QString& daName)
{
  QString filterName = "ITKPCMTileRegistration";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer itkRegistrationFilter = factory->create();
    if(itkRegistrationFilter.get() != nullptr)
    {
      QVariant var;

      // Set montage size
      var.setValue(montageSize);
      if(!setFilterProperty(itkRegistrationFilter, "MontageSize", var))
      {
        return;
      }

      // Set Common Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(itkRegistrationFilter, "CommonAttributeMatrixName", var))
      {
        return;
      }

      // Set Common Data Array Name
      var.setValue(daName);
      if(!setFilterProperty(itkRegistrationFilter, "CommonDataArrayName", var))
      {
        return;
      }

      // Set the list of image data containers
      var.setValue(dcNames);
      if(!setFilterProperty(itkRegistrationFilter, "ImageDataContainers", var))
      {
        return;
      }

      return itkRegistrationFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createTileStitchingFilter(IntVec3_t montageSize, QStringList dcNames, const QString& amName, const QString& daName, DataArrayPath montagePath, double tileOverlap)
{
  QString filterName = "ITKStitchMontage";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer itkStitchingFilter = factory->create();
    if(itkStitchingFilter.get() != nullptr)
    {
      QVariant var;

      // Set montage size
      var.setValue(montageSize);
      if(!setFilterProperty(itkStitchingFilter, "MontageSize", var))
      {
        return;
      }

      // Set Common Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(itkStitchingFilter, "CommonAttributeMatrixName", var))
      {
        return;
      }

      // Set Common Data Array Name
      var.setValue(daName);
      if(!setFilterProperty(itkStitchingFilter, "CommonDataArrayName", var))
      {
        return;
      }

      // Set the list of image data containers
      var.setValue(dcNames);
      if(!setFilterProperty(itkStitchingFilter, "ImageDataContainers", var))
      {
        return;
      }

      // Tile overlap
      var.setValue(tileOverlap);
      if(!setFilterProperty(itkStitchingFilter, "TileOverlap", var))
      {
        return;
      }

      // Montage results data array path components
      QString montageDataContainer = montagePath.getDataContainerName();
      var.setValue(montageDataContainer);
      if(!setFilterProperty(itkStitchingFilter, "MontageDataContainerName", var))
      {
        return;
      }

      QString montageAttriubeMatrixName = montagePath.getAttributeMatrixName();
      var.setValue(montageAttriubeMatrixName);
      if(!setFilterProperty(itkStitchingFilter, "MontageAttributeMatrixName", var))
      {
        return;
      }

      QString montageDataArrayName = montagePath.getDataArrayName();
      var.setValue(montageDataArrayName);
      if(!setFilterProperty(itkStitchingFilter, "MontageDataArrayName", var))
      {
        return;
      }

      return itkStitchingFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterFactory::setFilterProperty(AbstractFilter::Pointer filter, const char* propertyName, QVariant value)
{
  if(!filter->setProperty(propertyName, value))
  {
    statusBar()->showMessage(tr("%1: %2 property not set. Aborting.").arg(filter->getHumanLabel()).arg(propertyName));
    return false;
  }
  return true;
}
