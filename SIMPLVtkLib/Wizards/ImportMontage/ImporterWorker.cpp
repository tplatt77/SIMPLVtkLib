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

#include "ImporterWorker.h"

#include <QtCore/QCoreApplication>

#include <iostream>

#include "QtWidgets/VSQueueModel.h"
#include "QtWidgets/VSQueueItem.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImporterWorker::ImporterWorker()
: m_ImportSem(1)
{
  qRegisterMetaType<QVector<int>>();

  VSQueueModel* model = VSQueueModel::Instance();

  m_CurrentIndex = model->index(0, VSQueueItem::ItemData::Contents);

  connect(model, &VSQueueModel::dataChanged, this, &ImporterWorker::handleModelDataChanged);
  connect(model, &VSQueueModel::rowsAboutToBeRemoved, this, &ImporterWorker::handleRowsAboutToBeRemoved);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImporterWorker::~ImporterWorker()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::addDataImporter(const QString &name, VSAbstractImporter::Pointer importer)
{
  if (!importer)
  {
    return;
  }

  VSQueueModel* model = VSQueueModel::Instance();
  model->addImporter(name, importer, QIcon(":/SIMPL/icons/images/bullet_ball_blue.png"));
  if (!m_CurrentIndex.isValid())
  {
    m_CurrentIndex = model->index(model->rowCount() - 1, VSQueueItem::ItemData::Contents);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::insertDataImporter(int row, const QString &name, VSAbstractImporter::Pointer importer)
{
  if (!importer)
  {
    return;
  }

  VSQueueModel* model = VSQueueModel::Instance();
  model->insertImporter(row, name, importer, QIcon(":/SIMPL/icons/images/bullet_ball_blue.png"));
  if (!m_CurrentIndex.isValid())
  {
    m_CurrentIndex = model->index(model->rowCount() - 1, VSQueueItem::ItemData::Contents);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::removeDataImporter(VSAbstractImporter::Pointer importer)
{
  VSQueueModel* model = VSQueueModel::Instance();

  QPersistentModelIndex index = model->indexOfImporter(importer);
  if (index.isValid())
  {
    model->removeRow(index.row());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::cancelWorker()
{
  m_Cancelled = true;
  m_CurrentImporter->cancel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::handleRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
//  VSQueueModel* model = VSQueueModel::Instance();

//  if (m_CurrentIndex.row() >= first && m_CurrentIndex.row() <= last)
//  {
//    m_CurrentImporter->setCancel(true);
//    m_CurrentIndex = model->index(m_CurrentIndex.row() + 1, VSQueueItem::ItemData::Contents);
//  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::handleModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
  if (!roles.contains(VSQueueModel::Roles::ImporterRole))
  {
    return;
  }

//  VSQueueModel* model = VSQueueModel::Instance();

  int start = topLeft.row();
  int end = bottomRight.row();
  m_ImportSem.acquire();
  for (int i = start; i <= end; i++)
  {
//    QPersistentModelIndex index = model->index(i, VSQueueItem::ItemData::Contents);
//    VSAbstractImporter::Pointer importer = model->data(index, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();
//    if (m_ExecutionQueue.contains(index))
//    {
//      VSAbstractImporter::Pointer queuedImporter = m_ExecutionQueue.value(index);
//      if (importer != queuedImporter)
//      {
//        m_ExecutionQueue.insert(index, importer);
//      }
//    }
//    else
//    {

//    }

//    if (importer && !importer->getImported())
//    {
//      m_ImportSem.acquire();
//      m_ExecutionQueue.insert(index, importer);
//      m_ImportSem.release();
//    }
//    else if (!importer)
//    {
//      m_ImportSem.acquire();
//      m_ExecutionQueue.remove(index);
//      m_ImportSem.release();
//    }
  }
  m_ImportSem.release();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImporterWorker::process()
{
  m_Cancelled = false;

  VSQueueModel* model = VSQueueModel::Instance();

  m_ImportSem.acquire();
  while(m_CurrentIndex.isValid() && m_CurrentIndex.row() < model->rowCount())
  {
    m_ImportSem.release();

    QCoreApplication::processEvents();

    if (m_Cancelled)
    {
      emit cancelled();
      return;
    }

    m_ImportSem.acquire();

    m_CurrentImporter = model->data(m_CurrentIndex, VSQueueModel::Roles::ImporterRole).value<VSAbstractImporter::Pointer>();

    m_ImportSem.release();

    if (m_CurrentImporter && m_CurrentImporter->getState() != VSAbstractImporter::State::Finished)
    {
      m_CurrentImporter->execute();
    }

    m_ImportSem.acquire();
    m_CurrentIndex = model->index(m_CurrentIndex.row() + 1, VSQueueItem::ItemData::Contents);
  }
  m_ImportSem.release();

  emit finished();
}
