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

#include "VSPlaneWidget.h"

#include <vtkCommand.h>
#include <vtkImplicitPlaneRepresentation.h>
#include <vtkImplicitPlaneWidget2.h>
#include <vtkPlane.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkSmartPointer.h>

#include <QDoubleSpinBox>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class vtkPlaneCallback : public vtkCommand
{
public:
  static vtkPlaneCallback* New()
  {
    return new vtkPlaneCallback();
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkImplicitPlaneWidget2* planeWidget = reinterpret_cast<vtkImplicitPlaneWidget2*>(caller);
    vtkImplicitPlaneRepresentation* rep = reinterpret_cast<vtkImplicitPlaneRepresentation*>(planeWidget->GetRepresentation());
    rep->GetPlane(this->m_UsePlane);
    rep->GetPlane(this->m_ViewPlane);

    if(m_VSTransform)
    {
      m_VSTransform->localizePlane(this->m_UsePlane);
    }

    rep->DrawPlaneOn();

    m_VSPlaneWidget->updatePlaneWidget();
  }

  vtkPlaneCallback()
  : m_UsePlane(nullptr)
  , m_VSPlaneWidget(nullptr)
  , m_VSTransform(nullptr)
  {
  }

  void setUsePlane(vtkPlane* plane)
  {
    m_UsePlane = plane;
  }

  void setViewPlane(vtkPlane* plane)
  {
    m_ViewPlane = plane;
  }

  void setPlaneWidget(VSPlaneWidget* widget)
  {
    m_VSPlaneWidget = widget;
  }

  void setTransform(VSTransform* transform)
  {
    m_VSTransform = transform;
  }

private:
  vtkPlane* m_UsePlane;
  vtkPlane* m_ViewPlane;
  VSPlaneWidget* m_VSPlaneWidget;
  VSTransform* m_VSTransform;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget::VSPlaneWidget(QObject* parent, VSTransform* transform, double bounds[6], vtkRenderWindowInteractor* iren)
: VSAbstractWidget(parent, transform, bounds, iren)
{
  double normal[3] = {1.0, 0.0, 0.0};
  double viewNormal[3] = {1.0, 0.0, 0.0};
  transform->globalizeNormal(viewNormal);
  double* origin = calculateLocalOrigin(bounds, transform);
  double* viewOrigin = calculateGlobalOrigin(bounds);

  setupWidget(bounds, normal, origin, viewNormal, viewOrigin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget::VSPlaneWidget(const VSPlaneWidget& copy)
: VSAbstractWidget(copy.parent(), copy.getVSTransform(), copy.getBounds(), copy.getInteractor())
{
  double bounds[6];
  double normal[3];
  double origin[3];
  double viewNormal[3];
  double viewOrigin[3];

  copy.getBounds(bounds);
  copy.getNormal(normal);
  copy.getOrigin(origin);
  copy.m_ViewPlane->GetNormal(viewNormal);
  copy.m_ViewPlane->GetOrigin(viewOrigin);

  setupWidget(bounds, normal, origin, viewNormal, viewOrigin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget::VSPlaneWidget()
: VSAbstractWidget(nullptr, new VSTransform(), new double[6]{ -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 }, nullptr)
{
  double normal[3] = { 1.0, 0.0, 0.0 };
  double viewNormal[3] = { 1.0, 0.0, 0.0 };
  getVSTransform()->globalizeNormal(viewNormal);
  double* origin = calculateLocalOrigin(getBounds(), getVSTransform());
  double* viewOrigin = calculateGlobalOrigin(getBounds());

  setupWidget(getBounds(), normal, origin, viewNormal, viewOrigin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPlaneWidget::~VSPlaneWidget()
{
  m_PlaneRep->Delete();
  m_PlaneWidget->Delete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setupWidget(double bounds[6], double normal[3], double origin[3], double viewNormal[3], double viewOrigin[3])
{
  m_ViewPlane = VTK_PTR(vtkPlane)::New();
  m_UsePlane = VTK_PTR(vtkPlane)::New();

  m_UsePlane->SetOrigin(origin);
  m_UsePlane->SetNormal(normal);
  m_ViewPlane->SetOrigin(viewOrigin);
  m_ViewPlane->SetNormal(viewNormal);

  // Implicit Plane Widget
  m_PlaneRep = vtkImplicitPlaneRepresentation::New();
  m_PlaneRep->SetPlaceFactor(1.25);
  m_PlaneRep->PlaceWidget(bounds);
  m_PlaneRep->SetPlane(m_ViewPlane);
  m_PlaneRep->SetScaleEnabled(0);
  m_PlaneRep->SetOutlineTranslation(0);
  m_PlaneRep->DrawPlaneOff();
  m_PlaneRep->SetInteractionState(vtkImplicitPlaneRepresentation::Pushing);

  VTK_NEW(vtkPlaneCallback, myCallback);
  myCallback->setUsePlane(m_UsePlane);
  myCallback->setViewPlane(m_ViewPlane);
  myCallback->setPlaneWidget(this);
  myCallback->setTransform(getVSTransform());

  m_PlaneWidget = vtkImplicitPlaneWidget2::New();
  m_PlaneWidget->SetInteractor(getInteractor());
  m_PlaneWidget->SetRepresentation(m_PlaneRep);
  m_PlaneWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

  updatePlaneWidget();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::getNormal(double normals[3]) const
{
  m_UsePlane->GetNormal(normals);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setNormal(double normal[3])
{
  m_UsePlane->SetNormal(normal);
  m_ViewPlane->SetNormal(normal);
  getVSTransform()->globalizePlane(m_ViewPlane);

  drawPlaneOn();

  emit modified();
  emit normalChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setNormal(double x, double y, double z)
{
  double normal[3] = {x, y, z};
  setNormal(normal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
double* VSPlaneWidget::getOrigin() const
{
  return m_UsePlane->GetOrigin();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::getOrigin(double origin[3]) const
{
  m_UsePlane->GetOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setOrigin(double origin[3])
{
  m_UsePlane->SetOrigin(origin);

  m_ViewPlane->SetOrigin(origin);
  getVSTransform()->globalizePoint(origin);

  drawPlaneOn();

  emit modified();
  emit originChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSPlaneWidget::getNormalVector() const
{
  double normal[3];
  getNormal(normal);
  std::vector<double> normalVector(3);

  for(int i = 0; i < 3; i++)
  {
    normalVector[i] = normal[i];
  }

  return normalVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<double> VSPlaneWidget::getOriginVector() const
{
  double origin[3];
  getOrigin(origin);
  std::vector<double> originVector(3);

  for(int i = 0; i < 3; i++)
  {
    originVector[i] = origin[i];
  }

  return originVector;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setNormalVector(std::vector<double> normalVector)
{
  if(normalVector.size() != 3)
  {
    return;
  }

  double normal[3];
  for(int i = 0; i < 3; i++)
  {
    normal[i] = normalVector[i];
  }

  setNormal(normal);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setOriginVector(std::vector<double> originVector)
{
  if(originVector.size() != 3)
  {
    return;
  }

  double origin[3];
  for(int i = 0; i < 3; i++)
  {
    origin[i] = originVector[i];
  }

  setOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateBounds()
{
  int enabled = m_PlaneWidget->GetEnabled();
  m_PlaneWidget->EnabledOff();
  m_PlaneWidget->GetRepresentation()->PlaceWidget(getBounds());
  m_PlaneWidget->SetEnabled(enabled);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSPlaneWidget::isEnabled() const
{
  return m_PlaneWidget->GetEnabled();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::enable()
{
  m_PlaneWidget->EnabledOn();

  render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::disable()
{
  m_PlaneWidget->EnabledOff();

  render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::drawPlaneOn()
{
  m_PlaneRep->DrawPlaneOn();

  render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::drawPlaneOff()
{
  m_PlaneRep->DrawPlaneOff();

  render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updatePlaneWidget()
{
  double normals[3];
  double origin[3];
  getNormal(normals);
  m_UsePlane->GetOrigin(origin);

  getVSTransform()->globalizeNormal(normals);
  getVSTransform()->globalizePoint(origin);

  int enabled = m_PlaneWidget->GetEnabled();
  m_PlaneWidget->Off();

  m_ViewPlane->SetNormal(normals);
  m_ViewPlane->SetOrigin(origin);

  m_PlaneRep->SetPlane(m_ViewPlane);
  m_PlaneWidget->SetEnabled(enabled);

  render();

  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::readJson(QJsonObject& json)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::writeJson(const QJsonObject& json)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::setInteractor(vtkRenderWindowInteractor* interactor)
{
  VSAbstractWidget::setInteractor(interactor);
  m_PlaneWidget->SetInteractor(interactor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPlaneWidget::updateGlobalSpace()
{
  // reposition the vtkWidget
  updatePlaneWidget();
}
