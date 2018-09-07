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

#include "VSSliceValues.h"

#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSSliceValues::VSSliceValues(VSSliceFilter* filter)
: VSAbstractFilterValues(filter)
, m_PlaneWidget(new VSPlaneWidget(nullptr, filter->getTransform(), filter->getBounds(), nullptr))
{
  connect(m_PlaneWidget, &VSPlaneWidget::modified, this, &VSSliceValues::alertChangesWaiting);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceValues::applyValues()
{
  VSAbstractFilter::FilterListType filters = getSelection();
  for(VSAbstractFilter* filter : filters)
  {
    // Make sure this is the appropriate filter type first
    FilterType* filterType = dynamic_cast<FilterType*>(filter);
    if(filterType)
    {
      filterType->applyValues(this);

      // Disable the draw plane
      m_PlaneWidget->drawPlaneOff();
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceValues::resetValues()
{
  if(nullptr == dynamic_cast<VSSliceFilter*>(getFilter()))
  {
    return;
  }
  VSSliceFilter* filter = dynamic_cast<VSSliceFilter*>(getFilter());

  // Reset PlaneWidget
  m_PlaneWidget->setOrigin(filter->getLastOrigin());
  m_PlaneWidget->setNormal(filter->getLastNormal());

  m_PlaneWidget->drawPlaneOff();
  m_PlaneWidget->updatePlaneWidget();

  if(getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSSliceValues::hasChanges() const
{
  VSSliceFilter* filter = dynamic_cast<VSSliceFilter*>(getFilter());
  double* lastNormal = filter->getLastNormal();
  double* lastOrigin = filter->getLastOrigin();

  return !getPlaneWidget()->equals(lastOrigin, lastNormal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget* VSSliceValues::getPlaneWidget() const
{
  return m_PlaneWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceValues::setInteractor(vtkRenderWindowInteractor* interactor)
{
  VSAbstractFilterValues::setInteractor(interactor);

  m_PlaneWidget->setInteractor(interactor);
  updateRendering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceValues::setRenderingEnabled(bool enabled)
{
  VSAbstractFilterValues::setRenderingEnabled(enabled);

  m_PlaneWidget->setEnabled(enabled);
  updateRendering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSSliceValues::updateRendering()
{
  if(isRenderingEnabled() && getInteractor())
  {
    m_PlaneWidget->enable();
  }
  else
  {
    m_PlaneWidget->disable();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSSliceValues::createFilterWidget()
{
  return m_PlaneWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSSliceValues::getOrigin() const
{
  return m_PlaneWidget->getOrigin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSSliceValues::getNormal() const
{
  return m_PlaneWidget->getNormal();
}
