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
  setFilters(VSAbstractFilter::FilterListType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::setupGui()
{
  QIntValidator* pointSizeValidator = new QIntValidator(this);
  pointSizeValidator->setBottom(1);
  m_Ui->pointSizeEdit->setValidator(pointSizeValidator);

  // Hide Axes Grid and Annotations label and line
  m_Ui->viewAxesGridCheckBox->setVisible(false);
  m_Ui->annotationLabel->setVisible(false);
  m_Ui->line_2->setVisible(false);
  m_Ui->annotationWidget->setVisible(false);

  connect(m_Ui->pointSizeEdit, &QLineEdit::textChanged, this, &VSAdvancedVisibilitySettingsWidget::updatePointSize);
  connect(m_Ui->pointSphereCheckBox, &QCheckBox::stateChanged, this, &VSAdvancedVisibilitySettingsWidget::updateRenderPointSpheres);
  connect(m_Ui->viewAxesGridCheckBox, &QCheckBox::stateChanged, this, &VSAdvancedVisibilitySettingsWidget::setAxesGridVisible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::setFilters(VSAbstractFilter::FilterListType filters)
{
  m_Filters = filters;

  bool filterExists = (filters.size() > 0);
  if(filterExists && m_ViewWidget)
  {
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filters));
  }
  else
  {
    connectFilterViewSettings(VSFilterViewSettings::Collection());
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::connectFilterViewSettings(VSFilterViewSettings::Collection settings)
{
  for(VSFilterViewSettings* setting : m_ViewSettings)
  {
    disconnect(setting, &VSFilterViewSettings::pointRenderingChanged, this, &VSAdvancedVisibilitySettingsWidget::updatePointSettingVisibility);
    disconnect(setting, &VSFilterViewSettings::pointSizeChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSize);
    disconnect(setting, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSphere);
    disconnect(setting, &VSFilterViewSettings::gridVisibilityChanged, this, &VSAdvancedVisibilitySettingsWidget::listenAxesGridVisible);
  }

  m_ViewSettings = settings;

  for(VSFilterViewSettings* setting : m_ViewSettings)
  {
    connect(setting, &VSFilterViewSettings::pointRenderingChanged, this, &VSAdvancedVisibilitySettingsWidget::updatePointSettingVisibility);
    connect(setting, &VSFilterViewSettings::pointSizeChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSize);
    connect(setting, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSAdvancedVisibilitySettingsWidget::listenPointSphere);
    connect(setting, &VSFilterViewSettings::gridVisibilityChanged, this, &VSAdvancedVisibilitySettingsWidget::listenAxesGridVisible);
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
    connectFilterViewSettings(m_ViewWidget->getFilterViewSettings(m_Filters));
  }
  else
  {
    connectFilterViewSettings(VSFilterViewSettings::Collection());
  }

  updateViewSettingInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updateFilterInfo()
{
  if(m_ViewSettings.size() > 0)
  {
    m_Ui->pointSizeEdit->blockSignals(true);
    m_Ui->pointSphereCheckBox->blockSignals(true);

    int pointSize = VSFilterViewSettings::GetPointSize(m_ViewSettings);
    m_Ui->pointSizeEdit->setText(QString::number(pointSize));

    m_Ui->pointSphereCheckBox->setCheckState(VSFilterViewSettings::IsRenderingPointsAsSpheres(m_ViewSettings));

    m_Ui->pointSizeEdit->blockSignals(false);
    m_Ui->pointSphereCheckBox->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(m_ViewSettings.size() == 0)
  {
    updatePointSettingVisibility();
    return;
  }

  listenPointSize();
  listenPointSphere();
  listenAxesGridVisible();

  updatePointSettingVisibility();
  updateAnnotationVisibility();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updatePointSettingVisibility()
{
  bool visible = false;
  if(m_ViewSettings.size() > 0)
  {
    visible = VSFilterViewSettings::IsRenderingPoints(m_ViewSettings);
  }

  m_Ui->pointRenderingWidget->setVisible(visible);
  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updateAnnotationVisibility()
{
  bool validData = VSFilterViewSettings::HasValidSettings(m_ViewSettings);
  m_Ui->annotationWidget->setVisible(validData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updatePointSize(QString pointSizeStr)
{
  if(m_ViewSettings.size() == 0)
  {
    return;
  }

  if(!pointSizeStr.isEmpty())
  {
    int pointSize = pointSizeStr.toInt();
    VSFilterViewSettings::SetPointSize(m_ViewSettings, pointSize);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::updateRenderPointSpheres(int checkState)
{
  if(m_ViewSettings.size() == 0)
  {
    return;
  }

  VSFilterViewSettings::SetRenderPointsAsSpheres(m_ViewSettings, Qt::Unchecked != checkState);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::setAxesGridVisible(int checkState)
{
  if(m_ViewSettings.size() == 0)
  {
    return;
  }

  bool visible = checkState != Qt::Unchecked;
  VSFilterViewSettings::SetGridVisible(m_ViewSettings, visible);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::listenPointSize(int size)
{
  size = VSFilterViewSettings::GetPointSize(m_ViewSettings);

  m_Ui->pointSizeEdit->blockSignals(true);
  m_Ui->pointSizeEdit->setText(QString::number(size));
  m_Ui->pointSizeEdit->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::listenPointSphere(bool renderAsSpheres)
{
  Qt::CheckState checked = VSFilterViewSettings::IsRenderingPointsAsSpheres(m_ViewSettings);

  m_Ui->pointSphereCheckBox->blockSignals(true);
  m_Ui->pointSphereCheckBox->setCheckState(checked);
  m_Ui->pointSphereCheckBox->blockSignals(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAdvancedVisibilitySettingsWidget::listenAxesGridVisible(double show)
{
  Qt::CheckState checked = VSFilterViewSettings::IsGridVisible(m_ViewSettings);

  m_Ui->viewAxesGridCheckBox->blockSignals(true);
  m_Ui->viewAxesGridCheckBox->setCheckState(checked);
  m_Ui->viewAxesGridCheckBox->blockSignals(false);
}
