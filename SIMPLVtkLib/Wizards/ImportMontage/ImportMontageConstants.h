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
  namespace Zeiss
  {
    namespace FieldNames
    {
      const QString MontageName("zeissMontageName");
      const QString DataFilePath("zeissDataFilePath");
      const QString DataContainerPrefix("zeissDataContainerPrefix");
      const QString CellAttributeMatrixName("zeissCellAttributeMatrixName");
      const QString ImageDataArrayName("zeissImageDataArrayName");
      const QString MetadataAttrMatrixName("zeissMetadataAttrMatrixName");
      const QString ChangeTileOverlap("zeissChangeTileOverlap");
      const QString TileOverlap("zeissTileOverlap");
      const QString ConvertToGrayscale("zeissConvertToGrayscale");
      const QString ChangeOrigin("zeissChangeOrigin");
      const QString ChangeSpacing("zeissChangeSpacing");
      const QString ColorWeightingR("zeissColorWeightingR");
      const QString ColorWeightingG("zeissColorWeightingG");
      const QString ColorWeightingB("zeissColorWeightingB");
      const QString OriginX("zeissOriginX");
      const QString OriginY("zeissOriginY");
      const QString OriginZ("zeissOriginZ");
      const QString SpacingX("zeissSpacingX");
      const QString SpacingY("zeissSpacingY");
      const QString SpacingZ("zeissSpacingZ");
    }
  }

  namespace Fiji
  {
    namespace FieldNames
    {
      const QString MontageName("fijiMontageName");
	  const QString FijiListInfo("fijiListInfo");
      const QString DataFilePath("fijiDataFilePath");
      const QString NumberOfRows("fijiGenericNumOfRows");
      const QString NumberOfColumns("fijiGenericNumOfCols");
      const QString ChangeTileOverlap("fijiChangeTileOverlap");
      const QString TileOverlap("fijiTileOverlap");
      const QString CellAttributeMatrixName("fijiCellAttributeMatrixName");
      const QString DataContainerPrefix("fijiDataContainerPrefix");
      const QString ImageArrayName("fijiImageArrayName");
	  const QString ChangeOrigin("fijiChangeOrigin");
	  const QString ChangeSpacing("fijiChangeSpacing");
	  const QString OriginX("fijiOriginX");
	  const QString OriginY("fijiOriginY");
	  const QString OriginZ("fijiOriginZ");
	  const QString SpacingX("fijiSpacingX");
	  const QString SpacingY("fijiSpacingY");
	  const QString SpacingZ("fijiSpacingZ");
    }
  }

  namespace Robomet
  {
    namespace FieldNames
    {
      const QString MontageName("rbmMontageName");
      const QString DataFilePath("rbmDataFilePath");
      const QString NumberOfRows("rbmNumOfRows");
      const QString NumberOfColumns("rbmNumOfCols");
      const QString ChangeTileOverlap("rbmChangeTileOverlap");
      const QString TileOverlap("rbmTileOverlap");
      const QString SliceNumber("rbmSliceNumber");
      const QString ImageFilePrefix("rbmImageFilePrefix");
      const QString ImageFileSuffix("rbmImageFileSuffix");
      const QString ImageFileExtension("rbmImageFileExtension");
      const QString CellAttributeMatrixName("rbmCellAttributeMatrixName");
      const QString DataContainerPrefix("rbmDataContainerPrefix");
      const QString ImageArrayName("rbmImageArrayName");
      const QString RobometListInfo("rbmListInfo");
    }
  }

  namespace DREAM3D
  {
    namespace FieldNames
    {
      const QString MontageName("d3dMontageName");
      const QString DataFilePath("d3dDataFilePath");
      const QString CellAttributeMatrixName("d3dCellAttributeMatrixName");
      const QString NumberOfRows("d3dNumOfRows");
      const QString NumberOfColumns("d3dNumOfCols");
      const QString DataContainerPrefix("d3dDataContainerPrefix");
      const QString ImageArrayName("d3dImageArrayName");
      const QString TileOverlap("d3dTileOverlap");
      const QString Proxy("d3dProxy");
      const QString ChangeOrigin("d3dChangeOrigin");
      const QString ChangeSpacing("d3dChangeSpacing");
      const QString OriginX("d3dOriginX");
      const QString OriginY("d3dOriginY");
      const QString OriginZ("d3dOriginZ");
      const QString SpacingX("d3dSpacingX");
      const QString SpacingY("d3dSpacingY");
      const QString SpacingZ("d3dSpacingZ");
    }
  }

  namespace Generic
  {
    namespace FieldNames
    {
      const QString MontageName("genericMontageName");
      const QString NumberOfRows("genericNumOfRows");
      const QString NumberOfColumns("genericNumOfCols");
      const QString TileOverlap("genericTileOverlap");
      const QString FileListInfo("genericFileListInfo");
      const QString OutputFileName("genericOutputFileName");
      const QString MontageType("genericMontageType");
      const QString MontageOrder("genericMontageOrder");
    }
  }

  namespace FieldNames
  {
    const QString DisplayMontage("DisplayMontage");
    const QString DisplaySideBySide("DisplaySideBySide");
    const QString DisplayOutlineOnly("DisplayOutlineOnly");
    const QString InputType("InputType");
  }
}
