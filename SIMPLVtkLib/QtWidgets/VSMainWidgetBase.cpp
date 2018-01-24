/* ============================================================================
* Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include "VSMainWidgetBase.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMainWidgetBase::VSMainWidgetBase(QWidget* parent)
  : QWidget(parent)
  , m_Controller(new VSController())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::connectSlots()
{
  connect(m_Controller, SIGNAL(activeViewChanged(VSViewController*)), 
    this, SLOT(activeViewChanged(VSViewController*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController* VSMainWidgetBase::getController()
{
  return m_Controller;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterView* VSMainWidgetBase::getFilterView()
{
  return m_FilterView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setFilterView(VSFilterView* view)
{
  if(m_FilterView)
  {
    disconnect(m_FilterView, SIGNAL(filterClicked(VSAbstractFilter*)));
  }

  view->setModel(m_Controller->getFilterModel());

  m_FilterView = view;
  connect(view, SIGNAL(filterClicked(VSAbstractFilter*)), this, SLOT(changeCurrentFilter(VSAbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInfoWidget* VSMainWidgetBase::getInfoWidget()
{
  return m_InfoWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::setInfoWidget(VSInfoWidget* infoWidget)
{
  if(m_InfoWidget)
  {
    disconnect(this, SIGNAL(updateFilterInfo(VSAbstractFilter*, VSViewController*)),
      m_InfoWidget, SLOT(updateFilterInfo(VSAbstractFilter*, VSViewController*)));
  }

  m_InfoWidget = infoWidget;

  connect(this, SIGNAL(updateFilterInfo(VSAbstractFilter*, VSViewController*)), 
    infoWidget, SLOT(updateFilterInfo(VSAbstractFilter*, VSViewController*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSMainWidgetBase::getCurrentFilter()
{
  return m_CurrentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::activeViewChanged(VSViewController* controller)
{
  // Update VSFilterViewSettings to use the new ViewController's version
  emit updateFilterInfo(m_CurrentFilter, m_Controller->getActiveViewController());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidgetBase::changeCurrentFilter(VSAbstractFilter* filter)
{
  m_CurrentFilter = filter;

  emit updateFilterInfo(m_CurrentFilter, m_Controller->getActiveViewController());
}
