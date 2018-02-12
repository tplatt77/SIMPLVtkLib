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

#include "VSTransform.h"

#include <cmath>

#include <vtkTransform.h>

#include "SIMPLVtkLib/SIMPLBridge/VtkMacros.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform::VSTransform(VSTransform* parent)
  : m_Parent(parent)
{
  for(int i = 0; i < 3; i++)
  {
    m_Position[i] = 0.0;
    m_Rotation[i] = 0.0;
    m_Scale[i] = 1.0;
  }

  connect(this, &VSTransform::emitPosition, this, [=] { emit updatedPosition(getPosition()); });
  connect(this, &VSTransform::emitRotation, this, [=] { emit updatedRotation(getRotation()); });
  connect(this, &VSTransform::emitScale, this, [=] { emit updatedScale(getScale()); });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setParent(VSTransform* parent)
{
  if(m_Parent)
  {
    disconnect(m_Parent, SIGNAL(emitPosition()), this, SIGNAL(emitPosition()));
    disconnect(m_Parent, SIGNAL(emitRotation()), this, SIGNAL(emitRotation()));
    disconnect(m_Parent, SIGNAL(emitScale()), this, SIGNAL(emitScale()));
  }

  m_Parent = parent;

  if(parent)
  {
    connect(m_Parent, SIGNAL(emitPosition()), this, SIGNAL(emitPosition()));
    connect(m_Parent, SIGNAL(emitRotation()), this, SIGNAL(emitRotation()));
    connect(m_Parent, SIGNAL(emitScale()), this, SIGNAL(emitScale()));
  }

  emit emitPosition();
  emit emitRotation();
  emit emitScale();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSTransform* VSTransform::getParent()
{
  return m_Parent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getPosition()
{
  if(nullptr == m_Parent)
  {
    return getLocalPosition();
  }

  double* position = new double[3];
  double* parentPosition = m_Parent->getPosition();
  for(int i = 0; i < 3; i++)
  {
    position[i] = m_Position[i] + parentPosition[i];
  }

  return position;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getLocalPosition()
{
  double* position = new double[3];
  for(int i = 0; i < 3; i++)
  {
    position[i] = m_Position[i];
  }

  return position;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::rotateEulerAngles(double start[3], double rotation[3])
{
  for(int i = 0; i < 3; i++)
  {
    if(abs(rotation[i]) < 0.0001)
    {
      rotation[i] = 0.0;
    }
  }

  VTK_NEW(vtkTransform, transform);
  transform->RotateZ(start[2]);
  transform->RotateY(start[1]);
  transform->RotateX(start[0]);

  transform->RotateZ(rotation[2]);
  transform->RotateY(rotation[1]);
  transform->RotateX(rotation[0]);

  double* finalRotation = new double[3];
  transform->GetOrientation(finalRotation);
  return finalRotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getRotation()
{
  if(nullptr == m_Parent)
  {
    return getLocalRotation();
  }

  double* parentRotation = m_Parent->getRotation();
  double* rotation = rotateEulerAngles(m_Rotation, parentRotation);

  return rotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getLocalRotation()
{
  double* rotation = new double[3];
  for(int i = 0; i < 3; i++)
  {
    rotation[i] = m_Rotation[i];
  }

  return rotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getScale()
{
  if(nullptr == m_Parent)
  {
    return getLocalScale();
  }

  double* scale = new double[3];
  double* parentScale = m_Parent->getScale();
  for(int i = 0; i < 3; i++)
  {
    scale[i] = m_Scale[i] * parentScale[i];
  }

  return scale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSTransform::getLocalScale()
{
  double* scale = new double[3];
  for(int i = 0; i < 3; i++)
  {
    scale[i] = m_Scale[i];
  }

  return scale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setPosition(double position[3])
{
  for(int i = 0; i < 3; i++)
  {
    m_Position[i] = position[i];
  }

  emit emitPosition();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setRotation(double rotation[3])
{
  for(int i = 0; i < 3; i++)
  {
    m_Rotation[i] = rotation[i];
  }

  emit emitRotation();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSTransform::setScale(double scale[3])
{
  for(int i = 0; i < 3; i++)
  {
    m_Scale[i] = scale[i];
  }

  emit emitScale();
}
