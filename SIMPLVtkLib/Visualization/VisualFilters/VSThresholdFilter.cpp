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

#include "VSThresholdFilter.h"

#include <QtCore/QString>

#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractGeometry.h>
#include <vtkExtractSelectedThresholds.h>
#include <vtkExtractUnstructuredGrid.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkImplicitDataSet.h>
#include <vtkMergeFilter.h>
#include <vtkPointData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkStructuredPoints.h>
#include <vtkThreshold.h>
#include <vtkTrivialProducer.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/Visualization/VisualFilters//VSDataSetFilter.h"
#include "SIMPLVtkLib/SIMPLBridge/VSRenderController.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSThresholdWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdFilter::VSThresholdFilter(QWidget* parentWidget, VSAbstractFilter* parent)
: VSAbstractFilter(parentWidget, parent->getInteractor())
{
  setupUi(this);

  m_thresholdAlgorithm = nullptr;
  setParentFilter(parent);

  VTK_PTR(vtkDataArray) dataArray = getBaseDataArray(parent->getViewScalarId());
  double range[2] = {dataArray->GetRange()[0], dataArray->GetRange()[1]};

  m_thresholdWidget = new VSThresholdWidget(thresholdFunctionWidget, range, parent->getBounds(), parent->getInteractor());
  m_thresholdWidget->show();

  connect(m_thresholdWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));

  setFilter();

  setupScalarsComboBox();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdFilter::~VSThresholdFilter()
{
  delete m_thresholdWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setFilter()
{
  m_currentId = m_parentFilter->getViewScalarId();
  m_lastId = m_currentId;

  m_thresholdAlgorithm = VTK_PTR(vtkThreshold)::New();

  if(nullptr != m_parentFilter)
  {
    m_ParentProducer->SetOutput(m_parentFilter->getOutput());
  }

  m_thresholdAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
  VTK_PTR(vtkDataArray) dataArray = m_dataSet->GetCellData()->GetScalars();
  m_filterMapper->SetScalarRange(dataArray->GetRange()[0], dataArray->GetRange()[1]);

  setThresholdScalarId(m_currentId);

  m_thresholdWidget->setLowerThreshold(dataArray->GetRange()[0]);
  m_thresholdWidget->setUpperThreshold(dataArray->GetRange()[1]);

  m_filterMapper->SetInputConnection(m_ParentProducer->GetOutputPort());
  setViewScalarId(m_parentFilter->getViewScalarId());

  m_ConnectedInput = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setInputData(VTK_PTR(vtkDataSet) inputData)
{
  if(nullptr == inputData.GetPointer())
  {
    return;
  }

  if(nullptr == m_thresholdAlgorithm)
  {
    return;
  }

  m_thresholdWidget->setBounds(inputData->GetBounds());
  m_ParentProducer->SetOutput(inputData);
  m_ConnectedInput = false;

  m_dataSet = inputData;

  setDirty();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::calculateOutput()
{
  if(!m_ConnectedInput && m_parentFilter)
  {
    m_thresholdAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
    m_ConnectedInput = true;

    m_filterMapper->SetInputConnection(m_thresholdAlgorithm->GetOutputPort());
  }

  vtkDataSet* input = m_parentFilter->getOutput();
  input->GetCellData()->SetActiveScalars(scalarIdToName(m_currentId));

  m_thresholdAlgorithm->SetInputData(input);

  m_thresholdAlgorithm->ThresholdBetween(m_thresholdWidget->getLowerBound(), m_thresholdWidget->getUpperBound());
  m_thresholdAlgorithm->Update();
  m_dataSet = m_thresholdAlgorithm->GetOutput();

  m_dataSet->GetCellData()->SetActiveScalars(scalarIdToName(getViewScalarId()));

  updateMapperScalars();
  m_isDirty = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSThresholdFilter::getFilterName()
{
  return "Threshold";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractWidget* VSThresholdFilter::getWidget()
{
  return m_thresholdWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::apply()
{
  m_thresholdWidget->apply();
  m_lastId = m_currentId;

  setDirty();
  refresh();

  m_changesWaiting = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::reset()
{
  scalarsComboBox->setCurrentIndex(m_lastId);
  setThresholdScalarId(m_lastId);

  m_thresholdWidget->reset();

  m_changesWaiting = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setThresholdScalarId(int id)
{
  if(nullptr == m_dataSet)
  {
    return;
  }
  if(id < 0)
  {
    id = 0;
  }

  m_currentId = id;

  vtkDataArray* dataArray = getBaseDataArray(id);

  if(nullptr != dataArray)
  {
    m_thresholdWidget->setScalarRange(dataArray->GetRange()[0], dataArray->GetRange()[1]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setupScalarsComboBox()
{
  if(nullptr == m_dataSet)
  {
    return;
  }

  QList<QString> scalarNameList;
  vtkCellData* cellData = m_dataSet->GetCellData();

  for(int i = 0; i < cellData->GetNumberOfArrays(); i++)
  {
    scalarNameList.push_back(cellData->GetArray(i)->GetName());
  }

  scalarsComboBox->clear();
  scalarsComboBox->addItems(scalarNameList);

  if(scalarNameList.length() > 0)
  {
    scalarsComboBox->setCurrentIndex(0);
  }

  // set scalar set to threshold against
  connect(scalarsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setThresholdScalarId(int)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSThresholdFilter::getOutputType()
{
  return UNSTRUCTURED_GRID;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSThresholdFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}
