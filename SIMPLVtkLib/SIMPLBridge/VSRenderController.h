/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#ifndef _VSRenderController_h_
#define _VSRenderController_h_

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
//#include "QtWidgets/VtkMappingData.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

#include <QVector>

class vtkDataArray;
class vtkDataSet;
class vtkDataSetMapper;
class vtkActor;
class vtkScalarBarActor;
class vtkBorderRepresentation;
class vtkRenderer;

class VSAbstractFilter;
class VSDataSetFilter;

/**
 * @class VSRenderController VSRenderController.h
 * SIMPLView/VtkSIMPL/VtkSupport/VSRenderController.h
 * @brief This class handles VTK rendering for a collection of vtkDataSets.
 * The data set, data array, and component can all be changed, and the 
 * positions and visibility of scalar bars is tracked so that scalar bars are 
 * not simply stacked on top of each other at the same position whenever one is 
 * added.
 */
class SIMPLVtkLib_EXPORT VSRenderController
{
public:
  SIMPL_SHARED_POINTERS(VSRenderController)
  SIMPL_STATIC_NEW_MACRO(VSRenderController)
  SIMPL_TYPE_MACRO(VSRenderController)

  enum DataColorMethod
  {
    PointColoring = 1,
    CellColoring
  };

  struct VtkDataViewStruct_t
  {
    bool IsScalarBarActive = false;
    DataColorMethod ColorMethod = CellColoring;
    QString Name = "";

    VtkDataViewStruct_t()
    {
      IsScalarBarActive = false;
      ColorMethod = CellColoring;
      Name = "";
    }
  };

  struct VtkDataSetStruct_t
  {
    VTK_PTR(vtkDataSet) DataSet;
    QString Name = "";
    QVector<std::shared_ptr<VtkDataViewStruct_t>> ViewOptions;

    ~VtkDataSetStruct_t()
    {
      ViewOptions.clear();
    }
  };

  typedef QVector<std::shared_ptr<VtkDataSetStruct_t>> VtkDataStructVector_t;

  /**
  * @brief Deconstructor
  */
  virtual ~VSRenderController();

  /**
  * @brief Create a new VSRenderController shared_ptr
  * @param vtkDataStructs
  * @return
  */
  static Pointer CreateVSRenderController(VtkDataStructVector_t vtkDataStructs);

  /**
  * @brief Sets the active DataView
  * @param dataViewId
  */
  void setDataViewActive(int dataViewId);

  /**
  * @brief Returns the active VtkDataSetStruct_t
  * @return
  */
  std::shared_ptr<VtkDataSetStruct_t> getActiveDataSet();

  /**
  * @brief Returns the active VtkDataViewStruct_t
  * @return
  */
  std::shared_ptr<VtkDataViewStruct_t> getActiveDataView();

  /**
  * @brief Refreshes the vtkActors and vtkMappers
  */
  void refreshMappersAndActors();

  /**
  * @brief Refreshes vtkActors for the given view ID
  */
  void refreshActors(int viewId);

  /**
  * @brief Sets the VTK renderer to color by point array
  */
  void colorDataSetByPointArray();

  /**
  * @brief Sets the VTK renderer to color by cell array
  */
  void colorDataSetByCellArray();

  /**
  * @brief Resets any color settings from the VTK renderer
  */
  void removeColoring();

  /**
  * @brief Adds a filter to the render controller
  * @param filter
  */
  void addFilter(VSAbstractFilter* filter);

  /**
  * @brier Clears any stored filters from the render controller
  */
  void clearFilters();

  /**
  * @brief Returns the active filter
  * @return
  */
  VSAbstractFilter* getActiveFilter();

  /**
  * @brief Sets the active filter
  * @param active
  */
  void setActiveFilter(VSAbstractFilter* active);

  /**
  * @brief Sets the visibility for the given filter
  * @param filter
  * @param visible
  */
  void setFilterVisibility(VSAbstractFilter* filter, bool visible);

