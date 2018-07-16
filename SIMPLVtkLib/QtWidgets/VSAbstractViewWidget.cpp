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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QLayout>
#include <QtWidgets/QStyle>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractDataFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(QWidget* parent, Qt::WindowFlags windowFlags)
: QFrame(parent, windowFlags)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(const VSAbstractViewWidget& other)
: QFrame(nullptr)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::copyFilters(VSFilterViewSettings::Map filters)
{
  clearFilters();

  for(auto iter = filters.begin(); iter != filters.end(); iter++)
  {
    VSAbstractFilter* filter = iter->first;
    VSFilterViewSettings* viewSettings = new VSFilterViewSettings(*(iter->second));
    m_FilterViewSettings[filter] = viewSettings;

    connect(filter, &VSAbstractFilter::removeFilter, this, [=] { filterRemoved(filter); });

    connect(viewSettings, &VSFilterViewSettings::visibilityChanged, this, &VSAbstractViewWidget::setFilterVisibility);
    connect(viewSettings, &VSFilterViewSettings::gridVisibilityChanged, this, &VSAbstractViewWidget::setGridVisibility);
    connect(viewSettings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSAbstractViewWidget::setFilterArrayName);
    connect(viewSettings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSAbstractViewWidget::setFilterComponentIndex);
    connect(viewSettings, &VSFilterViewSettings::mapColorsChanged, this, &VSAbstractViewWidget::setFilterMapColors);
    connect(viewSettings, &VSFilterViewSettings::showScalarBarChanged, this, &VSAbstractViewWidget::setFilterShowScalarBar);
    connect(viewSettings, &VSFilterViewSettings::requiresRender, this, &VSAbstractViewWidget::renderView);
    connect(viewSettings, &VSFilterViewSettings::actorsUpdated, this, &VSAbstractViewWidget::updateScene);

    checkFilterViewSetting(viewSettings);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::clearFilters()
{
  for(auto iter = m_FilterViewSettings.begin(); iter != m_FilterViewSettings.end(); iter++)
  {
    VSFilterViewSettings* viewSettings = iter->second;
    viewSettings->setVisible(false);
    viewSettings->setScalarBarVisible(false);
    checkFilterViewSetting(viewSettings);

    viewSettings->deleteLater();
  }

  m_FilterViewSettings.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterAdded(VSAbstractFilter* filter, bool currentFilter)
{
  if(nullptr == filter)
  {
    return;
  }

  createFilterViewSettings(filter);

  if(dynamic_cast<VSAbstractDataFilter*>(filter))
  {
    getVisualizationWidget()->resetCamera();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::filterRemoved(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  VSFilterViewSettings* viewSettings = m_FilterViewSettings[filter];
  if(viewSettings)
  {
    changeFilterVisibility(viewSettings, false);
    viewSettings->deleteLater();
  }

  m_FilterViewSettings.erase(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::checkFilterViewSetting(VSFilterViewSettings* setting)
{
  if(nullptr == setting)
  {
    return;
  }

  if(setting->getScalarBarWidget())
  {
    setting->getScalarBarWidget()->SetInteractor(getVisualizationWidget()->GetInteractor());
  }

  changeFilterVisibility(setting, setting->isVisible());
  changeScalarBarVisibility(setting, setting->isScalarBarVisible());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::getActiveFilterSettings() const
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
VSFilterViewSettings* VSAbstractViewWidget::getFilterViewSettings(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return nullptr;
  }

  VSFilterViewSettings* settings = m_FilterViewSettings[filter];
  if(nullptr == settings)
  {
    settings = createFilterViewSettings(filter);
  }

  return settings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Map VSAbstractViewWidget::getAllFilterViewSettings() const
{
  return m_FilterViewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractViewWidget::getViewSettingsIndex(VSFilterViewSettings* settings)
{
  auto iter = m_FilterViewSettings.find(settings->getFilter());
  if(iter == m_FilterViewSettings.end())
  {
    return -1;
  }

  return std::distance(m_FilterViewSettings.begin(), iter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::createFilterViewSettings(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return nullptr;
  }

  // Do not overwrite filter view settings that already exist
  if(nullptr != m_FilterViewSettings[filter])
  {
    return m_FilterViewSettings[filter];
  }

  VSFilterViewSettings* viewSettings = new VSFilterViewSettings(filter);

  connect(filter, &VSAbstractFilter::removeFilter, this, [=] { filterRemoved(filter); });

  connect(viewSettings, &VSFilterViewSettings::visibilityChanged, this, &VSAbstractViewWidget::setFilterVisibility);
  connect(viewSettings, &VSFilterViewSettings::gridVisibilityChanged, this, &VSAbstractViewWidget::setGridVisibility);
  connect(viewSettings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSAbstractViewWidget::setFilterArrayName);
  connect(viewSettings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSAbstractViewWidget::setFilterComponentIndex);
  connect(viewSettings, &VSFilterViewSettings::mapColorsChanged, this, &VSAbstractViewWidget::setFilterMapColors);
  connect(viewSettings, &VSFilterViewSettings::showScalarBarChanged, this, &VSAbstractViewWidget::setFilterShowScalarBar);
  connect(viewSettings, &VSFilterViewSettings::requiresRender, this, &VSAbstractViewWidget::renderView);
  connect(viewSettings, &VSFilterViewSettings::actorsUpdated, this, &VSAbstractViewWidget::updateScene);
  connect(viewSettings, &VSFilterViewSettings::swappingActors, this, &VSAbstractViewWidget::swapActors);

  m_FilterViewSettings[filter] = viewSettings;

  if(filter->getParentFilter() && filter->getParentFilter()->getOutput())
  {
    VSFilterViewSettings* parentSettings = getFilterViewSettings(filter->getParentFilter());
    if(parentSettings)
    {
      viewSettings->copySettings(parentSettings);
    }
  }

  checkFilterViewSetting(viewSettings);

  return viewSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterVisibility(VSFilterViewSettings* viewSettings, const bool& filterVisible)
{
  if(nullptr == viewSettings)
  {
    return;
  }
  if(false == viewSettings->isValid())
  {
    return;
  }

  if(nullptr == getVisualizationWidget())
  {
    return;
  }

  if(nullptr == getVisualizationWidget()->getRenderer())
  {
    return;
  }

  if(filterVisible)
  {
    getVisualizationWidget()->getRenderer()->AddViewProp(viewSettings->getActor());

    if(viewSettings->isScalarBarVisible())
    {
      viewSettings->getScalarBarWidget()->SetEnabled(1);
    }
    // Do not show the grid if the filter is not visible
    if(viewSettings->isGridVisible())
    {
      VTK_PTR(vtkCubeAxesActor) cubeAxesActor = viewSettings->getCubeAxesActor();
      cubeAxesActor->SetCamera(getVisualizationWidget()->getRenderer()->GetActiveCamera());
      getVisualizationWidget()->getRenderer()->AddViewProp(cubeAxesActor);
    }
  }
  else
  {
    getVisualizationWidget()->getRenderer()->RemoveViewProp(viewSettings->getActor());
    getVisualizationWidget()->getRenderer()->RemoveViewProp(viewSettings->getCubeAxesActor());

    if(viewSettings->isScalarBarVisible())
    {
      viewSettings->getScalarBarWidget()->SetEnabled(0);
    }
  }

  emit visibilityChanged(viewSettings, filterVisible);
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeScalarBarVisibility(VSFilterViewSettings* viewSettings, const bool& visibility)
{
  if(nullptr == viewSettings)
  {
    return;
  }

  if(nullptr == viewSettings->getScalarBarWidget())
  {
    return;
  }

  if(viewSettings->getScalarBarWidget())
  {
    if(viewSettings->isVisible())
    {
      if(viewSettings->isScalarBarVisible())
      {
        viewSettings->getScalarBarWidget()->SetEnabled(1);
      }
      else
      {
        viewSettings->getScalarBarWidget()->SetEnabled(0);
      }
    }
    else
    {
      viewSettings->getScalarBarWidget()->SetEnabled(0);
    }
  }

  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterVisibility(const bool& filterVisible)
{
  VSFilterViewSettings* viewSettings = dynamic_cast<VSFilterViewSettings*>(sender());
  changeFilterVisibility(viewSettings, filterVisible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setGridVisibility(const bool& gridVisible)
{
  VSFilterViewSettings* viewSettings = dynamic_cast<VSFilterViewSettings*>(sender());
  if(nullptr == viewSettings)
  {
    return;
  }
  if(false == viewSettings->isValid())
  {
    return;
  }

  if(nullptr == getVisualizationWidget())
  {
    return;
  }

  if(nullptr == getVisualizationWidget()->getRenderer())
  {
    return;
  }

  if(gridVisible)
  {
    // Do not show the grid if the filter is not visible
    if(viewSettings->isVisible())
    {
      VTK_PTR(vtkCubeAxesActor) cubeAxesActor = viewSettings->getCubeAxesActor();
      cubeAxesActor->SetCamera(getVisualizationWidget()->getRenderer()->GetActiveCamera());
      getVisualizationWidget()->getRenderer()->AddViewProp(cubeAxesActor);
    }
  }
  else
  {
    getVisualizationWidget()->getRenderer()->RemoveViewProp(viewSettings->getCubeAxesActor());
  }

  emit gridVisibilityChanged(viewSettings, gridVisible);
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::swapActors(vtkProp3D* oldProp, vtkProp3D* newProp)
{
  getVisualizationWidget()->getRenderer()->RemoveViewProp(oldProp);
  getVisualizationWidget()->getRenderer()->AddViewProp(newProp);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterArrayName(const QString& name)
{
  // Handle in subclasses
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterComponentIndex(const int& index)
{
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterMapColors(const VSFilterViewSettings::ColorMapping& mapColorState)
{
  renderView();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setFilterShowScalarBar(const bool& showScalarBar)
{
  VSFilterViewSettings* viewSettings = dynamic_cast<VSFilterViewSettings*>(sender());
  changeScalarBarVisibility(viewSettings, showScalarBar);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterArrayName(QString name)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setActiveArrayName(name);
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
void VSAbstractViewWidget::changeFilterMapColors(VSFilterViewSettings::ColorMapping mapColorState)
{
  if(m_ActiveFilterSettings)
  {
    m_ActiveFilterSettings->setMapColors(mapColorState);
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
void VSAbstractViewWidget::mousePressed()
{
  emit markActive(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::mousePressEvent(QMouseEvent* event)
{
  mousePressed();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::keyPressEvent(QKeyEvent* event)
{
  QFrame::keyPressEvent(event);

  if(event->key() == Qt::Key::Key_Return)
  {
    emit applyCurrentFilter();
  }
  else if(event->key() == Qt::Key::Key_Escape)
  {
    emit resetCurrentFilter();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::splitVertically()
{
  splitWidget(Qt::Vertical);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::splitHorizontally()
{
  splitWidget(Qt::Horizontal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSplitter* VSAbstractViewWidget::splitWidget(Qt::Orientation orientation)
{
  QSplitter* splitter = new QSplitter();
  splitter->setOrientation(orientation);

  const int currentWidth = width();
  const int currentHeight = height();

  // Replace this with the splitter in the parent layout if possible
  // Otherwise, set the splitter's parent to this widget's parent
  QWidget* parent = parentWidget();
  QSplitter* parentSplitter = dynamic_cast<QSplitter*>(parent);

  if(parent)
  {
    // If already part of a QSplitter
    if(parentSplitter)
    {
      int index = parentSplitter->indexOf(this);
      parentSplitter->replaceWidget(index, splitter);
    }
    // If not already part of a QSplitter
    else
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
  }

  VSAbstractViewWidget* cloneWidget = clone();
  cloneWidget->getVisualizationWidget()->renderVtk();

  // Fill the splitter with both the view widget and a clone of it
  splitter->addWidget(this);
  splitter->addWidget(cloneWidget);

  // Add property to signify that this splitter is used for multiple view widgets
  splitter->setProperty("Visualization Splitter", true);

  if(Qt::Horizontal == orientation)
  {
    int newWidth = currentWidth / 2;
    QList<int> newSizes;
    newSizes.append(newWidth);
    newSizes.append(newWidth);
    splitter->setSizes(newSizes);
  }
  else
  {
    int newHeight = currentHeight / 2;
    QList<int> newSizes;
    newSizes.append(newHeight);
    newSizes.append(newHeight);
    splitter->setSizes(newSizes);
  }

  // Set the size policy to match the current widget's
  splitter->setSizePolicy(this->sizePolicy());

  // Emit signal notifying the main widget of a new view widget
  emit viewWidgetCreated(cloneWidget);

  updateClosable();
  cloneWidget->updateClosable();

  return splitter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractViewWidget::isClosable()
{
  QSplitter* parentSplitter = dynamic_cast<QSplitter*>(parentWidget());
  if(nullptr != parentSplitter)
  {
    QVariant var = parentSplitter->property("Visualization Splitter");
    return var.isValid() && var.toBool();
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::updateClosable()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::closeView()
{
  if(!isClosable())
  {
    return;
  }

  emit viewWidgetClosed();

  QSplitter* splitter = dynamic_cast<QSplitter*>(parentWidget());

  // If parent is a QSplitter...
  if(splitter)
  {
    int index = splitter->indexOf(this);
    QWidget* other = nullptr;

    if(1 == index)
    {
      other = splitter->widget(0);
    }
    else
    {
      other = splitter->widget(1);
    }

    QWidget* parent = splitter->parentWidget();
    QSplitter* parentSplitter = dynamic_cast<QSplitter*>(parent);
    // If another QSplitter...
    if(parentSplitter)
    {
      int parentIndex = parentSplitter->indexOf(splitter);
      parentSplitter->replaceWidget(parentIndex, other);
    }
    // If not another QSplitter..
    else
    {
      if(parent->layout())
      {
        parent->layout()->replaceWidget(splitter, other);
      }
      else
      {
        other->setParent(parent);
      }
    }

    splitter->deleteLater();

    // Check if other is a view widget
    VSAbstractViewWidget* otherView = dynamic_cast<VSAbstractViewWidget*>(other);
    if(otherView)
    {
      otherView->updateClosable();
    }
  }
  else
  {
    deleteLater();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setBlockRender(bool block)
{
  m_BlockRender = block;

  if(getVisualizationWidget() && getVisualizationWidget()->getRenderer())
  {
    if(block)
    {
      getVisualizationWidget()->getRenderer()->DrawOff();
    }
    else
    {
      getVisualizationWidget()->getRenderer()->DrawOn();
      renderView();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::renderView()
{
  if(m_BlockRender)
  {
    return;
  }

  VSQuickWidget* visualizationWidget = getVisualizationWidget();
  if(visualizationWidget)
  {
    //visualizationWidget->render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::resetCamera()
{
  if(m_BlockRender)
  {
    return;
  }

  VSQuickWidget* visualizationWidget = getVisualizationWidget();
  if(visualizationWidget && visualizationWidget->getRenderer())
  {
    visualizationWidget->getRenderer()->ResetCamera();
    visualizationWidget->renderVtk();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::updateScene()
{
  renderView();
  resetCamera();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSController* VSAbstractViewWidget::getController() const
{
  return m_Controller;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractViewWidget::isActive()
{
  return m_Active;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setActive(bool active)
{
  m_Active = active;

  style()->unpolish(this);
  style()->polish(this);
  repaint();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setController(VSController* controller)
{
  if(m_Controller)
  {
    disconnect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*)), this, SLOT(filterAdded(VSAbstractFilter*)));
    disconnect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(filterRemoved(VSAbstractFilter*)));
  }

  m_Controller = controller;
  connect(m_Controller, SIGNAL(filterAdded(VSAbstractFilter*, bool)), this, SLOT(filterAdded(VSAbstractFilter*, bool)));
  connect(m_Controller, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(filterRemoved(VSAbstractFilter*)));

  // Clear old filter view settings and create new ones
  clearFilters();

  QVector<VSAbstractFilter*> filters = controller->getAllFilters();
  for(VSAbstractFilter* filter : filters)
  {
    VSFilterViewSettings* viewSettings = new VSFilterViewSettings(filter);
    m_FilterViewSettings[filter] = viewSettings;

    connect(viewSettings, &VSFilterViewSettings::visibilityChanged, this, &VSAbstractViewWidget::setFilterVisibility);
    connect(viewSettings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSAbstractViewWidget::setFilterArrayName);
    connect(viewSettings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSAbstractViewWidget::setFilterComponentIndex);
    connect(viewSettings, &VSFilterViewSettings::mapColorsChanged, this, &VSAbstractViewWidget::setFilterMapColors);
    connect(viewSettings, &VSFilterViewSettings::showScalarBarChanged, this, &VSAbstractViewWidget::setFilterShowScalarBar);
    connect(viewSettings, &VSFilterViewSettings::requiresRender, this, &VSAbstractViewWidget::renderView);
    connect(viewSettings, &VSFilterViewSettings::actorsUpdated, this, &VSAbstractViewWidget::updateScene);

    // Check filter and scalar bar visibility
    checkFilterViewSetting(viewSettings);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractViewWidget::getFilterFromProp(vtkProp3D* prop)
{
  if(nullptr == prop)
  {
    return nullptr;
  }

  // Compare prop with all VSFilterViewSettings to find a match
  for(std::pair<VSAbstractFilter*, VSFilterViewSettings*> settingPair : getAllFilterViewSettings())
  {
    if(settingPair.second->getActor() == prop)
    {
      return settingPair.first;
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::selectFilter(VSAbstractFilter* filter)
{
  m_Controller->selectFilter(filter);
}
