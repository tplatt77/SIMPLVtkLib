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

#include "MontageUtilities.h"

#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QTextStream>

#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString MontageUtilities::GenerateDataContainerName(const QString& dataContainerPrefix, const IntVec2Type& montageStart, const IntVec2Type& montageEnd, int32_t row, int32_t col)
{
  IntVec3Type montageSize;
  std::transform(montageStart.begin(), montageStart.end(), montageEnd.begin(), montageSize.begin(), [](int32_t a, int32_t b) -> int32_t { return a + b + 1; });
  int32_t rowCount = montageSize[1];
  int32_t colCount = montageSize[0];

  int32_t rowCountPadding = MontageUtilities::CalculatePaddingDigits(rowCount);
  int32_t colCountPadding = MontageUtilities::CalculatePaddingDigits(colCount);
  int charPaddingCount = std::max(rowCountPadding, colCountPadding);

  QString dcName = dataContainerPrefix;
  QTextStream dcNameStream(&dcName);
  dcNameStream << "r";
  dcNameStream.setFieldWidth(charPaddingCount);
  dcNameStream.setFieldAlignment(QTextStream::AlignRight);
  dcNameStream.setPadChar('0');
  dcNameStream << row;
  dcNameStream.setFieldWidth(0);
  dcNameStream << "c";
  dcNameStream.setFieldWidth(charPaddingCount);
  dcNameStream << col;
  return dcName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy MontageUtilities::CreateMontageProxy(SIMPLH5DataReader& reader, const QString& filePath, const QStringList& checkedDCNames)
{
  bool success = reader.openFile(filePath);
  if(!success)
  {
    return {};
  }

  int err = 0;
  SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
  DataContainerArrayProxy proxy = reader.readDataContainerArrayStructure(&req, err);
  QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
  if(dataContainers.isEmpty())
  {
    return {};
  }

  QStringList dcNames = dataContainers.keys();
  for(int i = 0; i < dcNames.size(); i++)
  {
    QString dcName = dcNames[i];
    DataContainerProxy dcProxy = dataContainers[dcName];

    if(checkedDCNames.contains(dcName))
    {
      dcProxy.setFlag(Qt::Checked);
    }
    else
    {
      dcProxy.setFlag(Qt::Unchecked);
    }

    dataContainers[dcName] = dcProxy;
  }

  reader.closeFile();

  return proxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int32_t MontageUtilities::CalculatePaddingDigits(int32_t count)
{
  int zeroPadding = 0;
  if(count > 0)
  {
    zeroPadding++;
  }
  if(count > 9)
  {
    zeroPadding++;
  }
  if(count > 99)
  {
    zeroPadding++;
  }
  if(count > 999)
  {
    zeroPadding++;
  }
  if(count > 9999)
  {
    zeroPadding++;
  }
  return zeroPadding;
}
