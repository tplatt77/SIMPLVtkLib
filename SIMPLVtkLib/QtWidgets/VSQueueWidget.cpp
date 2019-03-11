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

#include "QtWidgets/VSQueueItemDelegate.h"

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
void VSQueueWidget::setupGui()
{
  connectSignalsSlots();

  VSQueueItemDelegate* delegate = new VSQueueItemDelegate(this);
  m_Ui->queueListWidget->setItemDelegate(delegate);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQueueWidget::Pointer VSQueueWidget::New(QWidget* parent)
{
  VSQueueWidget::Pointer sharedPtr (new VSQueueWidget(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::connectSignalsSlots()
{
  connect(m_Ui->clearBtn, &QPushButton::clicked, [=] { m_Ui->queueListWidget->clear(); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::addImporterToQueue(const QString &name, VSAbstractImporter::Pointer importer)
{
  m_Ui->queueListWidget->addItem(name);
  QListWidgetItem* listWidgetItem = m_Ui->queueListWidget->item(m_Ui->queueListWidget->count() - 1);

  QVariant var;
  var.setValue(importer);
  listWidgetItem->setData(ImporterRole, var);
  listWidgetItem->setIcon(QIcon(":/SIMPL/icons/images/bullet_ball_blue.png"));

  connect(importer.get(), &VSAbstractImporter::started, [=] {
    listWidgetItem->setIcon(QIcon(":/SIMPL/icons/images/bullet_ball_yellow.png"));
    m_Ui->queueListWidget->viewport()->update();
  });

  connect(importer.get(), &VSAbstractImporter::notifyStatusMessage, [=] (const QString &msg) {
    m_Ui->outputTextEdit->append(msg);
  });

  connect(importer.get(), &VSAbstractImporter::notifyErrorMessage, [=] (const QString &msg) {
    m_Ui->outputTextEdit->append(msg);
  });

  connect(importer.get(), &VSAbstractImporter::finished, [=] {
    listWidgetItem->setIcon(QIcon(":/SIMPL/icons/images/bullet_ball_green.png"));
    m_Ui->queueListWidget->viewport()->update();
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQueueWidget::removeImporterFromQueue(const QString &name, VSAbstractImporter::Pointer importer)
{
  QList<QListWidgetItem*> listWidgetItems = m_Ui->queueListWidget->findItems(name, Qt::MatchExactly);

  for (QListWidgetItem* listWidgetItem : listWidgetItems)
  {
    VSAbstractImporter::Pointer internalImporter = listWidgetItem->data(ImporterRole).value<VSAbstractImporter::Pointer>();
    if (internalImporter == importer)
    {
      disconnect(importer.get(), &VSAbstractImporter::started, 0, 0);
      disconnect(importer.get(), &VSAbstractImporter::notifyStatusMessage, 0, 0);
      disconnect(importer.get(), &VSAbstractImporter::notifyErrorMessage, 0, 0);
      disconnect(importer.get(), &VSAbstractImporter::finished, 0, 0);

      int row = m_Ui->queueListWidget->row(listWidgetItem);
      m_Ui->queueListWidget->takeItem(row);
      delete listWidgetItem;
    }
  }
}
