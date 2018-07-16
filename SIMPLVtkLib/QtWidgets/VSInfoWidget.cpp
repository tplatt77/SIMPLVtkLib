/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "VSInfoWidget.h"

#include <QtGui/QIntValidator>

#include "SIMPLVtkLib/QtWidgets/VSMainWidget.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"

#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSClipFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSCropFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSMaskFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSSIMPLDataContainerFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSSliceFilterWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSThresholdFilterWidget.h"

#include "ui_VSInfoWidget.h"

namespace
{
QString SolidColorStr = "Solid Color";
}

class VSInfoWidget::VSInternals : public Ui::VSInfoWidget
{
public:
  VSInternals()
  {
  }
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInfoWidget::VSInfoWidget(QWidget* parent)
: QWidget(parent)
, m_Internals(new VSInternals())
{
  m_Internals->setupUi(this);
  setupGui();
  setFilter(nullptr, nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setupGui()
{
  m_presetsDialog = new ColorPresetsDialog();

  QIntValidator* pointSizeValidator = new QIntValidator(this);
  pointSizeValidator->setBottom(1);
  m_Internals->pointSizeEdit->setValidator(pointSizeValidator);

  connect(m_Internals->representationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setRepresentationIndex(int)));
  connect(m_Internals->activeArrayCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(updateActiveArrayName(const QString&)));
  connect(m_Internals->activeComponentCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateActiveComponentIndex(int)));
  connect(m_Internals->pointSizeEdit, &QLineEdit::textChanged, this, &VSInfoWidget::updatePointSize);
  connect(m_Internals->pointSphereCheckBox, &QCheckBox::stateChanged, this, &VSInfoWidget::updateRenderPointSpheres);
  connect(m_Internals->mapScalarsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setScalarsMapped(int)));
  connect(m_Internals->showScalarBarCheckBox, &QCheckBox::stateChanged, this, &VSInfoWidget::setScalarBarVisible);
  connect(m_Internals->invertColorScaleBtn, &QPushButton::clicked, this, &VSInfoWidget::invertScalarBar);
  connect(m_Internals->alphaSlider, &QSlider::valueChanged, this, &VSInfoWidget::alphaSliderMoved);
  connect(m_Internals->selectPresetColorsBtn, &QPushButton::clicked, this, &VSInfoWidget::selectPresetColors);
  connect(m_presetsDialog, &ColorPresetsDialog::applyPreset, this, &VSInfoWidget::loadPresetColors);
  connect(m_Internals->viewAxesGridCheckBox, &QCheckBox::stateChanged, this, &VSInfoWidget::setAxesGridVisible);

  connect(m_Internals->applyBtn, &QPushButton::clicked, this, &VSInfoWidget::applyFilter);
  connect(m_Internals->resetBtn, &QPushButton::clicked, this, &VSInfoWidget::resetFilter);
  connect(m_Internals->deleteBtn, &QPushButton::clicked, this, &VSInfoWidget::deleteFilter);

  connect(m_Internals->colorBtn, &VSColorButton::changedColor, this, &VSInfoWidget::colorButtonChanged);

  m_Internals->applyBtn->setDisabled(true);
  m_Internals->resetBtn->setDisabled(true);
  m_Internals->deleteBtn->setDisabled(true);

