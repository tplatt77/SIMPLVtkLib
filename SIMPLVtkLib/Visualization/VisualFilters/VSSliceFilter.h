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

#include <QtWidgets/QWidget>

#include "Visualization/VisualFilters/VSAbstractFilter.h"

#include <vtkCutter.h>
#include <vtkPlane.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceValues.h"

/**
 * @class VSSliceFilter VSSliceFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h
 * @brief This class controls the slice filter and, as with other classes
 * inheriting from VSAbstractFilter, can be chained together to further
 * specify what part of the volume should be rendered.
 */
class SIMPLVtkLib_EXPORT VSSliceFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  /**
   * @brief Constructor
   * @param parent
   */
  VSSliceFilter(VSAbstractFilter* parent = nullptr);

  /**
   * @brief Copy constructor
   * @param copy
   */
  VSSliceFilter(const VSSliceFilter& copy);

  /**
   * @brief Deconstructor
   */
  virtual ~VSSliceFilter() = default;

  /**
   * @brief Create
   * @param json
   * @param parent
   * @return
   */
  static VSSliceFilter* Create(QJsonObject& json, VSAbstractFilter* parent);

  /**
   * @brief Returns the filter's name
   * @return
   */
  QString getFilterName() const override;

  /**
   * @brief Returns the tooltip to use for the filter
   * @return
   */
  virtual QString getToolTip() const override;

  /**
   * @brief Convenience method for determining what the filter does
   * @return
   */
  FilterType getFilterType() const override;

  /**
   * @brief Applies the current filter with the given values
   * @param values
   */
  void applyValues(VSSliceValues* values);

  /**
   * @brief Applies the updated values to the algorithm and updates the output
   * @param origin
   * @param normals
   */
  void apply(double origin[3], double normal[3]);

  /**
   * @brief Invokable method for QML to apply plane origin and normal to the algorithm.
   * @param origin
   * @param normal
   */
  Q_INVOKABLE void apply(std::vector<double> origin, std::vector<double> normal);

  /**
   * @brief Returns the output port to be used by vtkMappers and subsequent filters
   * @return
   */
  virtual vtkAlgorithmOutput* getOutputPort() override;

  /**
   * @brief Returns a smart pointer containing the output data from the filter
   * @return
   */
  virtual VTK_PTR(vtkDataSet) getOutput() const override;

  /**
   * @brief Returns the output data type
   * @return
   */
  dataType_t getOutputType() const override;

  /**
   * @brief Returns the required input data type
   * @return
   */
  static dataType_t GetRequiredInputType();

  /**
   * @brief Returns true if this filter type can be added as a child of
   * the given filter.  Returns false otherwise.
   * @param filter
   * @return
   */
  static bool CompatibleWithParent(VSAbstractFilter* filter);

  /**
   * @brief Returns true if this filter type can be added as a child of
   * the given filters.  Returns false otherwise.
   * @param filters
   * @return
   */
  static bool CompatibleWithParents(VSAbstractFilter::FilterListType filters);

  /**
   * @brief Returns the filter values associated with the filter
   * @return
   */
  VSAbstractFilterValues* getValues() override;

  /**
   * @brief Reads values from a json file into the filter
   * @param json
   */
  void readJson(QJsonObject& json);

  /**
   * @brief Writes values to a json file from the filter
   * @param json
   */
  void writeJson(QJsonObject& json) override;

  /**
   * @brief getUuid
   * @return
   */
  static QUuid GetUuid();

  /**
   * @brief getInfoString
   * @return Returns a formatted string that contains general infomation about
   * the filter.
   */
  QString getInfoString(SIMPL::InfoStringFormat format) const override;

protected:
  /**
   * @brief Initializes the algorithm and connects it to the vtkMapper
   */
  void createFilter() override;

  /**
   * @brief This method updates the input port and connects it to the vtkAlgorithm if it exists
   * @param filter
   */
  void updateAlgorithmInput(VSAbstractFilter* filter) override;

private:
  VTK_PTR(vtkCutter) m_SliceAlgorithm;
  VSSliceValues* m_SliceValues = nullptr;
};

Q_DECLARE_METATYPE(VSSliceFilter)
