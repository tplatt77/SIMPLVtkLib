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

#ifndef _VSCropFilter_h_
#define _VSCropFilter_h_

#include <QtWidgets/QWidget>

#include "VSAbstractFilter.h"
#include "ui_VSCropFilter.h"

#include <vtkBox.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkExtractVOI;
class VSCropWidget;

/**
 * @class VSCropFilter VSCropFilter.h SIMPLView/VtkSIMPL/VisualFilters/VSCropFilter.h
 * @brief This class is a visibility filter that crops a vtkDataSet to X, Y, 
 * and Z bounds. This class can be chained with other VSAbstractFilters to
 * further specify the data allowed to be visualized. This filter requires 
 * the incoming data type to be a vtkImageData, thus restricting it to following 
 * VSDataSetFilters.
 */
class SIMPLVtkLib_EXPORT VSCropFilter : public VSAbstractFilter, private Ui::VSCropFilter
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parentWidget
  * @param parent
  */
  VSCropFilter(QWidget* parentWidget, VSAbstractFilter* parent);

  /**
  * @brief Deconstructor
  */
  ~VSCropFilter();

  /**
  * @brief Sets the visualization filter's bounds
  * @param bounds
  */
  void setBounds(double* bounds) override;

  /**
  * @brief Initializes the filter and connects it to the vtkMapper
  */
  void setFilter() override;

  /**
  * @brief Sets the input data
  */
  void setInputData(VTK_PTR(vtkDataSet) inputData) override;

  /**
  * @brief Calculates the output from the algorithm
  */
  void calculateOutput() override;

  /**
  * @brief Returns the filter's name
  * @return
  */
  const QString getFilterName() override;

  /**
  * @brief Returns the VSAbstractWidget for the filter
  * @return
  */
  VSAbstractWidget* getWidget() override;

  /**
  * @brief Applies changes to the filter
  */
  void apply() override;
  
  /**
  * @brief Resets the filter to its last applied values
  */
  void reset() override;

  /**
  * @brief Returns the output data type
  * @return
  */
  dataType_t getOutputType() override;

  /**
  * @brief Returns the required incoming data type
  * @return
  */
  static dataType_t getRequiredInputType();

private:
  VTK_PTR(vtkExtractVOI) m_cropAlgorithm;

  VSCropWidget* m_cropWidget;
};

#endif /* _VSCropFilter_h_ */
