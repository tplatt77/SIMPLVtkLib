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

#include "VSMainWidget2.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTextFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMainWidget2::VSMainWidget2(QWidget* parent)
: VSMainWidgetBase(parent)
, m_Ui(new Ui::VSMainWidget2)
, m_VisualizationFiltersUi(new Ui::VisualizationFilterWidgets)
{
  m_Ui->setupUi(this);

  VSAbstractViewWidget* viewWidget = m_Ui->viewWidget;
  viewWidget->setController(getController());
  connectViewWidget(viewWidget);
  setActiveView(viewWidget);

  createFilterMenu();
  connectSlots();
  setCurrentFilter(nullptr);

  // Create pop-up widgets
  m_VisualizationFiltersPopup = new PopUpWidget();
  m_VisualizationSettingsPopup = new PopUpWidget();
  m_ColorMappingPopup = new PopUpWidget();
  m_AdvVisualizationSettingsPopup = new PopUpWidget();
  m_VisualizationTransformPopup = new PopUpWidget();

  // Create Visualization Widgets
  QWidget* visualizationFilters = new QWidget();
  m_VisualizationFiltersUi->setupUi(visualizationFilters);
  VSVisibilitySettingsWidget* visibilitySettingsWidget = new VSVisibilitySettingsWidget();
  VSColorMappingWidget* colorMappingWidget = new VSColorMappingWidget();
  VSAdvancedVisibilitySettingsWidget* advVisibilityWidget = new VSAdvancedVisibilitySettingsWidget();
  VSTransformWidget* transformWidget = new VSTransformWidget();

  // Set pop-up widgets
  m_VisualizationFiltersPopup->setWidget(visualizationFilters);
  m_VisualizationSettingsPopup->setWidget(visibilitySettingsWidget);
  m_ColorMappingPopup->setWidget(colorMappingWidget);
  m_AdvVisualizationSettingsPopup->setWidget(advVisibilityWidget);
  m_VisualizationTransformPopup->setWidget(transformWidget);

  setFilterView(m_VisualizationFiltersUi->visualFilterView);
  setFilterSettingsWidget(m_VisualizationFiltersUi->vsFilterWidget);
  setVisibilitySettingsWidget(visibilitySettingsWidget);
  setColorMappingWidget(colorMappingWidget);
  setAdvancedVisibilityWidget(advVisibilityWidget);
  setTransformWidget(transformWidget);

  connect(m_Ui->visualizationFiltersBtn, &QPushButton::clicked, this, &VSMainWidget2::showVisualizationFilters);
  connect(m_Ui->visualizationSettingsBtn, &QPushButton::clicked, this, &VSMainWidget2::showVisibilitySettings);
  connect(m_Ui->colorMappingBtn, &QPushButton::clicked, this, &VSMainWidget2::showColorMapping);
  connect(m_Ui->advVisualizationSettingsBtn, &QPushButton::clicked, this, &VSMainWidget2::showAdvVisibilitySettings);
  connect(m_Ui->transformBtn, &QPushButton::clicked, this, &VSMainWidget2::showVisualTransform);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::connectSlots()
{
  // Filter Slots
  connect(m_Ui->clipBtn, SIGNAL(clicked()), m_ActionAddClip, SLOT(trigger()));
  connect(m_Ui->sliceBtn, SIGNAL(clicked()), m_ActionAddSlice, SLOT(trigger()));
  connect(m_Ui->thresholdBtn, SIGNAL(clicked()), m_ActionAddThreshold, SLOT(trigger()));

  // Camera Slots
  connect(m_Ui->cameraXpBtn, SIGNAL(clicked()), this, SLOT(activeCameraXPlus()));
  connect(m_Ui->cameraYpBtn, SIGNAL(clicked()), this, SLOT(activeCameraYPlus()));
  connect(m_Ui->cameraZpBtn, SIGNAL(clicked()), this, SLOT(activeCameraZPlus()));
  connect(m_Ui->cameraXmBtn, SIGNAL(clicked()), this, SLOT(activeCameraXMinus()));
  connect(m_Ui->cameraYmBtn, SIGNAL(clicked()), this, SLOT(activeCameraYMinus()));
  connect(m_Ui->cameraZmBtn, SIGNAL(clicked()), this, SLOT(activeCameraZMinus()));

  connect(getController(), &VSController::filterAdded, this, [=] { renderAll(); });
  connect(getController(), &VSController::dataImported, this, [=] { resetCamera(); });
  connect(getController(), SIGNAL(applyingDataFilters(int)), this, SLOT(importNumFilters(int)));
  connect(getController(), SIGNAL(dataFilterApplied(int)), this, SLOT(importedFilterNum(int)));

  connect(this, &VSMainWidget2::selectedFiltersChanged, this, &VSMainWidget2::listenFiltersChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::renderAll()
{
  QVector<VSAbstractViewWidget*> viewWidgets = getAllViewWidgets();
  for(auto iter = viewWidgets.begin(); iter != viewWidgets.end(); iter++)
  {
    VSVisualizationWidget* visualizationWidget = (*iter)->getVisualizationWidget();
    if(visualizationWidget)
    {
      // Render the VTK widget
      visualizationWidget->render();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::resetCamera()
{
  QVector<VSAbstractViewWidget*> viewWidgets = getAllViewWidgets();
  for(VSAbstractViewWidget* viewWidget : viewWidgets)
  {
    viewWidget->resetCamera();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::setActiveView(VSAbstractViewWidget* viewWidget)
{
  VSMainWidgetBase::setActiveView(viewWidget);

  if(getActiveViewWidget() != nullptr)
  {
    m_Ui->cameraXmBtn->setEnabled(true);
    m_Ui->cameraXpBtn->setEnabled(true);
    m_Ui->cameraYmBtn->setEnabled(true);
    m_Ui->cameraYpBtn->setEnabled(true);
    m_Ui->cameraZmBtn->setEnabled(true);
    m_Ui->cameraZpBtn->setEnabled(true);

    setVisualizationSettings(viewWidget->getFilterViewSettings(getCurrentSelection()));
  }
  else
  {
    m_Ui->cameraXmBtn->setDisabled(true);
    m_Ui->cameraXpBtn->setDisabled(true);
    m_Ui->cameraYmBtn->setDisabled(true);
    m_Ui->cameraYpBtn->setDisabled(true);
    m_Ui->cameraZmBtn->setDisabled(true);
    m_Ui->cameraZpBtn->setDisabled(true);

    setVisualizationSettings(VSFilterViewSettings::Collection());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::setCurrentFilter(VSAbstractFilter* filter)
{
  VSMainWidgetBase::setCurrentFilter(filter);

  // Check if each Filter Type can be added
  // Clip Filter
  bool enableClip = VSClipFilter::CompatibleWithParent(filter);
  m_Ui->clipBtn->setEnabled(enableClip);
  m_ActionAddClip->setEnabled(enableClip);

  // Slice
  bool enableSlice = VSSliceFilter::CompatibleWithParent(filter);
  m_Ui->sliceBtn->setEnabled(enableSlice);
  m_ActionAddSlice->setEnabled(enableSlice);

  // Crop Filter
  bool enableCrop = VSCropFilter::CompatibleWithParent(filter);
  m_ActionAddCrop->setEnabled(enableCrop);

  // Mask
  bool enableMask = VSMaskFilter::CompatibleWithParent(filter);
  m_ActionAddMask->setEnabled(enableMask);

  // Threshold
  bool enableThreshold = VSThresholdFilter::CompatibleWithParent(filter);
  m_Ui->thresholdBtn->setEnabled(enableThreshold);
  m_ActionAddThreshold->setEnabled(enableThreshold);

  // Text
  bool enableText = VSTextFilter::CompatibleWithParent(filter);
  m_ActionAddText->setEnabled(enableText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::listenFiltersChanged(VSAbstractFilter::FilterListType filtersSelected)
{
  if(filtersSelected.size() == 0)
  {
    setVisualizationSettings(VSFilterViewSettings::Collection());
  }
  else
  {
    VSFilterViewSettings::Collection viewSettings = getActiveViewWidget()->getFilterViewSettings(filtersSelected);
    setVisualizationSettings(viewSettings);
  }


  // Clip Filter
  bool enableClip = VSClipFilter::CompatibleWithParents(filtersSelected);
  m_Ui->clipBtn->setEnabled(enableClip);
  m_ActionAddClip->setEnabled(enableClip);

  // Slice
  bool enableSlice = VSSliceFilter::CompatibleWithParents(filtersSelected);
  m_Ui->sliceBtn->setEnabled(enableSlice);
  m_ActionAddSlice->setEnabled(enableSlice);

  // Crop Filter
  bool enableCrop = VSCropFilter::CompatibleWithParents(filtersSelected);
  m_ActionAddCrop->setEnabled(enableCrop);

  // Mask
  bool enableMask = VSMaskFilter::CompatibleWithParents(filtersSelected);
  m_ActionAddMask->setEnabled(enableMask);

  // Threshold
  bool enableThreshold = VSThresholdFilter::CompatibleWithParents(filtersSelected);
  m_Ui->thresholdBtn->setEnabled(enableThreshold);
  m_ActionAddThreshold->setEnabled(enableThreshold);

  // Text
  bool enableText = VSTextFilter::CompatibleWithParents(filtersSelected);
  m_ActionAddText->setEnabled(enableText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::updateFilterLabel()
{
  if(m_VisualizationViewSettings.size() == 0)
  {
    m_Ui->filterLabel->setText("No Filter Selected");
    return;
  }

  QString filterText;
  if(m_VisualizationViewSettings.size() > 1)
  {
    filterText = "Multiple Filters Selected";
  }

  // VSFilterViewSettings found
  if(m_VisualizationViewSettings.size() == 1)
  {
    VSAbstractFilter* filter = m_VisualizationViewSettings.front()->getFilter();
    filterText = filter->getFilterName();
    switch(filter->getFilterType())
    {
    case VSAbstractFilter::FilterType::File:
      filterText += " File";
      break;
    case VSAbstractFilter::FilterType::Filter:
      filterText += " Filter";
      break;
    case VSAbstractFilter::FilterType::Pipeline:
      filterText += " Pipeline";
      break;
    default:
      break;
    }
  }
  
  // Check array and component names
  QString arrayComponentText;
  if(VSFilterViewSettings::HasValidSettings(m_VisualizationViewSettings))
  {
    arrayComponentText = VSFilterViewSettings::GetActiveComponentName(m_VisualizationViewSettings);
  }

  if(arrayComponentText.isEmpty())
  {
    m_Ui->filterLabel->setText(filterText);
  }
  else
  {
    m_Ui->filterLabel->setText(filterText + " | " + arrayComponentText);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::setVisualizationSettings(VSFilterViewSettings::Collection viewSettings)
{
  for(VSFilterViewSettings* settings : m_VisualizationViewSettings)
  {
    disconnect(settings, &VSFilterViewSettings::visibilityChanged, this, &VSMainWidget2::vsVisibilityChanged);
    disconnect(settings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSMainWidget2::vsArrayChanged);
    disconnect(settings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSMainWidget2::vsArrayChanged);
  }

  if(viewSettings.size() > 1)
  {
    m_Ui->filterLabel->setText("Multiple Filters Selected");
  }
  else
  {
    QString filterText = "No Filter Selected";

    // If only one filter is selected, check the filter type
    if(viewSettings.size() == 1)
    {
      filterText = viewSettings.front()->getFilterName();
      switch(viewSettings.front()->getFilter()->getFilterType())
      {
      case VSAbstractFilter::FilterType::File:
        filterText += " File";
        break;
      case VSAbstractFilter::FilterType::Filter:
        filterText += " Filter";
        break;
      case VSAbstractFilter::FilterType::Pipeline:
        filterText += " Pipeline";
        break;
      default:
        break;
      }
    }

    m_Ui->filterLabel->setText(filterText);
  }

  m_VisualizationViewSettings = viewSettings;
  
  for(VSFilterViewSettings* settings : m_VisualizationViewSettings)
  {
    connect(settings, &VSFilterViewSettings::visibilityChanged, this, &VSMainWidget2::vsVisibilityChanged);
    connect(settings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSMainWidget2::vsArrayChanged);
    connect(settings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSMainWidget2::vsArrayChanged);
  }

  bool validSettings = VSFilterViewSettings::HasValidSettings(viewSettings);
  m_Ui->visualizationSettingsBtn->setEnabled(validSettings);
  m_Ui->colorMappingBtn->setEnabled(validSettings);
  m_Ui->advVisualizationSettingsBtn->setEnabled(validSettings);
  m_Ui->transformBtn->setEnabled(viewSettings.size() > 0);

  vsVisibilityChanged();
  vsArrayChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::vsVisibilityChanged()
{
  if(m_VisualizationViewSettings.size() > 0)
  {
    bool settingsValid = VSFilterViewSettings::HasValidSettings(m_VisualizationViewSettings);
    m_Ui->visualizationSettingsBtn->setEnabled(settingsValid);
    m_Ui->colorMappingBtn->setEnabled(settingsValid);
    m_Ui->advVisualizationSettingsBtn->setEnabled(settingsValid);
  }
  else
  {
    m_Ui->visualizationSettingsBtn->setEnabled(false);
    m_Ui->colorMappingBtn->setEnabled(false);
    m_Ui->advVisualizationSettingsBtn->setEnabled(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::vsArrayChanged()
{
  updateFilterLabel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::showPopup(PopUpWidget* popup, QPushButton* button)
{
  QPoint position = button->mapToGlobal(QPoint(button->width() * 0.5, 0));
  int minimumWidth = 150;

  int padding = 25;
  QSize minimumSizeHint = popup->getWidget()->sizeHint();
  if(minimumSizeHint.width() < minimumWidth)
  {
    minimumSizeHint.setWidth(minimumWidth);
  }
  else if(minimumSizeHint.width() > popup->minimumSizeHint().width())
  {
    minimumSizeHint.setWidth(minimumSizeHint.width() + padding);
  }
  if(minimumSizeHint.height() > popup->minimumSizeHint().height())
  {
    minimumSizeHint.setHeight(minimumSizeHint.height() + padding);
  }
  else
  {
    minimumSizeHint.setHeight(popup->minimumSizeHint().height());
  }
  popup->resize(minimumSizeHint);

  // Try to open the popup widget within the width of this widget
  int xPos = position.x() - 0.5 * minimumSizeHint.width();
  int yPos = position.y() - minimumSizeHint.height();
  if(xPos < mapToGlobal(QPoint(0,0)).x())
  {
    xPos = mapToGlobal(QPoint(0,0)).x();
  }
  else if(xPos + minimumSizeHint.width() > mapToGlobal(QPoint(width(), 0)).x())
  {
    xPos = mapToGlobal(QPoint(width(), 0)).x() - minimumSizeHint.width();
  }

  QPoint targetPosition(xPos, yPos);
  popup->move(targetPosition);
  popup->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::showVisualizationFilters()
{
  showPopup(m_VisualizationFiltersPopup, m_Ui->visualizationFiltersBtn);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::showVisibilitySettings()
{
  showPopup(m_VisualizationSettingsPopup, m_Ui->visualizationSettingsBtn);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::showColorMapping()
{
  showPopup(m_ColorMappingPopup, m_Ui->colorMappingBtn);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::showAdvVisibilitySettings()
{
  showPopup(m_AdvVisualizationSettingsPopup, m_Ui->advVisualizationSettingsBtn);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::showVisualTransform()
{
  showPopup(m_VisualizationTransformPopup, m_Ui->transformBtn);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::activeCameraXPlus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camXPlus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::activeCameraYPlus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camYPlus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::activeCameraZPlus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camZPlus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::activeCameraXMinus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camXMinus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::activeCameraYMinus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camYMinus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::activeCameraZMinus()
{
  VSAbstractViewWidget* activeView = getActiveViewWidget();

  if(activeView && activeView->getVisualizationWidget())
  {
    activeView->getVisualizationWidget()->camZMinus();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMenu* VSMainWidget2::getFilterMenu()
{
  return m_FilterMenu;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::createFilterMenu()
{
  // Create Actions
  m_ActionAddClip = new QAction("Clip Filter");
  connect(m_ActionAddClip, SIGNAL(triggered()), this, SLOT(createClipFilter()));

  m_ActionAddCrop = new QAction("Crop Filter");
  connect(m_ActionAddCrop, SIGNAL(triggered()), this, SLOT(createCropFilter()));

  m_ActionAddSlice = new QAction("Slice Filter");
  connect(m_ActionAddSlice, SIGNAL(triggered()), this, SLOT(createSliceFilter()));

  m_ActionAddThreshold = new QAction("Threshold Filter");
  connect(m_ActionAddThreshold, SIGNAL(triggered()), this, SLOT(createThresholdFilter()));

  m_ActionAddMask = new QAction("Mask Filter");
  connect(m_ActionAddMask, SIGNAL(triggered()), this, SLOT(createMaskFilter()));

  m_ActionAddText = new QAction("Text Filter");
  connect(m_ActionAddText, SIGNAL(triggered()), this, SLOT(createTextFilter()));

  // Create Menu
  m_FilterMenu = new QMenu("Filters", this);
  m_FilterMenu->addAction(m_ActionAddClip);
  m_FilterMenu->addAction(m_ActionAddSlice);
  m_FilterMenu->addAction(m_ActionAddCrop);
  m_FilterMenu->addAction(m_ActionAddThreshold);
  m_FilterMenu->addAction(m_ActionAddMask);

  m_FilterMenu->addSeparator();

  m_FilterMenu->addAction(m_ActionAddText);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::importNumFilters(int max)
{
  if(max == 0)
  {
    m_Ui->progressBar->setMaximum(1);
  }
  else
  {
    m_Ui->progressBar->setMaximum(max);
  }

  m_Ui->progressBar->reset();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMainWidget2::importedFilterNum(int value)
{
  int maxValue = m_Ui->progressBar->maximum();
  if(value == maxValue)
  {
    m_Ui->progressBar->setValue(0);
  }
  else
  {
    m_Ui->progressBar->setValue(value);
  }
}
