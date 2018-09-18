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


#include "VSSIMPLDataContainerValues.h"

#include <QtWidgets/QListWidget>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSIMPLDataContainerValues::VSSIMPLDataContainerValues(VSSIMPLDataContainerFilter* filter)
: VSAbstractFilterValues(filter)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerValues::applyValues()
{
  VSAbstractFilter::FilterListType filters = getSelection();
  for(VSAbstractFilter* filter : filters)
  {
    // Make sure this is the appropriate filter type first
    FilterType* filterType = dynamic_cast<FilterType*>(filter);
    if(filterType)
    {
      filterType->apply();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerValues::resetValues()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSIMPLDataContainerValues::hasChanges() const
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSSIMPLDataContainerValues::createFilterWidget()
{
  QListWidget* arrayListWidget = new QListWidget();
  bool isPointData = getFilter()->isPointData();
  // Get the correct import settings
  SIMPLVtkBridge::DataArrayImportSettings importSettings;
  if(isPointData)
  {
    importSettings = m_WrappedDataContainer->m_ImportPointArrays;
  }
  else
  {
    importSettings = m_WrappedDataContainer->m_ImportCellArrays;
  }

  // Populate the QListWidget
  for(auto iter = importSettings.begin(); iter != importSettings.end(); iter++)
  {
    QListWidgetItem* item = new QListWidgetItem(arrayListWidget);
    item->setFlags(item->flags() | Qt::ItemFlag::ItemIsUserCheckable);
    item->setCheckState(iter->second ? Qt::Checked : Qt::Unchecked);
    item->setText(iter->first);
    arrayListWidget->addItem(item);
  }

  // Connections
  connect(arrayListWidget, &QListWidget::itemChanged, [=](QListWidgetItem* itemChanged) {
    QString arrayName = itemChanged->text();
    bool checked = itemChanged->checkState() == Qt::Checked;
    setImportAllowed(arrayName, checked);
  });
  connect(this, &VSSIMPLDataContainerValues::importArraySettingChanged, [=](QString matrixArrayName) {
    QList<QListWidgetItem*> items = arrayListWidget->findItems(matrixArrayName, Qt::MatchFlag::MatchCaseSensitive);
    if(items.size() == 1)
    {
      arrayListWidget->blockSignals(true);
      items[0]->setCheckState(allowsImport(matrixArrayName) ? Qt::Checked : Qt::Unchecked);
      arrayListWidget->blockSignals(false);
    }
  });

  return arrayListWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr VSSIMPLDataContainerValues::getWrappedDataContainer() const
{
  return m_WrappedDataContainer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerValues::setWrappedDataContainer(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc)
{
  m_WrappedDataContainer = wrappedDc;
  m_FullyWrapped = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSIMPLDataContainerValues::isFullyWrapped() const
{
  return m_FullyWrapped;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerValues::setFullyWrapped(bool wrapped)
{
  m_FullyWrapped = wrapped;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSIMPLDataContainerValues::allowsImport(QString matrixArrayName) const
{
  bool isPointData = getFilter()->isPointData();
  SIMPLVtkBridge::DataArrayImportSettings importSettings;
  if(isPointData)
  {
    importSettings = m_WrappedDataContainer->m_ImportPointArrays;
  }
  else
  {
    importSettings = m_WrappedDataContainer->m_ImportCellArrays;
  }

  return importSettings.at(matrixArrayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSIMPLDataContainerValues::setImportAllowed(QString matrixArrayName, bool allowImport)
{
  bool isPointData = getFilter()->isPointData();
  if(isPointData)
  {
    m_WrappedDataContainer->m_ImportPointArrays[matrixArrayName] = allowImport;
  }
  else
  {
    m_WrappedDataContainer->m_ImportCellArrays[matrixArrayName] = allowImport;
  }

  emit importArraySettingChanged(matrixArrayName);
}
