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

#include "VSFilterModel.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSPipelineFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSRootFilter.h"

#include <QtCore/QThread>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterModel::VSFilterModel(QObject* parent)
: QAbstractItemModel(parent)
, m_ModelLock(1)
{
  m_RootFilter = new VSRootFilter(this);

  connect(this, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(deleteFilter(VSAbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterModel::VSFilterModel(const VSFilterModel& model)
: QAbstractItemModel(model.parentObject())
, m_ModelLock(1)
{
  m_RootFilter = new VSRootFilter(this);

  VSAbstractFilter::FilterListType baseFilters = model.getBaseFilters();
  for(auto iter = baseFilters.begin(); iter != baseFilters.end(); iter++)
  {
    addFilter(*iter);
  }

  connect(this, SIGNAL(filterRemoved(VSAbstractFilter*)), this, SLOT(deleteFilter(VSAbstractFilter*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QObject* VSFilterModel::parentObject() const
{
  return QObject::parent();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::addFilter(VSAbstractFilter* filter, bool currentFilter)
{
  if(nullptr == filter)
  {
    return;
  }

  if(nullptr == filter->getParentFilter())
  {
    filter->setParentFilter(m_RootFilter);
  }

  emit filterAdded(filter, currentFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::removeFilter(VSAbstractFilter* filter)
{
  if(nullptr == filter)
  {
    return;
  }

  emit filterRemoved(filter);

  if(filter->getParentFilter())
  {
    filter->deleteFilter();
  }

  submit();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::deleteFilter(VSAbstractFilter* filter)
{
  // This should be emitted after the slots related to removing the filter
  // have already been added to the call order.
  filter->deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterModel::getFilterFromIndex(const QModelIndex& index) const
{
  if(index.isValid())
  {
    VSAbstractFilter* filter = static_cast<VSAbstractFilter*>(index.internalPointer());
    if(filter)
    {
      return filter;
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterModel::getIndexFromFilter(VSAbstractFilter* filter)
{
  if(filter == nullptr)
  {
    return QModelIndex();
  }

  return createIndex(filter->getChildIndex(), 0, filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSFilterModel::getBaseFilters() const
{
  return m_RootFilter->getChildren();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::FilterListType VSFilterModel::getAllFilters() const
{
  VSAbstractFilter::FilterListType filters = getBaseFilters();
  VSAbstractFilter::FilterListType baseFilters = getBaseFilters();

  for(auto iter = baseFilters.begin(); iter != baseFilters.end(); iter++)
  {
    filters.push_back((*iter));
    VSAbstractFilter::FilterListType descendants = (*iter)->getDescendants();

    filters.insert(filters.end(), descendants.begin(), descendants.end());
  }

  return filters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSRootFilter* VSFilterModel::getRootFilter() const
{
  return m_RootFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterModel::getPipelineFilter(FilterPipeline::Pointer pipeline)
{
  VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
  for(VSAbstractFilter* filter : baseFilters)
  {
    VSPipelineFilter* pipelineFilter = dynamic_cast<VSPipelineFilter*>(filter);
    if(pipelineFilter)
    {
      if(pipelineFilter->getFilterPipeline() == pipeline)
      {
        return pipelineFilter;
      }
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterModel::getPipelineFilter(QString pipelineName)
{
  VSAbstractFilter::FilterListType baseFilters = getBaseFilters();
  for(VSAbstractFilter* filter : baseFilters)
  {
    VSPipelineFilter* pipelineFilter = dynamic_cast<VSPipelineFilter*>(filter);
    if(pipelineFilter)
    {
      if(pipelineFilter->getPipelineName() == pipelineName)
      {
        return pipelineFilter;
      }
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::updateModelForView(VSFilterViewSettings::Map viewSettings)
{
  for(auto iter : viewSettings)
  {
    VSFilterViewSettings* settings = iter.second;
    VSAbstractFilter* filter = settings->getFilter();
    filter->setChecked(settings->isVisible());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterModel::columnCount(const QModelIndex& parent) const
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Qt::ItemFlags VSFilterModel::flags(const QModelIndex& index) const
{
  VSAbstractFilter* filter = getFilterFromIndex(index);
  if(filter)
  {
    // VSFilterModel is not checkable
    return filter->flags() & ~Qt::ItemIsUserCheckable;
  }

  return Qt::ItemFlags();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVariant VSFilterModel::data(const QModelIndex& index, int role) const
{
  VSAbstractFilter* targetFilter = getFilterFromIndex(index);
  if(!targetFilter)
  {
    return QVariant();
  }

  switch(role)
  {
  case Qt::DisplayRole:
  case Qt::ItemIsEditable:
    return targetFilter->getText();
  case Qt::ToolTipRole:
    return targetFilter->getToolTip();
  case Qt::FontRole:
    return targetFilter->font();
  case Qt::CheckStateRole:
    return targetFilter->isChecked() ? Qt::Checked : Qt::Unchecked;
  case FilterDataRole::FilterRole:
  {
    QVariant variant;
    variant.setValue(targetFilter);
    return variant;
  }
  case Qt::ItemIsAutoTristate:
    return false;
  default:
    break;
  }

  return QVariant();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  VSAbstractFilter* targetFilter = getFilterFromIndex(index);
  if(!targetFilter)
  {
    return false;
  }

  const QVector<int> roles(role);

  switch(role)
  {
  case Qt::ItemIsEditable:
    targetFilter->setText(value.toString());
    emit dataChanged(index, index, roles);
    return true;
  case Qt::CheckStateRole:
    targetFilter->setChecked(value.toInt() == Qt::Checked);
    emit dataChanged(index, index, roles);
    return true;
  default:
    return QAbstractItemModel::setData(index, value, role);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterModel::index(int row, int column, const QModelIndex& parent) const
{
  // column must be 0 for valid indices in this model
  if(column != 0)
  {
    return QModelIndex();
  }
  if(parent.isValid() && parent.column() != 0)
  {
    return QModelIndex();
  }

  // Root index
  if(!parent.isValid() && row == 0)
  {
    return createIndex(row, column, m_RootFilter);
  }

  // Valid parent
  if(parent.isValid())
  {
    VSAbstractFilter* parentFilter = getFilterFromIndex(parent);
    if(parentFilter && row < parentFilter->getChildren().size())
    {
      return createIndex(row, column, parentFilter->getChild(row));
    }
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterModel::parent(const QModelIndex& index) const
{
  if(!index.isValid())
  {
    return QModelIndex();
  }

  VSAbstractFilter* filter = getFilterFromIndex(index);
  if(!filter || filter == m_RootFilter)
  {
    return QModelIndex();
  }

  VSAbstractFilter* parentFilter = filter->getParentFilter();
  if(parentFilter)
  {
    return createIndex(parentFilter->getChildIndex(), 0, parentFilter);
  }

  return QModelIndex();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterModel::rowCount(const QModelIndex& parent) const
{
  VSAbstractFilter* filter = getFilterFromIndex(parent);
  if(filter)
  {
    return filter->getChildren().size();
  }

  return 0;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::beginInsertingFilter(VSAbstractFilter* parentFilter)
{
  if(nullptr == parentFilter)
  {
    return;
  }

  QModelIndex parentIndex = getIndexFromFilter(parentFilter);
  int position = parentFilter->getChildCount();

  emit beganInsertingFilter(parentFilter);
  beginInsertRows(parentIndex, position, position);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::endInsertingFilter(VSAbstractFilter* filter)
{
  endInsertRows();
  emit finishedInsertingFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::beginRemovingFilter(VSAbstractFilter* filter, int row)
{
  if(nullptr == filter || row < 0)
  {
    return;
  }

  QModelIndex index = getIndexFromFilter(filter);

  emit beganRemovingFilter(filter, row);
  beginRemoveRows(index, row, row);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterModel::endRemovingFilter(VSAbstractFilter* filter)
{
  endRemoveRows();

  emit filterRemoved(filter);
  emit finishedRemovingFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QModelIndex VSFilterModel::rootIndex() const
{
  return createIndex(0, 0, m_RootFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSFilterModel::getFilterText(const QModelIndex& index) const
{
  return data(index, Qt::DisplayRole).toString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFont VSFilterModel::getFilterFont(const QModelIndex& index) const
{
  VSAbstractFilter* filter = getFilterFromIndex(index);
  if(filter)
  {
    return filter->font();
  }

  return QFont();
}
