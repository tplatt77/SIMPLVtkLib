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

#ifndef _VtkVisualizationWidget_h_
#define _VtkVisualizationWidget_h_

#include <QtWidgets/QWidget>

#include "SVWidgetsLib/FilterParameterWidgets/FilterParameterWidget.h"
#include "SVWidgetsLib/Widgets/PopUpWidget.h"

#include "VtkSIMPL/VtkSupport/SIMPLVtkBridge.h"
#include "VtkSIMPL/QtWidgets/VtkEditFiltersWidget.h"

#include "ui_VtkVisualizationWidget.h"

class AbstractFilter;
class vtkRenderer;
class vtkOrientationMarkerWidget;
class VtkAbstractFilter;
class VtkMappingData;
class VtkToggleWidget;

class QAction;
class QMenu;
class QPoint;

/**
 * @class VtkVisualizationWidget VtkVisualizationWidget.h 
 * SIMPLView/VtkSIMPL/QtWidgets/VtkVisualizationWidget.h
 * @brief This class is used to visualize a DataContainerArray through VTK 
 * without the need to export the data to another program.
 */
class VtkVisualizationWidget : public QWidget, private Ui::VtkVisualizationWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  */
  VtkVisualizationWidget(QWidget* parent = nullptr);

  /**
  * @brief Deconstructor
  */
  virtual ~VtkVisualizationWidget();

  static VtkVisualizationWidget* Instance();

  /**
  * @brief Performs initial setup work for the GUI
  */
  virtual void setupGui();

  /**
  * @brief Adds a visualization filter
  * @param filter
  */
  void addVisualizationFilter(VtkAbstractFilter* filter);

  /**
  * @brief Saves a screenshot of the VTK visualization
  * @param fileName
  */
  void saveScreenshot(QString fileName);

public slots:
  /**
  * @brief Sets the DataContainerArray::Pointer to visualize
  * @param dca
  */
  void setDataContainerArray(DataContainerArray::Pointer dca);

  /**
  * @brief Changes the DataSet to use
  * @param setId
  */
  void changeDataSet(int setId);

  /**
  * @brief Changes the DataArray to render
  * @param viewId
  */
  void changeDataView(int viewId);

  /**
  * @brief Changes the DataArray component to render
  * @param componentId
  */
  void changeDataViewComponent(int componentId);

  /**
  * @brief Sets whether or not scalars should be mapped for the active filter
  * @param state
  */
  void mapFilterScalars(int state);

  /**
  * @brief Sets whether or not the lookup table should be shown for the active filter
  */
  void showLookupTable(int state);

  /**
  * @brief Opens the Save As dialog box for saving the VTK visualization widget 
  * as a VTK file or screenshot
  */
  void saveAs();

  /**
  * @brief Opens a dialog box for saving the VTK visualization widget as a VTK file
  */
  void saveAsVTK();

  /**
  * @brief Resets the camera to a preset axis
  */
  void camXPlus();

  /**
  * @brief Resets the camera to a preset axis
  */
  void camYPlus();

  /**
  * @brief Resets the camera to a preset axis
  */
  void camZPlus();

  /**
  * @brief Resets the camera to a preset axis
  */
  void camXMinus();

  /**
  * @brief Resets the camera to a preset axis
  */
  void camYMinus();

  /**
  * @brief Resets the camera to a preset axis
  */
  void camZMinus();

  /**
  * @brief Checks the vtkScalarBarWidget positions and visibility
  */
  void checkScalarMapping();

  void checkAxisWidget();

protected slots:
  void filterExecuted(AbstractFilter* filter);

  /**
  * @brief Cleans and empties the VTK visualization window
  */
  void cleanupVisualizationWindow();

  /**
  * @brief Resets the camera distance from the VTK data set
  */
  void resetCamera();

  /**
  * @brief Forces a render of the VTK widget
  */
  void renderVtk();

  /**
  * @brief Display the VtkEditFiltersWidget
  */
  void displayEditFiltersWidget();

  void addClipFilter();
  void addSliceFilter();
  void addMaskFilter();

protected:
  /**
  * @brief Connects Qt signals and slots
  */
  void connectSlots();

  /**
  * @brief Disconnects Qt signals and slots
  */
  void disconnectSlots();

  /**
  * @brief Sets the active visualization filter
  * @param filter
  */
  void setActiveFilter(VtkAbstractFilter* filter);

  /**
  * @brief Initializes the renderer and axes
  */
  void initializeRendererAndAxes();

  /**
  * @brief Clears the render window
  */
  void clearRenderWindow();

  /**
  * @brief Sets up the QComboBoxes for determining the data to display
  */
  void setupDataComboBoxes();

  /**
  * @brief Sets the DataArray component combo box for the given vtkDataArray
  * @param dataArray
  */
  void setViewComponentComboBox(vtkDataArray* dataArray);

private:
  static VtkVisualizationWidget* self;

  PopUpWidget* m_PopUpWidget;
  VtkEditFiltersWidget* m_EditFiltersWidget;
  AbstractFilter* m_ActiveVisualizationFilter;
  VtkRenderController::Pointer m_VtkRenderController;
  DataContainerArray::Pointer m_DataContainerArray;
  VTK_PTR(vtkRenderer) m_Renderer;
  VTK_PTR(vtkOrientationMarkerWidget) m_OrientationWidget;

  QMenu* m_contextMenu;

  VtkVisualizationWidget(const VtkVisualizationWidget&); // Copy Constructor Not Implemented
  void operator=(const VtkVisualizationWidget&);         // Operator '=' Not Implemented
};

#endif /* _VtkVisualizationWidget_h_ */
