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

#pragma once

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Winconsistent-missing-override"
#endif

#include <string>

#include <vtkDataArray.h>
#include <vtkDataSet.h>

#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainerArray.h"
#include "SIMPLib/Geometry/EdgeGeom.h"
#include "SIMPLib/Geometry/ImageGeom.h"
#include "SIMPLib/Geometry/QuadGeom.h"
#include "SIMPLib/Geometry/RectGridGeom.h"
#include "SIMPLib/Geometry/TetrahedralGeom.h"
#include "SIMPLib/Geometry/TriangleGeom.h"
#include "SIMPLib/Geometry/VertexGeom.h"

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"
#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkImageData;
class vtkDataArray;
class vtkScalarsToColors;
class vtkScalarBarActor;

/**
 * @class SIMPLVtkBridge SIMPLVtkBridge.h SIMPLVtkLib/SIMPLBridge/SIMPLVtkBridge.h
 * @brief This class handles conversion of data from SIMPLib to VTK through the
 * wrapping of DataContainerArrays, DataContainers, AttributeMatrices, and DataArrays
 * to collections of vtkDataSets and vtkDataArrays.
 */
class SIMPLVtkLib_EXPORT SIMPLVtkBridge
{
public:
  struct WrappedDataArray
  {
    QString m_ArrayName;
    AttributeMatrix::Pointer m_AttributeMatrix = nullptr;
    IDataArray::Pointer m_SIMPLArray = nullptr;
    VTK_PTR(vtkDataArray) m_VtkArray = nullptr;
  };

  using WrappedDataArrayPtr = std::shared_ptr<WrappedDataArray>;
  using WrappedDataArrayPtrCollection = std::vector<WrappedDataArrayPtr>;
  using DataArrayImportSettings = std::map<QString, bool>;

  struct WrappedDataContainer
  {
    VTK_PTR(vtkDataSet) m_DataSet = nullptr;
    WrappedDataArrayPtrCollection m_CellData;
    WrappedDataArrayPtrCollection m_PointData;
    QString m_Name;
    DataContainer::Pointer m_DataContainer = nullptr;
    double m_Origin[3] = {0.0, 0.0, 0.0};
    DataArrayImportSettings m_ImportCellArrays;
    DataArrayImportSettings m_ImportPointArrays;
  };

  using WrappedDataContainerPtr = std::shared_ptr<WrappedDataContainer>;
  using WrappedDataContainerPtrCollection = std::vector<WrappedDataContainerPtr>;

  /**
   * @brief Deconstructor
   */
  virtual ~SIMPLVtkBridge() = default;

  /**
   * @brief Wraps the DataContainers from SIMPLib's DataContainerArray in vtkDataSets where applicable
   * and returns a vector of WrappedDataContainerPtrs with information about the vtkDataSets.
   * Not all DataContainers can be wrapped in a vtkDataSet.
   * @param dca
   * @return
   */
  static WrappedDataContainerPtrCollection WrapDataContainerArrayAsStruct(DataContainerArray::Pointer dca);

  /**
   * @brief Wraps a DataContainer from SIMPLib in a vtkDataSet if applicable and returns a WrappedDataContainerPtr
   * with information about the vtkDataSet including smart pointers to both the SIMPLib DataArrays and vtkDataArrays.
   * Because data is not copied from SIMPLib's DataArrays, the data will be lost if all references to its smart pointers
   * go out of scope.
   * @param dc
   * @param types
   * @return
   */
  static WrappedDataContainerPtr WrapDataContainerAsStruct(DataContainer::Pointer dc);

  /**
   * @brief Wraps a DataContainer geometry from SIMPLib in a vtkDataSet if applicable and returns a WrappeddataContainerPtr
   * without any data arrays wrapped and imported. A nullptr is returned if there are no eligible AttributeMatrixes to wrap.
   * @param dc
   * @param types
   * @return
   */
  static WrappedDataContainerPtr WrapGeometryPtr(DataContainer::Pointer dc);

  /**
   * @brief Finish wrapping the given DataContainer.
   * This should never be called outside the main thread.
   * @param wrappedDc
   */
  static void FinishWrappingDataContainerStruct(WrappedDataContainerPtr wrappedDc);

  /**
   * @brief Wraps the DataArrays contained within SIMPLib's AttributeMatrix in vtkDataArrays for use in VTK
   * and returns a vector of structs containing information about those vtkDataArrays
   * @param am
   * @param importList
   * @return
   */
  static WrappedDataArrayPtrCollection WrapAttributeMatrixAsStructs(AttributeMatrix::Pointer am, DataArrayImportSettings importList = DataArrayImportSettings());

  /**
   * @brief Wraps a DataArray from SIMPLib in a vtkDataArray and returns a struct containing the wrapped
   * DataArray's name and smart pointers to both the SIMPLib and VTK data.  The vtkDataArray does not copy
   * the SIMPLib data, meaning that the SIMPLib DataArray cannot go out of scope before the vtkDataArray
   * or the data will not be accessible.
   * @param da
   * @return
   */
  static WrappedDataArrayPtr WrapIDataArrayAsStruct(IDataArray::Pointer da);

