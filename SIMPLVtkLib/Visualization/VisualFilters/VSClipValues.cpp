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


#include "VSClipValues.h"

#include "ui_VSClipFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipValues::VSClipValues(VSClipFilter* filter)
: VSAbstractFilterValues(filter)
, m_BoxWidget(new VSBoxWidget(nullptr, filter->getTransform(), filter->getBounds(), nullptr))
, m_PlaneWidget(new VSPlaneWidget(nullptr, filter->getTransform(), filter->getBounds(), nullptr))
{
  setClipType(VSClipFilter::ClipType::PLANE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::applyValues()
{
  VSAbstractFilter::FilterListType filters = getSelection();
  for(VSAbstractFilter* filter : filters)
  {
    // Make sure this is the appropriate filter type first
    VSClipFilter* clipFilter = dynamic_cast<VSClipFilter*>(filter);
    if(clipFilter)
    {
      clipFilter->applyValues(this);

      // Update visualization accordingly
      switch(m_ClipType)
      {
      case VSClipFilter::ClipType::PLANE:
        m_PlaneWidget->drawPlaneOff();
        break;
      case VSClipFilter::ClipType::BOX:
        break;
      default:
        break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::resetValues()
{
  if(nullptr == dynamic_cast<VSClipFilter*>(getFilter()))
  {
    return;
  }

  VSClipFilter* filter = dynamic_cast<VSClipFilter*>(getFilter());
  m_ClipType = filter->getLastClipType();

  // Reset BoxWidget
  m_BoxWidget->setTransform(filter->getLastBoxTransform());

  // Reset PlaneWidget
  m_PlaneWidget->setOrigin(filter->getLastPlaneOrigin());
  m_PlaneWidget->setNormals(filter->getLastPlaneNormal());

  // Reset Inverted
  if(VSClipFilter::ClipType::BOX == m_ClipType)
  {
    m_Inverted = filter->getLastBoxInverted();
  }
  else
  {
    m_Inverted = filter->getLastPlaneInverted();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::ClipType VSClipValues::getClipType() const
{
  return m_ClipType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setClipType(VSClipFilter::ClipType type)
{
  m_ClipType = type;

  updateRendering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipValues::isInverted() const
{
  return m_Inverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setInverted(bool inverted)
{
  m_Inverted = inverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget* VSClipValues::getPlaneWidget() const
{
  return m_PlaneWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSBoxWidget* VSClipValues::getBoxWidget() const
{
  return m_BoxWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setInteractor(vtkRenderWindowInteractor* interactor)
{
  VSAbstractFilterValues::setInteractor(interactor);

  m_BoxWidget->setInteractor(interactor);
  m_PlaneWidget->setInteractor(interactor);

  updateRendering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setRenderingEnabled(bool enabled)
{
  VSAbstractFilterValues::setRenderingEnabled(enabled);

  updateRendering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::updateRendering()
{
  if(isRenderingEnabled() && getInteractor())
  {
    switch(m_ClipType)
    {
    case VSClipFilter::ClipType::BOX:
      m_BoxWidget->updateBoxWidget();
      m_BoxWidget->enable();
      m_PlaneWidget->disable();
      break;
    case VSClipFilter::ClipType::PLANE:
      m_PlaneWidget->updatePlaneWidget();
      m_BoxWidget->disable();
      m_PlaneWidget->enable();
      break;
    default:
      break;
    }
  }
  else
  {
    m_BoxWidget->disable();
    m_PlaneWidget->disable();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSClipValues::createFilterWidget()
{
  Ui::VSClipFilterWidget ui;
  QWidget* filterWidget = new QWidget();
  ui.setupUi(filterWidget);
  ui.insideOutCheckBox->setChecked(m_Inverted);
  ui.clipTypeComboBox->setCurrentIndex(static_cast<int>(getClipType()));

  auto changeClipType = [=](int type)
  {
    setClipType(static_cast<VSClipFilter::ClipType>(type));
    switch(getClipType())
    {
    case VSClipFilter::ClipType::BOX:
      ui.gridLayout->removeWidget(m_PlaneWidget);
      m_PlaneWidget->setParent(nullptr);
      ui.gridLayout->addWidget(m_BoxWidget);
      break;
    case VSClipFilter::ClipType::PLANE:
      ui.gridLayout->removeWidget(m_BoxWidget);
      m_BoxWidget->setParent(nullptr);
      ui.gridLayout->addWidget(m_PlaneWidget);
      break;
    default:
      break;
    }
  };

  changeClipType(static_cast<int>(getClipType()));

  connect(ui.clipTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), changeClipType);
  connect(ui.insideOutCheckBox, &QCheckBox::stateChanged, [=](int state) { setInverted(Qt::Checked == state); });

  return filterWidget;
}
