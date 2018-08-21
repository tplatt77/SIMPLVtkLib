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

#include "VSColorMappingWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSColorMappingWidget::VSColorMappingWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSColorMappingWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilter(nullptr, nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setupGui()
{
  m_presetsDialog = new ColorPresetsDialog();

  connect(m_Ui->mapScalarsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setScalarsMapped(int)));
  connect(m_Ui->showScalarBarCheckBox, &QCheckBox::stateChanged, this, &VSColorMappingWidget::setScalarBarVisible);
  connect(m_Ui->invertColorScaleBtn, &QPushButton::clicked, this, &VSColorMappingWidget::invertScalarBar);
  connect(m_Ui->alphaSlider, &QSlider::valueChanged, this, &VSColorMappingWidget::alphaSliderMoved);
  connect(m_Ui->selectPresetColorsBtn, &QPushButton::clicked, this, &VSColorMappingWidget::selectPresetColors);
  connect(m_presetsDialog, &ColorPresetsDialog::applyPreset, this, &VSColorMappingWidget::loadPresetColors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(nullptr == m_ViewSettings)
  {
    m_Ui->showScalarBarCheckBox->setChecked(Qt::Unchecked);
    m_Ui->mapScalarsComboBox->setCurrentIndex(static_cast<int>(VSFilterViewSettings::ColorMapping::None));

    this->setEnabled(false);
    return;
  }

  this->setEnabled(true);

  m_Ui->showScalarBarCheckBox->setChecked(m_ViewSettings->isScalarBarVisible() ? Qt::Checked : Qt::Unchecked);
  m_Ui->mapScalarsComboBox->setCurrentIndex(static_cast<int>(m_ViewSettings->getMapColors()));
  m_Ui->alphaSlider->setValue(m_ViewSettings->getAlpha() * 100);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget)
{
  Q_UNUSED(filterWidget)

  m_Filter = filter;

  bool filterExists = (nullptr != filter);
  if(filterExists && m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filter));
  }
  else
  {
    connectFilterViewSettings(nullptr);
  }

  // Check if VSFilterSettings exist and are valid
  VSFilterViewSettings::ActorType actorType = VSFilterViewSettings::ActorType::Invalid;
  if(m_ViewSettings && m_ViewSettings->isValid())
  {
    actorType = m_ViewSettings->getActorType();
  }

  switch(actorType)
  {
  case VSFilterViewSettings::ActorType::DataSet:
    m_Ui->contentContainer->setVisible(true);
    m_Ui->colorMappingContainer->setVisible(true);
    break;
  case VSFilterViewSettings::ActorType::Image2D:
    m_Ui->contentContainer->setVisible(true);
    m_Ui->colorMappingContainer->setVisible(false);
    break;
  case VSFilterViewSettings::ActorType::Invalid:
  default:
    m_Ui->contentContainer->setVisible(false);
    break;
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
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
void VSColorMappingWidget::connectFilterViewSettings(VSFilterViewSettings* settings)
{
  if(m_ViewSettings)
  {
    disconnect(m_ViewSettings, &VSFilterViewSettings::mapColorsChanged, this, &VSColorMappingWidget::listenMapColors);
    disconnect(m_ViewSettings, &VSFilterViewSettings::alphaChanged, this, &VSColorMappingWidget::listenAlpha);
    disconnect(m_ViewSettings, &VSFilterViewSettings::showScalarBarChanged, this, &VSColorMappingWidget::listenScalarBar);
  }

  m_ViewSettings = settings;

  if(m_ViewSettings)
  {
    connect(settings, &VSFilterViewSettings::mapColorsChanged, this, &VSColorMappingWidget::listenMapColors);
    connect(settings, &VSFilterViewSettings::alphaChanged, this, &VSColorMappingWidget::listenAlpha);
    connect(settings, &VSFilterViewSettings::showScalarBarChanged, this, &VSColorMappingWidget::listenScalarBar);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setScalarsMapped(int colorMappingIndex)
{
  if(nullptr == m_ViewSettings)
  {
    return;
  }

  m_ViewSettings->setMapColors(static_cast<VSFilterViewSettings::ColorMapping>(colorMappingIndex));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::setScalarBarVisible(int checkState)
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
void VSColorMappingWidget::selectPresetColors()
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
void VSColorMappingWidget::loadPresetColors(const QJsonObject& preset, const QPixmap& pixmap)
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
void VSColorMappingWidget::invertScalarBar()
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
void VSColorMappingWidget::alphaSliderMoved(int value)
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
void VSColorMappingWidget::listenMapColors(VSFilterViewSettings::ColorMapping colorMapping)
{
  // m_Ui->mapScalarsComboBox->blockSignals(true);
  // m_Ui->mapScalarsComboBox->setCurrentIndex(static_cast<int>(colorMapping));
  // m_Ui->mapScalarsComboBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::listenAlpha(double alpha)
{
  // m_Ui->alphaSlider->blockSignals(true);
  // m_Ui->alphaSlider->setValue(alpha * 100);
  // m_Ui->alphaSlider->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSColorMappingWidget::listenScalarBar(bool show)
{
  // m_Ui->showScalarBarCheckBox->blockSignals(true);
  // m_Ui->showScalarBarCheckBox->setChecked(show);
  // m_Ui->showScalarBarCheckBox->blockSignals(false);
}
