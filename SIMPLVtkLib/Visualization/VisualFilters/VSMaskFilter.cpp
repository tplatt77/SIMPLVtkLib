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
VSMaskFilter::VSMaskFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_MaskAlgorithm = nullptr;
  setParentFilter(parent);

  //m_MaskWidget = new VSMaskWidget(maskFunctionWidget, "", parent->getBounds(), parent->getInteractor());
  //m_MaskWidget->show();

  //connect(m_MaskWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));

  setFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMaskFilter::~VSMaskFilter()
{
  m_MaskAlgorithm = nullptr;
  //delete m_MaskWidget;
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
  m_MaskAlgorithm = VTK_PTR(vtkThreshold)::New();

  m_MaskAlgorithm->SetInputConnection(m_ParentFilter->getOutputPort());

  //m_MaskWidget->updateMaskNames(m_parentFilter->getOutput());
  m_ConnectedInput = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void VSMaskFilter::calculateOutput()
//{
//  if(!m_ConnectedInput && m_ParentFilter)
//  {
//    m_MaskAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
//    m_ConnectedInput = true;
//
//    m_OutputProducer->SetInputConnection(m_MaskAlgorithm->GetOutputPort());
//  }
//
//  vtkDataSet* input = m_ParentFilter->getOutput();
//  int maskId = m_MaskWidget->getMaskId();
//
//  input->GetCellData()->SetActiveScalars(scalarIdToName(maskId));
//
//  m_MaskAlgorithm->SetInputData(input);
//
//  m_MaskAlgorithm->ThresholdByUpper(1.0);
//  m_MaskAlgorithm->Update();
//
//  m_isDirty = false;
//}

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
//VSAbstractWidget* VSMaskFilter::getWidget()
//{
//  return m_MaskWidget;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMaskFilter::apply()
{
  //m_MaskWidget->apply();
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
