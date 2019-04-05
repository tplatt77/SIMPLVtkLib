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

#pragma once

#include <QtCore/QAbstractItemModel>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "QtWidgets/VSAbstractImporter.h"

class VSQueueItem;
class ImporterWorker;

class SIMPLVtkLib_EXPORT VSQueueModel : public QAbstractItemModel
{
    Q_OBJECT

  public:
    SIMPL_TYPE_MACRO(VSQueueModel)

    enum Roles
    {
      ImporterRole = Qt::UserRole + 1
    };

    enum class QueueState : unsigned int
    {
      Idle,
      Executing,
      Canceling
    };

    VSQueueModel(QObject* parent = nullptr);

    ~VSQueueModel() override;

    SIMPL_GET_PROPERTY(QueueState, QueueState)

    void startQueue();
    void cancelQueue();

    void addImporter(const QString &name, VSAbstractImporter::Pointer importer, QIcon icon);
    void insertImporter(int row, const QString &name, VSAbstractImporter::Pointer importer, QIcon icon);

    void removeImporter(VSAbstractImporter::Pointer importer);

    void clearAll();

    QModelIndex indexOfImporter(VSAbstractImporter::Pointer importer);

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;

    bool isEmpty();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) Q_DECL_OVERRIDE;

    bool moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild) Q_DECL_OVERRIDE;

    Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

    bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;

    VSQueueItem* getRootItem();

  protected:
    void initialize();

  protected slots:
    /**
     * @brief handleDataQueueFinished
     */
    void handleDataQueueFinished();

  signals:
    void importerStateChanged(VSAbstractImporter* importer, VSAbstractImporter::State state);
    void queueStateChanged(VSQueueModel::QueueState queueState);
    void queueFinished();
    void notifyImporterMessage(const VSAbstractImporter::Pointer& importer, const AbstractMessage::Pointer& msg);

  private:
    static VSQueueModel* self;

    VSQueueItem* rootItem = nullptr;

    QThread* m_ImportDataWorkerThread = nullptr;
    ImporterWorker* m_ImportDataWorker = nullptr;
    QueueState m_QueueState = QueueState::Idle;

    VSQueueItem* getItem(const QModelIndex& index) const;

    void setQueueState(QueueState queueState);

  public:
    VSQueueModel(const VSQueueModel&) = delete;            // Copy Constructor Not Implemented
    VSQueueModel(VSQueueModel&&) = delete;                 // Move Constructor Not Implemented
    VSQueueModel& operator=(const VSQueueModel&) = delete; // Copy Assignment Not Implemented
    VSQueueModel& operator=(VSQueueModel&&) = delete;      // Move Assignment Not Implemented
};
Q_DECLARE_METATYPE(VSQueueModel::QueueState)
