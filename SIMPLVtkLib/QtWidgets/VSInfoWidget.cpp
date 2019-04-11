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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInfoWidget::VSInfoWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSInfoWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilters(VSAbstractFilter::FilterListType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setupGui()
{
  connect(m_Ui->filterWidget, &VSFilterSettingsWidget::filterDeleted, this, &VSInfoWidget::filterDeleted);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::applyFilter()
{
  m_Ui->filterWidget->applyFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::resetFilter()
{
  m_Ui->filterWidget->resetFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::deleteFilter()
{
  m_Ui->filterWidget->deleteFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setFilters(VSAbstractFilter::FilterListType filters)
{
  m_Ui->filterWidget->setFilters(filters);
  m_Ui->visibilityWidget->setFilters(filters);
  m_Ui->colorMappingWidget->setFilters(filters);
  m_Ui->advVisibilityWidget->setFilters(filters);
  m_Ui->datasetInfoWidget->setFilters(filters);

  if(filters.size() > 0)
  {
    m_Ui->transformWidget->setTransform(filters.front()->getTransform());
  }
  else
  {
    m_Ui->transformWidget->setTransform(nullptr);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSInfoWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  m_Ui->visibilityWidget->setViewWidget(viewWidget);
  m_Ui->colorMappingWidget->setViewWidget(viewWidget);
  m_Ui->advVisibilityWidget->setViewWidget(viewWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDatasetInfoWidget* VSInfoWidget::getDatasetInfoWidget() const
{
  return m_Ui->datasetInfoWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterSettingsWidget* VSInfoWidget::getFilterSettingsWidget() const
{
  return m_Ui->filterWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisibilitySettingsWidget* VSInfoWidget::getVisibilitySettingsWidget() const
{
  return m_Ui->visibilityWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSColorMappingWidget* VSInfoWidget::getColorMappingWidget() const
{
  return m_Ui->colorMappingWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAdvancedVisibilitySettingsWidget* VSInfoWidget::getAdvancedVisibilitySettingsWidget() const
{
  return m_Ui->advVisibilityWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransformWidget* VSInfoWidget::getTransformWidget() const
{
  return m_Ui->transformWidget;
}
