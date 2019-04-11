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

#include "VSDatasetInfoWidget.h"

#include <vtkImageData.h>

#include <QtGui/QIntValidator>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDatasetInfoWidget::VSDatasetInfoWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSDatasetInfoWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilters(VSAbstractFilter::FilterListType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::setupGui()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::setFilters(VSAbstractFilter::FilterListType filters)
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
  updateDatasetInfo();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::connectFilterViewSettings(VSFilterViewSettings::Collection settings)
{
  for(VSFilterViewSettings* setting : m_ViewSettings)
  {
    disconnect(setting, &VSFilterViewSettings::pointRenderingChanged, this, &VSDatasetInfoWidget::updatePointSettingVisibility);
    disconnect(setting, &VSFilterViewSettings::pointSizeChanged, this, &VSDatasetInfoWidget::listenPointSize);
    disconnect(setting, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSDatasetInfoWidget::listenPointSphere);
    disconnect(setting, &VSFilterViewSettings::gridVisibilityChanged, this, &VSDatasetInfoWidget::listenAxesGridVisible);
  }

  m_ViewSettings = settings;

  for(VSFilterViewSettings* setting : m_ViewSettings)
  {
    connect(setting, &VSFilterViewSettings::pointRenderingChanged, this, &VSDatasetInfoWidget::updatePointSettingVisibility);
    connect(setting, &VSFilterViewSettings::pointSizeChanged, this, &VSDatasetInfoWidget::listenPointSize);
    connect(setting, &VSFilterViewSettings::renderPointSpheresChanged, this, &VSDatasetInfoWidget::listenPointSphere);
    connect(setting, &VSFilterViewSettings::gridVisibilityChanged, this, &VSDatasetInfoWidget::listenAxesGridVisible);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
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
void VSDatasetInfoWidget::updateDatasetInfo()
{
  if(m_ViewSettings.size() > 0)
  {

    // Get the dataset info
    VSAbstractFilter* filter = m_Filters.front();
    VTK_PTR(vtkDataSet) outputData = filter->getOutput();
    filter->getOutput();
    vtkImageData* imageData = dynamic_cast<vtkImageData*>(outputData.Get());
    double* origin = imageData->GetOrigin();
    int* extent = imageData->GetExtent();
    double* bounds = imageData->GetBounds();

    // Display the dataset info
    QString value;
    value = origin[0];
    m_Ui->originXValue->setText(value);
    value = origin[1];
    m_Ui->originYValue->setText(value);
    value = origin[2];
    m_Ui->originZValue->setText(value);

    value = extent[0];
    m_Ui->extentXValue->setText(value);
    value = extent[1];
    m_Ui->extentYValue->setText(value);
    value = extent[2];
    m_Ui->extentZValue->setText(value);

    value = bounds[0];
    m_Ui->boundsXValue->setText(value);
    value = bounds[1];
    m_Ui->boundsYValue->setText(value);
    value = bounds[2];
    m_Ui->boundsZValue->setText(value);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::updateFilterInfo()
{
  if(m_ViewSettings.size() > 0)
  {
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::updateViewSettingInfo()
{
  // Clear the visualization settings if the current VSFilterViewSettings is null
  if(m_ViewSettings.size() == 0)
  {
    return;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::updatePointSettingVisibility()
{
  bool visible = false;
  if(m_ViewSettings.size() > 0)
  {
    visible = VSFilterViewSettings::IsRenderingPoints(m_ViewSettings);
  }

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::updateAnnotationVisibility()
{
  bool validData = VSFilterViewSettings::HasValidSettings(m_ViewSettings);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::updatePointSize(QString pointSizeStr)
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
void VSDatasetInfoWidget::updateRenderPointSpheres(int checkState)
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
void VSDatasetInfoWidget::setAxesGridVisible(int checkState)
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
void VSDatasetInfoWidget::listenPointSize(int size)
{
  size = VSFilterViewSettings::GetPointSize(m_ViewSettings);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::listenPointSphere(bool renderAsSpheres)
{
  Qt::CheckState checked = VSFilterViewSettings::IsRenderingPointsAsSpheres(m_ViewSettings);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSDatasetInfoWidget::listenAxesGridVisible(double show)
{
  Qt::CheckState checked = VSFilterViewSettings::IsGridVisible(m_ViewSettings);
}