  /**
  * @brief Sets the scalar array for the active filter
  * @param scalarId
  */
  void setActiveFilterScalars(int scalarId);

  /**
  * @brief Sets the array component for the active filter
  * @param componentId
  */
  void setActiveFilterComponent(int componentId);

  /**
  * @brief Sets the VtkDataSetFilters
  * @param dataSetFilters
  */
  void setDataSetFilters(QVector<VSDataSetFilter*> dataSetFilters);

  /**
  * @brief Returns the VtkDataSetFilters currently in use
  * @return
  */
  QVector<VSDataSetFilter*> getDataSetFilters();

  /**
  * @brief Updates which ScalarBarWidgets should be active
  */
  void checkScalarBarWidgets();

  /**
  * @brief Checks if a given filter's ScalarBar is visible
  * @param filter
  * @return
  */
  bool checkScalarsVisible(VSAbstractFilter* filter);

  /**
  * @brief Adds the filter's scalar bar to the screen in the appropriate position
  * @param filter
  */
  void addVisibleScalars(VSAbstractFilter* filter);

  /**
  * @brief Removes a filter's visible scalar bar from the screen
  * @param filter
  */
  void removeVisibleScalars(VSAbstractFilter* filter);

  /**
  * @brief Returns the position as an index to find the next appropriate space
  * for the scalar bar in the screen
  * @param defaultPos
  * @return
  */
  unsigned int findScalarWidgetPosition(unsigned int defaultPos);

  /**
  * @brief Returns the vtkBorderRepresentation for the given scalar bar preset position
  * @return position
  * @return
  */
  VTK_PTR(vtkBorderRepresentation) getScalarWidgetPosition(unsigned int position);

  /**
  * @brief Sets the given vtkScalarBarWidget to a given preset position
  * @param widget
  * @param position
  */
  void setScalarWidgetPosition(vtkScalarBarWidget* widget, unsigned int position);

  /**
  * @brief Checks if the given scalar ID is visible for a given VtkDataSetStruct_t
  * @param dataSetStruct
  * @param scalarId
  * @return
  */
  bool checkFiltersVisible(std::shared_ptr<VtkDataSetStruct_t> dataSetStruct, int scalarId);

  /**
  * @brief Checks if the given filter is visible
  * @param filter
  * @return
  */
  bool checkFiltersVisible(VSAbstractFilter* filter);

  /**
  * @brief Sets the camera to a preset position
  */
  void setCameraXPlus();

  /**
  * @brief Sets the camera to a preset position
  */
  void setCameraYPlus();

  /**
  * @brief Sets the camera to a preset position
  */
  void setCameraZPlus();

  /**
  * @brief Sets the camera to a preset position
  */
  void setCameraXMinus();

  /**
  * @brief Sets the camera to a preset position
  */
  void setCameraYMinus();

  /**
  * @brief Sets the camera to a preset position
  */
  void setCameraZMinus();

  SIMPL_INSTANCE_PROPERTY(VtkDataStructVector_t, VtkDataStructs)

  SIMPL_INSTANCE_PROPERTY(VTK_PTR(vtkRenderer), VSRenderControllerRenderer)

protected:
  /**
  * @brief Constructor
  */
  VSRenderController();

  /**
  * @brief Initializes the mappers and actors
  */
  void initializeMappersAndActors();

  /**
  * @brief Returns the vtkCamera's focal point and distance
  * @param focalPoint
  * @param distance
  */
  void getCameraFocalPointAndDistance(double* focalPoint, double& distance);

private:
  int m_dataSetId;
  int m_dataViewId;

  QVector<VSDataSetFilter*> m_dataSetFilters;

  VSAbstractFilter* m_activeFilter;
  QVector<VSAbstractFilter*> m_visibleFilters;
  QVector<VSAbstractFilter*> m_visibleScalarBarFilters;

  VSRenderController(const VSRenderController&); // Copy Constructor Not Implemented
  void operator=(const VSRenderController&);      // Operator '=' Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif /* _VSRenderController_h_ */
