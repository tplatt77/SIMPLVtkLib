/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "GenericMontageSettings.h"
#include <QtCore/qdebug.h>

 // -----------------------------------------------------------------------------
 //
 // -----------------------------------------------------------------------------
GenericMontageSettings::GenericMontageSettings()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMontageSettings::GenericMontageSettings(const GenericMontageSettings& model)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QObject* GenericMontageSettings::parentObject() const
{
	return QObject::parent();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t GenericMontageSettings::getGridSizeX() const
{
	return m_gridSizeX;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setGridSizeX(uint64_t gridSizeX)
{
	m_gridSizeX = gridSizeX;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t GenericMontageSettings::getGridSizeY() const
{
	return m_gridSizeY;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setGridSizeY(uint64_t gridSizeY)
{
	m_gridSizeY = gridSizeY;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double GenericMontageSettings::getTileOverlap() const
{
	return m_tileOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setTileOverlap(double tileOverlap)
{
	m_tileOverlap = tileOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double GenericMontageSettings::getIncreaseOverlap() const
{
	return m_increaseOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setIncreaseOverlap(double increaseOverlap)
{
	m_increaseOverlap = increaseOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t GenericMontageSettings::getFirstFileIndex() const
{
	return m_firstFileIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setFirstFileIndex(uint64_t firstFileIndex)
{
	m_firstFileIndex = firstFileIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
uint64_t GenericMontageSettings::getFrameRangeToCompare() const
{
	return m_frameRangeToCompare;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setFrameRangeToCompare(uint64_t frameRangeToCompare)
{
	m_frameRangeToCompare = frameRangeToCompare;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QDir GenericMontageSettings::getImagesDirectory() const
{
	return m_imagesDirectory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setImagesDirectory(QDir imagesDirectory)
{
	m_imagesDirectory = imagesDirectory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericMontageSettings::getFileNames() const
{
	return m_fileNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setFileNames(QString fileNames)
{
	m_fileNames = fileNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericMontageSettings::getLayoutFile() const
{
	return m_layoutFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setLayoutFile(QString layoutFile)
{
	m_layoutFile = layoutFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericMontageSettings::getMultiSeriesFile() const
{
	return m_multiSeriesFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setMultiSeriesFile(QString multiSeriesFile)
{
	m_multiSeriesFile = multiSeriesFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GenericMontageSettings::getOutputFileName() const
{
	return m_outputFileName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setOutputFileName(QString outputFileName)
{
	m_outputFileName = outputFileName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double GenericMontageSettings::getRegressionThreshold() const
{
	return m_regressionThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setRegressionThreshold(double regressionThreshold)
{
	m_regressionThreshold = regressionThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double GenericMontageSettings::getMaxDisplacementThreshold() const
{
	return m_maxDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setMaxDisplacementThreshold(double maxDisplacementThreshold)
{
	m_maxDisplacementThreshold = maxDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double GenericMontageSettings::getAbsoluteDisplacementThreshold() const
{
	return m_absoluteDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setAbsoluteDisplacementThreshold(double absoluteDisplacementThreshold)
{
	m_absoluteDisplacementThreshold = absoluteDisplacementThreshold;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isAddingTilesAsROI() const
{
	return m_addTilesAsROI;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  GenericMontageSettings::setAddingTilesAsROI(bool addTilesAsROI)
{
	m_addTilesAsROI = addTilesAsROI;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isComputingOverlap() const
{
	return m_computeOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  GenericMontageSettings::setComputingOverlap(bool computeOverlap)
{
	m_computeOverlap = computeOverlap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isInvertingXCoords() const
{
	return m_invertXCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setInvertingXCoords(bool invertXCoords)
{
	m_invertXCoords = invertXCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isInvertingYCoords() const
{
	return m_invertYCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setInvertingYCoords(bool invertYCoords)
{
	m_invertYCoords = invertYCoords;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isIgnoringZStatePos() const
{
	return m_ignoreZStatePos;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setIgnoringZStatePos(bool ignoreZStatePos)
{
	m_ignoreZStatePos = ignoreZStatePos;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isSubpixelAccuracy() const
{
	return m_subpixelAccuracy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setSubpixelAccuracy(bool subpixelAccuracy)
{
	m_subpixelAccuracy = subpixelAccuracy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isDownsamplingTiles() const
{
	return m_downsampleTiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setDownsamplingTiles(bool downsampleTiles)
{
	m_downsampleTiles = downsampleTiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isDisplayingFusion() const
{
	return m_displayFusion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setDisplayingFusion(bool displayFusion)
{
	m_displayFusion = displayFusion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isUsingVirtualInputImages() const
{
	return m_useVirtualInputImages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setUsingVirtualInputImages(bool useVirtualInputImages)
{
	m_useVirtualInputImages = useVirtualInputImages;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isConfirmingFiles() const
{
	return m_confirmFiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setConfirmingFiles(bool confirmFiles)
{
	m_confirmFiles = confirmFiles;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMontageSettings::isIgnoringCalibration() const
{
	return m_ignoreCalibration;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setIgnoringCalibration(bool ignoreCalibration)
{
	m_ignoreCalibration = ignoreCalibration;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMontageSettings::MontageType GenericMontageSettings::getMontageType() const
{
	return m_montageType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setMontageType(GenericMontageSettings::MontageType montageType)
{
	m_montageType = montageType;
	qInfo() << "Montage Type: " << m_montageType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMontageSettings::MontageOrder GenericMontageSettings::getMontageOrder() const
{
	return m_montageOrder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setMontageOrder(GenericMontageSettings::MontageOrder montageOrder)
{
	m_montageOrder = montageOrder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMontageSettings::FusionMethod GenericMontageSettings::getFusionMethod() const
{
	return m_fusionMethod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setFusionMethod(GenericMontageSettings::FusionMethod fusionMethod)
{
	m_fusionMethod = fusionMethod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMontageSettings::ComputationParameter GenericMontageSettings::getComputationParameters() const
{
	return m_computationParameters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setComputationParameters(GenericMontageSettings::ComputationParameter computationParameters)
{
	m_computationParameters = computationParameters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMontageSettings::ImageOutput GenericMontageSettings::getImageOutputMethod() const
{
	return m_imageOutputMethod;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMontageSettings::setImageOutputMethod(GenericMontageSettings::ImageOutput imageOutputMethod)
{
	m_imageOutputMethod = imageOutputMethod;
}
