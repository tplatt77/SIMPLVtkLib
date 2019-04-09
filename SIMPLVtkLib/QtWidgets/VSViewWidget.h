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

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/QtWidgets/VSInteractorStyleFilterCamera.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

/**
 * @class VSViewWidget VSViewWidget.h SIMPLVtkLib/QtWidgets/VSViewWidget.h
 * @brief This class handles a single visualization widget and any corresponding
 * widget layout.  As it inherits from VSAbstractViewWidget, this class also
 * handles the VSViewController and VSFilterViewSettings that go along with it.
 */
class SIMPLVtkLib_EXPORT VSViewWidget : public VSAbstractViewWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   * @param windowFlags
   */
  VSViewWidget(QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

  /**
   * @brief Copy constructor
   */
  VSViewWidget(const VSViewWidget& other);

  /**
   * @brief Deconstructor
   */
  virtual ~VSViewWidget() = default;

  /**
   * @brief Clones the widget and its view controller
   * @return
   */
  virtual VSAbstractViewWidget* clone() override;

  /**
   * @brief Returns the VSVisualizationWidget used
   */
  virtual VSVisualizationWidget* getVisualizationWidget() const override;

  /**
   * @brief Returns the visualization filter based on the given mouse coordinates.
   * @param pos
   * @return
   */
  VSAbstractFilter* getFilterAtMousePos(const QPoint& point) override;

  /**
   * @brief Returns the VSFilterViewSettings based on the given mouse coordinates.
   * @param pos
   * @return
   */
  VSFilterViewSettings* getFilterViewSettingsAtMousePos(const QPoint& point) override;

  /**
   * @brief Show only the filter for the given VSFilterViewSettings
   * @param settings
   */
  void showOnlyFilter(VSFilterViewSettings* settings);

  /**
   * @brief Show all filters in the visualization widget
   */
  void showAllFilters();

  /**
   * @brief Show only the scalar bar for the given VSFilterViewSettings
   * @param settings
   */
  void showOnlyScalarBar(VSFilterViewSettings* settings);

  /**
   * @brief Hide all scalar bars
   */
  void hideAllScalarBars();

  /**
   * @brief Returns the context menu for the given filter
   * @param filter
   */
  QMenu* getContextMenu(VSAbstractFilter* filter);

  /**
   * @brief Update the transform text
   * @param transformText
   */
  void updateTransformText(QString transformText);

protected:
  /**
   * @brief Performs initial setup work for the GUI
   */
  void setupGui();

  /**
   * @brief Connect Qt signals and slots
   */
  void connectSlots();

  /**
   * @brief Updates whether or not the view widget can be closed.
   */
  void updateClosable() override;

  /**
   * @brief Creates a context menu for the VSVisualizationWidget
   * @param pos
   */
  void showVisualizationContextMenu(const QPoint& pos);

protected slots:
  /**
   * @brief ScalarBar visibility changed for filter
   * @param viewSettings
   * @param showScalarBar
   */
  void setFilterShowScalarBar(const bool& showScalarBar) override;

private:
  class VSInternals;
  VSInternals* m_Internals;
  VSInteractorStyleFilterCamera* m_InteractorStyle;
};
