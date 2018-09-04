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
#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"

namespace
{
QItemSelectionModel::SelectionFlags mergeFlags = QItemSelectionModel::Select;
QItemSelectionModel::SelectionFlags selectFlags = QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current;
} // namespace

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(QWidget* parent, Qt::WindowFlags windowFlags)
: QFrame(parent, windowFlags)
{
  setupModel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractViewWidget::VSAbstractViewWidget(const VSAbstractViewWidget& other)
: QFrame(nullptr)
, m_Controller(other.m_Controller)
{
  setupModel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::setupModel()
{
  m_FilterViewModel = new VSFilterViewModel(this);
  m_SelectionModel = new QItemSelectionModel(m_FilterViewModel);

  connect(m_FilterViewModel, &VSFilterViewModel::viewSettingsCreated, this, &VSAbstractViewWidget::addViewSettings);
  connect(m_FilterViewModel, &VSFilterViewModel::viewSettingsRemoved, this, &VSAbstractViewWidget::removeViewSettings);

  connect(m_SelectionModel, &QItemSelectionModel::currentChanged, this, &VSAbstractViewWidget::listenCurrentIndexChanged);
  connect(m_SelectionModel, &QItemSelectionModel::selectionChanged, this, &VSAbstractViewWidget::localSelectionChanged);
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::copyModel(const VSFilterViewModel& other)
{
  m_FilterViewModel = new VSFilterViewModel(other);
  m_SelectionModel = new QItemSelectionModel(m_FilterViewModel);

  connect(m_FilterViewModel, &VSFilterViewModel::viewSettingsCreated, this, &VSAbstractViewWidget::addViewSettings);
  connect(m_FilterViewModel, &VSFilterViewModel::viewSettingsRemoved, this, &VSAbstractViewWidget::removeViewSettings);

  connect(m_SelectionModel, &QItemSelectionModel::currentChanged, this, &VSAbstractViewWidget::listenCurrentIndexChanged);
  connect(m_SelectionModel, &QItemSelectionModel::selectionChanged, this, &VSAbstractViewWidget::localSelectionChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::addViewSettings(VSFilterViewSettings* viewSettings)
{
  connect(viewSettings, &VSFilterViewSettings::visibilityChanged, this, &VSAbstractViewWidget::setFilterVisibility);
  connect(viewSettings, &VSFilterViewSettings::gridVisibilityChanged, this, &VSAbstractViewWidget::setGridVisibility);
  connect(viewSettings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSAbstractViewWidget::setFilterArrayName);
  connect(viewSettings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSAbstractViewWidget::setFilterComponentIndex);
  connect(viewSettings, &VSFilterViewSettings::mapColorsChanged, this, &VSAbstractViewWidget::setFilterMapColors);
  connect(viewSettings, &VSFilterViewSettings::showScalarBarChanged, this, &VSAbstractViewWidget::setFilterShowScalarBar);
  connect(viewSettings, &VSFilterViewSettings::requiresRender, this, &VSAbstractViewWidget::renderView);
  connect(viewSettings, &VSFilterViewSettings::actorsUpdated, this, &VSAbstractViewWidget::updateScene);

  checkFilterViewSetting(viewSettings);

  if(dynamic_cast<VSAbstractDataFilter*>(viewSettings->getFilter()) && getVisualizationWidget())
  {
    getVisualizationWidget()->resetCamera();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::removeViewSettings(VSFilterViewSettings* viewSettings)
{
  if(viewSettings)
  {
    changeFilterVisibility(viewSettings, false);
    checkFilterViewSetting(viewSettings);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::copyFilters(const VSFilterViewModel& filterViewModel)
{
  // clearFilters();
  m_FilterViewModel->deepCopy(filterViewModel);

  std::vector<VSFilterViewSettings*> filterViewSettings = m_FilterViewModel->getAllFilterViewSettings();
  for(VSFilterViewSettings* viewSettings : filterViewSettings)
  {
    addViewSettings(viewSettings);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::copySelection(const VSAbstractViewWidget& other)
{
  getSelectionModel()->blockSignals(true);
  QModelIndexList selection = other.getSelectionModel()->selectedIndexes();
  VSAbstractFilter::FilterListType selectedFilters = other.getFilterViewModel()->getFiltersFromIndexes(selection);
  QModelIndexList indexList = getFilterViewModel()->getIndexesFromFilters(selectedFilters);
  getSelectionModel()->select(createSelection(indexList), ::selectFlags);
  getSelectionModel()->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QItemSelection VSAbstractViewWidget::createSelection(const QModelIndexList& indexList) const
{
  QItemSelection selection;
  for(QModelIndex index : indexList)
  {
    QItemSelection temp(index, index);
    selection.merge(temp, QItemSelectionModel::Select);
  }

  return selection;
}

#if 0
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
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::checkFilterViewSetting(VSFilterViewSettings* setting)
{
  if(nullptr == setting)
  {
    return;
  }

  if(getVisualizationWidget())
  {
    if(setting->getScalarBarWidget())
    {
      setting->getScalarBarWidget()->SetInteractor(getVisualizationWidget()->GetInteractor());
    }

    changeFilterVisibility(setting, setting->isVisible());
    changeScalarBarVisibility(setting, setting->isScalarBarVisible());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings* VSAbstractViewWidget::getFilterViewSettings(VSAbstractFilter* filter)
{
  if(nullptr == m_FilterViewModel)
  {
    return nullptr;
  }

  return m_FilterViewModel->getFilterViewSettings(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Collection VSAbstractViewWidget::getFilterViewSettings(VSAbstractFilter::FilterListType filters)
{
  if(nullptr == m_FilterViewModel)
  {
    return VSFilterViewSettings::Collection();
  }

  return m_FilterViewModel->getFilterViewSettings(filters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::Map VSAbstractViewWidget::getAllFilterViewSettings() const
{
  if(m_FilterViewModel)
  {
    return m_FilterViewModel->getFilterViewSettingsMap();
  }

  return VSFilterViewSettings::Map();
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

  if(nullptr == viewSettings->getScalarBarWidget() || nullptr == getVisualizationWidget())
  {
    return;
  }

  if(viewSettings->getScalarBarWidget())
  {
    if(viewSettings->isVisible())
    {
      if(viewSettings->isScalarBarVisible())
      {
        viewSettings->getScalarBarWidget()->SetInteractor(getVisualizationWidget()->GetInteractor());
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
  cloneWidget->getVisualizationWidget()->render();

  // Fill the splitter with both the view widget and a clone of it
  splitter->addWidget(this);
  splitter->addWidget(cloneWidget);
  cloneWidget->getVisualizationWidget()->render();

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

  VSVisualizationWidget* visualizationWidget = getVisualizationWidget();
  if(visualizationWidget)
  {
    visualizationWidget->render();
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

  VSVisualizationWidget* visualizationWidget = getVisualizationWidget();
  if(visualizationWidget && visualizationWidget->getRenderer())
  {
    visualizationWidget->getRenderer()->ResetCamera();
    visualizationWidget->render();
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
VSFilterViewModel* VSAbstractViewWidget::getFilterViewModel() const
{
  return m_FilterViewModel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QItemSelectionModel* VSAbstractViewWidget::getSelectionModel() const
{
  return m_SelectionModel;
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
  m_Controller = controller;

  // Set the new VSFilterModel base
  m_FilterViewModel->setFilterModel(controller->getFilterModel());

  std::vector<VSFilterViewSettings*> filterViewSettings = m_FilterViewModel->getAllFilterViewSettings();
  for(VSFilterViewSettings* viewSettings : filterViewSettings)
  {
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

  emit controllerChanged(controller);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisualizationWidget* VSAbstractViewWidget::getVisualizationWidget() const
{
  return nullptr;
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
void VSAbstractViewWidget::selectFilter(VSAbstractFilter* filter, SelectionType selectionType)
{
  QModelIndex filterIndex = m_FilterViewModel->getIndexFromFilter(filter);
  QItemSelectionModel::SelectionFlags selectionFlag;
  switch(selectionType)
  {
  case SelectionType::Current:
    selectionFlag = ::selectFlags;
    m_SelectionModel->setCurrentIndex(filterIndex, selectionFlag);
    return;
    break;
  case SelectionType::AddSelection:
    selectionFlag = ::mergeFlags;
    m_SelectionModel->setCurrentIndex(filterIndex, selectionFlag);
    break;
  case SelectionType::RemoveSelection:
    selectionFlag = QItemSelectionModel::Deselect;
    break;
  }

  m_SelectionModel->select(filterIndex, selectionFlag);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::selectFilters(VSAbstractFilter::FilterListType filters)
{
  for(VSAbstractFilter* filter : filters)
  {
    if(filter == *filters.begin())
    {
      selectFilter(filter, SelectionType::Current);
    }
    else
    {
      selectFilter(filter, SelectionType::AddSelection);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::listenCurrentIndexChanged(const QModelIndex& current, const QModelIndex& previous)
{
  emit currentFilterChanged(getCurrentFilter());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::localSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
  QItemSelection selection;
  QModelIndexList filterIndicesSelected = m_FilterViewModel->convertIndicesToFilterModel(selected.indexes());
  for(QModelIndex index : filterIndicesSelected)
  {
    QItemSelection temp(index, index);
    selection.merge(temp, ::mergeFlags);
  }

  // Selected indices
  QModelIndexList selectedIndices = selected.indexes();
  for(QModelIndex index : selectedIndices)
  {
    VSFilterViewSettings* settings = m_FilterViewModel->getFilterViewSettingsByIndex(index);
    settings->setIsSelected(true);
  }

  // Deselected indices
  QModelIndexList deselectedIndices = deselected.indexes();
  for(QModelIndex index : deselectedIndices)
  {
    VSFilterViewSettings* settings = m_FilterViewModel->getFilterViewSettingsByIndex(index);
    settings->setIsSelected(false);
  }

  emit selectionChanged(selection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSAbstractViewWidget::getSelectedFilters() const
{
  QModelIndexList selection = m_SelectionModel->selectedIndexes();
  return m_FilterViewModel->getFiltersFromIndexes(selection);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractViewWidget::getCurrentFilter() const
{
#if 1
  QModelIndex currentIndex = m_SelectionModel->currentIndex();
  return m_FilterViewModel->getFilterFromIndex(currentIndex);
#else
  VSAbstractFilter::FilterListType selectedFilters = getSelectedFilters();
  if(selectedFilters.size() > 0)
  {
    return selectedFilters.front();
  }

  return nullptr;
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::changeFilterSelected(FilterStepChange stepDirection, bool addSelection)
{
  switch(stepDirection)
  {
  case FilterStepChange::Parent:
    selectFilterParent(addSelection);
    break;
  case FilterStepChange::Child:
    selectFilterChild(addSelection);
    break;
  case FilterStepChange::PrevSibling:
    selectFilterPrevSibling(addSelection);
    break;
  case FilterStepChange::NextSibling:
    selectFilterNextSibling(addSelection);
    break;
  default:
    break;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::selectFilterParent(bool addSelection)
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  SelectionType selectionType = addSelection ? SelectionType::AddSelection : SelectionType::Current;
  if(nullptr == currentFilter)
  {
    // Select the first base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterViewModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.front(), selectionType);
    }
  }
  else if(currentFilter->getParentFilter() != m_FilterViewModel->getRootFilter())
  {
    // Select the parent filter
    selectFilter(currentFilter->getParentFilter(), selectionType);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::selectFilterChild(bool addSelection)
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  SelectionType selectionType = addSelection ? SelectionType::AddSelection : SelectionType::Current;
  if(nullptr == currentFilter)
  {
    // Select the last base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterViewModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.back(), selectionType);
    }
  }
  else if(currentFilter->getChildCount() > 0)
  {
    // Select the first child filter
    selectFilter(currentFilter->getChild(0), selectionType);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::selectFilterPrevSibling(bool addSelection)
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  SelectionType selectionType = addSelection ? SelectionType::AddSelection : SelectionType::Current;
  if(nullptr == currentFilter)
  {
    // Select the first base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterViewModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.front(), selectionType);
    }
  }
  else if(currentFilter->getPrevSibling())
  {
    // Select the previous sibling filter
    selectFilter(currentFilter->getPrevSibling(), selectionType);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractViewWidget::selectFilterNextSibling(bool addSelection)
{
  VSAbstractFilter* currentFilter = getCurrentFilter();
  SelectionType selectionType = addSelection ? SelectionType::AddSelection : SelectionType::Current;
  if(nullptr == currentFilter)
  {
    // Select the last base filter if no selection exists
    VSAbstractFilter::FilterListType baseFilters = m_FilterViewModel->getBaseFilters();
    if(baseFilters.size() > 0)
    {
      selectFilter(baseFilters.back(), selectionType);
    }
  }
  else if(currentFilter->getNextSibling())
  {
    // Select the next sibling filter
    selectFilter(currentFilter->getNextSibling(), selectionType);
  }
}
