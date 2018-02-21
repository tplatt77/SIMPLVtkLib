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

#pragma once

#include <QtCore/QObject>

#include <vtkMatrix4x4.h>
#include <vtkTransform.h>

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"

/**
* @class VSTransform VSTransform.h SIMPLVtkLib/Visualization/VisualFilters/VSTransform.h
* @brief This class stores the local coordinates for placing an object in 3D 
* space including local values for position, rotation, and scale. Global values
* are found by applying the local value to the parent's global value.
*/
class VSTransform : public QObject
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  */
  VSTransform(VSTransform* parent = nullptr);

  /**
  * @brief Sets the parent transform
  * @param parent
  */
  void setParent(VSTransform* parent);

  /**
  * @brief Returns the parent transform
  * @return
  */
  VSTransform* getParent();

  /**
  * @brief Returns the transform's global position in 3D space
  * @return
  */
  double* getPosition();

  /**
  * @brief Returns the transform's local position in 3D space
  * @return
  */
  double* getLocalPosition();
  
  /**
  * @brief Returns the transform's global Euler rotation
  * @return
  */
  double* getRotation();

  /**
  * @brief Returns the transform's local Euler rotation
  * @return
  */
  double* getLocalRotation();

  /**
  * @brief Returns the transform's global scale
  * @return
  */
  double* getScale();

  /**
  * @brief Returns the transform's local scale
  * @return
  */
  double* getLocalScale();

  /**
  * @brief Sets the transform's local position
  * @param position
  */
  void setLocalPosition(double position[3]);

  /**
  * @brief Sets the transform's local Euler rotation
  * @param rotation
  */
  void setLocalRotation(double rotation[3]);

  /**
  * @brief Sets the transform's local scale
  * @param scale
  */
  void setLocalScale(double scale[3]);

  /**
  * @brief Returns the vtkTransform describing the global position, rotation, and scale.
  * @return
  */
  VTK_PTR(vtkTransform) getVtkTransform();

signals:
  void updatedPosition(double* position);
  void updatedRotation(double* rotation);
  void updatedScale(double* scale);
  void emitPosition();
  void emitRotation();
  void emitScale();
  void emitAll();
  void valuesChanged();

protected:
  /**
  * @brief Returns the rotation matrix for the current global rotation.
  * @return
  */
  VTK_PTR(vtkMatrix4x4) getRotationMatrix();

  /**
  * @brief Returns the vtkTransform describing the local position, rotation, and scale.
  * @return
  */
  VTK_PTR(vtkTransform) getLocalVtkTransform();
private:
  VSTransform* m_Parent = nullptr;

  double m_LocalPosition[3];
  double m_LocalRotation[3];
  double m_LocalScale[3];
};
