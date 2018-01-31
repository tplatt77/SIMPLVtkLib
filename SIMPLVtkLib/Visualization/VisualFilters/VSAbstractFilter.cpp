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

#include "VSAbstractFilter.h"

#include <QString>

#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include <vtkGenericDataObjectWriter.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSAbstractWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::VSAbstractFilter()
: QObject()
, QStandardItem()
, m_InputPort(nullptr)
{
  setCheckable(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::~VSAbstractFilter()
{
  while(m_Children.count() > 0)
  {
    VSAbstractFilter* child = m_Children[0];
    removeChild(child);
    delete child;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getParentFilter() const
{
  return m_ParentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setParentFilter(VSAbstractFilter* parent)
{
  if(m_ParentFilter != nullptr)
  {
    m_ParentFilter->removeChild(this);
  }

  if(nullptr == parent)
  {
    this->m_ParentFilter = nullptr;
    return;
  }
  else
  {
    m_ParentFilter = parent;
    //m_ParentFilter->m_Children.push_back(this);
    m_ParentFilter->addChild(this);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::addChild(VSAbstractFilter* child)
{
  if(m_Children.contains(child))
  {
    return;
  }

  m_Children.push_back(child);

  connect(this, SIGNAL(updatedOutputPort(VSAbstractFilter*)), child, SLOT(connectToOutuput(VSAbstractFilter*)));

  appendRow(child);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::removeChild(VSAbstractFilter* child)
{
  if(!m_Children.contains(child))
  {
    return;
  }

  m_Children.removeAll(child);
  child->setParentFilter(nullptr);

  disconnect(this, SIGNAL(updatedOutputPort(VSAbstractFilter*)), child, SLOT(connectToOutuput(VSAbstractFilter*)));

  removeRow(getIndexOfChild(child));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getAncestor()
{
  if(nullptr == m_ParentFilter)
  {
    return this;
  }

  return m_ParentFilter->getAncestor();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSAbstractFilter::getChildren() const
{
  return m_Children;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getIndexOfChild(VSAbstractFilter* child) const
{
  return m_Children.indexOf(child);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSAbstractFilter::getDescendants() const
{
  QVector<VSAbstractFilter*> descendants;

  int count = m_Children.size();
  for(int i = 0; i < count; i++)
  {
    descendants.push_back(m_Children[i]);
    descendants.append(m_Children[i]->getDescendants());
  }

  return descendants;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getChild(int index) const
{
  return m_Children.at(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr VSAbstractFilter::getWrappedDataContainer()
{
  VSDataSetFilter* dataSetFilter = getDataSetFilter();

  if(nullptr == dataSetFilter)
  {
    return nullptr;
  }

  return dataSetFilter->getWrappedDataContainer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractFilter::getBounds() const
{
  if(nullptr == m_ParentFilter)
  {
    return nullptr;
  }

  return m_ParentFilter->getBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDataSetFilter* VSAbstractFilter::getDataSetFilter()
{
  VSDataSetFilter* cast = dynamic_cast<VSDataSetFilter*>(this);

  if(cast != nullptr)
  {
    return cast;
  }

  if(nullptr == m_ParentFilter)
  {
    return nullptr;
  }

  return m_ParentFilter->getDataSetFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::compatibleInput(VSAbstractFilter::dataType_t inputType, VSAbstractFilter::dataType_t requiredType)
{
  if(requiredType == ANY_DATA_SET)
  {
    return true;
  }

  return requiredType == inputType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::saveFile(QString fileName)
{
  vtkDataSet* output = getOutput();

  VTK_NEW(vtkGenericDataObjectWriter, writer);

  writer->SetFileName(fileName.toStdString().c_str());
  writer->SetInputData(output);
  writer->Write();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::connectToOutuput(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  m_InputPort = filter->getOutputPort();

  if(m_ConnectedInput)
  {
    // Connect algorithm input and filter output
    updateAlgorithmInput(filter);
  }
  else
  {
    // Emit only when the filter is not connected to its algorithm
    emit updatedOutputPort(filter);
  }
}
