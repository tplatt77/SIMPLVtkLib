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

#include <vector>

#include <QtWidgets/QFrame>
#include <QtWidgets/QSplitter>

#include "SIMPLVtkLib/QtWidgets/VSVisualizationWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSController.h"
//#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilterWidgets/VSAbstractFilterWidget.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSAbstractViewWidget VSAbstractViewWidget.h SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h
 * @brief This abstract class applies the VSViewController and VSFilterViewSettings to
 * a VSVisualizationWidget.
 */
class SIMPLVtkLib_EXPORT VSAbstractViewWidget : public QFrame
{
  Q_OBJECT

public:
  Q_PROPERTY(bool Active READ isActive WRITE setActive)

  /**
   * @brief Deconstructor
   */
  virtual ~VSAbstractViewWidget() = default;

  /**
   * @brief Returns a pointer to the active VSFilterViewSettings
   * @return
   */
  VSFilterViewSettings* getActiveFilterSettings() const;

  /**
   * @brief Sets the active VSFilterViewSettings
   * @param settings
   */
  virtual void setActiveFilterSettings(VSFilterViewSettings* settings);

  /**
   * @brief Returns the VSFilterViewSettings for the given filter.
   * @param filter
   */
  VSFilterViewSettings* getFilterViewSettings(VSAbstractFilter* filter);

  /**
   * @brief Returns the container of VSFilterViewSettings
   * @return
   */
  VSFilterViewSettings::Map getAllFilterViewSettings() const;

  /**
   * @brief Returns the VSController used by this widget
   * @return
   */
  VSController* getController() const;

  /**
   * @brief Sets the VSController to use
   * @param controller
   */
  void setController(VSController* controller);

  /**
   * @brief Returns the VSFilterViewModel used
   * @return
   */
  VSFilterViewModel* getFilterViewModel() const;

  /**
   * @brief Returns true if the view is active.  Returns false otherwise.
   * @return
   */
  virtual bool isActive();

  /**
   * @brief setActive
   * @param active
   */
  virtual void setActive(bool active);

  /**
   * @brief Clones the widget and its view controller
   * @return
   */
  virtual VSAbstractViewWidget* clone() = 0;

  /**
   * @brief Returns the VSVisualizationWidget used
   */
  virtual VSVisualizationWidget* getVisualizationWidget() const = 0;

  /**
   * @brief Returns the corresponding VSAbstractFilter for the given prop
   * @param prop
   * @return
   */
  virtual VSAbstractFilter* getFilterFromProp(vtkProp3D* prop);

  /**
   * @brief Returns the visualization filter based on the given mouse coordinates.
   * @param pos
   * @return
   */
  virtual VSAbstractFilter* getFilterAtMousePos(const QPoint& point) = 0;

  /**
   * @brief Returns the VSFilterViewSettings based on the given mouse coordinates.
   * @param pos
   * @return
   */
  virtual VSFilterViewSettings* getFilterViewSettingsAtMousePos(const QPoint& point) = 0;

  virtual QMenu* getContextMenu(VSAbstractFilter* filter) = 0;

signals:
  void viewWidgetClosed();
  void markActive(VSAbstractViewWidget*);
  void viewWidgetCreated(VSAbstractViewWidget*);
  void visibilityChanged(VSFilterViewSettings*, bool);
  void gridVisibilityChanged(VSFilterViewSettings*, bool);
  void activeArrayIndexChanged(VSFilterViewSettings*, int);
  void activeComponentIndexChanged(VSFilterViewSettings*, int);
  void mapColorsChanged(VSFilterViewSettings*, VSFilterViewSettings::ColorMapping);
  void alphaChanged(VSFilterViewSettings*, double);
  void showScalarBarChanged(VSFilterViewSettings*, bool);
  void applyCurrentFilter();
  void resetCurrentFilter();
  void controllerChanged(VSController*);

public slots:
  /**
   * @brief Splits the view widget vertically in a QSplitter with a clone of the widget
   */
  void splitVertically();

  /**
   * @brief Splits the view widget horizontally in a QSplitter with a clone of the widget
   */
  void splitHorizontally();

  /**
   * @brief If a part of a QSplitter, remove the QSplitter and reparent the remaining widget
   */
  void closeView();

  /**
   * @brief Renders the visualization widget
   */
  void renderView();

  /**
   * @brief Resets the visualization widget's camera
   */
  void resetCamera();

