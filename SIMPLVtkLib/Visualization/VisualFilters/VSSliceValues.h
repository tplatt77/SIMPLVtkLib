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

#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

class VSSliceFilter;

/**
 * @class VSSliceValues VSSliceValues.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilters.h
 * @brief This class stores information for a given VSSliceFilter and renders
 * it to the screen to allow the user to manipulate it.
 */
class SIMPLVtkLib_EXPORT VSSliceValues : public VSAbstractFilterValues
{
  Q_OBJECT

public:
  using FilterType = VSSliceFilter;

  VSSliceValues(VSSliceFilter* filter);
  VSSliceValues(const VSSliceValues& values);
  virtual ~VSSliceValues() = default;

  /**
   * @brief Applies the current values to the selected filters
   */
  void applyValues() override;

  /**
   * @brief Resets the selected filters to their last applied values
   */
  void resetValues() override;

  /**
   * @brief Returns true if there are changes waiting to be applied.  Returns false otherwise.
   * @return
   */
  bool hasChanges() const override;

  /**
   * @brief Returns the plane representation for the slice
   * @return
   */
  VSPlaneWidget* getPlaneWidget() const;

  /**
   * @brief Sets the vtkRenderWindowInteractor so the user can manipulate values in the render window
   * @param interactor
   */
  void setInteractor(vtkRenderWindowInteractor* interactor) override;

  /**
   * @brief Sets whether or not the plane representation should be rendered to the screen
   * @param enabled
   */
  void setRenderingEnabled(bool enabled) override;

  /**
   * @brief Returns the plane widget
   * @return
   */
  QWidget* createFilterWidget() override;

  /**
   * @brief Returns the plane representation's origin
   * @return
   */
  double* getOrigin() const;

  /**
   * @brief Returns the plane representation's normal
   * @return
   */
  double* getNormal() const;

  /**
 * @brief Returns the origin of the last applied slice
 * @return
 */
  double* getLastOrigin() const;

  /**
   * @brief Returns the normal of the last applied slice
   * @return
   */
  double* getLastNormal() const;

  /**
   * @brief Sets the origin of the last applied slice
   * @param origin
   * @return
   */
  void setLastOrigin(double* origin);

  /**
   * @brief Sets the normal of the last applied slice
   * @param normal
   * @return
   */
  void setLastNormal(double* normal);

  /**
  * @brief Write values to Json
  * @param json
  */
  void writeJson(QJsonObject& json);

  /**
  * @brief Read values from Json
  * @param json
  */
  void readJson(QJsonObject& json);

signals:
  void lastOriginChanged();
  void lastNormalChanged();

protected:
  /**
   * @brief Updates the enabled state for the VSPlaneWidget
   */
  void updateRendering();

private:
  VSPlaneWidget* m_PlaneWidget = nullptr;
};
