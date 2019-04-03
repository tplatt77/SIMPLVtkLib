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

#include "VSAbstractFilterValues.h"

class VSSIMPLDataContainerFilter;
class QListWidgetItem;

/**
 * @class VSSIMPLDataContainerValues VSSIMPLDataContainerValues.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSSIMPLDataContainerValues.h
 * @brief This class handles information about what DataArrays should be imported
 * for the stored DataContainer. Changing these values allows the user to import
 * less information into the visualization to help reduce memory usage.
 */
class SIMPLVtkLib_EXPORT VSSIMPLDataContainerValues : public VSAbstractFilterValues
{
  Q_OBJECT

public:
  using FilterType = VSSIMPLDataContainerFilter;

  VSSIMPLDataContainerValues(VSSIMPLDataContainerFilter* filter);
  virtual ~VSSIMPLDataContainerValues() = default;

  /**
   * @brief Finishes importing the given geometry with the selected DataArrays imported
   */
  void applyValues() override;

  /**
   * @brief resetValues
   */
  void resetValues() override;

  /**
   * @brief Returns true if there are changes waiting to be applied.  Returns false otherwise.
   * @return
   */
  bool hasChanges() const override;

  /**
   * @brief Creates and returns a filter widget for the current values
   * @return
   */
  QWidget* createFilterWidget();

  /**
   * @brief Returns the WrappedDataContainer shared_ptr
   * @return
   */
  SIMPLVtkBridge::WrappedDataContainerPtr getWrappedDataContainer() const;

  /**
   * @brief Sets the WrappedDataContainer shared_ptr to a new value
   * @param wrappedDc
   */
  void setWrappedDataContainer(SIMPLVtkBridge::WrappedDataContainerPtr wrappedDc);

  /**
   * @brief Returns true if the current WrappedDataContainer has not had its import settings updated since its last import.
   * Returns false if the WrappedDataContainer has not been imported or has had its values changed since the last import.
   * @return
   */
  bool isFullyWrapped() const;

  /**
   * @brief Sets whether or not the DataContainer is marked as fully wrapped.
   * @param wrapped
   */
  void setFullyWrapped(bool wrapped);

  /**
   * @brief Checks if the given AttributeMatrix::DataArray should be imported under the current settings
   * @param matrixArrayName
   * @return
   */
  bool allowsImport(QString matrixArrayName) const;

  /**
   * @brief Sets whether the given AttributeMatrix::DataArray should be imported under the current settings
   * @param matrixArrayName
   * @param allowImport
   */
  void setImportAllowed(QString matrixArrayName, bool allowImport);

signals:
  void importArraySettingChanged(QString matrixArrayName);

private:
  SIMPLVtkBridge::WrappedDataContainerPtr m_WrappedDataContainer = nullptr;
  bool m_FullyWrapped = false;
};
