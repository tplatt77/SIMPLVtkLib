/* ============================================================================
 * Copyright (c) 2009-2018 BlueQuartz Software, LLC
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

#include "VSQuickWidget.h"

#include <QQmlError>
#include <QQuickItem>

#include "SIMPLVtkLib/QML/QmlMacros.h"

#include <vtkPointPicker.h>
#include <vtkAxesActor.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageWriter.h>
#include <vtkPNGWriter.h>
#include <vtkBMPWriter.h>
#include <vtkJPEGWriter.h>

#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>

#include "SIMPLVtkLib/QML/VSQmlLoader.h"
#include "SIMPLVtkLib/QtWidgets/VSInteractorStyleFilterCamera.h"
#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewModel.h"

VSQuickWidget* VSQuickWidget::m_LinkingWidget = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQuickWidget::VSQuickWidget(QWidget* parent)
: QQuickWidget(parent)
{
  setupGui();

  //initializeRendererAndAxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::setupGui()
{
  // Set the widget formatting so that VTK can interface with it.
  setFormat(QVTKOpenGLWidget::defaultFormat());

  setMouseTracking(false);
  connect(this, &QQuickWidget::statusChanged, this, &VSQuickWidget::updatedStatus);

  VSQmlLoader::registerTypes();

#if 0
  // Set the filter model as QML context property
  engine()->rootContext()->setContextProperty("filterModel", new VSFilterModel(this));
#endif

  setClearColor(QColor(77, 77, 89, 255));

  // Set Source
  setResizeMode(QQuickWidget::SizeRootObjectToView);
  setSource(VSQmlLoader::GetVtkViewUrl());

  QList<QQmlError> errors = this->errors();
  for(QQmlError error : errors)
  {
    qDebug() << error.toString();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::finishInit()
{
  // Context menu
  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(showContextMenu(const QPoint&)));

  getVtkView()->GetInteractorAdapter()->SetDevicePixelRatio(devicePixelRatio());

  // Set Camera's VSViewWidget
  VSInteractorStyleFilterCamera* interactorStyle;
  interactorStyle = VSInteractorStyleFilterCamera::SafeDownCast(getVtkView()->GetRenderWindow()->getInteractorStyle());
  if(interactorStyle)
  {
    interactorStyle->setViewWidget(m_ViewWidget);
  }

  if(getVtkView())
  {
    getVtkView()->setViewWidget(m_ViewWidget);
    applyFilterModel();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::updatedStatus(QQuickWidget::Status status)
{
  if(QQuickWidget::Status::Ready == status)
  {
    connect(getVtkView(), SIGNAL(rendererCreated()), this, SLOT(finishInit()));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::setViewWidget(VSAbstractViewWidget* view)
{
  m_ViewWidget = view;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlVtkView* VSQuickWidget::getVtkView()
{
  QQuickItem* root = rootObject();
  VSQmlVtkView* rootView = dynamic_cast<VSQmlVtkView*>(root);
  return rootView;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickItem* VSQuickWidget::getFilterTreeView()
{
  return rootObject()->findChild<QQuickItem*>("filterTree");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQuickWidget::applyFilterModel()
{
  QQuickItem* treeView = getFilterTreeView();
  if(treeView && m_ViewWidget && m_ViewWidget->getController())
  {
    QVariant modelVariant;
    //modelVariant.setValue(nullptr);
    //treeView->setProperty("filterViewModel", modelVariant);

    VSFilterViewModel* filterModel = m_ViewWidget->getFilterViewModel();
    modelVariant.setValue(filterModel);
    bool success = treeView->setProperty("filterViewModel", modelVariant);
    return success;
  }

  return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderWindowInteractor* VSQuickWidget::GetInteractor()
{
  if(getVtkView() && getVtkView()->GetRenderWindow())
  {
    return getVtkView()->GetRenderWindow()->GetInteractor();
  }
  
  return nullptr;
}

//// -----------------------------------------------------------------------------
////
//// -----------------------------------------------------------------------------
//void VSQuickWidget::initializeRendererAndAxes()
//{
//  if(nullptr == getRenderer().Get())
//  {
//    //getRenderer() = VTK_PTR(vtkRenderer)::New();
//    //getRenderer()->SetLayer(0);
//
//    //double bgColor[3] = { 0.3, 0.3, 0.35 };
//    //getRenderer()->SetBackground(bgColor);
//
//    VTK_PTR(vtkPointPicker) pointPicker = VTK_PTR(vtkPointPicker)::New();
//    pointPicker->SetTolerance(0.00025);
//
//    //vtkGenericOpenGLRenderWindow* ren = vtkGenericOpenGLRenderWindow::New();
//    //SetRenderWindow(ren);
//    //ren->Finalize();
//    //ren->SetMapped(1);
//    //ren->SetSize(width(), height());
//    //ren->SetPosition(x(), y());
//
//    //QVTKInteractor* iren = QVTKInteractor::New();
//    //iren->SetUseTDx(false);
//    //ren->SetInteractor(iren);
//    //iren->Initialize();
//
//    //vtkInteractorStyle* style = vtkInteractorStyleTrackballCamera::New();
//    //iren->SetInteractorStyle(style);
//
//    //iren->Delete();
//    //style->Delete();
//
//    getVtkView()->GetRenderWindow()->GetInteractor()->SetPicker(pointPicker);
//    //GetRenderWindow()->SetNumberOfLayers(m_NumRenderLayers);
//    //GetRenderWindow()->AddRenderer(getRenderer());
//  }
//
//  if(nullptr == m_OrientationWidget.Get())
//  {
//    VTK_NEW(vtkAxesActor, axes);
//
//    m_OrientationWidget = vtkOrientationMarkerWidget::New();
//    m_OrientationWidget->SetOutlineColor(0.93, 0.57, 0.13);
//    m_OrientationWidget->SetOrientationMarker(axes);
//    m_OrientationWidget->SetInteractor(GetInteractor());
//    m_OrientationWidget->SetEnabled(1);
//    m_OrientationWidget->InteractiveOff();
//  }
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::copy(VSQuickWidget* other)
{
  return;

  // Copy camera values from another visualization widget
  vtkCamera* otherCamera = other->getVtkView()->getRenderer()->GetActiveCamera();
  vtkCamera* thisCamera = getVtkView()->getRenderer()->GetActiveCamera();

  thisCamera->SetPosition(otherCamera->GetPosition());
  thisCamera->SetFocalPoint(otherCamera->GetFocalPoint());
  thisCamera->SetViewUp(otherCamera->GetViewUp());

  // TODO: Copy vtkWidget values from another visualization widget
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderer* VSQuickWidget::getRenderer()
{
  if(getVtkView())
  {
    return getVtkView()->getRenderer();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::renderVtk()
{
  if(getVtkView() && getVtkView()->GetRenderWindow())
  {
    //getVtkView()->GetRenderWindow()->Render();
    //getVtkView()->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::clearRenderWindow()
{
  getRenderer()->RemoveAllViewProps();
  renderVtk();
  //getRenderer()->Render();
  //getVtkView()->GetRenderWindow()->GetInteractor()->Render();
  //getRenderer()->ResetCamera();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::saveScreenshot(QString fileName)
{
  VTK_NEW(vtkWindowToImageFilter, screenshotFilter);
  screenshotFilter->SetInput(getVtkView()->getRenderer()->GetRenderWindow());
  screenshotFilter->SetInputBufferTypeToRGBA();
  screenshotFilter->ReadFrontBufferOff();
  screenshotFilter->Update();

  VTK_PTR(vtkImageWriter) imageWriter;

  if(fileName.endsWith(".png"))
  {
    imageWriter = VTK_PTR(vtkPNGWriter)::New();
  }
  else if(fileName.endsWith(".jpg"))
  {
    imageWriter = VTK_PTR(vtkJPEGWriter)::New();
  }
  else if(fileName.endsWith(".bmp"))
  {
    imageWriter = VTK_PTR(vtkBMPWriter)::New();
  }
  else
  {
    return;
  }

  imageWriter->SetFileName(fileName.toStdString().c_str());
  imageWriter->SetInputConnection(screenshotFilter->GetOutputPort());
  imageWriter->Write();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::saveScreenshotPath()
{
  QString filter = "PNG Image Data (*.png);;JPEG Image Data (*jpg);;Bitmap Image Data(*bmp)";

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Screenshot"), QDir::currentPath(), filter);

  if(fileName.isEmpty())
  {
    return;
  }

  else if(fileName.endsWith(".png") || fileName.endsWith(".jpg") || fileName.endsWith(".bmp"))
  {
    saveScreenshot(fileName);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::resetCamera()
{
  if(getRenderer())
  {
    getRenderer()->ResetCamera();
    renderVtk();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::getCameraFocalPointAndDistance(double* focalPoint, double& distance)
{
  if(nullptr == getRenderer())
  {
    return;
  }

  double* newFocalPoint = getRenderer()->GetActiveCamera()->GetFocalPoint();

  for(int i = 0; i < 3; i++)
  {
    focalPoint[i] = newFocalPoint[i];
  }

  double* position = getRenderer()->GetActiveCamera()->GetPosition();
  distance = sqrt(pow(position[0] - focalPoint[0], 2) + pow(position[1] - focalPoint[1], 2) + pow(position[2] - focalPoint[2], 2));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::camXPlus()
{
  if(nullptr == getRenderer())
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = getRenderer()->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0] - distance, focalPoint[1], focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  delete[] focalPoint;

  renderVtk();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::camYPlus()
{
  if(nullptr == getRenderer())
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = getRenderer()->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1] - distance, focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  delete[] focalPoint;

  renderVtk();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::camZPlus()
{
  if(nullptr == getRenderer())
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = getRenderer()->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] - distance);
  camera->SetViewUp(0.0, 1.0, 0.0);

  delete[] focalPoint;

  renderVtk();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::camXMinus()
{
  if(nullptr == getRenderer())
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = getRenderer()->GetActiveCamera();
  double* focalPoint = new double[3];
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0] + distance, focalPoint[1], focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  delete[] focalPoint;

  renderVtk();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::camYMinus()
{
  if(nullptr == getRenderer())
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = getRenderer()->GetActiveCamera();
  double focalPoint[3] = { 0.0, 0.0, 0.0 };
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1] + distance, focalPoint[2]);
  camera->SetViewUp(0.0, 0.0, 1.0);

  renderVtk();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::camZMinus()
{
  if(nullptr == getRenderer())
  {
    return;
  }

  resetCamera();

  vtkCamera* camera = getRenderer()->GetActiveCamera();
  double focalPoint[3] = { 0.0, 0.0, 0.0 };
  double distance;

  getCameraFocalPointAndDistance(focalPoint, distance);

  camera->SetPosition(focalPoint[0], focalPoint[1], focalPoint[2] + distance);
  camera->SetViewUp(0.0, 1.0, 0.0);

  renderVtk();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::focusInEvent(QFocusEvent* event)
{
  if(m_LinkingWidget && m_LinkingWidget != this)
  {
    linkCameraWith(m_LinkingWidget);
  }

  m_LinkingWidget = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::useOwnContextMenu(bool own)
{
  //m_OwnContextMenu = own;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::showContextMenu(const QPoint& pos)
{
  // If context menu is created by another widget, do nothing
  if(!m_OwnContextMenu)
  {
    return;
  }

  if(getVtkView() && getVtkView()->GetRenderWindow())
  {
    getVtkView()->GetRenderWindow()->createContextMenuCommand(getVtkView(), pos);
  }

  //QMenu contextMenu("Visualization", this);
  //contextMenu.addAction(getLinkCamerasAction());

  //contextMenu.exec(mapToGlobal(pos));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::startLinkCameras()
{
  // Link current camera to target camera
  m_LinkingWidget = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::linkCameraWith(VSQuickWidget* widget)
{
  getRenderer()->SetActiveCamera(widget->getRenderer()->GetActiveCamera());

  LinkedRenderWindowType widgetLinkedRenderWindows = widget->getLinkedRenderWindows();
  m_LinkedRenderWindows.insert(widgetLinkedRenderWindows.begin(), widgetLinkedRenderWindows.end());
  m_LinkedRenderWindows.insert(widget->getVtkView()->GetRenderWindow());
  m_LinkedRenderWindows.insert(getVtkView()->GetRenderWindow());
  widget->m_LinkedRenderWindows = m_LinkedRenderWindows;

  m_LinkingWidget = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQuickWidget::LinkedRenderWindowType VSQuickWidget::getLinkedRenderWindows()
{
  return m_LinkedRenderWindows;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* VSQuickWidget::getLinkCamerasAction()
{
  if(nullptr == m_LinkCameraAction)
  {
    m_LinkCameraAction = new QAction("Link Cameras", this);
    connect(m_LinkCameraAction, SIGNAL(triggered()), this, SLOT(startLinkCameras()));
  }

  return m_LinkCameraAction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSQuickWidget::getFilterFromScreenCoords(int pos[2])
{
  vtkInteractorObserver* obs = getVtkView()->GetRenderWindow()->GetInteractor()->GetInteractorStyle();
  VSInteractorStyleFilterCamera* style = VSInteractorStyleFilterCamera::SafeDownCast(obs);
  if(nullptr == style)
  {
    return nullptr;
  }

  //style->setViewWidget(m_ViewWidget);

  vtkProp3D* prop = nullptr;
  VSAbstractFilter* filter = nullptr;
  std::tie(prop, filter) = style->getFilterFromScreenCoords(pos);

  renderVtk();
  return filter;
}
