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

#include <QtCore/QObject>
#include <QtWidgets/QWidget>

#include <vtkRenderWindowInteractor.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

/**
 * @class VSAbstractFilterValues VSAbstractFilterValues.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilterValues.h
 * @brief This clas handles the storage and visualization of filter values.
 */
class SIMPLVtkLib_EXPORT VSAbstractFilterValues : public QObject
{
  Q_OBJECT

public:
  virtual ~VSAbstractFilterValues() = default;

  /**
   * @brief Returns the filter associated with the values
   * @return
   */
  VSAbstractFilter* getFilter() const;

  /**
   * @brief Returns the current selection being worked on
   * @return
   */
  virtual VSAbstractFilter::FilterListType getSelection() const;

  /**
   * @brief Sets the filter selection to work with
   * @param selection
   */
  virtual void setSelection(VSAbstractFilter::FilterListType selection);

  /**
   * @brief Applies the current values to the associated filter
   */
  virtual void applyValues() = 0;

  /**
   * @brief Resets the current values to thelast applied values for the associated filter
   */
  virtual void resetValues() = 0;

  /**
   * @brief Deletes the associated filter
   */
  void deleteFilter();

  /**
   * @brief Returns true if there are changes waiting to be applied.  Returns false otherwise.
   * @return
   */
  virtual bool hasChanges() const;

  /**
   * @brief Sets the vtkRenderWindowInteractor to render the representation
   * @param interactor
   */
  virtual void setInteractor(vtkRenderWindowInteractor* interactor);

  /**
   * @brief Sets whether or not VTK rendering is enabled for the representation
   * @param enabled
   */
  virtual void setRenderingEnabled(bool enabled);

  /**
   * @brief Creates and returns a filter input widget for the current values
   * @return
   */
  virtual QWidget* createFilterWidget();

signals:
  void alertChangesWaiting();

protected:
  VSAbstractFilterValues(VSAbstractFilter* filter);

  /**
   * @brief Returns whether or not rendering is enabled
   * @return
   */
  bool isRenderingEnabled();

  /**
   * @brief Returns the interactor to use for visualization
   * @return
   */
  vtkRenderWindowInteractor* getInteractor() const;

private:
  VSAbstractFilter::FilterListType m_Selection;
  bool m_RenderingEnabled = true;
  vtkRenderWindowInteractor* m_Interactor = nullptr;
};
