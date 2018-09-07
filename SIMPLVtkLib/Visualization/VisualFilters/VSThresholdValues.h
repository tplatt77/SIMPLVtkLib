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

class VSThresholdFilter;

/**
 * @class VSThresholdValues VSThresholdValues.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSThresholdValues.h
 * @brief This filter handles values for VSThresholdFilter and allows the user
 * to change them through the user interface
 */
class SIMPLVtkLib_EXPORT VSThresholdValues : public VSAbstractFilterValues
{
  Q_OBJECT

public:
  using FilterType = VSThresholdFilter;

  VSThresholdValues(VSThresholdFilter* filter);
  virtual ~VSThresholdValues();

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

  QWidget* createFilterWidget() override;

  /**
   * @brief Returns the name of the array to filter by
   * @return
   */
  QString getArrayName() const;

  /**
   * @brief Returns the range for the current array
   * @return
   */
  double* getRange() const;

  /**
   * @brief Returns the minimum allowed value
   * @return
   */
  double getMinValue() const;

  /**
   * @brief Returns the maximum allowed value
   * @return
   */
  double getMaxValue() const;

  void setArrayName(QString name);
  void setMinValue(double value);
  void setMaxValue(double value);

signals:
  void arrayNameChanged(QString);
  void rangeChanged(double[2]);
  void minValueChanged(double);
  void maxValueChanged(double);

protected:
  void setRange(double min, double max);
  int getMinPercent() const;
  int getMaxPercent() const;

  void setMinPercent(int percent);
  void setMaxPercent(int percent);

private:
  QString m_ThresholdArrayName;
  double* m_Range = nullptr;
  double m_MinValue = 0.0;
  double m_MaxValue = 1.0;
};
