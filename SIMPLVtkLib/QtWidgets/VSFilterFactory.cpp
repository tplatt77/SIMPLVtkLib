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

#include "SIMPLib/DataContainers/DataContainerArrayProxy.h"
#include "SIMPLib/FilterParameters/FloatVec3FilterParameter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterFactory::VSFilterFactory(QObject* parent)
: QObject(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createDataContainerReaderFilter(const QString& inputFile, const DataContainerArrayProxy &inputFileDCAProxy)
{
  QString filterName = "DataContainerReader";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer dataContainerReader = factory->create();
    if(dataContainerReader.get() != nullptr)
    {
      QVariant var;

      // Set input file
      var.setValue(inputFile);
      if(!setFilterProperty(dataContainerReader, "InputFile", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set data container array proxy
      var.setValue(inputFileDCAProxy);
      if(!setFilterProperty(dataContainerReader, "InputFileDataContainerArrayProxy", var))
      {
        return AbstractFilter::NullPointer();
      }
    }
    return dataContainerReader;
  }
  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createDataContainerWriterFilter(const QString& outputFile, bool writeXdmfFile, bool writeTimeSeriesMarkers)
{
  QString filterName = "DataContainerWriter";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer dataContainerWriter = factory->create();
    if(dataContainerWriter.get() != nullptr)
    {
      QVariant var;

      // Set output file
      var.setValue(outputFile);
      if(!setFilterProperty(dataContainerWriter, "OutputFile", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set whether to write Xdmf file
      var.setValue(writeXdmfFile);
      if(!setFilterProperty(dataContainerWriter, "WriteXdmfFile", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set whether to include Xdmf time markers
      var.setValue(writeTimeSeriesMarkers);
      if(!setFilterProperty(dataContainerWriter, "WriteTimeSeries", var))
      {
        return AbstractFilter::NullPointer();
      }
    }
    return dataContainerWriter;
  }
  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createSetOriginResolutionFilter(const DataArrayPath& dcPath, bool changeResolution, bool changeOrigin, FloatVec3Type resolution, FloatVec3Type origin)
{
  QString filterName = "SetOriginResolutionImageGeom";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer setOriginResolutionFilter = factory->create();
    if(setOriginResolutionFilter.get() != nullptr)
    {
      QVariant var;

      // Set the data container name
      var.setValue(dcPath);
      if(!setFilterProperty(setOriginResolutionFilter, "DataContainerName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the change spacing boolean flag (change resolution)
      var.setValue(changeResolution);
      if(!setFilterProperty(setOriginResolutionFilter, "ChangeResolution", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the spacing
      var.setValue(resolution);
      if(!setFilterProperty(setOriginResolutionFilter, "Spacing", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the change origin boolean flag (change resolution)
      var.setValue(changeOrigin);
      if(!setFilterProperty(setOriginResolutionFilter, "ChangeOrigin", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the origin
      var.setValue(origin);
      if(!setFilterProperty(setOriginResolutionFilter, "Origin", var))
      {
        return AbstractFilter::NullPointer();
      }

      return setOriginResolutionFilter;
    }
  }
  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImportFijiMontageFilter(const QString& fijiFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName,
                                                                       bool changeOrigin, const float* origin, bool changeSpacing, const float* spacing, int32_t lengthUnit)
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
      if(!setFilterProperty(importFijiMontageFilter, "InputFile", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Origin
      var.setValue(changeOrigin);
      if(!setFilterProperty(importFijiMontageFilter, "ChangeOrigin", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(changeOrigin)
      {
        float originX = origin[0];
        float originY = origin[1];
        float originZ = origin[2];
        FloatVec3Type newOrigin = {originX, originY, originZ};
        var.setValue(newOrigin);
        if(!setFilterProperty(importFijiMontageFilter, "Origin", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      // Set Spacing
      var.setValue(changeSpacing);
      if(!setFilterProperty(importFijiMontageFilter, "ChangeSpacing", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(changeSpacing)
      {
        float spacingX = spacing[0];
        float spacingY = spacing[1];
        float spacingZ = spacing[2];
        FloatVec3Type newSpacing = {spacingX, spacingY, spacingZ};
        var.setValue(newSpacing);
        if(!setFilterProperty(importFijiMontageFilter, "Spacing", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      // Set the Data Container Prefix
      var.setValue(dcPath);
      if(!setFilterProperty(importFijiMontageFilter, "DataContainerPath", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Cell Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(importFijiMontageFilter, "CellAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Image Array Name
      var.setValue(daName);
      if(!setFilterProperty(importFijiMontageFilter, "ImageDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Length Unit
      var.setValue(lengthUnit);
      if(!setFilterProperty(importFijiMontageFilter, "LengthUnit", var))
      {
        return AbstractFilter::NullPointer();
      }

      return importFijiMontageFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImportRobometMontageFilter(const QString& robometFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName, int sliceNumber,
                                                                          const QString& imageFilePrefix, const QString& imageFileExtension, bool changeOrigin, const float* origin,
                                                                          bool changeSpacing, const float* spacing, int32_t lengthUnit)
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
      if(!setFilterProperty(importRoboMetMontageFilter, "InputFile", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Origin
      var.setValue(changeOrigin);
      if(!setFilterProperty(importRoboMetMontageFilter, "ChangeOrigin", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(changeOrigin)
      {
        float originX = origin[0];
        float originY = origin[1];
        float originZ = origin[2];
        FloatVec3Type newOrigin = {originX, originY, originZ};
        var.setValue(newOrigin);
        if(!setFilterProperty(importRoboMetMontageFilter, "Origin", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      // Set Spacing
      var.setValue(changeSpacing);
      if(!setFilterProperty(importRoboMetMontageFilter, "ChangeSpacing", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(changeSpacing)
      {
        float spacingX = spacing[0];
        float spacingY = spacing[1];
        float spacingZ = spacing[2];
        FloatVec3Type newSpacing = {spacingX, spacingY, spacingZ};
        var.setValue(newSpacing);
        if(!setFilterProperty(importRoboMetMontageFilter, "Spacing", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      // Set the Data Container Path
      var.setValue(dcPath);
      if(!setFilterProperty(importRoboMetMontageFilter, "DataContainerPath", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Cell Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(importRoboMetMontageFilter, "CellAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Image Array Name
      var.setValue(daName);
      if(!setFilterProperty(importRoboMetMontageFilter, "ImageDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Slice number
      var.setValue(sliceNumber);
      if(!setFilterProperty(importRoboMetMontageFilter, "SliceNumber", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Image file prefix
      var.setValue(imageFilePrefix);
      if(!setFilterProperty(importRoboMetMontageFilter, "ImageFilePrefix", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Image file extension
      var.setValue(imageFileExtension);
      if(!setFilterProperty(importRoboMetMontageFilter, "ImageFileExtension", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Length Unit
      var.setValue(lengthUnit);
      if(!setFilterProperty(importRoboMetMontageFilter, "LengthUnit", var))
      {
        return AbstractFilter::NullPointer();
      }

      return importRoboMetMontageFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImportZeissZenMontageFilter(const QString& zeissFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName, bool convertToGrayscale,
                                                                           FloatVec3Type colorWeights, bool changeOrigin, FloatVec3Type origin)
{
  // Instantiate Import AxioVision V4 Montage filter
  QString filterName = "ImportZenInfoMontage";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer importZeissZenMontageFilter = factory->create();
    if(importZeissZenMontageFilter.get() != nullptr)
    {
      QVariant var;

      // Set the path for the Zeiss Zen File
      var.setValue(zeissFile);
      if(!setFilterProperty(importZeissZenMontageFilter, "InputFile", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Data Container Prefix
      var.setValue(dcPath);
      if(!setFilterProperty(importZeissZenMontageFilter, "DataContainerPath", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Cell Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(importZeissZenMontageFilter, "CellAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Image Array Name
      var.setValue(daName);
      if(!setFilterProperty(importZeissZenMontageFilter, "ImageDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Convert to Grayscale
      var.setValue(convertToGrayscale);
      if(!setFilterProperty(importZeissZenMontageFilter, "ConvertToGrayScale", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(convertToGrayscale)
      {
        float colorWeightR = colorWeights[0];
        float colorWeightG = colorWeights[1];
        float colorWeightB = colorWeights[2];
        FloatVec3Type colorWeights = {colorWeightR, colorWeightG, colorWeightB};
        var.setValue(colorWeights);
        if(!setFilterProperty(importZeissZenMontageFilter, "ColorWeights", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      // Set Origin
      var.setValue(changeOrigin);
      if(!setFilterProperty(importZeissZenMontageFilter, "ChangeOrigin", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(changeOrigin)
      {
        float originX = origin[0];
        float originY = origin[1];
        float originZ = origin[2];
        FloatVec3Type newOrigin = {originX, originY, originZ};
        var.setValue(newOrigin);
        if(!setFilterProperty(importZeissZenMontageFilter, "Origin", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      return importZeissZenMontageFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImportZeissMontageFilter(const QString& zeissFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName,
                                                                        const QString &metaAmName, bool importAllMetadata, bool convertToGrayscale, FloatVec3Type colorWeights,
                                                                        bool changeOrigin, FloatVec3Type origin, bool changeSpacing, FloatVec3Type spacing)
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
        return AbstractFilter::NullPointer();
      }

      // Set the Data Container Prefix
      var.setValue(dcPath);
      if(!setFilterProperty(importZeissMontageFilter, "DataContainerPath", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Cell Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(importZeissMontageFilter, "CellAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Image Array Name
      var.setValue(daName);
      if(!setFilterProperty(importZeissMontageFilter, "ImageDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the Metadata Attribute MatrixName
      var.setValue(metaAmName);
      if(!setFilterProperty(importZeissMontageFilter, "MetaDataAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Import All Metadata
      var.setValue(importAllMetadata);
      if(!setFilterProperty(importZeissMontageFilter, "ImportAllMetaData", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Convert to Grayscale
      var.setValue(convertToGrayscale);
      if(!setFilterProperty(importZeissMontageFilter, "ConvertToGrayScale", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(convertToGrayscale)
      {
        float colorWeightR = colorWeights[0];
        float colorWeightG = colorWeights[1];
        float colorWeightB = colorWeights[2];
        FloatVec3Type colorWeights = {colorWeightR, colorWeightG, colorWeightB};
        var.setValue(colorWeights);
        if(!setFilterProperty(importZeissMontageFilter, "ColorWeights", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      // Set Origin
      var.setValue(changeOrigin);
      if(!setFilterProperty(importZeissMontageFilter, "ChangeOrigin", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(changeOrigin)
      {
        float originX = origin[0];
        float originY = origin[1];
        float originZ = origin[2];
        FloatVec3Type newOrigin = {originX, originY, originZ};
        var.setValue(newOrigin);
        if(!setFilterProperty(importZeissMontageFilter, "Origin", var))
        {
          return AbstractFilter::NullPointer();
        }
      }

      // Set Spacing
      var.setValue(changeSpacing);
      if(!setFilterProperty(importZeissMontageFilter, "ChangeSpacing", var))
      {
        return AbstractFilter::NullPointer();
      }

      if(changeSpacing)
      {
        float spacingX = spacing[0];
        float spacingY = spacing[1];
        float spacingZ = spacing[2];
        FloatVec3Type newSpacing = {spacingX, spacingY, spacingZ};
        var.setValue(newSpacing);
        if(!setFilterProperty(importZeissMontageFilter, "Spacing", var))
        {
          return AbstractFilter::NullPointer();
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
AbstractFilter::Pointer VSFilterFactory::createPCMTileRegistrationFilter(IntVec3Type montageStart, IntVec3Type montageEnd, const QString &dcPrefix, const QString& amName, const QString& daName)
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

      // Set montage start
      var.setValue(montageStart);
      if(!setFilterProperty(itkRegistrationFilter, "MontageStart", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set montage end
      var.setValue(montageEnd);
      if(!setFilterProperty(itkRegistrationFilter, "MontageEnd", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Common Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(itkRegistrationFilter, "CommonAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Common Data Array Name
      var.setValue(daName);
      if(!setFilterProperty(itkRegistrationFilter, "CommonDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the list of image data containers
      var.setValue(dcPrefix);
      if(!setFilterProperty(itkRegistrationFilter, "DataContainerPrefix", var))
      {
        return AbstractFilter::NullPointer();
      }

      return itkRegistrationFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createTileStitchingFilter(IntVec3Type montageSize, const QStringList &dcNames, const QString& amName, const QString& daName, const DataArrayPath &montagePath)
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
        return AbstractFilter::NullPointer();
      }

      // Set Common Attribute Matrix Name
      var.setValue(amName);
      if(!setFilterProperty(itkStitchingFilter, "CommonAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set Common Data Array Name
      var.setValue(daName);
      if(!setFilterProperty(itkStitchingFilter, "CommonDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the list of image data containers
      var.setValue(dcNames);
      if(!setFilterProperty(itkStitchingFilter, "ImageDataContainers", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Montage results data array path components
      QString montageDataContainer = montagePath.getDataContainerName();
      var.setValue(montageDataContainer);
      if(!setFilterProperty(itkStitchingFilter, "MontageDataContainerName", var))
      {
        return AbstractFilter::NullPointer();
      }

      QString montageAttriubeMatrixName = montagePath.getAttributeMatrixName();
      var.setValue(montageAttriubeMatrixName);
      if(!setFilterProperty(itkStitchingFilter, "MontageAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      QString montageDataArrayName = montagePath.getDataArrayName();
      var.setValue(montageDataArrayName);
      if(!setFilterProperty(itkStitchingFilter, "MontageDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      return itkStitchingFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImageFileReaderFilter(const QString& inputFile, const DataArrayPath& dcPath)
{
  QString filterName = "ITKImageReader";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer itkImageReaderFilter = factory->create();
    if(itkImageReaderFilter.get() != nullptr)
    {
      QVariant var;

      // Set file name
      var.setValue(inputFile);
      if(!setFilterProperty(itkImageReaderFilter, "FileName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set data container name
      var.setValue(dcPath);
      if(!setFilterProperty(itkImageReaderFilter, "DataContainerName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set cell attribute matrix name
      QString cellAttributeMatrixName = "CellData";
      var.setValue(cellAttributeMatrixName);
      if(!setFilterProperty(itkImageReaderFilter, "CellAttributeMatrixName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set image data array name
      QString imageDataArrayName = "ImageData";
      var.setValue(imageDataArrayName);
      if(!setFilterProperty(itkImageReaderFilter, "ImageDataArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      return itkImageReaderFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createImageFileWriterFilter(const QString& outputFile, const DataArrayPath& imageArrayPath)
{
  QString filterName = "ITKImageWriter";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer itkImageWriterFilter = factory->create();
    if(itkImageWriterFilter.get() != nullptr)
    {
      QVariant var;

      // Set file name
      var.setValue(outputFile);
      if(!setFilterProperty(itkImageWriterFilter, "FileName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set plane [Currently default to XY (the first option in the DREAM3D GUI)]
      int plane = 0;
      var.setValue(plane);
      if(!setFilterProperty(itkImageWriterFilter, "Plane", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set image data array name
      var.setValue(imageArrayPath);
      if(!setFilterProperty(itkImageWriterFilter, "ImageArrayPath", var))
      {
        return AbstractFilter::NullPointer();
      }

      return itkImageWriterFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createResampleImageFilter(bool saveAsNewArray, const QString& outputArrayName, const DataArrayPath& imageArrayPath, IntVec3Type outputSize,
                                                                   int subsamplingRate)
{
  QString filterName = "ITKResampleImage";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer itkResampleImageFilter = factory->create();
    if(itkResampleImageFilter.get() != nullptr)
    {
      QVariant var;

      // Save as new array
      var.setValue(saveAsNewArray);
      if(!setFilterProperty(itkResampleImageFilter, "SaveAsNewArray", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set output array name
      var.setValue(outputArrayName);
      if(!setFilterProperty(itkResampleImageFilter, "NewCellArrayName", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the output spacing to the subsampling rate
      var.setValue(subsamplingRate);
      if(!setFilterProperty(itkResampleImageFilter, "OutputSpacing", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set the output size
      var.setValue(outputSize);
      if(!setFilterProperty(itkResampleImageFilter, "OutputSize", var))
      {
        return AbstractFilter::NullPointer();
      }

      // Set image data array name as input
      var.setValue(imageArrayPath);
      if(!setFilterProperty(itkResampleImageFilter, "SelectedCellArrayPath", var))
      {
        return AbstractFilter::NullPointer();
      }

      return itkResampleImageFilter;
    }
  }

  return AbstractFilter::NullPointer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer VSFilterFactory::createRemoveArrays(QStringList dataArraysToRemove, DataContainerArray::Pointer dataContainerArray)
{
  QString filterName = "RemoveArrays";
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer factory = fm->getFactoryFromClassName(filterName);

  if(factory.get() != nullptr)
  {
    AbstractFilter::Pointer removeArraysFilter = factory->create();
    if(removeArraysFilter.get() != nullptr)
    {
      QVariant var;
      DataContainerArrayProxy arraysToRemove = DataContainerArrayProxy(dataContainerArray.get());
      for(QString dataArrayName : dataArraysToRemove)
      {
        QMap<QString, DataContainerProxy>::iterator dcIterator = arraysToRemove.getDataContainers().begin();
        while(dcIterator != arraysToRemove.getDataContainers().end())
        {          
          dcIterator.value().setFlag(Qt::Unchecked);

          QMap<QString, AttributeMatrixProxy>::iterator amIterator = dcIterator.value().getAttributeMatricies().begin();
          while(amIterator != dcIterator.value().getAttributeMatricies().end())
          {
            amIterator.value().setFlag(Qt::Unchecked);

            QMap<QString, DataArrayProxy>::iterator daIterator = amIterator.value().getDataArrays().begin();
            while(daIterator != amIterator.value().getDataArrays().end())
            {
              if(daIterator.key() == dataArrayName)
              {
                daIterator.value().setFlag(Qt::Checked);
              }
              else
              {
                daIterator.value().setFlag(Qt::Unchecked);
              }

              daIterator++;
            } // end daIterator            

            amIterator++;
          } // end amIterator          

          dcIterator++;
        } // end dcIterator
      }

      // Save as new array
      var.setValue(arraysToRemove);
      if(!setFilterProperty(removeArraysFilter, "DataArraysToRemove", var))
      {
        return AbstractFilter::NullPointer();
      }

      return removeArraysFilter;
    }
  }

  return AbstractFilter::NullPointer();
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterFactory::setFilterProperty(const AbstractFilter::Pointer &filter, const char* propertyName, const QVariant &value)
{
  if(!filter->setProperty(propertyName, value))
  {
    notifyErrorMessage(tr("%1: %2 property not set. Aborting.").arg(filter->getHumanLabel()).arg(propertyName), -1000);
    return false;
  }
  return true;
}