  m_Internals->colorBtn->hide();
  m_Internals->colorLabel->hide();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::applyFilter()
{
  if(nullptr == m_Filter)
  {
    return;
  }

  m_FilterWidget->apply();

  bool hasChanges = m_FilterWidget->hasChanges();
  m_Internals->applyBtn->setEnabled(hasChanges);
  m_Internals->resetBtn->setEnabled(hasChanges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::resetFilter()
{
  if(nullptr == m_Filter)
  {
    return;
  }

  m_FilterWidget->reset();

  bool hasChanges = m_FilterWidget->hasChanges();
  m_Internals->applyBtn->setEnabled(hasChanges);
  m_Internals->resetBtn->setEnabled(hasChanges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::deleteFilter()
{
  if(nullptr == m_Filter)
  {
    return;
  }

  emit filterDeleted(m_Filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget)
{
  if(m_FilterWidget != nullptr)
  {
    m_Internals->filterWidgetLayout->removeWidget(m_FilterWidget);
    m_FilterWidget->hide();
    m_FilterWidget = nullptr;

    disconnect(m_FilterWidget, SIGNAL(changesMade()), this, SLOT(changesWaiting()));
  }

  m_Filter = filter;
  m_FilterWidget = filterWidget;

  bool filterExists = (nullptr != filter);
  if(filterExists && m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filter));
  }
  else
  {
    connectFilterViewSettings(nullptr);
  }

  if(filterExists)
  {
    m_Internals->transformWidget->setTransform(filter->getTransform());
  }
  else
  {
    m_Internals->transformWidget->setTransform(nullptr);
  }

  // Check if VSFilterSettings exist and are valid
  VSFilterViewSettings::ActorType actorType = VSFilterViewSettings::ActorType::Invalid;
  if(m_ViewSettings && m_ViewSettings->isValid())
  {
    listenSolidColor(m_ViewSettings->getSolidColor());
    actorType = m_ViewSettings->getActorType();
  }

  if(m_FilterWidget != nullptr)
  {
    m_Internals->filterWidgetLayout->addWidget(m_FilterWidget);
    m_FilterWidget->show();

    connect(m_FilterWidget, SIGNAL(changesMade()), this, SLOT(changesWaiting()));
    bool hasChanges = m_FilterWidget->hasChanges();
    m_Internals->applyBtn->setEnabled(hasChanges);
    m_Internals->resetBtn->setEnabled(hasChanges);
  }
  else
  {
    m_Internals->applyBtn->setEnabled(false);
    m_Internals->resetBtn->setEnabled(false);
  }

  switch(actorType)
  {
  case VSFilterViewSettings::ActorType::DataSet:
    m_Internals->viewSettingsWidget->setVisible(true);
    m_Internals->arrayVisibilityWidget->setVisible(true);
    m_Internals->mappingWidget->setVisible(true);
    break;
  case VSFilterViewSettings::ActorType::Image2D:
    m_Internals->viewSettingsWidget->setVisible(true);
    m_Internals->arrayVisibilityWidget->setVisible(false);
    m_Internals->mappingWidget->setVisible(false);
    break;
  case VSFilterViewSettings::ActorType::Invalid:
  default:
    m_Internals->viewSettingsWidget->setVisible(false);
    m_Internals->mappingWidget->setVisible(false);
    break;
  }

  m_Internals->deleteBtn->setEnabled(filterExists);

  updateFilterInfo();
  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::connectFilterViewSettings(VSFilterViewSettings* settings)
{
  if(m_ViewSettings)
  {
    disconnect(m_ViewSettings, &VSFilterViewSettings::representationChanged, this, &VSInfoWidget::listenRepresentationType);
    disconnect(m_ViewSettings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSInfoWidget::listenArrayName);
    disconnect(m_ViewSettings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSInfoWidget::listenComponentIndex);
    disconnect(m_ViewSettings, &VSFilterViewSettings::pointSizeChanged, this, &VSInfoWidget::listenPointSize);
    disconnect(m_ViewSettings, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSInfoWidget::listenPointSphere);
    disconnect(m_ViewSettings, &VSFilterViewSettings::mapColorsChanged, this, &VSInfoWidget::listenMapColors);
    disconnect(m_ViewSettings, &VSFilterViewSettings::alphaChanged, this, &VSInfoWidget::listenAlpha);
    disconnect(m_ViewSettings, &VSFilterViewSettings::showScalarBarChanged, this, &VSInfoWidget::listenScalarBar);
    disconnect(m_ViewSettings, &VSFilterViewSettings::solidColorChanged, this, &VSInfoWidget::listenSolidColor);
    disconnect(m_ViewSettings, &VSFilterViewSettings::gridVisibilityChanged, this, &VSInfoWidget::listenAxesGridVisible);
    disconnect(m_ViewSettings, &VSFilterViewSettings::dataLoaded, this, &VSInfoWidget::updateFilterInfo);
  }

  m_ViewSettings = settings;

  if(m_ViewSettings)
  {
    connect(settings, &VSFilterViewSettings::representationChanged, this, &VSInfoWidget::listenRepresentationType);
    connect(settings, &VSFilterViewSettings::activeArrayNameChanged, this, &VSInfoWidget::listenArrayName);
    connect(settings, &VSFilterViewSettings::activeComponentIndexChanged, this, &VSInfoWidget::listenComponentIndex);
    connect(settings, &VSFilterViewSettings::pointSizeChanged, this, &VSInfoWidget::listenPointSize);
    connect(settings, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSInfoWidget::listenPointSphere);
    connect(settings, &VSFilterViewSettings::mapColorsChanged, this, &VSInfoWidget::listenMapColors);
    connect(settings, &VSFilterViewSettings::alphaChanged, this, &VSInfoWidget::listenAlpha);
    connect(settings, &VSFilterViewSettings::showScalarBarChanged, this, &VSInfoWidget::listenScalarBar);
    connect(settings, &VSFilterViewSettings::solidColorChanged, this, &VSInfoWidget::listenSolidColor);
    connect(settings, &VSFilterViewSettings::gridVisibilityChanged, this, &VSInfoWidget::listenAxesGridVisible);
    connect(settings, &VSFilterViewSettings::dataLoaded, this, &VSInfoWidget::updateFilterInfo);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  m_ViewWidget = viewWidget;

  if(m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filter));
  }
  else
  {
    connectFilterViewSettings(nullptr);
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateFilterInfo()
{
  // Add and set the array / component combo box values
  m_Internals->activeArrayCombo->blockSignals(true);
  m_Internals->activeComponentCombo->blockSignals(true);
  m_Internals->activeArrayCombo->clear();

  if(m_Filter)
  {
    m_Internals->activeArrayCombo->addItem(m_ViewSettings->getSolidColorIcon(), ::SolidColorStr);

    QStringList arrayNames = m_Filter->getArrayNames();
    QIcon arrayIcon = m_Filter->isPointData() ? m_ViewSettings->getPointDataIcon() : m_ViewSettings->getCellDataIcon();
    for(QString arrayName : arrayNames)
    {
      m_Internals->activeArrayCombo->addItem(arrayIcon, arrayName);
    }

    if(m_ViewSettings)
    {
      QString activeArrayName = m_ViewSettings->getActiveArrayName();
      int activeCompIndex = m_ViewSettings->getActiveComponentIndex();
      setComboArrayName(activeArrayName);
      m_Internals->activeComponentCombo->setCurrentIndex(activeCompIndex + 1);

      m_Internals->pointSizeEdit->blockSignals(true);
      m_Internals->pointSphereCheckBox->blockSignals(true);

      int pointSize = m_ViewSettings->getPointSize();
      m_Internals->pointSizeEdit->setText(QString::number(pointSize));

      bool renderPointSpheres = m_ViewSettings->renderPointsAsSpheres();
      m_Internals->pointSphereCheckBox->setChecked(renderPointSpheres ? Qt::Checked : Qt::Unchecked);

      m_Internals->pointSizeEdit->blockSignals(false);
      m_Internals->pointSphereCheckBox->blockSignals(false);
    }
    else
    {
      m_Internals->activeArrayCombo->setCurrentIndex(-1);
    }
  }

  m_Internals->activeArrayCombo->blockSignals(false);
  m_Internals->activeComponentCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(nullptr == m_ViewSettings)
  {
    m_Internals->activeArrayCombo->setCurrentIndex(-1);
    m_Internals->activeComponentCombo->setCurrentIndex(-1);

    m_Internals->showScalarBarCheckBox->setChecked(Qt::Unchecked);
    m_Internals->mapScalarsComboBox->setCurrentIndex(static_cast<int>(VSFilterViewSettings::ColorMapping::None));

    m_Internals->viewSettingsContainer->setEnabled(m_Filter);
    m_Internals->viewSettingsWidget->setEnabled(false);

    updatePointSettingVisibility();
    return;
  }

  // Apply the current filter view settings to the widget
  bool validSettings = m_ViewSettings && m_ViewSettings->isValid();
  m_Internals->viewSettingsContainer->setEnabled(m_Filter);
  m_Internals->viewSettingsWidget->setEnabled(validSettings);

  // Representation
  m_Internals->representationCombo->setCurrentIndex(m_ViewSettings->getRepresentationi());

  QString activeArrayName = m_ViewSettings->getActiveArrayName();
  int activeComponentIndex = m_ViewSettings->getActiveComponentIndex() + 1;

  // Array
  setComboArrayName(activeArrayName);
  updateActiveArrayName(activeArrayName);

  // Components
  int numComponents = m_ViewSettings->getNumberOfComponents(activeArrayName);
  if(numComponents > 1)
  {
    m_Internals->activeComponentCombo->setCurrentIndex(activeComponentIndex);
  }

  // Point Size
  m_Internals->pointSizeEdit->setText(QString::number(m_ViewSettings->getPointSize()));
  bool renderingPointSpheres = m_ViewSettings->renderPointsAsSpheres();
  Qt::CheckState pointSphereCheckState = renderingPointSpheres ? Qt::Checked : Qt::Unchecked;
  m_Internals->pointSphereCheckBox->setCheckState(pointSphereCheckState);

  m_Internals->showScalarBarCheckBox->setChecked(m_ViewSettings->isScalarBarVisible() ? Qt::Checked : Qt::Unchecked);
  m_Internals->mapScalarsComboBox->setCurrentIndex(static_cast<int>(m_ViewSettings->getMapColors()));
  m_Internals->alphaSlider->setValue(m_ViewSettings->getAlpha() * 100);

  if(m_ViewSettings->isValid())
  {
    double* solidColor = m_ViewSettings->getSolidColor();
    QColor newColor = QColor::fromRgbF(solidColor[0], solidColor[1], solidColor[2]);
    m_Internals->colorBtn->setColor(newColor, false);
  }

  updatePointSettingVisibility();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updatePointSettingVisibility()
{
  bool visible = false;

  if(m_ViewSettings)
  {
    visible = m_ViewSettings->isRenderingPoints();
  }

  m_Internals->pointRenderingWidget->setVisible(visible);
  
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setRepresentationIndex(int index)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  VSFilterViewSettings::Representation rep = static_cast<VSFilterViewSettings::Representation>(index);
  m_ViewSettings->setRepresentation(rep);

  updatePointSettingVisibility();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateActiveArrayName(QString name)
{
  if(::SolidColorStr == name && m_Internals->activeArrayCombo->currentIndex() == 0)
  {
    name = QString::null;
  }

  bool isColor = name.isNull();
  m_Internals->colorBtn->setVisible(isColor);
  m_Internals->colorLabel->setVisible(isColor);
  m_Internals->componentLabel->setVisible(!isColor);
  m_Internals->activeComponentCombo->setVisible(!isColor);

  int componentIndex = 0;
  if(m_ViewSettings && m_ViewSettings->getActiveArrayName() == name)
  {
    componentIndex = m_ViewSettings->getActiveComponentIndex();
  }

  // Set the active component combo box values
  m_Internals->activeComponentCombo->clear();

  QStringList componentList = m_Filter->getComponentList(name);
  bool multiComponents = componentList.size() > 1;
  m_Internals->activeComponentCombo->setEnabled(multiComponents);

  if(multiComponents)
  {
    m_Internals->activeComponentCombo->setEnabled(true);
    m_Internals->activeComponentCombo->addItems(componentList);
    m_Internals->activeComponentCombo->setCurrentIndex(componentIndex);
  }
  else
  {
    m_Internals->activeComponentCombo->setEnabled(false);
  }

  if(m_ViewSettings)
  {
    m_ViewSettings->setActiveArrayName(name);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateActiveComponentIndex(int index)
{
  if(m_ViewSettings)
  {
    if(m_Internals->activeComponentCombo->maxCount() > 1)
    {
      index--;
    }

    m_ViewSettings->setActiveComponentIndex(index);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updatePointSize(QString pointSize)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  if(!pointSize.isEmpty())
  {
    m_ViewSettings->setPointSize(pointSize.toInt());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::updateRenderPointSpheres(int checkState)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->setRenderPointsAsSpheres(Qt::Checked == checkState);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setScalarsMapped(int index)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->setMapColors(static_cast<VSFilterViewSettings::ColorMapping>(index));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setScalarBarVisible(int checkState)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  bool checked = checkState == Qt::Checked;
  m_ViewSettings->setScalarBarVisible(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::selectPresetColors()
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_presetsDialog->show();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::loadPresetColors(const QJsonObject& preset, const QPixmap& pixmap)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->loadPresetColors(preset);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::invertScalarBar()
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->invertScalarBar();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::alphaSliderMoved(int value)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->setAlpha(value / 100.0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::colorButtonChanged(QColor color)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  double colorArray[3];
  colorArray[0] = color.redF();
  colorArray[1] = color.greenF();
  colorArray[2] = color.blueF();

  m_ViewSettings->setSolidColor(colorArray);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setAxesGridVisible(int checkState)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  bool gridVisible = (checkState == Qt::Checked);
  m_ViewSettings->setGridVisible(gridVisible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setComboArrayName(QString arrayName)
{
  if(arrayName.isNull())
  {
    m_Internals->activeArrayCombo->setCurrentText(::SolidColorStr);
  }
  else
  {
    m_Internals->activeArrayCombo->setCurrentText(arrayName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenRepresentationType(const VSFilterViewSettings::Representation& rep)
{
  int index = static_cast<int>(rep);
  m_Internals->representationCombo->blockSignals(true);
  m_Internals->representationCombo->setCurrentIndex(index);
  m_Internals->representationCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenArrayName(const QString& arrayName)
{
  m_Internals->activeArrayCombo->blockSignals(true);
  if(arrayName.isNull())
  {
    m_Internals->activeArrayCombo->setCurrentText(::SolidColorStr);
  }
  else
  {
    m_Internals->activeArrayCombo->setCurrentText(arrayName);
  }
  m_Internals->activeArrayCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenComponentIndex(const int& index)
{
  m_Internals->activeComponentCombo->blockSignals(true);
  m_Internals->activeComponentCombo->setCurrentIndex(index+1);
  m_Internals->activeComponentCombo->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenPointSize(const int& size)
{
  m_Internals->pointSizeEdit->blockSignals(true);
  m_Internals->pointSizeEdit->setText(QString::number(size));
  m_Internals->pointSizeEdit->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenPointSphere(const bool& renderAsSpheres)
{
  m_Internals->pointSphereCheckBox->blockSignals(true);
  m_Internals->pointSphereCheckBox->setChecked(renderAsSpheres ? Qt::Checked : Qt::Unchecked);
  m_Internals->pointSphereCheckBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenMapColors(const VSFilterViewSettings::ColorMapping& state)
{
  m_Internals->mapScalarsComboBox->blockSignals(true);
  m_Internals->mapScalarsComboBox->setCurrentIndex(static_cast<int>(state));
  m_Internals->mapScalarsComboBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenAlpha(const double& alpha)
{
  m_Internals->alphaSlider->blockSignals(true);
  m_Internals->alphaSlider->setValue(alpha * 100);
  m_Internals->alphaSlider->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenScalarBar(const bool& show)
{
  m_Internals->showScalarBarCheckBox->blockSignals(true);
  m_Internals->showScalarBarCheckBox->setChecked(show);
  m_Internals->showScalarBarCheckBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenSolidColor(const double* color)
{
  if(nullptr == color)
  {
    return;
  }

  QColor newColor = QColor::fromRgbF(color[0], color[1], color[2]);
  m_Internals->colorBtn->setColor(newColor, false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::listenAxesGridVisible(const double& show)
{
  m_Internals->viewAxesGridCheckBox->blockSignals(true);
  m_Internals->viewAxesGridCheckBox->setCheckState(show ? Qt::Checked : Qt::Unchecked);
  m_Internals->viewAxesGridCheckBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::changesWaiting()
{
  m_Internals->applyBtn->setEnabled(true);
  m_Internals->resetBtn->setEnabled(true);
}
