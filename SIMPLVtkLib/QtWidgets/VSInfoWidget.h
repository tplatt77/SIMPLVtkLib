/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "ui_VSInfoWidget.h"

class VSAbstractFilterWidget;

/**
 * @class VSInfoWidget VSInfoWidget.h SIMPLVtkLib/QtWidgets/VSInfoWidget.h
 * @brief This class handles displaying information about a visual filter and
 * its view settings in the active VSViewController
 */
class SIMPLVtkLib_EXPORT VSInfoWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSInfoWidget(QWidget* parent = nullptr);

  /**
   * @brief Deconstructor
   */
  virtual ~VSInfoWidget() = default;

  /**
   * @brief Returns the VSFilterSettings widget
   * @return
   */
  VSFilterSettingsWidget* getFilterSettingsWidget() const;

  /**
   * @brief Returns the VSVisibilitySettings widget
   * @return
   */
  VSVisibilitySettingsWidget* getVisibilitySettingsWidget() const;

  /**
   * @brief Returns the VSColorMapping widget
   * @return
   */
  VSColorMappingWidget* getColorMappingWidget() const;

  /**
   * @brief Returns the VSAdvancedVisibilitySettings widget
   * @return
   */
  VSAdvancedVisibilitySettingsWidget* getAdvancedVisibilitySettingsWidget() const;

  /**
   * @brief Returns the VSTransformWidget
   * @return
   */
  VSTransformWidget* getTransformWidget() const;

signals:
  void filterDeleted(VSAbstractFilter* filter);

public slots:
  /**
   * @brief Changes the filters being displayed
   * @param filters
   */
  void setFilters(VSAbstractFilter::FilterListType filters);

  /**
   * @brief Changes the VSViewController used for fetching VSFilterViewSettings
   * @param viewController
   */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

  /**
   * @brief Applies the active filter with the current widget values.
   */
  void applyFilter();

  /**
   * @brief Resets current widget to the last applied filter values
   */
  void resetFilter();

  /**
   * @brief Deletes the active filter
   */
  void deleteFilter();

protected:
  /**
   * @brief Performs initial setup for the GUI
   */
  void setupGui();

private:
  QSharedPointer<Ui::VSInfoWidget> m_Ui;
};
