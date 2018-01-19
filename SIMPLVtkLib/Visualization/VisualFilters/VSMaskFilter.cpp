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

#include "VSMaskFilter.h"

#include <QString>

#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkThreshold.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSMaskWidget.h"

#include <vtkRenderWindowInteractor.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskFilter::VSMaskFilter(QWidget* parentWidget, VSAbstractFilter* parent)
: VSAbstractFilter(parentWidget, parent->getInteractor())
{
  setupUi(this);

  m_maskAlgorithm = nullptr;
  setParentFilter(parent);

  m_maskWidget = new VSMaskWidget(maskFunctionWidget, "", parent->getBounds(), parent->getInteractor());
  m_maskWidget->show();

  connect(m_maskWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));

  setFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskFilter::~VSMaskFilter()
{
  m_maskAlgorithm = nullptr;
  delete m_maskWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::setFilter()
{
  m_maskAlgorithm = VTK_PTR(vtkThreshold)::New();

  if(m_parentFilter != nullptr)
  {
    m_dataSet = m_parentFilter->getOutput();
    m_ParentProducer->SetOutput(m_dataSet);
  }

  m_maskAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
  m_filterMapper->SetInputConnection(m_ParentProducer->GetOutputPort());
  setViewScalarId(m_parentFilter->getViewScalarId());

  m_maskWidget->updateMaskNames(m_parentFilter->getOutput());
  m_ConnectedInput = false;

  VTK_PTR(vtkDataArray) dataArray = m_dataSet->GetCellData()->GetScalars();
  m_filterMapper->SetScalarRange(dataArray->GetRange()[0], dataArray->GetRange()[1]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::setInputData(VTK_PTR(vtkDataSet) inputData)
{
  if(nullptr == inputData.GetPointer())
  {
    return;
  }

  if(nullptr == m_maskAlgorithm)
  {
    return;
  }

  m_ParentProducer->SetOutput(inputData);
  m_ConnectedInput = false;

  m_dataSet = inputData;

  m_maskWidget->updateMaskNames(inputData);
  setDirty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::calculateOutput()
{
  if(!m_ConnectedInput && m_parentFilter)
  {
    m_maskAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
    m_ConnectedInput = true;

    m_filterMapper->SetInputConnection(m_maskAlgorithm->GetOutputPort());
  }

  vtkDataSet* input = m_parentFilter->getOutput();
  int maskId = m_maskWidget->getMaskId();

  input->GetCellData()->SetActiveScalars(scalarIdToName(maskId));

  m_maskAlgorithm->SetInputData(input);

  m_maskAlgorithm->ThresholdByUpper(1.0);
  m_maskAlgorithm->Update();
  m_dataSet = m_maskAlgorithm->GetOutput();

  m_dataSet->GetCellData()->SetActiveScalars(scalarIdToName(getViewScalarId()));

  updateMapperScalars();

  m_isDirty = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSMaskFilter::getFilterName()
{
  return "Mask";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractWidget* VSMaskFilter::getWidget()
{
  return m_maskWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::apply()
{
  m_maskWidget->apply();

  setDirty();
  refresh();

  m_changesWaiting = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::reset()
{
  m_maskWidget->reset();

  m_changesWaiting = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSMaskFilter::getOutputType()
{
  return UNSTRUCTURED_GRID;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSMaskFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}
