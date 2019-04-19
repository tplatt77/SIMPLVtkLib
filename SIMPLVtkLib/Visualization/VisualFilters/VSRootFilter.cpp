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

#include "VSRootFilter.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUuid>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include "SIMPLVtkLib/Visualization/Controllers/VSFilterModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSRootFilter::VSRootFilter(VSFilterModel* model)
: VSTextFilter(nullptr, "Root Item", "This item should not be visible")
, m_Model(model)
{
  setParent(model);
  setCheckable(false);
  setSelectable(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterModel* VSRootFilter::getModel() const
{
  return m_Model;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSRootFilter::getFilterName() const
{
  return "Root Item";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSRootFilter::writeJson(QJsonObject& json)
{
  VSTextFilter::writeJson(json);

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSRootFilter::GetUuid()
{
  return QUuid("{b02e564e-6ef9-58cb-a4a2-9d067f92bc7c}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSRootFilter::CompatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilterValues* VSRootFilter::getValues()
{
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterType VSRootFilter::getFilterType() const
{
  return FilterType::Placeholder;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSRootFilter::getInfoString(SIMPL::InfoStringFormat format) const
{
  return QString();
}
