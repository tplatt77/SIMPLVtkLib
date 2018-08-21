/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
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
#include <QtCore/QSemaphore>

#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class VSRootFilter;

/**
 * @class VSFilterModel VSFilterModel.h SIMPLVtkLib/QtWidgets/VSFilterModel.h
 * @brief This class handles the visual filter model for the VSController.
 */
class SIMPLVtkLib_EXPORT VSFilterModel : public QAbstractItemModel
{
  Q_OBJECT

  Q_PROPERTY(QModelIndex rootIndex READ rootIndex)

public:
  enum FilterDataRole : int
  {
    FilterRole = 0x100
  };

  /**
   * @brief Constructor
   * @param parent
   */
  VSFilterModel(QObject* parent = nullptr);

  /**
   * @brief Copy constructor
   * @param model
   */
  VSFilterModel(const VSFilterModel& model);

  /**
   * @brief Deconstructor
   */
  virtual ~VSFilterModel() = default;

  /**
   * @brief Returns the QObject parent since the parent() method is overloaded
   * @return
   */
  QObject* parentObject() const;

  /**
   * @brief Returns the visual filter stored at the given index
   * @param index
   * @return
   */
  VSAbstractFilter* getFilterFromIndex(const QModelIndex& index) const;

  /**
   * @brief Returns the model index of the given filter
   * @param filter
   * @return
   */
  QModelIndex getIndexFromFilter(VSAbstractFilter* filter);

  /**
   * @brief Returns a vector of top-level filters in the model
   * @return
   */
  VSAbstractFilter::FilterListType getBaseFilters() const;

  /**
   * @brief Returns a vector of all visual filters in the model
   * @return
   */
  VSAbstractFilter::FilterListType getAllFilters() const;

  /**
   * @brief Returns the root filter in the model.
   * @return
   */
  VSRootFilter* getRootFilter() const;

  /**
   * @brief Returns the first matching VSPipelineFilter with the given FilterPipeline
   * @param pipeline
   * @return
   */
  VSAbstractFilter* getPipelineFilter(FilterPipeline::Pointer pipeline);

  /**
   * @brief Returns the first matching VSPipelineFilter with the given pipeline name
   * @param pipelineName
   * @return
   */
  VSAbstractFilter* getPipelineFilter(QString pipelineName);

  ////////////////////////////////
  // QAbstractItemModel methods //
  ////////////////////////////////
  /**
   * @brief Returns the Qt::ItemFlags for the given index
   * @param index
   * @return
   */
  Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the number of columns under the given parent index
   * @param parent
   * @return
   */
  int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the data for the given index and requested role
   * @param index
   * @param role
   * @return
   */
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

  /**
   * @brief Sets the data for the given index and role.  Returns true if the process was successful.  Returns false otherwise.
   * @param index
   * @param value
   * @param role
   * @return
   */
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

  /**
   * @brief Returns the QModelIndex for the row and column under the specified parent index
   * @param row
   * @param column
   * @param parent
   * @return
   */
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the QModelIndex for the parent of the given index
   * @param index
   * @return
   */
  QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the number of rows belonging to the given parent
   * @param parent
   * @return
   */
  int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

  /**
   * @brief beginInsertingFilter
   * @param parentFilter
   */
  void beginInsertingFilter(VSAbstractFilter* parentFilter);

  /**
   * @brief endInsertingFilter
   * @param filter
   */
  void endInsertingFilter(VSAbstractFilter* filter);

  /**
   * @brief beginRemovingFilter
   * @param filter
   * @param row
   */
  void beginRemovingFilter(VSAbstractFilter* filter, int row);

  /**
   * @brief endRemovingFilter
   * @param filter
   */
  void endRemovingFilter(VSAbstractFilter* filter);

  /**
   * @brief Returns the root index for the model
   * @return
   */
  Q_INVOKABLE QModelIndex rootIndex() const;

  /**
   * @brief Returns the filter text for the given index
   * @param index
   * @return
   */
  Q_INVOKABLE QString getFilterText(const QModelIndex& index) const;

  /**
   * @brief Returns the filter font for the given index
   * @param index
   * @return
   */
  Q_INVOKABLE QFont getFilterFont(const QModelIndex& index) const;

signals:
  void filterAdded(VSAbstractFilter* filter, bool currentFilter = false);
  void filterRemoved(VSAbstractFilter* filter);
  // Connect to VSFilterViewModel
  void beganInsertingFilter(VSAbstractFilter* filter);
  void beganRemovingFilter(VSAbstractFilter* filter, int row);
  void finishedInsertingFilter();
  void finishedRemovingFilter();

public slots:
  /**
   * @brief Updates the model to reflect the view settings found in a given view controller
   * @param viewSettings
   */
  void updateModelForView(VSFilterViewSettings::Map viewSettings);

  /**
   * @brief Adds a filter to the model
   * @param filter
   */
  void addFilter(VSAbstractFilter* filter, bool currentFilter = true);

  /**
   * @brief Removes a filter from the model
   * @param filter
   */
  void removeFilter(VSAbstractFilter* filter);

private slots:
  /**
   * @brief Deletes the target filter.  This slot should only be called through the signal emitted in removeFilter
   * @param filter
   */
  void deleteFilter(VSAbstractFilter* filter);

private:
  QSemaphore m_ModelLock;
  VSRootFilter* m_RootFilter = nullptr;
};

Q_DECLARE_METATYPE(VSFilterModel)
