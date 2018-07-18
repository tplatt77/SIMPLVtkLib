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

#include <QtCore/QSemaphore>
#include <QtCore/QAbstractItemModel>

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
   */
  VSAbstractFilter::FilterListType getAllFilters() const;

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
  Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

  int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
  QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
  int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;
  //QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
  
  void beginInsertingFilter(VSAbstractFilter* parentFilter);
  void endInsertingFilter();
  void beginRemovingFilter(VSAbstractFilter* filter, int row);
  void endRemovingFilter();

signals:
  void filterAdded(VSAbstractFilter* filter, bool currentFilter);
  void filterRemoved(VSAbstractFilter* filter);

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

protected:
  void generateRoleNames();

private:
  QSemaphore m_ModelLock;

  VSRootFilter* m_RootFilter = nullptr;
  QHash<int, QByteArray> m_RoleNames;
};

Q_DECLARE_METATYPE(VSFilterModel)
