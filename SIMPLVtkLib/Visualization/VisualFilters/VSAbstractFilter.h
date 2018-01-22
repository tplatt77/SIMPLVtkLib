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

#ifndef _VSAbstractFilter_h_
#define _VSAbstractFilter_h_

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <memory>

#include <vtkSmartPointer.h>
#include <vtkTrivialProducer.h>

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/SIMPLBridge/VSRenderController.h"
#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class QString;
class vtkAlgorithm;
class vtkUnstructuredGridAlgorithm;
class vtkAlgorithmOutput;
class vtkColorTransferFunction;
class vtkDataObject;
class vtkDataSet;
class vtkRenderWindowInteractor;
class vtkActor;
class vtkDataSetMapper;
class vtkDataArray;
class vtkScalarsToColors;
class vtkScalarBarActor;
class vtkScalarBarWidget;

class VSAbstractWidget;
class VSDataSetFilter;
class VSLookupTableController;

/**
* @class VSAbstractFilter VSAbstractFilter.h 
* SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h
* @brief This is the base class for all visual filters in SIMPLVtkLib.
* Classes that inherit from this handle various vtk algorithms for filtering 
* out parts of the vtkDataSet input into the top-level VSDataSetFilter.  Filters
* can be chained together to be more specific about what kind of data should be 
* shown by pushing the output of a filter as the input for each of its child filters.
*/
class SIMPLVtkLib_EXPORT VSAbstractFilter : public QWidget
{
  Q_OBJECT

public:
  enum dataType_t
  {
    IMAGE_DATA,
    UNSTRUCTURED_GRID,
    POLY_DATA,
    ANY_DATA_SET
  };

  VSAbstractFilter(QWidget* parent, VTK_PTR(vtkRenderWindowInteractor) interactor);
  ~VSAbstractFilter();

  virtual void setFilter() = 0;
  void setInteractor(vtkRenderWindowInteractor* interactor);

  VSAbstractFilter* getParentFilter();
  void setParentFilter(VSAbstractFilter* parent);
  VSAbstractFilter* getAncestor();
  QVector<VSAbstractFilter*> getChildren() const;
  VSAbstractFilter* getChild(int index);
  int getIndexOfChild(VSAbstractFilter* child) const;
  QVector<VSAbstractFilter*> getDescendants() const;

  virtual std::shared_ptr<VSRenderController::VtkDataSetStruct_t> getDataSetStruct();

  virtual void setBounds(double* bounds) = 0;
  virtual double* getBounds();

  virtual void setInputData(VTK_PTR(vtkDataSet) inputData) = 0;
  virtual void calculateOutput() = 0;
  vtkDataSet* getOutput();

  void refresh();
  bool isDirty();
  bool hasChangesWaiting();

  virtual const QString getFilterName() = 0;

  VTK_PTR(vtkActor) getActor();
  VTK_PTR(vtkDataSetMapper) getMapper();
  VTK_PTR(vtkScalarBarWidget) getScalarBarWidget();
  VTK_PTR(vtkScalarBarWidget) getScalarBarWidget(int id);
  bool sharesScalarBar(VSAbstractFilter* other);

  virtual VSAbstractWidget* getWidget();

  int getViewScalarId();
  void setViewScalarId(int id);
  int getViewScalarComponentId();
  void setViewScalarComponentId(int id);

  const char* scalarIdToName(int scalarId);

  void saveFile(QString fileName);

  VTK_PTR(vtkDataArray) getBaseDataArray(int id);

  virtual bool canDelete();

  virtual void apply();
  virtual void reset();

  bool getScalarsMapped();
  void setMapScalars(bool map);
  bool getLookupTableActive();
  void setLookupTableActive(bool show);
  void setFilterActive(bool active);

  void setJsonRgbArray(const QJsonObject& preset);

  virtual dataType_t getOutputType() = 0;
  static bool compatibleInput(dataType_t inputType, dataType_t requiredType);

  void invertLookupTable();
  VTK_PTR(vtkColorTransferFunction) getColorTransferFunction();

  VTK_PTR(vtkRenderWindowInteractor) getInteractor();

signals:
  void modified();
  void resized(bool shouldRepaint = false);

protected slots:
  void changesWaiting();

protected:
  VSAbstractFilter* m_parentFilter;
  QVector<VSAbstractFilter*> m_children;

  VTK_PTR(vtkDataSet) m_dataSet;
  VTK_PTR(vtkDataSetMapper) m_filterMapper;
  VTK_PTR(vtkActor) m_filterActor;

  VTK_PTR(vtkDataArray) getScalarSet(int id);
  VTK_PTR(vtkDataArray) getScalarSet();

  VSLookupTableController* m_lookupTable;

  void updateMapperScalars();

  VSDataSetFilter* getDataSetFilter();

  double* getScalarRange();
  vtkScalarsToColors* getScalarLookupTable();

  bool m_isDirty;
  bool m_changesWaiting;
  void setDirty();

  bool m_ConnectedInput = false;
  VTK_PTR(vtkTrivialProducer) m_ParentProducer;

private:
  void addChild(VSAbstractFilter* child);
  void removeChild(VSAbstractFilter* child);

  int m_viewScalarId;
  int m_viewScalarComponentId;

  bool m_mapScalars;
  bool m_showLookupTable;

  bool m_active;

  VTK_PTR(vtkScalarBarWidget) m_scalarBarWidget;

  VTK_PTR(vtkRenderWindowInteractor) m_interactor;
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
