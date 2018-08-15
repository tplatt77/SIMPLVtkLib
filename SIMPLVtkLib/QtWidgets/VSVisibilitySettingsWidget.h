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

#include "ui_VSVisibilitySettingsWidget.h"

class SIMPLVtkLib_EXPORT VSVisibilitySettingsWidget : public QWidget
{
  Q_OBJECT

public:
  VSVisibilitySettingsWidget(QWidget* parent = nullptr);
  virtual ~VSVisibilitySettingsWidget() = default;

public slots:
  /**
   * @brief Changes the filter being displayed
   * @param filter
   * @param filterWidget
   */
  void setFilter(VSAbstractFilter* filter, VSAbstractFilterWidget* filterWidget);

  /**
   * @brief Changes the VSViewController used for fetching VSFilterViewSettings
   * @param viewController
   */
  void setViewWidget(VSAbstractViewWidget* viewWidget);

protected slots:
  /**
   * @brief Sets the actorRepresentation for the current filter
   * @param index
   */
  void setRepresentationIndex(int index);

  /**
   * @brief Handles the active array combo box being changed
   * @param index
   */
  void updateActiveArrayName(QString name);

  /**
   * @brief Handles the active component combo box being changed
   * @param index
   */
  void updateActiveComponentIndex(int index);

  /**
   * @brief Slot for handling changes in the VSColorButton used for selecting the VSFilterViewSetting's solid color
   * @param color
   */
  void colorButtonChanged(QColor color);

  /**
   * @brief Listens for the active VSFilterViewSettings representation type to change
   * @param settings
   * @param rep
   */
  void listenRepresentationType(VSFilterViewSettings* settings, VSFilterViewSettings::Representation rep);

  /**
   * @brief Listens for the active VSFilterViewSettings active array name to change
   * @param settings
   * @param name
   */
  void listenArrayName(VSFilterViewSettings* settings, QString name);

  /**
   * @brief Listens for the active VSFilterViewSettings active component index to change
   * @param settings
   * @param index
   */
  void listenComponentIndex(VSFilterViewSettings* settings, int index);

  /**
   * @brief Listens for the active VSFilterViewSettings solid color to change
   * @param settings
   * @param color
   */
  void listenSolidColor(VSFilterViewSettings* settings, double* color);

protected:
  /**
   * @brief Performs initial setup for the GUI
   */
  void setupGui();
  /**
   * @brief Updates the information on the visual filter
   */
  void updateFilterInfo();

  /**
   * @brief Updates the information on the VSFilterViewSettings
   */
  void updateViewSettingInfo();

  /**
   * @brief Connects to the given VSFilterViewSettings to take advantage of its signals and slots
   * @param settings
   */
  void connectFilterViewSettings(VSFilterViewSettings* settings);

  /**
   * @brief Updates the active array combo box
   * @param arrayName
   */
  void setComboArrayName(QString arrayName);

private:
  QSharedPointer<Ui::VSVisibilitySettingsWidget> m_Ui;
  VSAbstractFilter* m_Filter = nullptr;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
  VSFilterViewSettings* m_ViewSettings = nullptr;
};