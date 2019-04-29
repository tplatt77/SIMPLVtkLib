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

#include "VSSliceFilter.h"

#include <QtCore/QJsonArray>
#include <QtCore/QString>
#include <QtCore/QUuid>

#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkUnstructuredGrid.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilter::VSSliceFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_SliceAlgorithm = nullptr;
  setParentFilter(parent);

  m_SliceValues = new VSSliceValues(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilter::VSSliceFilter(const VSSliceFilter& copy)
: VSAbstractFilter()
{
  m_SliceAlgorithm = nullptr;
  setParentFilter(copy.getParentFilter());

  m_SliceValues = new VSSliceValues(*(copy.m_SliceValues));
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceFilter* VSSliceFilter::Create(QJsonObject& json, VSAbstractFilter* parent)
{
  VSSliceFilter* filter = new VSSliceFilter(parent);
  filter->m_SliceValues->readJson(json);
  
  filter->setInitialized(true);
  filter->readTransformJson(json);

  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::createFilter()
{
  m_SliceAlgorithm = vtkSmartPointer<vtkCutter>::New();
  m_SliceAlgorithm->SetInputConnection(getParentFilter()->getOutputPort());
  setConnectedInput(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSSliceFilter::getFilterName() const
{
  return "Slice";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSSliceFilter::getToolTip() const
{
  return "Slice Filter";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterType VSSliceFilter::getFilterType() const
{
  return FilterType::Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::applyValues(VSSliceValues* values)
{
  if(values)
  {
    apply(values->getOrigin(), values->getNormal());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::apply(double origin[3], double normal[3])
{
  if(nullptr == m_SliceAlgorithm)
  {
    createFilter();
  }

  // Save the applied values for resetting Plane-Type widgets
  m_SliceValues->setLastOrigin(origin);
  m_SliceValues->setLastNormal(normal);

  VTK_NEW(vtkPlane, planeWidget);
  planeWidget->SetOrigin(origin);
  planeWidget->SetNormal(normal);

  m_SliceAlgorithm->SetCutFunction(planeWidget);
  m_SliceAlgorithm->Update();

  emit updatedOutputPort(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::apply(std::vector<double> originVector, std::vector<double> normalVector)
{
  if(originVector.size() != 3 || normalVector.size() != 3)
  {
    return;
  }

  double* origin = new double[3];
  double* normal = new double[3];
  for(int i = 0; i < 3; i++)
  {
    origin[i] = originVector[i];
    normal[i] = normalVector[i];
  }

  apply(origin, normal);

  delete[] origin;
  delete[] normal;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::readJson(QJsonObject& json)
{
  m_SliceValues->readJson(json);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::writeJson(QJsonObject& json)
{
  VSAbstractFilter::writeJson(json);
  m_SliceValues->writeJson(json);

  json["Uuid"] = GetUuid().toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkAlgorithmOutput* VSSliceFilter::getOutputPort()
{
  if(getConnectedInput() && m_SliceAlgorithm)
  {
    return m_SliceAlgorithm->GetOutputPort();
  }
  else if(getParentFilter())
  {
    return getParentFilter()->getOutputPort();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) VSSliceFilter::getOutput() const
{
  if(getConnectedInput() && m_SliceAlgorithm)
  {
    return m_SliceAlgorithm->GetOutput();
  }
  else if(getParentFilter())
  {
    return getParentFilter()->getOutput();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceFilter::updateAlgorithmInput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  setInputPort(filter->getOutputPort());

  if(getConnectedInput() && m_SliceAlgorithm)
  {
    m_SliceAlgorithm->SetInputConnection(filter->getOutputPort());
  }
  else
  {
    emit updatedOutputPort(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUuid VSSliceFilter::GetUuid()
{
  return QUuid("{7a5a24be-13db-5101-8a7c-5a8a7979fbbb}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSSliceFilter::getOutputType() const
{
  return POLY_DATA;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSSliceFilter::GetRequiredInputType()
{
  return ANY_DATA_SET;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSliceFilter::CompatibleWithParent(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return false;
  }

  if(CompatibleInput(filter->getOutputType(), GetRequiredInputType()))
  {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSliceFilter::CompatibleWithParents(VSAbstractFilter::FilterListType filters)
{
  if(filters.size() == 0)
  {
    return false;
  }

  for(VSAbstractFilter* filter : filters)
  {
    if(false == CompatibleWithParent(filter))
    {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilterValues* VSSliceFilter::getValues()
{
  return m_SliceValues;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSSliceFilter::getInfoString(SIMPL::InfoStringFormat format) const
{
  return QString();
}
