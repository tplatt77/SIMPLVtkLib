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

#include "VSClipFilter.h"

#include <QApplication>
#include <QString>

#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTableBasedClipDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::VSClipFilter(VSAbstractFilter* parent)
: VSAbstractFilter()
{
  m_ClipAlgorithm = nullptr;
  setParentFilter(parent);

  //m_PlaneWidget = new VSPlaneWidget(clipFunctionWidget, parent->getBounds(), parent->getInteractor());
  //m_BoxWidget = new VSBoxWidget(clipFunctionWidget, parent->getBounds(), parent->getInteractor());

  //connect(clipTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeClipType(int)));
  //connect(insideOutCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setInsideOut(int)));

  //connect(m_PlaneWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));
  //connect(m_BoxWidget, SIGNAL(modified()), this, SLOT(changesWaiting()));

  setFilter();

  m_LastInsideOutState = 0;
  changeClipType(PLANE, false);

  setText(getFilterName());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::~VSClipFilter()
{
  m_ClipAlgorithm = nullptr;
  delete m_PlaneWidget;
  delete m_BoxWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setBounds(double* bounds)
{
  if(nullptr == bounds)
  {
    return;
  }

  m_PlaneWidget->setBounds(bounds);
  m_BoxWidget->setBounds(bounds);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setFilter()
{
  m_ClipAlgorithm = vtkSmartPointer<vtkTableBasedClipDataSet>::New();
  m_ClipAlgorithm->SetClipFunction(getWidget()->getImplicitFunction());

  if(nullptr != m_ParentFilter)
  {
    m_ParentProducer->SetInputConnection(m_ParentFilter->getOutputPort());
  }

  m_ClipAlgorithm->SetInputConnection(m_ParentProducer->GetOutputPort());
  m_OutputProducer->SetInputConnection(m_ParentProducer->GetOutputPort());

  m_ConnectedInput = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void VSClipFilter::calculateOutput()
//{
//  if(!m_ConnectedInput && m_ParentFilter)
//  {
//    m_ClipAlgorithm->SetInputData(m_ParentFilter->getOutput());
//    m_ConnectedInput = true;
//
//    m_OutputProducer->SetInputConnection(m_ClipAlgorithm->GetOutputPort());
//  }
//
//  m_ClipAlgorithm->Update();
//
//  m_isDirty = false;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString VSClipFilter::getFilterName()
{
  return "Clip";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//VSAbstractWidget* VSClipFilter::getWidget()
//{
//  switch(m_CurrentClipType)
//  {
//  case PLANE:
//    return m_PlaneWidget;
//  case BOX:
//    return m_BoxWidget;
//  default:
//    return m_PlaneWidget;
//  }
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::apply()
{
  m_LastClipType = m_CurrentClipType;
  m_LastInsideOutState = m_ClipAlgorithm->GetInsideOut();

  VSAbstractWidget* currentWidget = getWidget();
  currentWidget->apply();
  m_ClipAlgorithm->SetClipFunction(currentWidget->getImplicitFunction());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::changeClipType(int type, bool shouldRepaint)
{
  m_CurrentClipType = (clipType_t)type;

  switch(m_CurrentClipType)
  {
  case PLANE:
    m_PlaneWidget->show();
    m_PlaneWidget->enable();

    m_BoxWidget->hide();
    m_BoxWidget->disable();
    break;
  case BOX:
    m_PlaneWidget->hide();
    m_PlaneWidget->disable();

    m_BoxWidget->show();
    m_BoxWidget->enable();
    break;
  default:
    m_PlaneWidget->show();
    m_PlaneWidget->enable();

    m_BoxWidget->hide();
    m_BoxWidget->disable();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipFilter::setInsideOut(int state)
{
  m_ClipAlgorithm->SetInsideOut(state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSClipFilter::getOutputType()
{
  return UNSTRUCTURED_GRID;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::dataType_t VSClipFilter::getRequiredInputType()
{
  return ANY_DATA_SET;
}
