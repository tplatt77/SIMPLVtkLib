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

#include "VSAdvancedVisibilitySettingsWidget.h"

#include <QtGui/QIntValidator>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAdvancedVisibilitySettingsWidget::VSAdvancedVisibilitySettingsWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSAdvancedVisibilitySettingsWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilter(nullptr, nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::setupGui()
{
  QIntValidator* pointSizeValidator = new QIntValidator(this);
  pointSizeValidator->setBottom(1);
  m_Ui->pointSizeEdit->setValidator(pointSizeValidator);

  connect(m_Ui->pointSizeEdit, &QLineEdit::textChanged, this, &VSAdvancedVisibilitySettingsWidget::updatePointSize);
  connect(m_Ui->pointSphereCheckBox, &QCheckBox::stateChanged, this, &VSAdvancedVisibilitySettingsWidget::updateRenderPointSpheres);
  connect(m_Ui->viewAxesGridCheckBox, &QCheckBox::stateChanged, this, &VSAdvancedVisibilitySettingsWidget::setAxesGridVisible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::setFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget)
{
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

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::connectFilterViewSettings(VSFilterViewSettings* settings)
{
  if(m_ViewSettings)
  {
    disconnect(m_ViewSettings, &VSFilterViewSettings::pointSizeChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSize);
    disconnect(m_ViewSettings, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSphere);
    disconnect(m_ViewSettings, &VSFilterViewSettings::gridVisibilityChanged, this, &VSAdvancedVisibilitySettingsWidget::listenAxesGridVisible);
  }

  m_ViewSettings = settings;

  if(m_ViewSettings)
  {
    connect(settings, &VSFilterViewSettings::pointSizeChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSize);
    connect(settings, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSphere);
    connect(settings, &VSFilterViewSettings::gridVisibilityChanged, this, &VSAdvancedVisibilitySettingsWidget::listenAxesGridVisible);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
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
void VSAdvancedVisibilitySettingsWidget::updateFilterInfo()
{
  if(m_Filter)
  {

    if(m_ViewSettings)
    {
      m_Ui->pointSizeEdit->blockSignals(true);
      m_Ui->pointSphereCheckBox->blockSignals(true);

      int pointSize = m_ViewSettings->getPointSize();
      m_Ui->pointSizeEdit->setText(QString::number(pointSize));

      bool renderPointSpheres = m_ViewSettings->renderPointsAsSpheres();
      m_Ui->pointSphereCheckBox->setChecked(renderPointSpheres ? Qt::Checked : Qt::Unchecked);

      m_Ui->pointSizeEdit->blockSignals(false);
      m_Ui->pointSphereCheckBox->blockSignals(false);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(nullptr == m_ViewSettings)
  {
    updatePointSettingVisibility();
    return;
  }

  // Point Size
  m_Ui->pointSizeEdit->setText(QString::number(m_ViewSettings->getPointSize()));
  bool renderingPointSpheres = m_ViewSettings->renderPointsAsSpheres();
  Qt::CheckState pointSphereCheckState = renderingPointSpheres ? Qt::Checked : Qt::Unchecked;
  m_Ui->pointSphereCheckBox->setCheckState(pointSphereCheckState);

  updatePointSettingVisibility();
  updateAnnotationVisibility();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updatePointSettingVisibility()
{
  bool visible = false;

  if(m_ViewSettings)
  {
    visible = m_ViewSettings->isRenderingPoints();
  }

  m_Ui->pointRenderingWidget->setVisible(visible);

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updateAnnotationVisibility()
{
  bool validData = m_ViewSettings && m_ViewSettings->isValid();
  m_Ui->annotationWidget->setVisible(validData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updatePointSize(QString pointSize)
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
void VSAdvancedVisibilitySettingsWidget::updateRenderPointSpheres(int checkState)
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
void VSAdvancedVisibilitySettingsWidget::setAxesGridVisible(int checkState)
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
void VSAdvancedVisibilitySettingsWidget::listenPointSize(int size)
{
  m_Ui->pointSizeEdit->blockSignals(true);
  m_Ui->pointSizeEdit->setText(QString::number(size));
  m_Ui->pointSizeEdit->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::listenPointSphere(bool renderAsSpheres)
{
  m_Ui->pointSphereCheckBox->blockSignals(true);
  m_Ui->pointSphereCheckBox->setChecked(renderAsSpheres ? Qt::Checked : Qt::Unchecked);
  m_Ui->pointSphereCheckBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::listenAxesGridVisible(double show)
{
  m_Ui->viewAxesGridCheckBox->blockSignals(true);
  m_Ui->viewAxesGridCheckBox->setCheckState(show ? Qt::Checked : Qt::Unchecked);
  m_Ui->viewAxesGridCheckBox->blockSignals(false);
}
