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

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSThresholdWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdFilter::VSThresholdFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_ThresholdAlgorithm = nullptr;
  setParentFilter(parent);

  //VTK_PTR(vtkDataArray) dataArray = getBaseDataArray(parent->getViewScalarId());
  //double range[2] = {dataArray->GetRange()[0], dataArray->GetRange()[1]};

  //m_ThresholdWidget = new VSThresholdWidget(thresholdFunctionWidget, range, parent->getBounds(), parent->getInteractor());
  //m_ThresholdWidget->show();

  //connect(m_ThresholdWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));

  setFilter();

  //setupScalarsComboBox();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSThresholdFilter::~VSThresholdFilter()
{
  //delete m_ThresholdWidget;
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
  m_CurrentId = 0;
  m_LastId = m_CurrentId;

  m_ThresholdAlgorithm = VTK_PTR(vtkThreshold)::New();

  if(nullptr != m_ParentFilter)
  {
    m_ParentProducer->SetInputConnection(m_ParentFilter->getOutputPort());
  }

  m_ThresholdAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
  VTK_PTR(vtkDataArray) dataArray = getWrappedDataContainer()->m_DataSet->GetCellData()->GetScalars();

  setThresholdScalarId(m_CurrentId);

  //m_ThresholdWidget->setLowerThreshold(dataArray->GetRange()[0]);
  //m_ThresholdWidget->setUpperThreshold(dataArray->GetRange()[1]);

  m_OutputProducer->SetInputConnection(m_ParentProducer->GetOutputPort());

  m_ConnectedInput = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void VSThresholdFilter::calculateOutput()
//{
//  if(!m_ConnectedInput && m_ParentFilter)
//  {
//    m_ThresholdAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
//    m_ConnectedInput = true;
//
//    m_OutputProducer->SetInputConnection(m_ThresholdAlgorithm->GetOutputPort());
//  }
//
//  vtkDataSet* input = m_ParentFilter->getOutput();
//  input->GetCellData()->SetActiveScalars(scalarIdToName(m_CurrentId));
//
//  m_ThresholdAlgorithm->SetInputData(input);
//
//  m_ThresholdAlgorithm->ThresholdBetween(m_ThresholdWidget->getLowerBound(), m_ThresholdWidget->getUpperBound());
//  m_ThresholdAlgorithm->Update();
//
//  m_isDirty = false;
//}

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
//VSAbstractWidget* VSThresholdFilter::getWidget()
//{
//  return m_ThresholdWidget;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::apply()
{
  //m_ThresholdWidget->apply();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSThresholdFilter::setThresholdScalarId(int id)
{
  if(nullptr == getWrappedDataContainer())
  {
    return;
  }
  if(id < 0)
  {
    id = 0;
  }

  m_CurrentId = id;

  //vtkDataArray* dataArray = getBaseDataArray(id);
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
