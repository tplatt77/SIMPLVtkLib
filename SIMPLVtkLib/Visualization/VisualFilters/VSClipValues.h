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

#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

/**
 * @class VSClipValues VSClipValues.h SIMPLVtkLib/Visualization/VSClipValues.h
 * @brief This class stores information about and allows user interaction for
 * various clip types for VSClipFilter.
 */
class SIMPLVtkLib_EXPORT VSClipValues : public VSAbstractFilterValues
{
  Q_OBJECT

  Q_PROPERTY(VSClipFilter::ClipType lastClipType READ getLastClipType WRITE setLastClipType NOTIFY clipTypeChanged)
  Q_PROPERTY(bool lastBoxInverted READ getLastBoxInverted NOTIFY lastBoxInvertedChanged)
  Q_PROPERTY(bool lastPlaneInverted READ getLastPlaneInverted NOTIFY lastPlaneInvertedChanged)
  Q_PROPERTY(double* lastPlaneOrigin READ getLastPlaneOrigin NOTIFY lastPlaneOriginChanged)
  Q_PROPERTY(double* lastPlaneNormal READ getLastPlaneNormal NOTIFY lastPlaneNormalChanged)
  Q_PROPERTY(std::vector<double> lastBoxTranslation READ getLastBoxTranslationVector NOTIFY lastBoxTranslationChanged)
  Q_PROPERTY(std::vector<double> lastBoxRotation READ getLastBoxRotationVector NOTIFY lastBoxRotationChanged)
  Q_PROPERTY(std::vector<double> lastBoxScale READ getLastBoxScaleVector NOTIFY lastBoxScaleChanged)

public:
  using FilterType = VSClipFilter;

  VSClipValues(VSClipFilter* filter);
  VSClipValues(const VSClipValues& values);
  virtual ~VSClipValues() = default;

  /**
   * @brief Returns the current VSClipFilter
   * @return
   */
  VSClipFilter* getClipFilter() const;

  /**
   * @brief Applies the current values to the selected filters
   */
  void applyValues() override;

  /**
   * @brief Resets the selected filters to their last-applied values
   */
  void resetValues() override;

  /**
   * @brief Returns true if there are changes waiting to be applied.  Returns false otherwise.
   * @return
   */
  bool hasChanges() const override;

  /**
   * @brief Returns the current ClipType
   * @return
   */
  VSClipFilter::ClipType getClipType() const;

  /**
   * @brief Sets the current ClipType
   * @param type
   */
  void setClipType(VSClipFilter::ClipType type);

  /**
   * @brief Returns true if the current clip should be inverted.  Returns false otherwise.
   * @return
   */
  bool isInverted() const;

  /**
   * @brief Sets the inverted state of the current clip values
   * @param inverted
   */
  void setInverted(bool inverted);

  /**
   * @brief Returns the plane representation for the clip
   * @return
   */
  VSPlaneWidget* getPlaneWidget() const;

  /**
   * @brief Returns the box representation for the clip
   * @return
   */
  VSBoxWidget* getBoxWidget() const;

  /**
   * @brief Sets the vtkRenderWindowInteractor for visualizing the current representation
   * @param interactor
   */
  void setInteractor(vtkRenderWindowInteractor* interactor) override;

  /**
   * @brief Sets whether or not the representation should be rendered to the screen
   * @param enabled
   */
  void setRenderingEnabled(bool enabled) override;

  /**
   * @brief Creates and returns a filter input widget for the current values
   * @return
   */
  QWidget* createFilterWidget() override;

  /**
   * @brief Returns whether or not the last applied plane was inverted
   * @return
   */
  bool getLastPlaneInverted() const;

  /**
   * @brief Returns the origin of the last applied plane
   * @return
   */
  double* getLastPlaneOrigin() const;

  /**
   * @brief Returns the normal of the last applied plane
   * @return
   */
  double* getLastPlaneNormal() const;

  /**
   * @brief Returns whether or not the last applied box was inverted
   * @return
   */
  bool getLastBoxInverted() const;

  /**
   * @brief Sets whether or not the last applied plane was inverted
   * @param inverted
   * @return
   */
  void setLastPlaneInverted(bool inverted);

  /**
   * @brief Sets the origin of the last applied plane
   * @param origin
   * @return
   */
  void setLastPlaneOrigin(double* origin);

  /**
   * @brief Sets the normal of the last applied plane
   * @param normal
   * @return
   */
  void setLastPlaneNormal(double* normal);

  /**
   * @brief Sets whether or not the last applied box was inverted
   * @param inverted
   * @return
   */
  void setLastBoxInverted(bool inverted);

  /**
   * @brief Returns the vtkTransform of the last applied box
   * @return
   */
  VTK_PTR(vtkTransform) getLastBoxTransform() const;

  /**
   * @brief Returns the translation portion of the last applied box transform
   * @return
   */
  std::vector<double> getLastBoxTranslationVector() const;

  /**
   * @brief Returns the rotation portion of the last applied box transform
   * @return
   */
  std::vector<double> getLastBoxRotationVector() const;

  /**
   * @brief Returns the scale portion of the last applied box transform
   * @return
   */
  std::vector<double> getLastBoxScaleVector() const;

  /**
   * @brief Sets the vtkTransform of the last applied box
   * @return
   */
  void setLastBoxTransform(VTK_PTR(vtkTransform) transform);

  /**
   * @brief Returns the clip type of the last applied clip
   * @return
   */
  VSClipFilter::ClipType getLastClipType() const;

  /**
   * @brief Sets the clip type of the last applied clip
   * @param type
   */
  void setLastClipType(VSClipFilter::ClipType type);

  /**
   * @brief Loads JSon values
   * @param json
   */
  void loadJSon(QJsonObject& json);

  /**
   * @brief Writes values to JSon
   * @param json
   */
  void writeJson(QJsonObject& json);

signals:
  void clipTypeChanged(VSClipFilter::ClipType);
  void isInvertedChanged(bool);
  void lastPlaneInvertedChanged();
  void lastBoxInvertedChanged();
  void lastPlaneOriginChanged();
  void lastPlaneNormalChanged();
  void lastBoxTranslationChanged();
  void lastBoxRotationChanged();
  void lastBoxScaleChanged();

protected:
  /**
   * @brief Updates value visualization
   */
  void updateRendering();

private:
  bool m_Inverted = false;
  VSBoxWidget* m_BoxWidget = nullptr;
  VSPlaneWidget* m_PlaneWidget = nullptr;
  VSClipFilter::ClipType m_ClipType = VSClipFilter::ClipType::PLANE;
  VSClipFilter::ClipType m_LastClipType = VSClipFilter::ClipType::PLANE;
  bool m_LastPlaneInverted = false;
  bool m_LastBoxInverted = false;

  VTK_PTR(vtkTransform) m_LastBoxTransform;
};
