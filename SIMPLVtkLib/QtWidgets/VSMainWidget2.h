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

#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>

#include "SIMPLVtkLib/QtWidgets/VSMainWidgetBase.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include "ui_VSMainWidget2.h"
#include "ui_VisibilitySettingsContainer.h"
#include "ui_VisualizationFilterWidgets.h"

/**
 * @class VSMainWidget2 VSMainWidget2.h SIMPLVtkLib/QtWidgets/VSMainWidget2.h
 * @brief This class works as SIMPLVtkLib's main widget that displays information
 * relating to the stored VSController. It subclasses from VSMainWidgetBase for
 * most of its implementation but is provided with a UI file for formatting the
 * widget.  This widget includes its own information and filter view widgets
 * accessible through PopUpWidgets available through the buttons at the bottom
 * of the widget
 */
class SIMPLVtkLib_EXPORT VSMainWidget2 : public VSMainWidgetBase
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSMainWidget2(QWidget* parent = nullptr);

  /**
   * @brief Deconstructor
   */
  virtual ~VSMainWidget2() = default;

  /**
   * @brief Returns a QMenu with all the Add Filter actions
   * @return
   */
  QMenu* getFilterMenu();

  /**
   * @brief Returns the SVOverlayWidgetButton controlling the filter list
   * @return
   */
  SVOverlayWidgetButton* getFilterListOverlayButton() const;

  /**
   * @brief Returns the SVOverlayWidgetButton controlling the view settings
   * @return
   */
  SVOverlayWidgetButton* getViewSettingsOverlayButton() const;

  /**
   * @brief Adds an SVOverlayWidgetButton as overlapping the visualization overlay widgets
   * @param overlayButton
   */
  void addOverlayButton(SVOverlayWidgetButton* overlayButton);

protected:
  /**
   * @brief Connect Qt signals and slots
   */
  virtual void connectSlots() override;

  /**
   * @brief Creates the filter menu
   */
  virtual void createFilterMenu();

  /**
   * @brief showVisualizationFilters
   */
  void showVisualizationFilters();

  /**
   * @brief showVisibilitySettings
   */
  void showVisibilitySettings();

  /**
   * @brief Updates the filter label based on the current array and component
   */
  void updateFilterLabel();

  /**
   * @brief Updates the enabled and check state for overlay buttons based on the number of filters as well as active filter selection.
   */
  void updateOverlayButtons();

protected slots:
  /**
   * @brief Notifies change in the active VSAbstractViewWidget
   * @param viewWidget
   */
  void setActiveView(VSAbstractViewWidget* viewWidget) override;

  /**
   * @brief setCurrentFilter
   * @param filter
   */
  void setCurrentFilter(VSAbstractFilter* filter) override;

  /**
   * @brief Handle cases where the selected filters were changed
   * @param filtersSelected
   */
  void listenFiltersChanged(VSAbstractFilter::FilterListType filtersSelected);

  /**
   * @brief Handle cases where a filter is added to the visualization pipeline
   * @param filter
   * @param currentFilter
   */
  void listenFilterAdded(VSAbstractFilter* filter, bool currentFilter);

  /**
   * @brief Handle cases where a filter is removed from the visualization pipeline
   * @param filter
   */
  void listenFilterRemoved(VSAbstractFilter* filter);

  /**
   * @brief Updates the enable states for Create Filter buttons based on the current selection
   */
  void updateFilterButtons();

  /**
   * @brief Sets the visualization settings to use for the bottom bar.  If multiple filters are used, none are connected
   * @param viewSettings
   * @param multipleFilters
   */
  void setVisualizationSettings(VSFilterViewSettings::Collection viewSettings);

  /**
   * @brief Handles changes in VSFilterViewSettings visibility
   */
  void vsVisibilityChanged();

  /**
   * @brief Handles changes in VSFilterViewSettings active array and component
   */
  void vsArrayChanged();

  /**
   * @brief Sets the active view camera position to the X+ axis
   */
  void activeCameraXPlus();

  /**
   * @brief Sets the active view camera position to the Y+ axis
   */
  void activeCameraYPlus();

  /**
   * @brief Sets the active view camera position to the Z+ axis
   */
  void activeCameraZPlus();

  /**
   * @brief Sets the active view camera position to the X- axis
   */
  void activeCameraXMinus();

  /**
   * @brief Sets the active view camera position to the Y- axis
   */
  void activeCameraYMinus();

  /**
   * @brief Sets the active view camera position to the Z- axis
   */
  void activeCameraZMinus();

  /**
   * @brief Renders all VSViewWidgets
   */
  void renderAll();

  /**
   * @brief Resets all render view cameras
   */
  void resetCamera();

  /**
   * @brief Sets the progress bar's maximum value based on the
   * number of filters being imported.
   * @param max
   */
  void importNumFilters(int max);

  /**
   * @brief Sets the progress bar's current value based on the
   * number of filters already imported.
   * @param value
   */
  void importedFilterNum(int value);

private:
  QSharedPointer<Ui::VSMainWidget2> m_Ui = nullptr;
  QSharedPointer<Ui::VisualizationFilterWidgets> m_VisualizationFiltersUi = nullptr;
  QSharedPointer<Ui::VisibilitySettingsContainer> m_VisibilityContainerUi = nullptr;

  QMenu* m_FilterMenu = nullptr;
  QAction* m_ActionAddText = nullptr;
  QAction* m_ActionAddClip = nullptr;
  QAction* m_ActionAddCrop = nullptr;
  QAction* m_ActionAddSlice = nullptr;
  QAction* m_ActionAddMask = nullptr;
  QAction* m_ActionAddThreshold = nullptr;

  VSFilterViewSettings::Collection m_VisualizationViewSettings;
};
