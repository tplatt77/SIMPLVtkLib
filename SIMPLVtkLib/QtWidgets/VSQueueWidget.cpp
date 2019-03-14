/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include "VSQueueWidget.h"

#include <QtWidgets/QColorDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include "QtWidgets/VSQueueItem.h"
#include "QtWidgets/VSQueueItemDelegate.h"
#include "QtWidgets/VSQueueModel.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueWidget::VSQueueWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::VSQueueWidget)
{
  m_Ui->setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueWidget::~VSQueueWidget()
{
  VSQueueModel* model = VSQueueModel::Instance();
  disconnect(model, &VSQueueModel::notifyStatusMessage, 0, 0);
  disconnect(model, &VSQueueModel::notifyErrorMessage, 0, 0);
  disconnect(model, &VSQueueModel::dataChanged, 0, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::setupGui()
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connectSignalsSlots();

  VSQueueItemDelegate* delegate = new VSQueueItemDelegate(this);
  m_Ui->queueListView->setItemDelegate(delegate);

  m_Ui->queueListView->installEventFilter(this);

  VSQueueModel* model = VSQueueModel::Instance();
  m_Ui->queueListView->setModel(model);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueWidget::Pointer VSQueueWidget::New(QWidget* parent)
{
  VSQueueWidget::Pointer sharedPtr(new VSQueueWidget(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::connectSignalsSlots()
{
  VSQueueModel* model = VSQueueModel::Instance();
  connect(model, &VSQueueModel::notifyStatusMessage, [=](const QString& msg) { m_Ui->outputTextEdit->append(msg); });
  connect(model, &VSQueueModel::notifyErrorMessage, [=](const QString& msg) { m_Ui->outputTextEdit->append(msg); });

  connect(model, &VSQueueModel::dataChanged, [=] (const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {
    if (roles.contains(VSQueueModel::Roles::ImporterRole))
    {
      updateNavigationButtons();
    }
  });

  connect(m_Ui->startStopBtn, &QPushButton::clicked, this, &VSQueueWidget::startStopButtonClicked);

  //  connect(m_Ui->clearBtn, &QPushButton::clicked, [=] { m_Ui->queueListView->clear(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::startStopButtonClicked()
{
  if (m_QueueState == QueueState::Idle)
  {
    emit startQueueTriggered();
  }
  else
  {
    emit stopQueueTriggered();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::updateNavigationButtons()
{
  VSQueueModel* model = VSQueueModel::Instance();
  bool executing = false;
  bool filtersAtReady = false;
  for (int row = 0; row < model->rowCount(); row++)
  {
    QModelIndex index = model->index(row, VSQueueItem::ItemData::Contents);
    VSAbstractImporter::Pointer importer = model->data(index, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
    if (importer)
    {
      if (importer->getState() == VSAbstractImporter::State::Executing)
      {
        m_Ui->startStopBtn->setText("Stop");
        m_Ui->startStopBtn->setEnabled(true);
        m_QueueState = QueueState::Executing;
        executing = true;
      }
      if (importer->getState() == VSAbstractImporter::State::Ready)
      {
        filtersAtReady = true;
      }
    }
  }

  if (!executing)
  {
    m_QueueState = QueueState::Idle;
    m_Ui->startStopBtn->setText("Start");
    m_Ui->startStopBtn->setEnabled(filtersAtReady);
  }
}
