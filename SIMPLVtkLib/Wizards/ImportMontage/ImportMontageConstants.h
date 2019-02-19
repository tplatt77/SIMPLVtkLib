/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#pragma once

#if defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#endif

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
namespace ImportMontage
{
  namespace FieldNames
  {
    const QString DisplayMontage("DisplayMontage");
    const QString DisplaySideBySide("DisplaySideBySide");
    const QString DisplayOutlineOnly("DisplayOutlineOnly");
    const QString DataFilePath("DataFilePath");
    const QString NumberOfRows("numOfRows");
    const QString NumberOfColumns("numOfCols");
    const QString GenericNumberOfRows("genericNumOfRows");
    const QString GenericNumberOfColumns("genericNumOfCols");
    const QString SliceNumber("sliceNumber");
    const QString ImageFilePrefix("imageFilePrefix");
    const QString ImageFileSuffix("imageFileSuffix");
    const QString ImageFileExtension("imageFileExtension");
    const QString ImageDataContainerPrefix("imageDataContainerPrefix");
    const QString CellAttributeMatrixName("cellAttrMatrixName");
    const QString ImageArrayName("imageArrayName");
    const QString UsingImageFileList("UsingImageFileList");
    const QString UsingConfigFile("UsingConfigFile");
    const QString UsingDREAM3DFile("UsingDREAM3DFile");
    const QString InputType("InputType");
    const QString MontageType("montageType");
    const QString MontageOrder("montageOrder");
    const QString DREAM3DFileName("dream3dFileName");
    const QString GenericTileOverlap("genericTileOverlap");
    const QString DREAM3DTileOverlap("dream3dTileOverlap");
    const QString GenericFileListInfo("GenericFileListInfo");
    const QString OutputFileName("outputFileName");
    const QString DREAM3DProxy("DREAM3DProxy");
	const QString ZeissDataFilePath("ZeissDataFilePath");
	const QString ZeissNumberOfRows("ZeissNumOfRows");
	const QString ZeissNumberOfColumns("ZeissNumOfCols");
	const QString ZeissDataContainerPrefix("ZeissDataContainerPrefix");
	const QString ZeissCellAttributeMatrixName("ZeissCellAttributeMatrixName");
	const QString ZeissImageDataArrayName("ZeissImageDataArrayName");
	const QString ZeissMetadataAttrMatrixName("ZeissMetadataAttrMatrixName");
	const QString ZeissImportAllMetadata("ZeissImportAllMetadata");
	const QString ZeissConvertToGrayscale("ZeissConvertToGrayscale");
	const QString ZeissChangeOrigin("ZeissChangeOrigin");
	const QString ZeissChangeSpacing("ZeissChangeSpacing");
	const QString ZeissColorWeightingR("ZeissColorWeightingR");
	const QString ZeissColorWeightingG("ZeissColorWeightingG");
	const QString ZeissColorWeightingB("ZeissColorWeightingB");
	const QString ZeissOriginX("ZeissOriginX");
	const QString ZeissOriginY("ZeissOriginY");
	const QString ZeissOriginZ("ZeissOriginZ");
	const QString ZeissSpacingX("ZeissSpacingX");
	const QString ZeissSpacingY("ZeissSpacingY");
	const QString ZeissSpacingZ("ZeissSpacingZ");
  }
}