  /**
   * @brief Updates the scene by rendering the view and then resetting the camera
   */
  void updateScene();

  /**
   * @brief Sets whether or not the render process should be blocked
   * @param block
   */
  void setBlockRender(bool block);

  /**
   * @brief Select the given filter
   * @param filter
   */
  void selectFilter(VSAbstractFilter* filter);

protected slots:
  /**
   * @brief Change filter visibility
   * @param filterVisible
   */
  void setFilterVisibility(const bool& filterVisible);

  /**
   * @brief Change filter grid visibility
   * @param gridVisible
   */
  void setGridVisibility(const bool& gridVisible);

  /**
   * @brief Removes the old prop from the renderer if applicable and adds the new one
   * @param oldProp
   * @param newProp
   */
  void swapActors(vtkProp3D* oldProp, vtkProp3D* newProp);

  /**
   * @brief Change the active filter's array name
   * @param name
   */
  void changeFilterArrayName(QString name);

  /**
   * @brief Change the active filter's component index
   * @param index
   */
  void changeFilterComponentIndex(int index);

  /**
   * @brief Change the active filter's color map setting
   * @param mapColorState
   */
  void changeFilterMapColors(VSFilterViewSettings::ColorMapping mapColorState);

  /**
   * @brief Change the active filter's scalar bar visibility
   * @param showScalarBar
   */
  void changeFilterShowScalarBar(bool showScalarBar);

  /**
   * @brief Active array changed for filter
   * @param index
   */
  virtual void setFilterArrayName(const QString& name);

  /**
   * @brief Active array component changed for filter
   * @param index
   */
  virtual void setFilterComponentIndex(const int& index);

  /**
   * @brief Color mapping changed for filter
   * @param mapColors
   */
  virtual void setFilterMapColors(const VSFilterViewSettings::ColorMapping& mapColorState);

  /**
   * @brief ScalarBar visibility changed for filter
   * @param showScalarBar
   */
  virtual void setFilterShowScalarBar(const bool& showScalarBar);

  /**
   * @brief Sets the active ViewController when the mouse is pressed inside the widget();
   */
  virtual void mousePressed();

protected:
  /**
   * @brief Constructor
   * @param parent
   * @param windowFlags
   */
  VSAbstractViewWidget(QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

  /**
   * @brief Copy constructor
   * @param other
   */
  VSAbstractViewWidget(const VSAbstractViewWidget& other);

  /**
   * @brief Create connections to the VSFilterViewModel
   */
  void setupModel();

  /**
   * @brief Adds a new VSFilterViewSettings object and makes necessary connections
   * @param viewSettings
   */
  void addViewSettings(VSFilterViewSettings* viewSettings);

  /**
   * @brief Removes an existing VSFilterViewSettings object
   * @param viewSettings
   */
  void removeViewSettings(VSFilterViewSettings* viewSettings);

  void changeFilterVisibility(VSFilterViewSettings* settings, const bool& visibility);
  void changeScalarBarVisibility(VSFilterViewSettings* settings, const bool& visibility);

  /**
   * @brief Clears the filters from the view widget
   */
  // void clearFilters();

  /**
   * @brief Copies the VSFilterViewModel
   * @param filterViewModel
   */
  void copyFilters(const VSFilterViewModel& filterViewModel);

  /**
   * @brief Check the visibility of a filter and scalar bar through VSFilterViewSettings
   */
  void checkFilterViewSetting(VSFilterViewSettings* setting);

  /**
   * @brief Returns a QSplitter with this widget and a clone of it
   * @return
   */
  virtual QSplitter* splitWidget(Qt::Orientation orientation);

  /**
   * @brief Returns true if the widget is closable.  Returns false otherwise.
   */
  virtual bool isClosable();

  /**
   * @brief Updates whether or not the view widget can be closed.
   */
  virtual void updateClosable();

  /**
   * @brief Handle mouse press events including marking the VSViewController as the active one
   * @param event
   */
  virtual void mousePressEvent(QMouseEvent* event) override;

  /**
   * @brief Handle key press events including applying or resetting the active filter
   * @param event
   */
  void keyPressEvent(QKeyEvent* event) override;

private:
  VSFilterViewSettings* m_ActiveFilterSettings = nullptr;
  // VSFilterViewSettings::Map m_FilterViewSettings;
  VSFilterViewModel* m_FilterViewModel = nullptr;
  VSController* m_Controller = nullptr;
  bool m_BlockRender = false;
  bool m_Active = false;
};
