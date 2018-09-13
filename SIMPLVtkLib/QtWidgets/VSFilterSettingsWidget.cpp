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

#include "VSFilterSettingsWidget.h"

#include "SIMPLVtkLib/QtWidgets/VSColorButton.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterSettingsWidget::VSFilterSettingsWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSFilterSettingsWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilters(VSAbstractFilter::FilterListType());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::setupGui()
{
  connect(m_Ui->applyBtn, &QPushButton::clicked, this, &VSFilterSettingsWidget::applyFilter);
  connect(m_Ui->resetBtn, &QPushButton::clicked, this, &VSFilterSettingsWidget::resetFilter);
  connect(m_Ui->deleteBtn, &QPushButton::clicked, this, &VSFilterSettingsWidget::deleteFilter);

  m_Ui->applyBtn->setDisabled(true);
  m_Ui->resetBtn->setDisabled(true);
  m_Ui->deleteBtn->setDisabled(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::applyFilter()
{
  if(nullptr == getCurrentFilter())
  {
    return;
  }

  getCurrentFilter()->getValues()->applyValues();

  bool hasChanges = getCurrentFilter()->getValues()->hasChanges();
  m_Ui->applyBtn->setEnabled(hasChanges);
  m_Ui->resetBtn->setEnabled(hasChanges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::resetFilter()
{
  bool hasChanges = false;
  for(VSAbstractFilter* filter : m_Filters)
  {
    filter->getValues()->resetValues();
    hasChanges &= filter->getValues()->hasChanges();
  }

  m_Ui->applyBtn->setEnabled(hasChanges);
  m_Ui->resetBtn->setEnabled(hasChanges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::deleteFilter()
{
  VSAbstractFilter::FilterListType filters = m_Filters;
  for(VSAbstractFilter* filter : filters)
  {
    emit filterDeleted(filter);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterSettingsWidget::getCurrentFilter() const
{
  //if(m_CurrentFilter)
  //{
  //  return m_CurrentFilter;
  //}

  if(m_Filters.size() > 0)
  {
    return m_Filters.front();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::setFilters(VSAbstractFilter::FilterListType filters)
{
  if(m_ViewWidget)
  {
    VSRootFilter* rootFilter = m_ViewWidget->getFilterViewModel()->getRootFilter();
    filters.remove(rootFilter);
  }

  if(m_FilterWidget != nullptr)
  {
    m_FilterWidget->hide();
    m_Ui->filterWidgetLayout->removeWidget(m_FilterWidget);
    //m_FilterWidget->deleteLater();
    m_FilterWidget = nullptr;

    disconnect(getCurrentFilter()->getValues(), &VSAbstractFilterValues::alertChangesWaiting, this, &VSFilterSettingsWidget::changesWaiting);
  }

  m_Filters = filters;
  if(filters.size() > 0)
  {
    m_Ui->filterControlsWidget->show();

    if(VSAbstractFilter::SameFilterType(filters) && filters.size() > 0)
    {
      m_FilterWidget = getCurrentFilter()->getValues()->createFilterWidget();
      m_Ui->filterWidgetLayout->addWidget(m_FilterWidget);
      m_FilterWidget->show();

      connect(getCurrentFilter()->getValues(), &VSAbstractFilterValues::alertChangesWaiting, this, &VSFilterSettingsWidget::changesWaiting);
      bool hasChanges = getCurrentFilter()->getValues()->hasChanges();
      m_Ui->applyBtn->setEnabled(hasChanges);
      m_Ui->resetBtn->setEnabled(hasChanges);
    }
    else
    {
      // Incompatible filters selected
      m_Ui->applyBtn->setEnabled(false);
      m_Ui->resetBtn->setEnabled(false);
    }
  }
  else
  {
    m_Ui->filterControlsWidget->hide();
  }

  bool filterExists = (filters.size() > 0);
  m_Ui->deleteBtn->setEnabled(filterExists);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  m_ViewWidget = viewWidget;

  vtkRenderWindowInteractor* interactor = nullptr;
  if(m_ViewWidget)
  {
    interactor = m_ViewWidget->getVisualizationWidget()->GetInteractor();
  }
  
  // Update where filter widgets are rendering
  for(VSAbstractFilter* filter : m_Filters)
  {
    filter->getValues()->setInteractor(interactor);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::changesWaiting()
{
  m_Ui->applyBtn->setEnabled(true);
  m_Ui->resetBtn->setEnabled(true);
}
