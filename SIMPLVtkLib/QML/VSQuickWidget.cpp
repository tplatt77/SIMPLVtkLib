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

#include "VSInteractorStyleFilterCamera.h"

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
  setFormat(QVTKOpenGLWidget::defaultFormat());

  m_InteractorAdaptor = new QVTKInteractorAdapter(this);
  m_InteractorAdaptor->SetDevicePixelRatio(devicePixelRatio());

  static bool initialized = false;
  if(!initialized)
  {
    //QML_REGISTER(VSQmlVtkView);
    qmlRegisterType<VSQmlVtkView>("VSQml", SIMPLVtkLib::Version::Major().toInt(), SIMPLVtkLib::Version::Minor().toInt(), "VSQmlVtkView");
    initialized = true;
  }

  setResizeMode(QQuickWidget::SizeRootObjectToView);

  setSource(QUrl("qrc:/QML/VSVtk.qml"));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlVtkView* VSQuickWidget::getVtkView()
{
  return rootObject()->findChild<VSQmlVtkView*>();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderWindowInteractor* VSQuickWidget::GetInteractor()
{
  return getVtkView()->GetRenderWindow()->GetInteractor();
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
VTK_PTR(vtkRenderer) VSQuickWidget::getRenderer()
{
  return getVtkView()->getRenderer();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::setInteractorStyle(vtkInteractorStyle* style)
{
  //if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
  {
    //getVtkView()->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    //style->SetDefaultRenderer(getRenderer());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::render()
{
  if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
  {
    getVtkView()->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::clearRenderWindow()
{
  getRenderer()->RemoveAllViewProps();
  getRenderer()->Render();
  getVtkView()->GetRenderWindow()->GetInteractor()->Render();
  getRenderer()->ResetCamera();
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
    render();
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

  if(getRenderer()->GetRenderWindow() && getRenderer()->GetRenderWindow()->GetInteractor())
  {
    getRenderer()->GetRenderWindow()->GetInteractor()->Render();
  }
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

  if(getRenderer()->GetRenderWindow() && getRenderer()->GetRenderWindow()->GetInteractor())
  {
    getRenderer()->GetRenderWindow()->GetInteractor()->Render();
  }
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

  if(getRenderer()->GetRenderWindow() && getRenderer()->GetRenderWindow()->GetInteractor())
  {
    getRenderer()->GetRenderWindow()->GetInteractor()->Render();
  }
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

  if(getRenderer()->GetRenderWindow() && getRenderer()->GetRenderWindow()->GetInteractor())
  {
    getRenderer()->GetRenderWindow()->GetInteractor()->Render();
  }
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

  if(getRenderer()->GetRenderWindow() && getRenderer()->GetRenderWindow()->GetInteractor())
  {
    getRenderer()->GetRenderWindow()->GetInteractor()->Render();
  }
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

  if(getRenderer()->GetRenderWindow() && getRenderer()->GetRenderWindow()->GetInteractor())
  {
    getRenderer()->GetRenderWindow()->GetInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQuickWidget::event(QEvent* evt)
{
  switch(evt->type())
  {
  case QEvent::MouseMove:
  case QEvent::MouseButtonPress:
  case QEvent::MouseButtonRelease:
  case QEvent::MouseButtonDblClick:
    // skip events that are explicitly handled by overrides to avoid duplicate
    // calls to InteractorAdaptor->ProcessEvent().
    break;

  case QEvent::Resize:
    // we don't let QVTKInteractorAdapter process resize since we handle it
    // in this->recreateFBO().
    break;

  default:
    if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
    {
      m_InteractorAdaptor->ProcessEvent(evt, getVtkView()->GetRenderWindow()->GetInteractor());
    }
  }
  return QQuickWidget::event(evt);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::mousePressEvent(QMouseEvent* event)
{
  QQuickWidget::mousePressEvent(event);

  if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
  {
    m_InteractorAdaptor->ProcessEvent(event,
      getVtkView()->GetRenderWindow()->GetInteractor());
  }

  emit mousePressed();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::mouseMoveEvent(QMouseEvent* event)
{
  QQuickWidget::mouseMoveEvent(event);

  if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
  {
    m_InteractorAdaptor->ProcessEvent(event,
      getVtkView()->GetRenderWindow()->GetInteractor());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::mouseReleaseEvent(QMouseEvent* event)
{
  QQuickWidget::mouseReleaseEvent(event);

  if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
  {
    m_InteractorAdaptor->ProcessEvent(event,
      getVtkView()->GetRenderWindow()->GetInteractor());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQuickWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
  QQuickWidget::mouseDoubleClickEvent(event);

  if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
  {
    m_InteractorAdaptor->ProcessEvent(event,
      getVtkView()->GetRenderWindow()->GetInteractor());
  }
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
  m_OwnContextMenu = own;
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

  QMenu contextMenu("Visualization", this);
  contextMenu.addAction(getLinkCamerasAction());

  contextMenu.exec(mapToGlobal(pos));
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

  vtkProp3D* prop = nullptr;
  VSAbstractFilter* filter = nullptr;
  std::tie(prop, filter) = style->getFilterFromScreenCoords(pos);

  render();
  return filter;
}
