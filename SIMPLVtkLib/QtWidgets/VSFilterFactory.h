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

#pragma once

#include <QtCore/QObject>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FloatVec3.h"
#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSFilterFactory VSFilterFactory.h
 * SIMPLVtkLib/QtWidgets/VSFilterFactory.h
 */
class SIMPLVtkLib_EXPORT VSFilterFactory : public QObject
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(VSFilterFactory)
  SIMPL_STATIC_NEW_MACRO(VSFilterFactory)
  SIMPL_TYPE_MACRO(VSFilterFactory)

  /**
   * @brief Deconstructor
   */
  ~VSFilterFactory() override = default;

  /**
   * @brief Creates a filter that imports a Fiji montage, and sets all the necessary properties
   * @param robometFile The Fiji file path
   * @param dcPath The path to the data containers the image data will be stored in
   * @param amName The name of the attribute matrix that the image data will be stored in
   * @param daName The name of the image data array
   * @param changeOrigin Boolean that overrides the origins in the image files.
   * @param origin XYZ origin array that overrides the origins from the image files.  This parameter
   * isn't needed if changeOrigin is false.
   * @param montageStart The starting column and row of the montage
   * @param montageEnd The ending column and row of the montage
   * @param changeSpacing Boolean that overrides the spacings coming from the image files.
   * @param spacing XYZ spacing array that overrides the spacings from the image files.  This parameter
   * isn't needed if changeSpacing is false.
   * @return
   */
  AbstractFilter::Pointer createImportFijiMontageFilter(const QString& fijiFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName, bool changeOrigin, const float* origin,
                                                        IntVec2Type montageStart, IntVec2Type montageEnd, bool changeSpacing, const float* spacing, int32_t lengthUnit);

  /**
   * @brief Creates a filter that imports a Robomet montage, and sets all the necessary properties
   * @param robometFile The Robomet file path
   * @param dcPath The path to the data containers the image data will be stored in
   * @param amName The name of the attribute matrix that the image data will be stored in
   * @param daName The name of the image data array
   * @param sliceNumber The slice number of the Robomet data
   * @param imageFilePrefix The prefix of the image files
   * @param imageFileExtension The extension of the image files
   * @param changeOrigin Boolean that overrides the origins in the image files.
   * @param origin XYZ origin array that overrides the origins from the image files.  This parameter
   * isn't needed if changeOrigin is false.
   * @param montageStart The starting column and row of the montage
   * @param montageEnd The ending column and row of the montage
   * @param changeSpacing Boolean that overrides the spacings coming from the image files.
   * @param spacing XYZ spacing array that overrides the spacings from the image files.  This parameter
   * isn't needed if changeSpacing is false.
   * @return
   */
  AbstractFilter::Pointer createImportRobometMontageFilter(const QString& robometFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName, int sliceNumber,
                                                           const QString& imageFilePrefix, const QString& imageFileExtension, bool changeOrigin, const float* origin, IntVec2Type montageStart,
                                                           IntVec2Type montageEnd, bool changeSpacing, const float* spacing, int32_t lengthUnit);

  /**
   * @brief Creates a filter that imports a Zeiss montage, and sets all the necessary properties
   * @param zeissFile The Zeiss XML file path
   * @param dcPath The path to the data containers the image data will be stored in
   * @param amName The name of the attribute matrix that the image data will be stored in
   * @param daName The name of the image data array
   * @param metaAmName The name of the image metadata array
   * @param importAllMetadata Boolean that imports all of the image metadata
   * @param convertToGrayscale Boolean that converts the image data to grayscale
   * @param colorWeights RGB array of color weights used during the grayscale conversion.  This parameter
   * isn't needed if convertToGrayscale is false.
   * @param changeOrigin Boolean that overrides the origins in the image files.
   * @param origin XYZ origin array that overrides the origins from the image files.  This parameter
   * isn't needed if changeOrigin is false.
   * @param montageStart The starting column and row of the montage
   * @param montageEnd The ending column and row of the montage
   * @param changeSpacing Boolean that overrides the spacings coming from the image files.
   * @param spacing XYZ spacing array that overrides the spacings from the image files.  This parameter
   * isn't needed if changeSpacing is false.
   * @return
   */
  AbstractFilter::Pointer createImportZeissMontageFilter(const QString& zeissFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName, const QString& metaAmName,
                                                         bool importAllMetadata, bool convertToGrayscale, FloatVec3Type colorWeights, bool changeOrigin, FloatVec3Type origin, IntVec2Type montageStart,
                                                         IntVec2Type montageEnd, bool changeSpacing, FloatVec3Type spacing);

  /**
   * @brief Creates a filter that imports a Zeiss Zen montage, and sets all the necessary properties
   * @param zeissFile The Zeiss Zen file path
   * @param dcPath The path to the data containers the image data will be stored in
   * @param amName The name of the attribute matrix that the image data will be stored in
   * @param daName The name of the image data array
   * @param convertToGrayscale Boolean that converts the image data to grayscale
   * @param colorWeights RGB array of color weights used during the grayscale conversion.  This parameter
   * isn't needed if convertToGrayscale is false.
   * @param changeOrigin Boolean that overrides the origins in the image files.
   * @param origin XYZ origin array that overrides the origins from the image files.  This parameter
   * isn't needed if changeOrigin is false.
   * @param montageStart The starting column and row of the montage
   * @param montageEnd The ending column and row of the montage
   * @return
   */
  AbstractFilter::Pointer createImportZeissZenMontageFilter(const QString& zeissFile, const DataArrayPath& dcPath, const QString& amName, const QString& daName, bool convertToGrayscale,
                                                            FloatVec3Type colorWeights, bool changeOrigin, FloatVec3Type origin, IntVec2Type montageStart, IntVec2Type montageEnd);

  /**
   * @brief Creates a PCM Tile Registration filter, and sets all necessary properties
   * @param montageStart The start of the montage that is being registered
   * @param montageEnd The end of the montage that is being registered
   * @param dcPrefix The prefix of the name of the data containers that contain the image files from the montage
   * @param amName Common attribute matrix name in each data container
   * @param daName Common data array name in each common attribute matrix
   * @return
   */
  AbstractFilter::Pointer createPCMTileRegistrationFilter(IntVec2Type montageStart, IntVec2Type montageEnd, const QString& dcPrefix, const QString& amName, const QString& daName);

  /**
   * @brief Creates a PCM Tile Registration filter, and sets all necessary properties
   * @param montageStart The start of the montage that is being stitched
   * @param montageEnd The end of the montage that is being stitched
   * @param dcPrefix The prefix of the name of the data containers that contain the image files from the montage
   * @param amName Common attribute matrix name in each data container
   * @param daName Common data array name in each common attribute matrix
   * @param montagePath The path to the created montage data array
   * @return
   */
  AbstractFilter::Pointer createTileStitchingFilter(IntVec2Type montageStart, IntVec2Type montageEnd, const QString& dcPrefix, const QString& amName, const QString& daName,
                                                    const DataArrayPath& montagePath);

  /**
   * @brief Creates a Set Origin Resolution filter that sets the origin and resolution of a data container's image geometry
   * @param dcPath The path to the data container
   * @param changeResolution Boolean that determines whether or not to change the resolution
   * @param changeOrigin Boolean that determines whether or not to change the origin
   * @param resolution The resolution to set into the image geometry
   * @param origin The origin to set into the image geometry
   * @return
   */
  AbstractFilter::Pointer createSetOriginResolutionFilter(const DataArrayPath& dcPath, bool changeResolution, bool changeOrigin, FloatVec3Type resolution, FloatVec3Type origin);

  /**
   * @brief Creates a Data Container Reader filter that reads the data from a DREAM3D file and sets all necessary properties
   * @param inputFile The DREAM3D file path to read
   * @param inputFileDCAProxy The proxy structure from the DREAM3D file
   * @return
   */
  AbstractFilter::Pointer createDataContainerReaderFilter(const QString& inputFile, const DataContainerArrayProxy& inputFileDCAProxy);

  /**
   * @brief Creates a Data Container Writer filter that write the data to a DREAM3D file and sets all necessary properties
   * @param outputFile The DREAM3D file path to write to
   * @return
   */
  AbstractFilter::Pointer createDataContainerWriterFilter(const QString& outputFile, bool writeXdmfFile, bool writeTimeSeriesMarkers);

  /**
   * @brief Creates an Image Reader filter that reads the data from an image file and sets all necessary properties
   * @param inputFile The image file path to read
   * @param dcPath The path to the data container
   * @return
   */
  AbstractFilter::Pointer createImageFileReaderFilter(const QString& inputFile, const DataArrayPath& dcPath);

  /**
   * @brief Creates an Image Writer filter that writes the data from an image to a file
   * @param outputFile The image file path to write to
   * @param dcName The name of the data container
   * @return
   */
  AbstractFilter::Pointer createImageFileWriterFilter(const QString& outputFile, const DataArrayPath& imageArrayPath);

protected:
  /**
   * @brief Constructor
   * @param parent
   */
  VSFilterFactory(QObject* parent = nullptr);

signals:
  void notifyErrorMessage(const QString& msg, int code);

private:
  /**
   * @brief printPropertyError
   * @param filter
   * @param propertyName
   * @param value
   */
  bool setFilterProperty(const AbstractFilter::Pointer& filter, const char* propertyName, const QVariant& value);

public:
  VSFilterFactory(const VSFilterFactory&) = delete;            // Copy Constructor Not Implemented
  VSFilterFactory(VSFilterFactory&&) = delete;                 // Move Constructor Not Implemented
  VSFilterFactory& operator=(const VSFilterFactory&) = delete; // Copy Assignment Not Implemented
  VSFilterFactory& operator=(VSFilterFactory&&) = delete;      // Move Assignment Not Implemented
};
