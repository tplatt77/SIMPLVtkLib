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

#include "VSAbstractFilter.h"

#include <vtkDataSet.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkTableBasedClipDataSet.h>
#include <vtkTransform.h>

#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

class VSClipValues;

/**
 * @class VSClipFilter VSClipFilter.h
 * SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h
 * @brief This class is used to create a clip filter over a set of data.
 * This class can be chained with itself or other classes inheriting from
 * VSAbstractFilter to be more specific about the data being visualized.
 * VSClipFilter can use both plan and box clip types as well as inverting
 * the clip applied.
 */
class SIMPLVtkLib_EXPORT VSClipFilter : public VSAbstractFilter
{
  Q_OBJECT

public:
  enum class ClipType : int
  {
    PLANE = 0,
    BOX = 1
  };
  Q_ENUMS(ClipType)

  Q_PROPERTY(QStringList clipTypes READ getClipTypes)

  /**
   * @brief Constructor
   * @param parent
   */
  VSClipFilter(VSAbstractFilter* parent = nullptr);

  /**
   * @brief Copy constructor
   * @param copy
   */
  VSClipFilter(const VSClipFilter& copy);

  /**
   * @brief Deconstructor
   */
  virtual ~VSClipFilter() = default;

  /**
   * @brief Create
   * @param json
   * @param parent
   * @return
   */
  static VSClipFilter* Create(QJsonObject& json, VSAbstractFilter* parent);

  /**
   * @brief Returns a QStringList of all ClipTypes
   * @return
   */
  static QStringList getClipTypes();

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
   * @brief Returns the VSClipValues for displaying and interacting with the filter values
   * @return
   */
  VSClipValues* getValues() const;

  /**
   * @brief Sets the VSClipValues for displaying and interacting with the filter
   * @param values
   */
  void applyValues(VSClipValues* values);

  /**
   * @brief Applies the clip filter using a plane with the given values
   * @param origin
   * @param normal
   * @param inverted
   */
  void apply(double origin[3], double normal[3], bool inverted = false);

  /**
   * @brief Applies the clip filter using a box with the given vtkPlanes
   * @param dataSet
   * @param inverted
   */
  void apply(VTK_PTR(vtkPlanes) planes, VTK_PTR(vtkTransform) transform, bool inverted = false);

  /**
   * @brief Q_INVOKABLE version of apply for a plane with the given values
   * @param origin
   * @param normal
   * @param inverted
   */
  Q_INVOKABLE void apply(std::vector<double> origin, std::vector<double> normal, bool inverted = false);

  /**
   * @brief Q_INVOKABLE version of apply for a box with the given values
   * @param origin
   * @param rotation
   * @param scale
   * @param inverted
   */
  Q_INVOKABLE void apply(std::vector<double> origin, std::vector<double> rotation, std::vector<double> scale, bool inverted = false);

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
   * @brief Returns the ouput data type
   * @return
   */
  dataType_t getOutputType() const override;

  /**
   * @brief Returns the required incoming data type
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

signals:
  void clipTypeChanged();

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
  VTK_PTR(vtkTableBasedClipDataSet) m_ClipAlgorithm;
  VSClipValues* m_ClipValues = nullptr;
};

Q_DECLARE_METATYPE(VSClipFilter)
