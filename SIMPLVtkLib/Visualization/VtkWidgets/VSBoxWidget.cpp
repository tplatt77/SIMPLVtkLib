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

#include "VSBoxWidget.h"

#include <cmath>

#include <vtkBox.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkPlanes.h>
#include <vtkPolyData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

#include <vtkSmartPointer.h>

#include <QtWidgets/QDoubleSpinBox>

#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"


#define PI 3.14159

class vtkBoxCallback : public vtkCommand
{
public:
  static vtkBoxCallback* New()
  {
    return new vtkBoxCallback();
  }

  virtual void Execute(vtkObject* caller, unsigned long, void*)
  {
    vtkBoxWidget2* boxWidget = reinterpret_cast<vtkBoxWidget2*>(caller);
    vtkBoxRepresentation* rep = reinterpret_cast<vtkBoxRepresentation*>(boxWidget->GetRepresentation());
    rep->GetTransform(transform);

    qtBoxWidget->updateSpinBoxes();
    qtBoxWidget->modified();
  }

  vtkBoxCallback()
  : transform(0)
  , qtBoxWidget(0)
  {
  }
  vtkTransform* transform;
  VSBoxWidget* qtBoxWidget;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSBoxWidget::VSBoxWidget(QWidget* parent, double bounds[6], vtkRenderWindowInteractor* iren)
: VSAbstractWidget(parent, bounds, iren)
{
  setupUi(this);

  viewTransform = vtkSmartPointer<vtkTransform>::New();
  useTransform = vtkSmartPointer<vtkTransform>::New();

  viewPlanes = vtkSmartPointer<vtkPlanes>::New();

  vtkSmartPointer<vtkBoxCallback> myCallback = vtkSmartPointer<vtkBoxCallback>::New();
  myCallback->transform = viewTransform;
  myCallback->qtBoxWidget = this;

  // Implicit Plane Widget
  boxRep = vtkSmartPointer<vtkBoxRepresentation>::New();
  boxRep->SetPlaceFactor(1.25);
  boxRep->PlaceWidget(bounds);
  boxRep->SetTransform(viewTransform);

  boxWidget = vtkSmartPointer<vtkBoxWidget2>::New();
  boxWidget->SetInteractor(iren);
  boxWidget->SetRepresentation(boxRep);
  boxWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

  updateSpinBoxes();

  // adjust the vtkWidget when values are changed
  connect(translationXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(translationYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(translationZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));

  connect(rotationXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(rotationYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(rotationZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));

  connect(scaleXSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(scaleYSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
  connect(scaleZSpinBox, SIGNAL(editingFinished()), this, SLOT(spinBoxValueChanged()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSBoxWidget::~VSBoxWidget()
{
  boxWidget->EnabledOff();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setScale(double scale[3])
{
  scaleXSpinBox->setValue(scale[0]);
  scaleYSpinBox->setValue(scale[1]);
  scaleZSpinBox->setValue(scale[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setScale(double x, double y, double z)
{
  double scale[3] = {x, y, z};

  setScale(scale);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setOrigin(double origin[3])
{
  VSAbstractWidget::setOrigin(origin);

  viewTransform->Translate(origin);

  translationXSpinBox->setValue(origin[0]);
  translationYSpinBox->setValue(origin[1]);
  translationZSpinBox->setValue(origin[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setOrigin(double x, double y, double z)
{
  double origin[3] = {x, y, z};
  setOrigin(origin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setRotation(double rotation[3])
{
  rotationXSpinBox->setValue(rotation[0]);
  rotationYSpinBox->setValue(rotation[1]);
  rotationZSpinBox->setValue(rotation[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setRotation(double x, double y, double z)
{
  double rotation[3] = {x, y, z};

  setRotation(rotation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::updateBounds()
{
  boxWidget->EnabledOff();
  boxWidget->GetRepresentation()->PlaceWidget(bounds);
  boxWidget->EnabledOn();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::updateOrigin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::enable()
{
  boxWidget->EnabledOn();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::disable()
{
  boxWidget->EnabledOff();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::updateSpinBoxes()
{
  double scale[3];
  double rotation[3];

  vtkMatrix4x4* matrix = viewTransform->GetMatrix();

  getTranslation(origin);
  getScale(scale);
  getRotation(rotation);

  translationXSpinBox->setValue(origin[0]);
  translationYSpinBox->setValue(origin[1]);
  translationZSpinBox->setValue(origin[2]);

  scaleXSpinBox->setValue(scale[0]);
  scaleYSpinBox->setValue(scale[1]);
  scaleZSpinBox->setValue(scale[2]);

  rotationXSpinBox->setValue(rotation[0]);
  rotationYSpinBox->setValue(rotation[1]);
  rotationZSpinBox->setValue(rotation[2]);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::spinBoxValueChanged()
{
  origin[0] = translationXSpinBox->value();
  origin[1] = translationYSpinBox->value();
  origin[2] = translationZSpinBox->value();

  double scale[3];
  scale[0] = scaleXSpinBox->value();
  scale[1] = scaleYSpinBox->value();
  scale[2] = scaleZSpinBox->value();

  double rotation[3];
  rotation[0] = rotationXSpinBox->value();
  rotation[1] = rotationYSpinBox->value();
  rotation[2] = rotationZSpinBox->value();

  setMatrix(origin, scale, rotation);

  updateBoxWidget();
  emit modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::updateBoxWidget()
{
  boxRep->SetTransform(viewTransform);

  m_renderWindowInteractor->Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::getTranslation(double translation[3])
{
  viewTransform->GetPosition(translation);

  //vtkMatrix4x4* matrix = viewTransform->GetMatrix();

  //translation[0] = matrix->GetElement(0, 3);
  //translation[1] = matrix->GetElement(1, 3);
  //translation[2] = matrix->GetElement(2, 3);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::getScale(double scale[3])
{
  viewTransform->GetScale(scale);

  //vtkMatrix4x4* matrix = viewTransform->GetMatrix();
  //// scale[i] = sqrt(x^2 + y^2 + z^2)

  //double var00 = matrix->GetElement(0, 0);
  //double var01 = matrix->GetElement(0, 1);
  //double var02 = matrix->GetElement(0, 2);
  //scale[0] = sqrt(std::pow(var00, 2) + std::pow(var01, 2) + std::pow(var02, 2));

  //double var10 = matrix->GetElement(1, 0);
  //double var11 = matrix->GetElement(1, 1);
  //double var12 = matrix->GetElement(1, 2);
  //scale[1] = sqrt(std::pow(var10, 2) + std::pow(var11, 2) + std::pow(var12, 2));

  //double var20 = matrix->GetElement(2, 0);
  //double var21 = matrix->GetElement(2, 1);
  //double var22 = matrix->GetElement(2, 2);
  //scale[2] = sqrt(std::pow(var20, 2) + std::pow(var21, 2) + std::pow(var22, 2));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::getRotation(double rotation[3])
{
  viewTransform->GetOrientation(rotation);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setMatrix(double position[3], double scale[3], double rotation[3])
{
  vtkMatrix4x4* matrix = vtkMatrix4x4::New();
  viewTransform->SetMatrix(matrix);

  viewTransform->Scale(scale);

  viewTransform->RotateZ(rotation[2]);
  viewTransform->RotateY(rotation[1]);
  viewTransform->RotateX(rotation[0]);

  viewTransform->Translate(position);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSBoxWidget::setValues(double rotation[3], double translation[3], double scale[3])
{
  scaleXSpinBox->setValue(scale[0]);
  scaleYSpinBox->setValue(scale[1]);
  scaleZSpinBox->setValue(scale[2]);

  rotationXSpinBox->setValue(rotation[0]);
  rotationYSpinBox->setValue(rotation[1]);
  rotationZSpinBox->setValue(rotation[2]);

  translationXSpinBox->setValue(translation[0]);
  translationYSpinBox->setValue(translation[1]);
  translationZSpinBox->setValue(translation[2]);

  setMatrix(translation, scale, rotation);
  updateBoxWidget();
}

