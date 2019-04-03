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

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtWidgets/QButtonGroup>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SVWidgetsLib/QtSupport/QtSPluginFrame.h"

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"

#include "ui_DatasetListWidget.h"

typedef struct
{
  QStringList DatasetNames;
} DatasetListInfo_t;

Q_DECLARE_METATYPE(DatasetListInfo_t)

/**
 * @class DatasetListWidget DatasetListWidget.h SIMPLVtkLib/Wizards/ImportMontage/UI_Files/DatasetListWidget.h
 * @brief This class represents the User Interface for the Dataset file import module
 * of the SIMPLView program. The user interface subclasses QFrame which
 * should make it able to be embedded in most Qt applications. This module controls
 * the import of Dataset data files into an HDF5 file
 * for better data management and archiving.
 *
 * @date Jan 30, 2011
 * @version 1.0
 */
class SIMPLVtkLib_EXPORT DatasetListWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parameter The FilterParameter object that this widget represents
   * @param filter The instance of the filter that this parameter is a part of
   * @param parent The parent QWidget for this Widget
   */
  DatasetListWidget(QWidget* parent = nullptr);

  ~DatasetListWidget() override;

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui();

  /**
   * @brief isComplete
   * @return
   */
  bool isComplete() const;

  /**
   * @brief getDatasetListInfo
   * @return
   */
  DatasetListInfo_t getDatasetListInfo();

  void clear();

  void addDataset(VSAbstractFilter* dataset);

  VSAbstractFilter* getDataset(int row);

  void setSelected(int row, bool selected);
  QList<QListWidgetItem*> selectedItems();

signals:
  void itemSelectionChanged();

private:
  QSharedPointer<Ui::DatasetListWidget> m_Ui;

  QList<QWidget*> m_WidgetList;

  /**
   * @brief connectSignalsSlots
   */
  void connectSignalsSlots();

  /**
   * @brief GenerateFileList This method will generate a file list using a slice number and row/column counts
   * @param filenameList
   * @param hasMissingFiles
   * @param inputPath
   * @return
   */
  QVector<QString> generateFileList(QStringList filenameList,
	  bool& hasMissingFiles, const QString& inputPath);

public:
  DatasetListWidget(const DatasetListWidget&) = delete; // Copy Constructor Not Implemented
  DatasetListWidget(DatasetListWidget&&) = delete;      // Move Constructor Not Implemented
  DatasetListWidget& operator=(const DatasetListWidget&) = delete; // Copy Assignment Not Implemented
  DatasetListWidget& operator=(DatasetListWidget&&) = delete;      // Move Assignment Not Implemented
};

