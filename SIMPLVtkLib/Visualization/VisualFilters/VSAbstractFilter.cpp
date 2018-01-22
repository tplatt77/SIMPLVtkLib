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

#include "VSAbstractFilter.h"

#include <QString>

#include <vtkActor.h>
#include <vtkAlgorithm.h>
#include <vtkAlgorithmOutput.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkTextProperty.h>
#include <vtkUnstructuredGridAlgorithm.h>

#include <vtkGenericDataObjectWriter.h>

#include "Visualization/VisualFilters/VSDataSetFilter.h"
#include "SIMPLBridge/SIMPLVtkBridge.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"
#include "SIMPLBridge/VSRenderController.h"
#include "Visualization/VtkWidgets/VSAbstractWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::VSAbstractFilter(QWidget* parent, VTK_PTR(vtkRenderWindowInteractor) interactor)
: QWidget(parent)
, m_interactor(interactor)
{
  m_parentFilter = nullptr;

  m_dataSet = nullptr;

  m_filterMapper = VTK_PTR(vtkDataSetMapper)::New();
  m_filterMapper->ScalarVisibilityOn();
  setMapScalars(true);

  m_lookupTable = new VSLookupTableController();
  m_filterMapper->SetLookupTable(m_lookupTable->getColorTransferFunction());

  m_filterActor = VTK_PTR(vtkActor)::New();
  m_filterActor->SetMapper(m_filterMapper);

  m_viewScalarId = 0;
  m_viewScalarComponentId = 0;

  m_isDirty = false;
  m_changesWaiting = true;

  m_mapScalars = true;
  m_showLookupTable = true;
  m_active = true;

  m_scalarBarWidget = VTK_PTR(vtkScalarBarWidget)::New();
  m_scalarBarWidget->SetInteractor(interactor);
  VTK_PTR(vtkScalarBarActor) scalarBarActor = m_scalarBarWidget->GetScalarBarActor();

  scalarBarActor->SetLookupTable(m_lookupTable->getColorTransferFunction());

  vtkTextProperty* titleProperty = scalarBarActor->GetTitleTextProperty();
  titleProperty->SetJustificationToCentered();
  titleProperty->SetFontSize(titleProperty->GetFontSize() * 1.5);

// introduced in 7.1.0rc1, prevents resizing title to fill width
#if VTK_MAJOR_VERSION >= 7 && VTK_MINOR_VERSION >= 1
  scalarBarActor->UnconstrainedFontSizeOn();
#endif

  scalarBarActor->SetTitleRatio(0.75);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter::~VSAbstractFilter()
{
  while(m_children.count() > 0)
  {
    VSAbstractFilter* child = m_children[0];
    removeChild(child);
    delete child;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setInteractor(vtkRenderWindowInteractor* iren)
{
  m_interactor = iren;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getParentFilter()
{
  return m_parentFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setParentFilter(VSAbstractFilter* parent)
{
  if(m_parentFilter != nullptr)
  {
    m_parentFilter->removeChild(this);
  }

  if(nullptr == parent)
  {
    this->m_parentFilter = nullptr;
    this->m_dataSet = nullptr;
    return;
  }
  else
  {
    m_parentFilter = parent;
    m_dataSet = m_parentFilter->getOutput();
    m_parentFilter->m_children.push_back(this);

    m_ParentProducer = VTK_PTR(vtkTrivialProducer)::New();
    m_ParentProducer->SetOutput(parent->getOutput());
    m_filterMapper->SetInputConnection(m_ParentProducer->GetOutputPort());

    if(nullptr != getDataSetStruct())
    {
      if(nullptr != getDataSetStruct()->DataSet)
      {
        if(nullptr != getDataSetStruct()->DataSet->GetCellData())
        {
          if(nullptr != getDataSetStruct()->DataSet->GetCellData()->GetArray(m_viewScalarId))
          {
            m_lookupTable->setRange(getDataSetStruct()->DataSet->GetCellData()->GetArray(m_viewScalarId)->GetRange());
          }
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkDataSet* VSAbstractFilter::getOutput()
{
  if(m_isDirty)
  {
    calculateOutput();
  }

  return m_dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::refresh()
{
  if(m_isDirty)
  {
    calculateOutput();
  }

  for(int i = 0; i < m_children.length(); i++)
  {
    m_children[i]->refresh();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::isDirty()
{
  return m_isDirty;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::hasChangesWaiting()
{
  return m_changesWaiting;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::addChild(VSAbstractFilter* child)
{
  if(m_children.contains(child))
  {
    return;
  }

  m_children.push_back(child);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::removeChild(VSAbstractFilter* child)
{
  if(!m_children.contains(child))
  {
    return;
  }

  m_children.removeAll(child);
  child->setParent(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getAncestor()
{
  if(nullptr == m_parentFilter)
  {
    return this;
  }

  return m_parentFilter->getAncestor();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSAbstractFilter::getChildren() const
{
  return m_children;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getIndexOfChild(VSAbstractFilter* child) const
{
  return m_children.indexOf(child);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<VSAbstractFilter*> VSAbstractFilter::getDescendants() const
{
  QVector<VSAbstractFilter*> descendants;

  int count = m_children.size();
  for(int i = 0; i < count; i++)
  {
    descendants.push_back(m_children[i]);
    descendants.append(m_children[i]->getDescendants());
  }

  return descendants;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSAbstractFilter::getChild(int index)
{
  return m_children.at(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::shared_ptr<VSRenderController::VtkDataSetStruct_t> VSAbstractFilter::getDataSetStruct()
{
  VSDataSetFilter* dataSetFilter = getDataSetFilter();

  if(nullptr == dataSetFilter)
  {
    return nullptr;
  }

  return dataSetFilter->getDataSetStruct();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkActor) VSAbstractFilter::getActor()
{
  return m_filterActor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSetMapper) VSAbstractFilter::getMapper()
{
  return m_filterMapper;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractFilter::getBounds()
{
  if(nullptr == m_parentFilter)
  {
    return nullptr;
  }

  return m_parentFilter->getBounds();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractWidget* VSAbstractFilter::getWidget()
{
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkScalarBarWidget) VSAbstractFilter::getScalarBarWidget()
{
  return getScalarBarWidget(m_viewScalarId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkScalarBarWidget) VSAbstractFilter::getScalarBarWidget(int id)
{
  m_scalarBarWidget->SetInteractor(m_interactor);
  return m_scalarBarWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::sharesScalarBar(VSAbstractFilter* other)
{
  if(nullptr == other)
  {
    return false;
  }

  if(other->getDataSetFilter() != getDataSetFilter())
  {
    return false;
  }

  if(other->m_viewScalarId != m_viewScalarId || other->m_viewScalarComponentId != m_viewScalarComponentId)
  {
    return false;
  }

  return m_lookupTable->equals(other->m_lookupTable);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setViewScalarId(int id)
{
  if(nullptr == m_dataSet)
  {
    return;
  }

  if(id < 0)
  {
    id = 0;
  }
  m_viewScalarId = id;
  m_viewScalarComponentId = 0;

  std::shared_ptr<VSRenderController::VtkDataSetStruct_t> dataSetStruct = getDataSetFilter()->getDataSetStruct();

  VTK_PTR(vtkDataArray) dataArray = dataSetStruct->DataSet->GetCellData()->GetArray(id);
  
  vtkCellData* cellData = m_dataSet->GetCellData();
  if(cellData && dataArray.Get() )
  {
    cellData->SetActiveScalars(dataArray->GetName());
    m_filterMapper->SetScalarRange(dataArray->GetRange()[0], dataArray->GetRange()[1]);
    m_filterMapper->SetScalarModeToDefault();
    
    m_lookupTable->setRange(dataArray->GetRange());
    
    m_filterMapper->Update();
    
    m_scalarBarWidget->GetScalarBarActor()->SetTitle(dataArray->GetName());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getViewScalarId()
{
  return m_viewScalarId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSAbstractFilter::getViewScalarComponentId()
{
  return m_viewScalarComponentId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setViewScalarComponentId(int id)
{
  if(nullptr == m_dataSet || nullptr == m_filterMapper)
  {
    return;
  }

  if(id < 0)
  {
    id = 0;
  }

  m_viewScalarComponentId = id;

  VTK_PTR(vtkDataArray) dataArray = m_dataSet->GetCellData()->GetArray(m_viewScalarId);
  int numComponents = dataArray->GetNumberOfComponents();
  VTK_PTR(vtkScalarsToColors) lookupTable = m_filterMapper->GetLookupTable();

  if(numComponents == 1)
  {
    m_scalarBarWidget->GetScalarBarActor()->SetTitle(dataArray->GetName());
  }
  else if(id < numComponents)
  {
    double* range = dataArray->GetRange(id);

    m_filterMapper->SetScalarModeToUseCellFieldData();

    m_filterMapper->ColorByArrayComponent(m_viewScalarId, m_viewScalarComponentId);
    m_filterMapper->SetScalarRange(range);
    m_filterMapper->Update();

    m_lookupTable->setRange(range);

    m_scalarBarWidget->GetScalarBarActor()->SetTitle(dataArray->GetComponentName(id));
  }
  else if(id == numComponents)
  {
    double* range = dataArray->GetRange(-1);

    m_filterMapper->SetScalarModeToUseCellFieldData();

    m_filterMapper->ColorByArrayComponent(m_viewScalarId, -1);
    m_filterMapper->SetScalarRange(range);
    m_filterMapper->Update();

    lookupTable->SetVectorModeToMagnitude();
    m_lookupTable->setRange(range);

    m_scalarBarWidget->GetScalarBarActor()->SetTitle((QString(dataArray->GetName()) + " Magnitude").toStdString().c_str());
  }

  lookupTable->Build();
  getScalarBarWidget()->GetScalarBarActor()->SetLookupTable(lookupTable);

  m_filterMapper->Update();
  m_interactor->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const char* VSAbstractFilter::scalarIdToName(int scalarId)
{
  if(nullptr == m_dataSet)
  {
    return "";
  }

  vtkDataArray* dataArray = m_dataSet->GetCellData()->GetArray(scalarId);
  if(nullptr == dataArray)
  {
    return "";
  }

  return dataArray->GetName();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataArray) VSAbstractFilter::getBaseDataArray(int id)
{
  VSAbstractFilter* ancestor = getAncestor();
  if(nullptr == ancestor)
  {
    return nullptr;
  }

  if(nullptr == ancestor->m_dataSet)
  {
    return nullptr;
  }

  return ancestor->m_dataSet->GetCellData()->GetArray(id);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSDataSetFilter* VSAbstractFilter::getDataSetFilter()
{
  VSDataSetFilter* cast = dynamic_cast<VSDataSetFilter*>(this);

  if(cast != nullptr)
  {
    return cast;
  }

  if(nullptr == m_parentFilter)
  {
    return nullptr;
  }

  return m_parentFilter->getDataSetFilter();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataArray) VSAbstractFilter::getScalarSet(int id)
{
  return m_dataSet->GetCellData()->GetArray(id);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataArray) VSAbstractFilter::getScalarSet()
{
  if(m_dataSet->GetCellData()->GetVectors() != nullptr)
  {
    return m_dataSet->GetCellData()->GetVectors();
  }

  return m_dataSet->GetCellData()->GetScalars();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::apply()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::reset()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::canDelete()
{
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setDirty()
{
  m_isDirty = true;

  for(int i = 0; i < m_children.size(); i++)
  {
    m_children[i]->setDirty();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::updateMapperScalars()
{
  VTK_PTR(vtkDataArray) dataArray = getScalarSet();
  if(nullptr == dataArray)
  {
    return;
  }

  m_dataSet->GetCellData()->SetActiveScalars(dataArray->GetName());
  m_filterMapper->SetInputData(m_dataSet);
  m_filterMapper->SetScalarRange(getScalarRange());
  m_filterMapper->Update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSAbstractFilter::getScalarRange()
{
  VSAbstractFilter* ancestorFilter = getAncestor();
  if(nullptr == ancestorFilter)
  {
    return nullptr;
  }

  vtkDataArray* dataArray = ancestorFilter->m_dataSet->GetCellData()->GetArray(m_viewScalarId);

  return dataArray->GetRange();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkScalarsToColors* VSAbstractFilter::getScalarLookupTable()
{
  if(nullptr == m_lookupTable)
  {
    return nullptr;
  }

  return m_lookupTable->getColorTransferFunction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::compatibleInput(VSAbstractFilter::dataType_t inputType, VSAbstractFilter::dataType_t requiredType)
{
  if(requiredType == ANY_DATA_SET)
  {
    return true;
  }

  return requiredType == inputType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::changesWaiting()
{
  m_changesWaiting = true;

  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::getScalarsMapped()
{
  return m_mapScalars;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setMapScalars(bool map)
{
  if(map)
  {
    m_filterMapper->SetColorModeToMapScalars();
    m_filterMapper->UseLookupTableScalarRangeOn();
    setViewScalarId(m_viewScalarId);
  }
  else
  {
    m_filterMapper->UseLookupTableScalarRangeOff();
    m_filterMapper->SetColorModeToDirectScalars();
  }

  m_mapScalars = map;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSAbstractFilter::getLookupTableActive()
{
  return m_showLookupTable;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setLookupTableActive(bool show)
{
  m_showLookupTable = show;

  if(nullptr == getDataSetStruct())
  {
    return;
  }

  m_scalarBarWidget->SetEnabled(m_showLookupTable && m_active);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setFilterActive(bool active)
{
  m_active = active;

  if(nullptr == getDataSetStruct())
  {
    return;
  }

  m_scalarBarWidget->SetEnabled(m_showLookupTable && m_active);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::saveFile(QString fileName)
{
  vtkDataSet* output = getOutput();

  VTK_NEW(vtkGenericDataObjectWriter, writer);

  writer->SetFileName(fileName.toStdString().c_str());
  writer->SetInputData(output);
  writer->Write();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::setJsonRgbArray(const QJsonObject& preset)
{
  if(nullptr == m_lookupTable)
  {
    return;
  }

  m_lookupTable->parseRgbJson(preset);
  m_filterMapper->SetLookupTable(m_lookupTable->getColorTransferFunction());
  m_filterMapper->Update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSAbstractFilter::invertLookupTable()
{
  if(nullptr == m_lookupTable)
  {
    return;
  }

  m_lookupTable->invert();
  m_filterMapper->Update();

  if(m_interactor != nullptr)
  {
    m_interactor->GetRenderWindow()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkColorTransferFunction) VSAbstractFilter::getColorTransferFunction()
{
  if(nullptr == m_lookupTable)
  {
    return nullptr;
  }

  return m_lookupTable->getColorTransferFunction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkRenderWindowInteractor) VSAbstractFilter::getInteractor()
{
  return m_interactor;
}
