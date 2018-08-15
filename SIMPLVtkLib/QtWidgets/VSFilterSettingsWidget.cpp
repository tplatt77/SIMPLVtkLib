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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterSettingsWidget::VSFilterSettingsWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSFilterSettingsWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  setFilter(nullptr, nullptr);
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
  if(nullptr == m_Filter)
  {
    return;
  }

  m_FilterWidget->apply();

  bool hasChanges = m_FilterWidget->hasChanges();
  m_Ui->applyBtn->setEnabled(hasChanges);
  m_Ui->resetBtn->setEnabled(hasChanges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::resetFilter()
{
  if(nullptr == m_Filter)
  {
    return;
  }

  m_FilterWidget->reset();

  bool hasChanges = m_FilterWidget->hasChanges();
  m_Ui->applyBtn->setEnabled(hasChanges);
  m_Ui->resetBtn->setEnabled(hasChanges);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::deleteFilter()
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
void VSFilterSettingsWidget::setFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget)
{
  if(m_FilterWidget != nullptr)
  {
    m_Ui->filterWidgetLayout->removeWidget(m_FilterWidget);
    m_FilterWidget->hide();
    m_FilterWidget = nullptr;

    disconnect(m_FilterWidget, SIGNAL(changesMade()), this, SLOT(changesWaiting()));
  }

  m_Filter = filter;
  m_FilterWidget = filterWidget;

  if(m_FilterWidget != nullptr)
  {
    m_Ui->filterWidgetLayout->addWidget(m_FilterWidget);
    m_FilterWidget->show();

    connect(m_FilterWidget, SIGNAL(changesMade()), this, SLOT(changesWaiting()));
    bool hasChanges = m_FilterWidget->hasChanges();
    m_Ui->applyBtn->setEnabled(hasChanges);
    m_Ui->resetBtn->setEnabled(hasChanges);
  }
  else
  {
    m_Ui->applyBtn->setEnabled(false);
    m_Ui->resetBtn->setEnabled(false);
  }

  bool filterExists = (nullptr != filter);
  m_Ui->deleteBtn->setEnabled(filterExists);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterSettingsWidget::changesWaiting()
{
  m_Ui->applyBtn->setEnabled(true);
  m_Ui->resetBtn->setEnabled(true);
}
