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

#include "VSFilterViewSettings.h"

#include <vtkColorTransferFunction.h>
#include <vtkDataSetMapper.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::VSFilterViewSettings(VSAbstractFilter* filter)
  : QObject(nullptr)
  , m_Filter(filter)
{
  setupActors();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewSettings::VSFilterViewSettings(const VSFilterViewSettings& copy)
  : QObject(nullptr)
  , m_Filter(copy.m_Filter)
  , m_ShowFilter(copy.m_ShowFilter)
  , m_ActiveArray(copy.m_ActiveArray)
  , m_ActiveComponent(copy.m_ActiveComponent)
  , m_MapColors(copy.m_MapColors)
  , m_ShowScalarBar(copy.m_ShowScalarBar)
{
  setupActors();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterViewSettings::getFilter()
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::getVisible()
{
  return m_ShowFilter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getActiveArrayIndex()
{
  return m_ActiveArray;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSFilterViewSettings::getActiveComponentIndex()
{
  return m_ActiveComponent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::getMapColors()
{
  return m_MapColors;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewSettings::isScalarBarVisible()
{
  return m_ShowScalarBar;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkActor) VSFilterViewSettings::getActor()
{
  return m_Actor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkScalarBarWidget) VSFilterViewSettings::getScalarBarWidget()
{
  return m_ScalarBarWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::show()
{
  setVisible(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::hide()
{
  setVisible(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setVisible(bool visible)
{
  m_ShowFilter = visible;

  emit visibilityChanged(m_ShowFilter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setActiveArrayIndex(int index)
{
  m_ActiveArray = index;

  emit activeArrayIndexChanged(m_ActiveArray);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setActiveComponentIndex(int index)
{
  m_ActiveComponent = index;

  emit activeComponentIndexChanged(m_ActiveComponent);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setMapColors(bool mapColors)
{
  m_MapColors = mapColors;

  emit mapColorsChanged(m_MapColors);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setScalarBarVisible(bool visible)
{
  m_ShowScalarBar = visible;

  emit showScalarBarChanged(m_ShowScalarBar);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewSettings::setupActors()
{
  m_Mapper = VTK_PTR(vtkDataSetMapper)::New();
  //m_Mapper->SetInputConnection(m_Filter->getOutputPort());

  m_Actor = VTK_PTR(vtkActor)::New();
  m_Actor->SetMapper(m_Mapper);

  m_LookupTable = new VSLookupTableController();
  //m_LookupTable->getColorTransferFunction();

  m_ScalarBarActor = VTK_PTR(vtkScalarBarActor)::New();
  m_ScalarBarWidget = VTK_PTR(vtkScalarBarWidget)::New();
  m_ScalarBarWidget->SetScalarBarActor(m_ScalarBarActor);
}
