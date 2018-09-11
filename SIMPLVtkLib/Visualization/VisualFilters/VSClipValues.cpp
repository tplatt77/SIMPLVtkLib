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

#include <QtCore/QUuid>

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

  connect(m_BoxWidget, &VSBoxWidget::modified, this, &VSClipValues::alertChangesWaiting);
  connect(m_PlaneWidget, &VSPlaneWidget::modified, this, &VSClipValues::alertChangesWaiting);

  double useOrigin[3];
  double useNormal[3];
  // Reset "Last" values
  for(int i = 0; i < 3; i++)
  {
    useOrigin[i] = 0.0;
    useNormal[i] = 0.0;
  }
  // Set the direction of the plane normal
  useNormal[0] = 1.0;
  m_PlaneWidget->setUsePlaneNormal(useNormal);
  m_PlaneWidget->setUsePlaneOrigin(useOrigin);

  m_LastBoxTransform = VTK_PTR(vtkTransform)::New();

  m_LastClipType = VSClipFilter::ClipType::PLANE;
  m_LastPlaneInverted = false;
  m_LastBoxInverted = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipValues::VSClipValues(const VSClipValues& values)
: VSAbstractFilterValues(values.getFilter())
, m_BoxWidget(new VSBoxWidget(nullptr, values.getFilter()->getTransform(), values.getFilter()->getBounds(), nullptr))
, m_PlaneWidget(new VSPlaneWidget(nullptr, values.getFilter()->getTransform(), values.getFilter()->getBounds(), nullptr))
, m_ClipType(values.m_ClipType)
, m_LastClipType(values.m_LastClipType)
, m_LastBoxInverted(values.m_LastBoxInverted)
, m_LastBoxTransform(values.m_LastBoxTransform)
, m_LastPlaneInverted(values.m_LastPlaneInverted)
{
  m_PlaneWidget->setUsePlaneNormal(values.m_PlaneWidget->getUsePlaneNormal());
  m_PlaneWidget->setUsePlaneOrigin(values.m_PlaneWidget->getUsePlaneOrigin());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter* VSClipValues::getClipFilter() const
{
  return dynamic_cast<VSClipFilter*>(getFilter());
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
  m_ClipType = getLastClipType();
  emit clipTypeChanged(m_ClipType);

  // Reset BoxWidget
  m_BoxWidget->setTransform(getLastBoxTransform());

  // Reset PlaneWidget
  m_PlaneWidget->setOrigin(getLastPlaneOrigin());
  m_PlaneWidget->setNormal(getLastPlaneNormal());

  if(getInteractor())
  {
    getInteractor()->Render();
  }

  // Reset Inverted
  if(VSClipFilter::ClipType::BOX == m_ClipType)
  {
    m_Inverted = getLastBoxInverted();
  }
  else
  {
    m_Inverted = getLastPlaneInverted();
  }

  updateRendering();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipValues::hasChanges() const
{
  if(getSelection().size() > 1)
  {
    return true;
  }

  VSClipFilter* filter = dynamic_cast<VSClipFilter*>(getFilter());
  if(m_ClipType != getLastClipType())
  {
    return true;
  }

  switch(m_ClipType)
  {
  case VSClipFilter::ClipType::BOX:
  {
    if(m_Inverted != getLastBoxInverted())
    {
      return true;
    }

    std::vector<double> lastTranslation = getLastBoxTranslationVector();
    std::vector<double> lastRotation = getLastBoxRotationVector();
    std::vector<double> lastScale = getLastBoxScaleVector();
    if(!getBoxWidget()->equals(lastTranslation, lastRotation, lastScale))
    {
      return true;
    }
  }
  break;
  case VSClipFilter::ClipType::PLANE:
    if(!getPlaneWidget()->equals(getLastPlaneOrigin(), getLastPlaneNormal()))
    {
      return true;
    }
    break;
  default:
    return false;
  }

  return false;
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

  emit clipTypeChanged(type);
  emit alertChangesWaiting();
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
  emit isInvertedChanged(inverted);
  emit alertChangesWaiting();
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

  auto changeClipType = [=](int type, bool setType = true) {
    if(setType)
    {
      setClipType(static_cast<VSClipFilter::ClipType>(type));
    }
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

  connect(this, &VSClipValues::isInvertedChanged, [=](bool inverted) {
    ui.insideOutCheckBox->blockSignals(true);
    ui.insideOutCheckBox->setChecked(inverted);
    ui.insideOutCheckBox->blockSignals(false);
  });
  connect(this, &VSClipValues::clipTypeChanged, [=](VSClipFilter::ClipType clipType) {
    ui.clipTypeComboBox->blockSignals(true);
    int type = static_cast<int>(clipType);
    ui.clipTypeComboBox->setCurrentIndex(type);
    changeClipType(type, false);
    ui.clipTypeComboBox->blockSignals(false);
  });

  return filterWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::loadJSon(QJsonObject& json)
{
  setLastClipType(static_cast<VSClipFilter::ClipType>(json["Last Clip Type"].toInt()));
  setClipType(static_cast<VSClipFilter::ClipType>(json["Clip Type"].toInt()));
  setLastPlaneInverted(json["Last Plane Inverted"].toBool());
  setLastBoxInverted(json["Last Box Inverted"].toBool());
  setInverted(json["Inverted"].toBool());
  
  m_BoxWidget->readJson(json);
  m_PlaneWidget->readJson(json);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::writeJson(QJsonObject& json)
{
  json["Clip Type"] = static_cast<int>(m_ClipType);
  json["Last Clip Type"] = static_cast<int>(m_LastClipType);
  json["Last Plane Inverted"] = m_LastPlaneInverted;
  json["Last Box Inverted"] = m_LastBoxInverted;
  json["Inverted"] = isInverted();

  m_PlaneWidget->writeJson(json);
  m_BoxWidget->writeJson(json);

  vtkMatrix4x4* matrix = m_LastBoxTransform->GetMatrix();
  double* matrixData = matrix->GetData();
  QJsonArray boxTransformData;
  for(int i = 0; i < 16; i++)
  {
    boxTransformData.append(matrixData[i]);
  }

  json["Box Transform Data"] = boxTransformData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipValues::getLastPlaneInverted() const
{
  return m_LastPlaneInverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipValues::getLastPlaneOrigin() const
{
  return m_PlaneWidget->getOrigin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSClipValues::getLastPlaneNormal() const
{
  return m_PlaneWidget->getNormal();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSClipValues::getLastBoxInverted() const
{
  return m_LastBoxInverted;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkTransform) VSClipValues::getLastBoxTransform() const
{
  return m_LastBoxTransform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipValues::getLastBoxTranslationVector() const
{
  std::vector<double> origin(3);
  double* position = new double[3];
  m_LastBoxTransform->GetPosition(position);
  for(int i = 0; i < 3; i++)
  {
    origin[i] = position[i];
  }

  return origin;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipValues::getLastBoxRotationVector() const
{
  std::vector<double> rotation(3);
  double* rotationPtr = new double[3];
  m_LastBoxTransform->GetOrientation(rotationPtr);
  for(int i = 0; i < 3; i++)
  {
    rotation[i] = rotationPtr[i];
  }

  return rotation;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSClipValues::getLastBoxScaleVector() const
{
  std::vector<double> scale(3);
  double* scalePtr = new double[3];
  m_LastBoxTransform->GetOrientation(scalePtr);
  for(int i = 0; i < 3; i++)
  {
    scale[i] = scalePtr[i];
  }

  return scale;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSClipFilter::ClipType VSClipValues::getLastClipType() const
{
  return m_LastClipType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setLastPlaneInverted(bool inverted)
{
  m_LastPlaneInverted = inverted;
  emit lastPlaneInvertedChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setLastPlaneOrigin(double* origin)
{
  m_PlaneWidget->setUsePlaneOrigin(origin);
  emit lastPlaneOriginChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setLastPlaneNormal(double* normal)
{
  m_PlaneWidget->setUsePlaneNormal(normal);
  emit lastPlaneNormalChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setLastBoxInverted(bool inverted)
{
  m_LastBoxInverted = inverted;
  emit lastBoxInvertedChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setLastBoxTransform(VTK_PTR(vtkTransform) transform)
{
  m_LastBoxTransform = transform;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSClipValues::setLastClipType(VSClipFilter::ClipType type)
{
  m_LastClipType = type;
  emit clipTypeChanged(type);
}
