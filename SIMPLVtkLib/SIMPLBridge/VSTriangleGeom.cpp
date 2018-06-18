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

#include "VSTriangleGeom.h"

#include <cmath>

#include <vtkCellType.h>
#include <vtkCellTypes.h>
#include <vtkIdTypeArray.h>

const int CELL_TYPE = VTK_TRIANGLE;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VtkTriangleGrid* VtkTriangleGrid::New()
{
  return new VtkTriangleGrid();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTriangleGeom* VSTriangleGeom::New()
{
  VSTriangleGeom* geom = new VSTriangleGeom();
  return geom;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTriangleGeom::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Elements: " << GetNumberOfCells() << endl;
  os << indent << "CellType: " << vtkCellTypes::GetClassNameFromTypeId(CELL_TYPE) << endl;
  os << indent << "CellSize: " << GetMaxCellSize() << endl;
  os << indent << "NumberOfCells: " << GetNumberOfCells() << endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTriangleGeom::VSTriangleGeom()
: vtkObject()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTriangleGeom::SetGeometry(TriangleGeom::Pointer geom)
{
  m_Geom = geom;
  geom->findElementsContainingVert();
  geom->findElementSizes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSTriangleGeom::GetNumberOfCells()
{
  if(nullptr == m_Geom)
  {
    vtkErrorMacro("Wrapper Geometry missing a Geometry object");
    return -1;
  }

  return m_Geom->getNumberOfElements();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSTriangleGeom::GetCellType(vtkIdType cellId)
{
  if(0 == GetNumberOfCells())
  {
    return VTK_EMPTY_CELL;
  }

  return CELL_TYPE;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTriangleGeom::GetCellPoints(vtkIdType cellId, vtkIdList* ptIds)
{
  const int numVerts = 3;

  int64_t verts[numVerts];
  m_Geom->getVertsAtTri(cellId, verts);

  ptIds->SetNumberOfIds(numVerts);
  for(int i = 0; i < numVerts; i++)
  {
    ptIds->SetId(i, verts[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTriangleGeom::GetPointCells(vtkIdType ptId, vtkIdList* cellIds)
{
  ElementDynamicList::Pointer elementsContainingList = m_Geom->getElementsContainingVert();

  DynamicListArray<uint16_t, int64_t>::ElementList listArray = elementsContainingList->getElementList(ptId);

  cellIds->SetNumberOfIds(listArray.ncells);
  for(int i = 0; i < listArray.ncells; i++)
  {
    cellIds->SetId(i, listArray.cells[i]);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSTriangleGeom::GetMaxCellSize()
{
  return std::ceil(m_MaxCellSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTriangleGeom::GetIdsOfCellsOfType(int type, vtkIdTypeArray* array)
{
  if(CELL_TYPE == type)
  {
    int numValues = GetNumberOfCells();

    array = vtkIdTypeArray::New();
    array->SetNumberOfTuples(numValues);
    array->SetNumberOfComponents(1);

    vtkIdType* arrayValues = new vtkIdType[numValues];
    for(int i = 0; i < numValues; i++)
    {
      arrayValues[i] = i;
    }

    array->SetVoidArray(arrayValues, numValues, 0);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSTriangleGeom::IsHomogeneous()
{
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTriangleGeom::Allocate(vtkIdType numCells, int extSize)
{
  vtkErrorMacro("Read only container.");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSTriangleGeom::InsertNextCell(int type, vtkIdList* ptIds)
{
  vtkErrorMacro("Read only container.");
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSTriangleGeom::InsertNextCell(int type, vtkIdType npts, vtkIdType* ptIds)
{
  vtkErrorMacro("Read only container.");
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkIdType VSTriangleGeom::InsertNextCell(int type, vtkIdType npts, vtkIdType* ptIds, vtkIdType nfaces, vtkIdType* faces)
{
  // TriangleGeometry should probably not be modified by VTK calls
  // To prevent spamming errors when loading this data, do not call vtkErrorMacro
  return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTriangleGeom::ReplaceCell(vtkIdType cellId, int npts, vtkIdType* pts)
{
  vtkErrorMacro("Read only container.");
}
