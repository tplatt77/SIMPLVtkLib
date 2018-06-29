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

#include "VSVisualizationController.h"

#include <QtCore/QObject>
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>

#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkPointPicker.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkWindowToImageFilter.h>
#include <vtkImageWriter.h>
#include <vtkBMPWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>

#include "SIMPLVtkLib/QtWidgets/VSInteractorStyleFilterCamera.h"

VSVisualizationController* VSVisualizationController::m_LinkingWidget = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisualizationController::VSVisualizationController(QObject* parent, unsigned int numLayers)
: QObject(parent)
, m_NumRenderLayers(numLayers)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderWindowInteractor* VSVisualizationController::getInteractor()
{
  if(getRenderWindow())
  {
    return getRenderWindow()->GetInteractor();
  }
  
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VTK_PTR(vtkRenderer) VSVisualizationController::getRenderer()
{
  if(getRenderWindow())
  {
    return getRenderWindow()->GetRenderers()->GetFirstRenderer();
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::copy(VSVisualizationController* other)
{
  // Copy camera values from another visualization widget
  vtkCamera* otherCamera = other->getRenderer()->GetActiveCamera();
  vtkCamera* thisCamera = getRenderer()->GetActiveCamera();

  thisCamera->SetPosition(otherCamera->GetPosition());
  thisCamera->SetFocalPoint(otherCamera->GetFocalPoint());
  thisCamera->SetViewUp(otherCamera->GetViewUp());

  // TODO: Copy vtkWidget values from another visualization widget
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::setInteractorStyle(vtkInteractorStyle* style)
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
void VSVisualizationController::renderVtk()
{
  if(getRenderWindow() && getInteractor())
  {
    getInteractor()->Render();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::clearRenderWindow()
{
  getRenderer()->RemoveAllViewProps();
  getRenderer()->Render();
  getInteractor()->Render();
  getRenderer()->ResetCamera();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::saveScreenshot(QString fileName)
{
  VTK_NEW(vtkWindowToImageFilter, screenshotFilter);
  screenshotFilter->SetInput(getRenderWindow());
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
void VSVisualizationController::saveScreenshotPath()
{
  QString filter = "PNG Image Data (*.png);;JPEG Image Data (*jpg);;Bitmap Image Data(*bmp)";

  QString fileName = QFileDialog::getSaveFileName(getWidget(), tr("Save Screenshot"), QDir::currentPath(), filter);

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
void VSVisualizationController::resetCamera()
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
void VSVisualizationController::getCameraFocalPointAndDistance(double* focalPoint, double& distance)
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
void VSVisualizationController::camXPlus()
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
void VSVisualizationController::camYPlus()
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
void VSVisualizationController::camZPlus()
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
void VSVisualizationController::camXMinus()
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
void VSVisualizationController::camYMinus()
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
void VSVisualizationController::camZMinus()
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

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationBase::mousePressEvent(QMouseEvent* event)
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
void VSVisualizationBase::mouseMoveEvent(QMouseEvent* event)
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
void VSVisualizationBase::mouseReleaseEvent(QMouseEvent* event)
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
void VSVisualizationBase::mouseDoubleClickEvent(QMouseEvent* event)
{
  QQuickWidget::mouseDoubleClickEvent(event);

  if(getVtkView() && getVtkView()->GetRenderWindow() && getVtkView()->GetRenderWindow()->GetInteractor())
  {
    m_InteractorAdaptor->ProcessEvent(event,
      getVtkView()->GetRenderWindow()->GetInteractor());
  }
}
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::focusInEvent(QFocusEvent* event)
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
void VSVisualizationController::useOwnContextMenu(bool own)
{
  m_OwnContextMenu = own;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::showContextMenu(const QPoint& pos)
{
  // If context menu should not be created by this widget, do nothing
  if(!m_OwnContextMenu)
  {
    return;
  }

  QMenu contextMenu("Visualization", getWidget());
  contextMenu.addAction(getLinkCamerasAction());

  contextMenu.exec(getWidget()->mapToGlobal(pos));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::startLinkCameras()
{
  // Link current camera to target camera
  m_LinkingWidget = this;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVisualizationController::linkCameraWith(VSVisualizationController* widget)
{
  getRenderer()->SetActiveCamera(widget->getRenderer()->GetActiveCamera());

  LinkedRenderWindowType widgetLinkedRenderWindows = widget->getLinkedRenderWindows();
  m_LinkedRenderWindows.insert(widgetLinkedRenderWindows.begin(), widgetLinkedRenderWindows.end());
  m_LinkedRenderWindows.insert(widget->getRenderWindow());
  m_LinkedRenderWindows.insert(getRenderWindow());
  widget->m_LinkedRenderWindows = m_LinkedRenderWindows;

  m_LinkingWidget = nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVisualizationController::LinkedRenderWindowType VSVisualizationController::getLinkedRenderWindows()
{
  return m_LinkedRenderWindows;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QAction* VSVisualizationController::getLinkCamerasAction()
{
  if(nullptr == m_LinkCameraAction)
  {
    m_LinkCameraAction = new QAction("Link Cameras", getWidget());
    connect(m_LinkCameraAction, SIGNAL(triggered()), getWidget(), SLOT(startLinkCameras()));
  }

  return m_LinkCameraAction;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSVisualizationController::getFilterFromScreenCoords(int pos[2])
{
  vtkInteractorObserver* obs = getInteractor()->GetInteractorStyle();
  VSInteractorStyleFilterCamera* style = VSInteractorStyleFilterCamera::SafeDownCast(obs);
  if(nullptr == style)
  {
    return nullptr;
  }

  vtkProp3D* prop = nullptr;
  VSAbstractFilter* filter = nullptr;
  std::tie(prop, filter) = style->getFilterFromScreenCoords(pos);

  renderVtk();
  return filter;
}