  /**
   * @brief Creates and returns DataArrayImportSettings for the given array names
   * @param arrayNames
   * @return
   */
  static DataArrayImportSettings CreateImportSettings(QStringList arrayNames);

  /**
   * @brief Returns a QStringList of all cell array names
   * @param wrappedDc
   * @return
   */
  static QStringList GetCellArrayNames(WrappedDataContainerPtr wrappedDc);

  /**
   * @brief Returns a QStringList of all point array names
   * @param wrappedDc
   * @return
   */
  static QStringList GetPointArrayNames(WrappedDataContainerPtr wrappedDc);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's EdgeGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(EdgeGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's ImageGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(ImageGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's QuadGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(QuadGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's RectGridGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(RectGridGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's TetrahedralGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(TetrahedralGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's TriangleGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(TriangleGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's VertexGeom
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(VertexGeom::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataSet from SIMPLib's IGeometry
   * @param geom
   * @return
   */
  static VTK_PTR(vtkDataSet) WrapGeometry(IGeometry::Pointer geom);

  /**
   * @brief Creates and returns a vtkDataArray from SIMPLib's SharedVertexList
   * @param vertexArray
   * @return
   */
  static VTK_PTR(vtkDataArray) WrapVertices(SharedVertexList::Pointer vertexArray);

  /**
   * @brief Creates and returns a vtkDataArray from SIMPLib's IDataArray
   * @param array
   * @return
   */
  static VTK_PTR(vtkDataArray) WrapIDataArray(IDataArray::Pointer array);

  template <typename T> static VTK_PTR(T) WrapIDataArrayTemplate(IDataArray::Pointer array)
  {
    VTK_NEW(T, vtkArray);
    vtkArray->SetNumberOfComponents(array->getNumberOfComponents());
    vtkArray->SetNumberOfTuples(array->getNumberOfTuples());

    vtkArray->SetVoidArray(array->getVoidPointer(0), array->getSize(), 1);

    int numComp = vtkArray->GetNumberOfComponents();
    bool isCharArray = vtkArray->IsA("vtkUnsignedCharArray");
    bool isFloatArray = vtkArray->IsA("vtkFloatArray");
    if(isCharArray && numComp == 3)
    {
      QString arrayName = array->getName();
      vtkArray->SetComponentName(0, qPrintable(arrayName + " R"));
      vtkArray->SetComponentName(1, qPrintable(arrayName + " G"));
      vtkArray->SetComponentName(2, qPrintable(arrayName + " B"));
    }
    else if(isFloatArray && numComp == 3)
    {
      QString arrayName = array->getName();
      vtkArray->SetComponentName(0, qPrintable(arrayName + " X"));
      vtkArray->SetComponentName(1, qPrintable(arrayName + " Y"));
      vtkArray->SetComponentName(2, qPrintable(arrayName + " Z"));
    }
    else
    {
      for(int i = 0; i < numComp; i++)
      {
        QString compName = array->getName() + " Comp_" + QString::number(i + 1);
        vtkArray->SetComponentName(i, qPrintable(compName));
      }
    }

    return vtkArray;
  }

protected:
  /**
   * @brief Constructor
   */
  SIMPLVtkBridge();

  /**
   * @brief Appends the AttributeMatrix name to the wrapped DataArray names
   * @param wrappedArrays
   */
  static void AppendAttrMatrixToNames(WrappedDataArrayPtrCollection& wrappedArrays);

  /**
   * @brief Checks for and handles DataArrays with the same name between two collections.
   * This is called after wrapping both cell and point data to prevent the two from listing the same array.
   * @param collection1
   * @param collection2
   */
  static void HandleArrayNameCollisions(WrappedDataArrayPtrCollection& collection1, WrappedDataArrayPtrCollection& collection2);

  /**
   * @brief Merges the WrappedDataArrayPtrCollection from the given AttributeMatrix into the wrapped DataContainer
   * @param wrappedDc
   * @param am
   */
  static bool MergeWrappedArrays(WrappedDataArrayPtrCollection& oldWrapping, const WrappedDataArrayPtrCollection& newWrapping);

  /**
   * @brief Wraps the given AttributeMatrix and merges it into the given collection if it matches the required tuple count
   * @param wrappedDcStruct
   * @param am
   * @param wrappedCollection
   * @param tuplesReq
   * @param importList
   */
  static bool WrapAttrMatrixData(AttributeMatrix::Pointer am, WrappedDataArrayPtrCollection& wrappedCollection, const int tuplesReq, DataArrayImportSettings importList);

  /**
   * @brief Returns true if the given IDataArray can be wrapped. Returns false otherwise.
   * @param array
   * @return
   */
  static bool CanWrapDataArray(IDataArray::Pointer array);

private:
  SIMPLVtkBridge(const SIMPLVtkBridge&); // Copy Constructor Not Implemented
  void operator=(const SIMPLVtkBridge&); // Operator '=' Not Implemented
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
