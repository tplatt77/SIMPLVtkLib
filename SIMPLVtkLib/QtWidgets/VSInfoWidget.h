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

#include "SVWidgetsLib/Dialogs/ColorPresetsDialog.h"

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class QVTKInteractor;
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

signals:
  void filterDeleted(VSAbstractFilter* filter);

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
   * @brief Slot for handling user interaction with the mapScalarsComboBox
   * @param colorMappingIndex
   */
  void setScalarsMapped(int colorMappingIndex);

  /**
   * @brief Slot for handling user interaction with the showScalarBarCheckBox
   * @param checkState
   */
  void setScalarBarVisible(int checkState);

  /**
   * @brief Slot for creating color preset dialog
   */
  void selectPresetColors();

  /**
   * @brief Slot for handling color preset data
   * @param preset
   * @param pixmap
   */
  void loadPresetColors(const QJsonObject& preset, const QPixmap& pixmap);

  /**
   * @brief Slot for inverting the current scalar bar
   */
  void invertScalarBar();

  /**
   * @brief Slot for handling user interaction with the alphaSlider for object transparency
   * @param value
   */
  void alphaSliderMoved(int value);

  /**
   * @brief Slot for handling changes in the VSColorButton used for selecting the VSFilterViewSetting's solid color
   * @param color
   */
  void colorButtonChanged(QColor color);

  /**
  * @brief Slot for handling user interaction with the viewGridCheckBox
  * @param checkState
  */
  void setAxesGridVisible(int checkState);

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
   * @brief Listens for the active VSFilterViewSettings point size to change
   * @param settings
   * @param size
   */
  void listenPointSize(VSFilterViewSettings* settings, int size);

  /**
  * @brief Listens for the active VSFilterViewSettings render point spheres setting to change
  * @param settings
  * @param renderAsSpheres
  */
  void listenPointSphere(VSFilterViewSettings* settings, bool renderAsSpheres);

  /**
   * @brief Listens for the active VSFilterViewSettings map colors value to change
   * @param settings
   * @param colorMapping
   */
  void listenMapColors(VSFilterViewSettings* settings, VSFilterViewSettings::ColorMapping colorMapping);

  /**
   * @brief Listens for the active VSFilterViewSettings alpha value to change
   * @param settings
   * @param alpha
   */
  void listenAlpha(VSFilterViewSettings* settings, double alpha);

  /**
   * @brief Listens for the active VSFilterViewSettings scalar bar visibility to change
   * @param settings
   * @param show
   */
  void listenScalarBar(VSFilterViewSettings* settings, bool show);

  /**
   * @brief Listens for the active VSFilterViewSettings solid color to change
   * @param settings
   * @param color
   */
  void listenSolidColor(VSFilterViewSettings* settings, double* color);

  /**
   * @brief Listens for the active VSFilterViewSettings axes grid visibility to change
   * @param settings
   * @param show
   */
  void listenAxesGridVisible(VSFilterViewSettings* settings, double show);

  /**
   * @brief Enables the Apply and Reset buttons when the current filter widget has changes waiting
   */
  void changesWaiting();

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
   * @brief Updates the visibility settings for widgets related to point rendering;
   */
  void updatePointSettingVisibility();

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
  class VSInternals;
  VSInternals* m_Internals;

  VSAbstractFilter* m_Filter = nullptr;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
  VSAbstractFilterWidget* m_FilterWidget = nullptr;
  VSFilterViewSettings* m_ViewSettings = nullptr;
  ColorPresetsDialog* m_presetsDialog = nullptr;
};
