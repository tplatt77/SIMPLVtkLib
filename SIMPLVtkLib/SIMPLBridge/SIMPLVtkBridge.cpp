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

#include "SIMPLVtkBridge.h"

#include <vtkCellData.h>
#include <vtkCellDataToPointData.h>
#include <vtkCharArray.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkIntArray.h>
#include <vtkLine.h>
#include <vtkLongLongArray.h>
#include <vtkLookupTable.h>
#include <vtkMappedUnstructuredGrid.h>
#include <vtkNamedColors.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolygon.h>
#include <vtkRectilinearGrid.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>
#include <vtkShortArray.h>
#include <vtkStructuredPoints.h>
#include <vtkTextProperty.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkUnsignedLongLongArray.h>
#include <vtkUnsignedShortArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVertexGlyphFilter.h>

#include "SIMPLVtkLib/SIMPLBridge/VSEdgeGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSQuadGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSTetrahedralGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSTriangleGeom.h"
#include "SIMPLVtkLib/SIMPLBridge/VSVertexGeom.h"

#define AM_COLLISIONS 1
#define AM_MULTIPLE 2
#define AM_ALWAYS 3
#define AM_APPEND AM_MULTIPLE

namespace
{
  const AttributeMatrix::Types CellTypes = { AttributeMatrix::Type::Cell, AttributeMatrix::Type::Face, AttributeMatrix::Type::Edge };
  const AttributeMatrix::Types PointTypes = { AttributeMatrix::Type::Vertex };
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::SIMPLVtkBridge()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtrCollection SIMPLVtkBridge::WrapDataContainerArrayAsStruct(DataContainerArray::Pointer dca)
{
  SIMPLVtkBridge::WrappedDataContainerPtrCollection wrappedDataContainers;

  if(!dca)
  {
    return wrappedDataContainers;
  }

  DataContainerArray::Container dcs = dca->getDataContainers();

  for(DataContainerArray::Container::iterator dc = dcs.begin(); dc != dcs.end(); ++dc)
  {
    WrappedDataContainerPtr wrappedDataContainer = WrapDataContainerAsStruct((*dc));

    if(wrappedDataContainer)
    {
      wrappedDataContainers.push_back(wrappedDataContainer);
    }
  }

  return wrappedDataContainers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr SIMPLVtkBridge::WrapDataContainerAsStruct(DataContainer::Pointer dc)
{
  if(!dc)
  {
    return nullptr;
  }
  if(!dc->getGeometry())
  {
    return nullptr;
  }
  VTK_PTR(vtkDataSet) dataSet = WrapGeometry(dc->getGeometry());
  if(!dataSet)
  {
    return nullptr;
  }
  else
  {
    WrappedDataContainerPtr wrappedDcStruct(new WrappedDataContainer());
    wrappedDcStruct->m_DataSet = dataSet;
    wrappedDcStruct->m_DataContainer = dc;
    wrappedDcStruct->m_Name = dc->getName();

    DataContainer::Container_t attrMats = dc->getAttributeMatrices();

    for(DataContainer::Container_t::iterator attrMat = attrMats.begin(); attrMat != attrMats.end(); ++attrMat)
    {
      if(!(*attrMat))
      {
        continue;
      }
      // Wrap Cell data
      if(AttributeMatrix::Type::Cell == (*attrMat)->getType())
      {
        wrappedDcStruct->m_CellData = WrapAttributeMatrixAsStructs((*attrMat));

        // Add vtkDataArrays to the vtkDataSet
        vtkCellData* cellData = dataSet->GetCellData();
        for(WrappedDataArrayPtr wrappedCellData : wrappedDcStruct->m_CellData)
        {
          cellData->AddArray(wrappedCellData->m_VtkArray);
        }

        // Create point data from cell data
        VTK_NEW(vtkCellDataToPointData, cell2Point);
        cell2Point->SetInputData(dataSet);
        cell2Point->PassCellDataOn();
        cell2Point->Update();

        VTK_PTR(vtkDataSet) pointDataSet = nullptr;
        switch(dataSet->GetDataObjectType())
        {
        case VTK_IMAGE_DATA:
          pointDataSet = cell2Point->GetImageDataOutput();
          break;
        case VTK_STRUCTURED_GRID:
          pointDataSet = cell2Point->GetUnstructuredGridOutput();
          break;
        case VTK_RECTILINEAR_GRID:
          pointDataSet = cell2Point->GetRectilinearGridOutput();
          break;
        case VTK_STRUCTURED_POINTS:
          pointDataSet = cell2Point->GetStructuredPointsOutput();
          break;
        case VTK_UNSTRUCTURED_GRID:
          pointDataSet = cell2Point->GetUnstructuredGridOutput();
          break;
        case VTK_POLY_DATA:
          pointDataSet = cell2Point->GetPolyDataOutput();
          break;
        default:
          pointDataSet = cell2Point->GetOutput();
          break;
        }

        dataSet->DeepCopy(pointDataSet);
      }
      // Wrap Vertex data
      else if(AttributeMatrix::Type::Vertex == (*attrMat)->getType())
      {
        wrappedDcStruct->m_PointData = WrapAttributeMatrixAsStructs((*attrMat));

        // Add vtkDataArrays to the vtkDataSet
        vtkPointData* pointData = dataSet->GetPointData();
        for(WrappedDataArrayPtr wrappedPointData : wrappedDcStruct->m_PointData)
        {
          pointData->AddArray(wrappedPointData->m_VtkArray);
        }
      }
      else
      {
        continue;
      }

      // Set the active cell / point data scalars
      vtkPointData* pointData = dataSet->GetPointData();
      if(pointData->GetNumberOfArrays() > 0)
      {
        pointData->SetActiveScalars(pointData->GetArray(0)->GetName());
      }

      vtkCellData* cellData = dataSet->GetCellData();
      if(cellData->GetNumberOfArrays() > 0)
      {
        cellData->SetActiveScalars(cellData->GetArray(0)->GetName());
      }
    }

    return wrappedDcStruct;
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataContainerPtr SIMPLVtkBridge::WrapGeometryPtr(DataContainer::Pointer dc)
{
  if(!dc)
  {
    return nullptr;
  }
  if(!dc->getGeometry())
  {
    return nullptr;
  }
  VTK_PTR(vtkDataSet) dataSet = WrapGeometry(dc->getGeometry());
  if(!dataSet)
  {
    return nullptr;
  }
  else
  {
    WrappedDataContainerPtr wrappedDcStruct(new WrappedDataContainer());
    wrappedDcStruct->m_DataSet = dataSet;
    wrappedDcStruct->m_DataContainer = dc;
    wrappedDcStruct->m_Name = dc->getName();
    wrappedDcStruct->m_ImportCellArrays = CreateImportSettings(GetCellArrayNames(wrappedDcStruct));
    wrappedDcStruct->m_ImportPointArrays = CreateImportSettings(GetPointArrayNames(wrappedDcStruct));

    return wrappedDcStruct;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::DataArrayImportSettings SIMPLVtkBridge::CreateImportSettings(QStringList arrayNames)
{
  DataArrayImportSettings importSettings;
  for(QString arrayName : arrayNames)
  {
    importSettings[arrayName] = true;
  }

  return importSettings;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList SIMPLVtkBridge::GetCellArrayNames(WrappedDataContainerPtr wrappedDc)
{
  QStringList arrayNames;
  for(AttributeMatrix::Pointer am : wrappedDc->m_DataContainer->getAttributeMatrices())
  {
    AttributeMatrix::Type amType = am->getType();
    if(::CellTypes.contains(amType))
    {
      QStringList daNames = am->getAttributeArrayNames();
      for(QString daName : daNames)
      {
        if(CanWrapDataArray(am->getAttributeArray(daName)))
        {
          arrayNames.push_back(am->getName() + "::" + daName);
        }
      }
    }
  }

  return arrayNames;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QStringList SIMPLVtkBridge::GetPointArrayNames(WrappedDataContainerPtr wrappedDc)
{
  QStringList arrayNames;
  for(AttributeMatrix::Pointer am : wrappedDc->m_DataContainer->getAttributeMatrices())
  {
    AttributeMatrix::Type amType = am->getType();
    if(::PointTypes.contains(amType))
    {
      QStringList daNames = am->getAttributeArrayNames();
      for(QString daName : daNames)
      {
        if(CanWrapDataArray(am->getAttributeArray(daName)))
        {
          arrayNames.push_back(am->getName() + "::" + daName);
        }
      }
    }
  }

  return arrayNames;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLVtkBridge::AppendAttrMatrixToNames(WrappedDataArrayPtrCollection& wrappedArrays)
{
  for(WrappedDataArrayPtr wrappedArray : wrappedArrays)
  {
    // Renaming requires an AttributeMatrix
    if(nullptr == wrappedArray->m_AttributeMatrix)
    {
      continue;
    }

    QString matrixPrefix = " [" + wrappedArray->m_AttributeMatrix->getName() + "]";
    QString arrayName = wrappedArray->m_SIMPLArray->getName();
    if(!arrayName.startsWith(matrixPrefix))
    {
      arrayName.append(matrixPrefix);
      wrappedArray->m_ArrayName = arrayName;
      wrappedArray->m_VtkArray->SetName(qPrintable(arrayName));
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLVtkBridge::HandleArrayNameCollisions(WrappedDataArrayPtrCollection& collection1, WrappedDataArrayPtrCollection& collection2)
{
  WrappedDataArrayPtrCollection allWrappings;
  allWrappings.insert(allWrappings.end(), collection1.begin(), collection1.end());
  allWrappings.insert(allWrappings.end(), collection2.begin(), collection2.end());

#if AM_APPEND == AM_COLLISIONS
  // Check for array name collisions
  bool hasCollision = false;
  int totalCount = allWrappings.size();
  for(int i = 1; i < totalCount && !hasCollision; i++)
  {
    for(int j = 0; j < i && !hasCollision; j++)
    {
      hasCollision = (allWrappings[i]->m_ArrayName == allWrappings[j]->m_ArrayName);
    }
  }
#elif AM_APPEND == AM_ALWAYS
  bool hasCollision = true;
#else
  bool hasCollision = false;
  return;
#endif

  // Handle Collisions
  if(hasCollision)
  {
    AppendAttrMatrixToNames(allWrappings);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLVtkBridge::MergeWrappedArrays(WrappedDataArrayPtrCollection& oldWrapping, const WrappedDataArrayPtrCollection& newWrapping)
{
  bool compatible = true;
  bool multipleAm = (oldWrapping.size() > 0 && newWrapping.size() > 0);

  if(multipleAm)
  {
    IDataArray::Pointer oldArray = oldWrapping[0]->m_SIMPLArray;
    IDataArray::Pointer newArray = newWrapping[0]->m_SIMPLArray;

    compatible = oldArray->getNumberOfTuples() == newArray->getNumberOfTuples();
  }

  if(compatible)
  {
    oldWrapping.insert(oldWrapping.end(), newWrapping.begin(), newWrapping.end());

#if AM_APPEND == AM_MULTIPLE
    if(multipleAm)
    {
      AppendAttrMatrixToNames(oldWrapping);
    }
#endif
  }

  return compatible;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLVtkBridge::WrapAttrMatrixData(AttributeMatrix::Pointer attrMat, WrappedDataArrayPtrCollection& wrappedCollection, const int tuplesReq, DataArrayImportSettings importList)
{
  if(nullptr == attrMat)
  {
    return false;
  }

  int numTuples = 0;
  QVector<size_t> tupleDims = attrMat->getTupleDimensions();
  int count = tupleDims.size();
  for(int i = 0; i < count; i++)
  {
    if(i == 0)
    {
      numTuples = tupleDims[i];
    }
    else
    {
      numTuples *= tupleDims[i];
    }
  }

  if(tuplesReq == numTuples)
  {
    WrappedDataArrayPtrCollection newCollectionData = WrapAttributeMatrixAsStructs(attrMat, importList);

    // Merge the new cell data into the existing data
    if(MergeWrappedArrays(wrappedCollection, newCollectionData))
    {
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void SIMPLVtkBridge::FinishWrappingDataContainerStruct(WrappedDataContainerPtr wrappedDcStruct)
{
  if(nullptr == wrappedDcStruct)
  {
    return;
  }

  VTK_PTR(vtkDataSet) dataSet = wrappedDcStruct->m_DataSet;
  wrappedDcStruct->m_CellData.clear();
  wrappedDcStruct->m_PointData.clear();
  DataContainer::Container_t amMap = wrappedDcStruct->m_DataContainer->getAttributeMatrices();

  for(DataContainer::Container_t::iterator amIter = amMap.begin(); amIter != amMap.end(); ++amIter)
  {
    AttributeMatrix::Pointer attrMat = (*amIter);

    if(!attrMat)
    {
      continue;
    }
    if(attrMat->getTupleDimensions().size() == 0)
    {
      continue;
    }

    // Wrap Cell / Point Data
    VTK_PTR(vtkDataSet) dataSet = wrappedDcStruct->m_DataSet;
    AttributeMatrix::Type amType = attrMat->getType();
    if(::CellTypes.contains(amType))
    {
      int numCells = dataSet->GetNumberOfCells();
      WrapAttrMatrixData(attrMat, wrappedDcStruct->m_CellData, numCells, wrappedDcStruct->m_ImportCellArrays);
    }
    else if(::PointTypes.contains(amType))
    {
      int numPoints = dataSet->GetNumberOfPoints();
      WrapAttrMatrixData(attrMat, wrappedDcStruct->m_PointData, numPoints, wrappedDcStruct->m_ImportPointArrays);
    }
  }

  // Handle Array Collisons before adding them to the vtkDataSet
  HandleArrayNameCollisions(wrappedDcStruct->m_CellData, wrappedDcStruct->m_PointData);

  // Add CellData to the vtkDataSet
  vtkCellData* cellData = dataSet->GetCellData();
  for(WrappedDataArrayPtr wrappedCellData : wrappedDcStruct->m_CellData)
  {
    QString matrixArrayName = wrappedCellData->m_AttributeMatrix->getName() + "::" + wrappedCellData->m_ArrayName;
    if(wrappedDcStruct->m_ImportCellArrays.find(matrixArrayName) != wrappedDcStruct->m_ImportCellArrays.end() &&
       wrappedDcStruct->m_ImportCellArrays[matrixArrayName] == true)
    {
      cellData->AddArray(wrappedCellData->m_VtkArray);
    }
    else
    {
      cellData->RemoveArray(qPrintable(wrappedCellData->m_ArrayName));
    }
  }
  // Remove unwanted arrays
  for(auto iter = wrappedDcStruct->m_ImportCellArrays.begin(); iter != wrappedDcStruct->m_ImportCellArrays.end(); iter++)
  {
    if(iter->second == false)
    {
      QStringList path = iter->first.split("::");
      QString amName = path[0];
      QString daName = path[1];
      cellData->RemoveArray(qPrintable(daName));
      cellData->RemoveArray(qPrintable("[" + amName + "]" + daName));
    }
  }
  cellData->Update();
  // Add PointData to the vtkDataSet
  vtkPointData* pointData = dataSet->GetPointData();
  for(WrappedDataArrayPtr wrappedPointData : wrappedDcStruct->m_PointData)
  {
    QString matrixArrayName = wrappedPointData->m_AttributeMatrix->getName() + "::" + wrappedPointData->m_ArrayName;
    if(wrappedDcStruct->m_ImportPointArrays.find(matrixArrayName) != wrappedDcStruct->m_ImportPointArrays.end() &&
       wrappedDcStruct->m_ImportPointArrays[matrixArrayName] == true)
    {
      pointData->AddArray(wrappedPointData->m_VtkArray);
    }
    else
    {
      pointData->RemoveArray(qPrintable(wrappedPointData->m_ArrayName));
    }
  }
  // Remove unwanted arrays
  for(auto iter = wrappedDcStruct->m_ImportPointArrays.begin(); iter != wrappedDcStruct->m_ImportPointArrays.end(); iter++)
  {
    if(iter->second == false)
    {
      QStringList path = iter->first.split("::");
      QString amName = path[0];
      QString daName = path[1];
      cellData->RemoveArray(qPrintable(daName));
      cellData->RemoveArray(qPrintable("[" + amName + "]" + daName));
    }
  }
  pointData->Update();

  // Set the active cell / point data scalars
  if(pointData->GetNumberOfArrays() > 0)
  {
    pointData->SetActiveScalars(pointData->GetArray(0)->GetName());
  }

  if(cellData->GetNumberOfArrays() > 0)
  {
    cellData->SetActiveScalars(cellData->GetArray(0)->GetName());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataArrayPtrCollection SIMPLVtkBridge::WrapAttributeMatrixAsStructs(AttributeMatrix::Pointer attrMat, DataArrayImportSettings importList)
{
  WrappedDataArrayPtrCollection wrappedDataArrays;

  if(!attrMat)
  {
    return wrappedDataArrays;
  }

  QString matrixName = attrMat->getName();
  QStringList arrayNames = attrMat->getAttributeArrayNames();
  for(QString arrayName : arrayNames)
  {
    IDataArray::Pointer array = attrMat->getAttributeArray(arrayName);
    if(nullptr == array)
    {
      continue;
    }
    QString matrixArrayName = matrixName + "::" + arrayName;
    if(importList.find(matrixArrayName) != importList.end() && importList.at(matrixArrayName) == false)
    {
      // If the user specified that this array should not be imported, skip it
      continue;
    }

    WrappedDataArrayPtr wrappedDataArray = WrapIDataArrayAsStruct(array);
    if(wrappedDataArray)
    {
      wrappedDataArray->m_AttributeMatrix = attrMat;
      wrappedDataArrays.push_back(wrappedDataArray);
    }
  }

  return wrappedDataArrays;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPLVtkBridge::WrappedDataArrayPtr SIMPLVtkBridge::WrapIDataArrayAsStruct(IDataArray::Pointer dataArray)
{
  if(!dataArray)
  {
    return nullptr;
  }

  VTK_PTR(vtkDataArray) vtkArray = WrapIDataArray(dataArray);
  if(!vtkArray)
  {
    return nullptr;
  }
  else
  {
    WrappedDataArrayPtr wrappedDataArray(new WrappedDataArray());

    QString arrayName = dataArray->getName();
    vtkArray->SetName(qPrintable(arrayName));

    wrappedDataArray->m_ArrayName = arrayName;
    wrappedDataArray->m_SIMPLArray = dataArray;
    wrappedDataArray->m_VtkArray = vtkArray;

    return wrappedDataArray;
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataArray) SIMPLVtkBridge::WrapVertices(SharedVertexList::Pointer vertexArray)
{
  VTK_NEW(vtkFloatArray, vtkArray);
  vtkArray->SetNumberOfComponents(vertexArray->getNumberOfComponents());
  vtkArray->SetNumberOfTuples(vertexArray->getNumberOfTuples());

  vtkArray->SetVoidArray(vertexArray->getVoidPointer(0), vertexArray->getSize(), 1);

  return vtkArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(EdgeGeom::Pointer geom)
{
  VTK_NEW(VSEdgeGrid, dataSet);
  VSEdgeGeom* edgeGeom = dataSet->GetImplementation();
  edgeGeom->SetGeometry(geom);

  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(QuadGeom::Pointer geom)
{
  VTK_NEW(VSQuadGrid, dataSet);
  VSQuadGeom* quadGeom = dataSet->GetImplementation();
  quadGeom->SetGeometry(geom);

  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(RectGridGeom::Pointer geom)
{
  // vtkRectilinearGrid requires 3 arrays with the x, y, and z coordinates of points
  VTK_PTR(vtkDataArray) xCoords = WrapIDataArray(geom->getXBounds());
  VTK_PTR(vtkDataArray) yCoords = WrapIDataArray(geom->getYBounds());
  VTK_PTR(vtkDataArray) zCoords = WrapIDataArray(geom->getZBounds());

  int x = xCoords->GetNumberOfTuples();
  int y = yCoords->GetNumberOfTuples();
  int z = zCoords->GetNumberOfTuples();

  // Create the vtkRectilinearGrid and apply values
  VTK_NEW(vtkRectilinearGrid, vtkRectGrid);
  vtkRectGrid->SetXCoordinates(xCoords);
  vtkRectGrid->SetYCoordinates(yCoords);
  vtkRectGrid->SetZCoordinates(zCoords);
  vtkRectGrid->SetExtent(0, x, 0, y, 0, z);
  vtkRectGrid->SetDimensions(x, y, z);

  return vtkRectGrid;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(TetrahedralGeom::Pointer geom)
{
  VTK_NEW(VSTetrahedralGrid, dataSet);
  VSTetrahedralGeom* tetGeom = dataSet->GetImplementation();
  tetGeom->SetGeometry(geom);

  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(TriangleGeom::Pointer geom)
{
  VTK_NEW(VtkTriangleGrid, dataSet);
  VSTriangleGeom* triGeom = dataSet->GetImplementation();
  triGeom->SetGeometry(geom);

  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(VertexGeom::Pointer geom)
{
  VTK_NEW(VSVertexGrid, dataSet);
  VSVertexGeom* vertGeom = dataSet->GetImplementation();
  vertGeom->SetGeometry(geom);

  VTK_NEW(vtkPoints, points);
  VTK_PTR(vtkDataArray) vertexArray = WrapVertices(geom->getVertices());
  points->SetDataTypeToFloat();
  points->SetData(vertexArray);
  dataSet->SetPoints(points);

  return dataSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(ImageGeom::Pointer image)
{
  FloatVec3Type res = image->getSpacing();
  FloatVec3Type origin = image->getOrigin();
  SizeVec3Type dims = image->getDimensions();

  VTK_NEW(vtkImageData, vtkImage);
//  vtkImage->SetExtent(origin[0], origin[0] + std::get<0>(dims), origin[1], origin[1] + std::get<1>(dims), origin[2], origin[2] + std::get<2>(dims));
  vtkImage->SetDimensions(dims[0] + 1, dims[1] + 1, dims[2] + 1);
  vtkImage->SetSpacing(res[0], res[1], res[2]);
  vtkImage->SetOrigin(origin[0], origin[1], origin[2]);

  return vtkImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataSet) SIMPLVtkBridge::WrapGeometry(IGeometry::Pointer geom)
{
  if(std::dynamic_pointer_cast<EdgeGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<EdgeGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<ImageGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<ImageGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<QuadGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<QuadGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<RectGridGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<RectGridGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<TetrahedralGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<TetrahedralGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<TriangleGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<TriangleGeom>(geom));
  }
  else if(std::dynamic_pointer_cast<VertexGeom>(geom))
  {
    return WrapGeometry(std::dynamic_pointer_cast<VertexGeom>(geom));
  }

  // Default to nullptr if the type does not match a supported geometry
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkDataArray) SIMPLVtkBridge::WrapIDataArray(IDataArray::Pointer array)
{
  if(std::dynamic_pointer_cast<UInt8ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedCharArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int8ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkCharArray>(array);
  }
  else if(std::dynamic_pointer_cast<UInt16ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedShortArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int16ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkShortArray>(array);
  }
  else if(std::dynamic_pointer_cast<UInt32ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedIntArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int32ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkIntArray>(array);
  }
  else if(std::dynamic_pointer_cast<UInt64ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkUnsignedLongLongArray>(array);
  }
  else if(std::dynamic_pointer_cast<Int64ArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkLongLongArray>(array);
  }
  else if(std::dynamic_pointer_cast<FloatArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkFloatArray>(array);
  }
  else if(std::dynamic_pointer_cast<DoubleArrayType>(array))
  {
    return WrapIDataArrayTemplate<vtkDoubleArray>(array);
  }
  else
  {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool SIMPLVtkBridge::CanWrapDataArray(IDataArray::Pointer array)
{
  if(std::dynamic_pointer_cast<UInt8ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<Int8ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<UInt16ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<Int16ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<UInt32ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<Int32ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<UInt64ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<Int64ArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<FloatArrayType>(array))
  {
    return true;
  }
  else if(std::dynamic_pointer_cast<DoubleArrayType>(array))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
// VSRenderController::Pointer SIMPLVtkBridge::WrapImageGeomAsVtkImageData(ImageGeom::Pointer image, FloatArrayType::Pointer cellData)
//{
//  size_t dims[3] = {0, 0, 0};
//  float res[3] = {0.0f, 0.0f, 0.0f};
//  float origin[3] = {0.0f, 0.0f, 0.0f};
//
//  image->getDimensions(dims);
//  image->getResolution(res);
//  image->getOrigin(origin);
//
//  VTK_NEW(vtkImageData, vtk_image);
//  vtk_image->SetExtent(0, dims[0], 0, dims[1], 0, dims[2]);
//  vtk_image->SetDimensions(dims[0] + 1, dims[1] + 1, dims[2] + 1);
//  vtk_image->SetSpacing(res[0], res[1], res[2]);
//  vtk_image->SetOrigin(origin[0], origin[1], origin[2]);
//
//  VTK_NEW(vtkFloatArray, flt);
//  flt->SetVoidArray(cellData->getVoidPointer(0), cellData->getSize(), 1);
//
//  vtk_image->GetCellData()->SetScalars(flt);
//
//  std::shared_ptr<VSRenderController::VtkDataViewStruct_t> vtkDataViewStruct(new VSRenderController::VtkDataViewStruct_t());
//  vtkDataViewStruct->Name = cellData->getName();
//
//  std::shared_ptr<VSRenderController::VtkDataSetStruct_t> vtkDataSetStruct(new VSRenderController::VtkDataSetStruct_t());
//  vtkDataSetStruct->DataSet = vtk_image;
//  vtkDataSetStruct->Name = image->getName();
//  vtkDataSetStruct->ViewOptions.push_back(vtkDataViewStruct);
//
//  VSRenderController::VtkDataStructVector_t vtkDataStructVector;
//  vtkDataStructVector.push_back(vtkDataSetStruct);
//
//  VSRenderController::Pointer vtkDataBlob = VSRenderController::CreateVSRenderController(vtkDataStructVector);
//
//  return vtkDataBlob;
//}
