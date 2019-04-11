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

#include "ui_VSDatasetInfoWidget.h"

/**
 * @class VSDatasetInfoWidget VSDatasetInfoWidget.h
 * SIMPLVtkLib/QtWidgets/VSDatasetInfoWidget.h
 * @brief This class handles miscellaneous visibility options for a filter or filters
 * in VSMainWidgetBase that do not fall under other categories
 */
class SIMPLVtkLib_EXPORT VSDatasetInfoWidget : public QWidget
{
  Q_OBJECT

public:
  VSDatasetInfoWidget(QWidget* parent = nullptr);
  virtual ~VSDatasetInfoWidget() = default;

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

protected slots:
  /**
   * @brief Handle changes to the point size through the user interface
   * @param pointSize
   */
  void updatePointSize(QString pointSize);

  /**
   * @brief Handles changes to rendering point spheres through the user interface
   * @param renderSpheres
   */
  void updateRenderPointSpheres(int checkState);

  /**
   * @brief Slot for handling user interaction with the viewGridCheckBox
   * @param checkState
   */
  void setAxesGridVisible(int checkState);

  /**
   * @brief Listens for the active VSFilterViewSettings point size to change
   * @param size
   */
  void listenPointSize(int size = 1);

  /**
   * @brief Listens for the active VSFilterViewSettings render point spheres setting to change
   * @param renderAsSpheres
   */
  void listenPointSphere(bool renderAsSpheres = false);

  /**
   * @brief Listens for the active VSFilterViewSettings axes grid visibility to change
   * @param show
   */
  void listenAxesGridVisible(double show = false);

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
   * @brief Updates the information on the dataset
   */
  void updateDatasetInfo();

  /**
   * @brief Updates the information on the VSFilterViewSettings
   */
  void updateViewSettingInfo();

  /**
   * @brief Updates the visibility settings for widgets related to point rendering;
   */
  void updatePointSettingVisibility();

  /**
   * @brief Updates the annotation visibility settings based on filter output
   */
  void updateAnnotationVisibility();

  /**
   * @brief Connects to the given VSFilterViewSettings to take advantage of its signals and slots
   * @param settings
   */
  void connectFilterViewSettings(VSFilterViewSettings::Collection settings);

private:
  QSharedPointer<Ui::VSDatasetInfoWidget> m_Ui;
  VSAbstractFilter::FilterListType m_Filters;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
  VSFilterViewSettings::Collection m_ViewSettings;
};