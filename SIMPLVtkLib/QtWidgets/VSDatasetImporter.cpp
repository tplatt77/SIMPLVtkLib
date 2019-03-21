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

#include "VSDatasetImporter.h"

#include <QtCore/QFileInfo>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSFileNameFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDatasetImporter::VSDatasetImporter(const QString &filePath)
: VSAbstractImporter()
, m_FilePath(filePath)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDatasetImporter::~VSDatasetImporter()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDatasetImporter::Pointer VSDatasetImporter::New(const QString &filePath)
{
  VSDatasetImporter::Pointer sharedPtr(new VSDatasetImporter(filePath));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSDatasetImporter::getName()
{
  QFileInfo fi(m_FilePath);
  return fi.fileName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetImporter::execute()
{
  setState(State::Executing);

  VSFileNameFilter* textFilter = new VSFileNameFilter(m_FilePath);
  VSDataSetFilter* filter = new VSDataSetFilter(m_FilePath, textFilter);

  setState(State::Finished);
  emit resultReady(textFilter, filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetImporter::cancel()
{
  // Cancel
  setState(State::Canceled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetImporter::reset()
{
  // Cancel
  setState(State::Ready);
}
