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

class VSMaskFilter;

/**
 * @class VSMaskValues VSMaskValues.h SIMPLVtkLib/Visualization/VisualFiltes/VSMaskValues.h
 * @brief This class handles values for VSMaskFilter and allows the user to change the
 * masking array through the user interface.
 */
class SIMPLVtkLib_EXPORT VSMaskValues : public VSAbstractFilterValues
{
  Q_OBJECT

public:
  using FilterType = VSMaskFilter;

  VSMaskValues(VSMaskFilter* filter);
  VSMaskValues(const VSMaskValues& values);
  virtual ~VSMaskValues() = default;

  /**
   * @brief Applies the current values to the selected filters
   */
  void applyValues() override;

  /**
   * @brief Resets the selected filters back to their last applied values
   */
  void resetValues() override;

  /**
   * @brief Returns true if there are changes waiting to be applied.  Returns false otherwise.
   * @return
   */
  bool hasChanges() const override;

  /**
   * @brief Creates and returns a filter input widget for the current values
   * @return
   */
  QWidget* createFilterWidget() override;

  /**
   * @brief Returns the current mask array name
   * @return
   */
  QString getMaskName() const;

  /**
   * @brief Sets the mask array name
   * @param maskName
   */
  void setMaskName(QString maskName);

  /**
   * @brief Returns the name of the array last applied as a mask
   * @return
   */
  QString getLastArrayName() const;

  /**
   * @brief Sets the name of the array last applied as a mask
   * @return
   */
  void setLastArrayName(QString lastArrayName);

  /**
   * @brief Loads values from Json
   * @param json
   */
  void loadJson(QJsonObject& json);

  /**
   * @brief Writes value to Json
   * @param json
   */
  void writeJson(QJsonObject& json);

signals:
  void maskNameChanged(QString name);
  void lastArrayNameChanged(QString name);

private:
  QString m_MaskArrayName;
  QString m_LastArrayName;
  bool m_FreshFilter = true;
};
