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

#ifndef _simplvtkarray_h_
#define _simplvtkarray_h_

#include "vtkGenericDataArray.h"

#include "SIMPLib/DataArrays/DataArray.hpp"

template <class T> class SIMPLVtkArray : public vtkGenericDataArray<SIMPLVtkArray<T>>, T >
{
public:
  /**
   * @brief Constructor
   * @param dataArray
   */
  SIMPLVtkArray(DataArray<T>::Pointer dataArray)
  : vtkGenericDataArray<SIMPLVtkArray<T>, T>()
  , m_DataArray(dataArray)
  {
  }

  /**
   * @brief Returns the value for the given ID
   * @param valueIdx
   * @return
   */
  inline T GetValue(vtkIdType valueIdx) const
  {
    return m_DataArray->getValue(valueIdx);
  }

  /**
   * @brief Sets the value for the given ID
   * @param valueIdx
   * @param value
   */
  inline void SetValue(vtkIdType valueIdx, T value)
  {
    m_DataArray->setValue(valueIdx, value);
  }

  /**
   * @brief Copies the tuple pointer for the given index
   * @param tupleIdx
   * @param tuple
   */
  inline void GetTypedTuple(vtkIdType tupleIdx, T* tuple)
  {
    tuple = m_DataArray->getTuplePointer(tupleIdx);
  }

  /**
   * @brief Sets the tuple pointer for the given index
   * @param tupleIdx
   * @param tuple
   */
  inline void SetTypedTuple(vtkIdType tupleIdx, const T* tuple)
  {
    m_DataArray->initializeTuple(tupleIdx, tuple);
  }

  /**
   * @brief Returns the component specified by tuple and component ID
   * @param tupleIdx
   * @param compIdx
   * @return
   */
  inline T GetTypedComponent(vtkIdType tupleIdx, int compIdx) const
  {
    return m_DataArray->getComponent(tupleIdx, compIdx);
  }

  /**
   * @brief Sets the component specified by tuple and component ID
   * @param tupleIdx
   * @param compIdx
   * @param value
   */
  inline void SetTypedComponent(vtkIdType tupleIdx, int compIdx, T value)
  {
    m_DataArray->setComponent(tupleIdx, compIdx, value);
  }

protected:
  /**
   * @brief Allocates space for a given number of tuples. Old data should not be preserved
   * @param numTuples
   * @return
   */
  inline bool AllocateTuples(vtkIdType numTuples)
  {
    m_DataArray->resize(numTuples);
  }

  /**
   * @brief Allocates space for a given number of tuples.  Old data should be preserved
   * @param numTuples
   * @return
   */
  inline bool ReallocateTuples(vtkIdType numTuples)
  {
    m_DataArray->resize(numTuples);
  }

private:
  DataArray<T>::Pointer m_DataArray;
};

#endif
