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

#include <QtWidgets/QLayout>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(QWidget* parent)
  : QWidget(parent)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSViewController* VSAbstractViewWidget::getViewController()
{
  return m_ViewController;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setViewController(VSViewController* controller)
{
  m_ViewController = controller;

  connect(controller, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(filterAdded(VSAbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterAdded(VSAbstractFilter* filter)
{
  VSFilterViewSettings* filterViewSettings = m_ViewController->getViewSettings(filter);

  if(nullptr == filterViewSettings)
  {
    return;
  }

  connect(filterViewSettings, SIGNAL(visibilityChanged(VSFilterViewSettings*, bool)), this, SLOT(filterVisibilityChanged(VSFilterViewSettings*, bool)));
  connect(filterViewSettings, SIGNAL(activeArrayIndexChanged(VSFilterViewSettings*, int)), this, SLOT(filterArrayIndexChanged(VSFilterViewSettings*, int)));
  connect(filterViewSettings, SIGNAL(activeComponentIndexChanged(VSFilterViewSettings*, int)), this, SLOT(filterComponentIndexChanged(VSFilterViewSettings*, int)));
  connect(filterViewSettings, SIGNAL(mapColorsChanged(VSFilterViewSettings*, bool)), this, SLOT(filterMapColorsChanged(VSFilterViewSettings*, bool)));
  connect(filterViewSettings, SIGNAL(showScalarBarChanged(VSFilterViewSettings*, bool)), this, SLOT(filterShowScalarBarChanged(VSFilterViewSettings*, bool)));

  filterViewSettings->getScalarBarWidget()->SetInteractor(getVisualizationWidget()->GetInteractor());

  filterVisibilityChanged(filterViewSettings, filterViewSettings->getVisible());
  filterShowScalarBarChanged(filterViewSettings, filterViewSettings->isScalarBarVisible());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::getActiveFilterSettings()
{
  return m_ActiveFilterSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setActiveFilterSettings(VSFilterViewSettings* settings)
{
  m_ActiveFilterSettings = settings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterVisibilityChanged(VSFilterViewSettings* viewSettings, bool filterVisible)
{
  if(nullptr == getVisualizationWidget())
  {
    return;
  }

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
void VSAbstractViewWidget::filterArrayIndexChanged(VSFilterViewSettings* viewSettings, int index)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterComponentIndexChanged(VSFilterViewSettings* viewSettings, int index)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterMapColorsChanged(VSFilterViewSettings* viewSettings, bool mapColors)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterShowScalarBarChanged(VSFilterViewSettings* viewSettings, bool showScalarBar)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterVisibility(VSFilterViewSettings* viewSettings, bool filterVisible)
{
  viewSettings->setVisible(filterVisible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterArrayIndex(int index)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setActiveArrayIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterComponentIndex(int index)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setActiveComponentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterMapColors(bool mapColors)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setMapColors(mapColors);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterShowScalarBar(bool showScalarBar)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setScalarBarVisible(showScalarBar);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::mousePressEvent(QMouseEvent* event)
{
  if(m_ViewController)
  {
    m_ViewController->markActive();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSplitter* VSAbstractViewWidget::splitVertically()
{
  return splitWidget(Qt::Vertical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSplitter* VSAbstractViewWidget::splitHorizontally()
{
  return splitWidget(Qt::Horizontal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSplitter* VSAbstractViewWidget::splitWidget(Qt::Orientation orientation)
{
  QSplitter* splitter = new QSplitter();

  // Replace this with the splitter in the parent layout if possible
  // Otherwise, set the splitter's parent to this widget's parent
  QWidget* parent = parentWidget();
  if(parent)
  {
    if(parent->layout())
    {
      parent->layout()->replaceWidget(this, splitter);
    }
    else
    {
      splitter->setParent(parent);
    }
  }

  // Fill the splitter with both the view widget and a clone of it
  splitter->addWidget(this);
  splitter->addWidget(clone());

  return splitter;
}
