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

#include "VSViewWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewController* VSViewWidget::getViewController()
{
  return m_ViewController;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::setViewController(VSViewController* controller)
{
  m_ViewController = controller;

  // TODO: connect signals / slots
  // TODO: Add visible filters
  // TODO: Add visible vtkScalarBarWidgets
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSViewWidget::getActiveFilterSettings()
{
  return m_ActiveFilterSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::setActiveFilterSettings(VSFilterViewSettings* settings)
{
  m_ActiveFilterSettings = settings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::filterVisibilityChanged(VSFilterViewSettings* viewSettings, bool filterVisible)
{
  if(filterVisible)
  {
    getVisualizationWidget()->getRenderer()->AddActor(viewSettings->getActor());

    if(viewSettings->isScalarBarVisible())
    {
      //getVisualizationWidget()->getRenderer()->AddActor2D(viewSettings->getScalarBarWidget());
    }
  }
  else
  {
    getVisualizationWidget()->getRenderer()->RemoveActor(viewSettings->getActor());

    if(viewSettings->isScalarBarVisible())
    {
      //getVisualizationWidget()->getRenderer()->RemoveActor2D(viewSettings->getScalarBarWidget());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::filterArrayIndexChanged(VSFilterViewSettings* viewSettings, int index)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::filterComponentIndexChanged(VSFilterViewSettings* viewSettings, int index)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::filterMapColorsChanged(VSFilterViewSettings* viewSettings, bool mapColors)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::filterShowScalarBarChanged(VSFilterViewSettings* viewSettings, bool showScalarBar)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::changeFilterVisibility(VSFilterViewSettings* viewSettings, bool filterVisible)
{
  viewSettings->setVisible(filterVisible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::changeFilterArrayIndex(int index)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setActiveArrayIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::changeFilterComponentIndex(int index)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setActiveComponentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::changeFilterMapColors(bool mapColors)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setMapColors(mapColors);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSViewWidget::changeFilterShowScalarBar(bool showScalarBar)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setScalarBarVisible(showScalarBar);
  }
}
