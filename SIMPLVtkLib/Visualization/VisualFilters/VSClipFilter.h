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
#ifndef _VSClipFilter_H_
#define _VSClipFilter_H_

#include <QtWidgets/QWidget>

#include "VSAbstractFilter.h"
#include "ui_VSClipFilter.h"

#include <vtkPlane.h>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkClipDataSet;
class vtkTableBasedClipDataSet;
class vtkImplicitPlaneWidget2;
class VSPlaneWidget;
class VSBoxWidget;

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
  enum clipType_t
  {
    PLANE = 0,
    BOX = 1
  };

  /**
  * @brief Constructor
  * @param parentWidget
  * @param parent
  */
  VSClipFilter(VSAbstractFilter* parent);

  /**
  * @brief Deconstructor
  */
  ~VSClipFilter();

  /**
  * @brief Sets the visualization filter's bounds
  * @param bounds
  */
  void setBounds(double* bounds) override;

  /**
  * @brief Initializes the algorithm and connects it to the vtkMapper
  */
  void setFilter() override;

  /**
  * @brief Returns the filter's name
  * @return
  */
  const QString getFilterName() override;

  /**
  * @brief Returns the VSAbstractWidget that goes with this filter
  * @return
  */
  //VSAbstractWidget* getWidget() override;

  /**
  * @brief Applies changes to the filter and updates the output
  */
  void apply() override;

  /**
  * @brief Returns the ouput data type
  * @return
  */
  dataType_t getOutputType() override;

  /**
  * @brief Returns the required incoming data type
  */
  static dataType_t getRequiredInputType();

protected slots:
  /**
  * @brief Changes the clip type
  * @param type
  * @param shouldRepaint
  */
  void changeClipType(int type, bool shouldRepaint = true);

  /**
  * @brief Sets whether or not the clip should be inverted
  * @param state
  */
  void setInsideOut(int state);

private:
  VTK_PTR(vtkPlane) m_ClipPlane;
  VTK_PTR(vtkTableBasedClipDataSet) m_ClipAlgorithm;

  clipType_t m_CurrentClipType, m_LastClipType;
  int m_LastInsideOutState;

  VSPlaneWidget* m_PlaneWidget;
  VSBoxWidget* m_BoxWidget;
};

#endif /* _VSClipFilter_H_ */
