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

#include <QtWidgets/QListView>

class VSQueueModel;

class VSQueueListView : public QListView
{
    Q_OBJECT

  public:
    VSQueueListView(QWidget* parent = nullptr);

    ~VSQueueListView() override;

    /**
     * @brief clearAllImports
     */
    void clearAllImports();

    /**
     * @brief clearCompletedImports
     */
    void clearCompletedImports();

    /**
     * @brief resetImporter
     */
    void resetImporter(const QModelIndex &index);

    /**
     * @brief stopImporter
     * @param index
     */
    void stopImporter(const QModelIndex &index);

    /**
     * @brief clearImporter
     * @param index
     */
    bool clearImporter(const QModelIndex &index);

    /**
     * @brief getQueueModel
     * @return
     */
    VSQueueModel* getQueueModel();

  protected:
    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief handleClearAllImports
     */
    void handleClearAllImports();

    /**
     * @brief handleClearCompletedImports
     */
    void handleClearCompletedImports();

  protected slots:
    /**
     * @brief requestContextMenu
     * @param pos
     */
    void requestContextMenu(const QPoint& pos);

  public:
    VSQueueListView(const VSQueueListView&) = delete; // Copy Constructor Not Implemented
    VSQueueListView(VSQueueListView&&) = delete;      // Move Constructor Not Implemented
    VSQueueListView& operator=(const VSQueueListView&) = delete; // Copy Assignment Not Implemented
    VSQueueListView& operator=(VSQueueListView&&) = delete;      // Move Assignment Not Implemented
};

