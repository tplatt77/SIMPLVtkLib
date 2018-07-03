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

#include "VSQmlRenderWindow.h"

#include <QOpenGLFramebufferObject>

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkProp3DCollection.h>

#include <vtkAxesActor.h>
#include <vtkPointPicker.h>
#include <vtkRendererCollection.h>
#include <vtkAssemblyPath.h>
#include <vtkAssemblyNode.h>

#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/QML/Commands/VSSelectCommand.h"
#include "SIMPLVtkLib/QML/Commands/VSContextMenuCommand.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlRenderWindow::VSQmlRenderWindow()
: QObject(nullptr)
, vtkExternalOpenGLRenderWindow()
, m_FboRenderer(nullptr)
{
  OffScreenRenderingOn();
  connect(this, SIGNAL(internal_PropPickPosition(int*)), this, SLOT(internal_GetFilterFromScreenCoords(int*)));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlRenderWindow* VSQmlRenderWindow::New()
{
  return new VSQmlRenderWindow();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::OpenGLInitState()
{
  vtkExternalOpenGLRenderWindow::OpenGLInitState();
  MakeCurrent();
  initializeOpenGLFunctions();
  //vtkExternalOpenGLRenderWindow::OpenGLInitState();

  glUseProgram(0);
  //if(getRenderer())
  //{
  //  double* bgColor = getRenderer()->GetBackground();
  //  glClearColor(bgColor[0], bgColor[0], bgColor[0], 1.0);
  //  //glClearColor(0.3, 0.3, 0.35, 1.0);
  //}
  //
  //glEnable(GL_BLEND);
  //glEnable(GL_DEBUG_OUTPUT);
  //glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_FASTEST);
  //glDepthMask(GL_TRUE);

  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::Render()
{
  // Tell the FramebufferObject::Renderer to update
  if(this->m_FboRenderer) 
  {
    this->m_FboRenderer->update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::internalRender()
{
  //createSelectionCommand(QPoint(50, 50));
  while(!m_CommandList.empty())
  {
    VSAbstractCommand* command = m_CommandList.front();
    m_CommandList.pop();
    command->exec(getInteractorStyle(), m_ViewWidget);
    
    //delete command;
  }

  // Render only on the FramebufferObject's request
  vtkExternalOpenGLRenderWindow::Render();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::setFramebufferObject(QOpenGLFramebufferObject* fbo)
{
  this->BackLeftBuffer = this->BackBuffer = this->FrontLeftBuffer = this->FrontBuffer = static_cast<unsigned int>(GL_COLOR_ATTACHMENT0);

  auto size = fbo->size();

  this->Size[0] = size.width();
  this->Size[1] = size.height();
  this->NumberOfFrameBuffers = 1;
  this->FrameBufferObject = static_cast<unsigned int>(fbo->handle());
  this->DepthRenderBufferObject = 0;
  this->TextureObjects[0] = static_cast<unsigned int>(fbo->texture());
  this->OffScreenRendering = 1;
  this->OffScreenUseFrameBuffer = 1;
  this->Modified();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::initializeAxes()
{
  //if(nullptr == GetRenderer().Get())
  {
    //getRenderer() = VTK_PTR(vtkRenderer)::New();
    //getRenderer()->SetLayer(0);

    //double bgColor[3] = { 0.3, 0.3, 0.35 };
    //getRenderer()->SetBackground(bgColor);

    m_PointPicker = VTK_PTR(vtkPointPicker)::New();
    m_PointPicker->SetTolerance(0.00025);

    //vtkGenericOpenGLRenderWindow* ren = vtkGenericOpenGLRenderWindow::New();
    //SetRenderWindow(ren);
    //ren->Finalize();
    //ren->SetMapped(1);
    //ren->SetSize(width(), height());
    //ren->SetPosition(x(), y());

    //QVTKInteractor* iren = QVTKInteractor::New();
    //iren->SetUseTDx(false);
    //ren->SetInteractor(iren);
    //iren->Initialize();

    //vtkInteractorStyle* style = vtkInteractorStyleTrackballCamera::New();
    //iren->SetInteractorStyle(style);

    //iren->Delete();
    //style->Delete();

    //GetInteractor()->SetPicker(m_PointPicker);
    //GetRenderWindow()->SetNumberOfLayers(m_NumRenderLayers);
    //GetRenderWindow()->AddRenderer(getRenderer());
  }

  m_PropPicker = VTK_PTR(vtkPropPicker)::New();
  GetInteractor()->SetPicker(m_PropPicker);

  if(nullptr == m_OrientationWidget.Get())
  {
    VTK_NEW(vtkAxesActor, axes);

    m_OrientationWidget = vtkOrientationMarkerWidget::New();
    m_OrientationWidget->SetOutlineColor(0.93, 0.57, 0.13);
    m_OrientationWidget->SetOrientationMarker(axes);
    m_OrientationWidget->SetInteractor(GetInteractor());
    m_OrientationWidget->SetEnabled(1);
    m_OrientationWidget->InteractiveOff();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInteractorStyleFilterCamera* VSQmlRenderWindow::getInteractorStyle()
{
  if(GetInteractor() && GetInteractor()->GetInteractorStyle())
  {
    return VSInteractorStyleFilterCamera::SafeDownCast(GetInteractor()->GetInteractorStyle());
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderer* VSQmlRenderWindow::getRenderer(int index)
{
  if(index < 0)
  {
    return nullptr;
  }

  vtkRenderer* renderer = GetRenderers()->GetFirstRenderer();
  for(int i = 1; i <= index && renderer != nullptr; i++)
  {
    renderer = GetRenderers()->GetNextItem();
  }

  return renderer;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//VTK_PTR(vtkPropPicker) VSQmlRenderWindow::getPropPicker()
//{
//  return m_PropPicker;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  m_ViewWidget = viewWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSInteractorStyleFilterCamera::FilterProp VSQmlRenderWindow::getFilterPropFromScreenCoords(int* pos)
{
  m_PropPickMutex.lock();

  emit internal_PropPickPosition(pos);

  // Hold the thread until the VTK thread handles the call
  m_PropPickMutex.lock();
  m_PropPickMutex.unlock();

  return m_FilterProp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSQmlRenderWindow::getFilterFromScreenCoords(int* pos)
{
  return getFilterPropFromScreenCoords(pos).second;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::internal_GetFilterFromScreenCoords(int* pos)
{
  // Reset the FilterProp
  m_FilterProp.first = nullptr;
  m_FilterProp.second = nullptr;

  vtkRenderer* renderer = getRenderer();

#if 0
  m_PointPicker->Pick(pos[0], pos[1], 0, renderer);
  m_FilterProp.first = m_PointPicker->GetProp3Ds()->GetLastProp3D();
#else
  m_PropPicker->PickProp(pos[0], pos[1], renderer);
  m_FilterProp.first = dynamic_cast<vtkProp3D*>(m_PropPicker->GetViewProp());
#endif

#if 0
  vtkAssemblyPath* path = renderer->PickProp(pos[0], pos[1]);
  if(path)
  {
    vtkAssemblyNode* node = path->GetFirstNode();
    if(node)
    {
      vtkProp* prop = node->GetViewProp();
      m_FilterProp.first = dynamic_cast<vtkProp3D*>(prop);
    }
  }
#endif
  
  if(m_ViewWidget)
  {
    m_FilterProp.second = m_ViewWidget->getFilterFromProp(m_FilterProp.first);
  }
  
  // Allow the caller thread to continue
  m_PropPickMutex.unlock();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int* VSQmlRenderWindow::pointToRenderCoord(QPoint pos)
{
  int* screenSize = GetScreenSize();
  int height = screenSize[1];
  int* outputCoord = new int[2];
  outputCoord[0] = pos.x();
  outputCoord[1] = height - pos.y();

  return outputCoord;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::createSelectionCommand(VSQmlVtkView* view, QPoint pos)
{
  m_CommandList.push(new VSSelectCommand(this, view, pos));

//  GetInteractor()->SetPicker(m_PropPicker);
//  int* mouse = pointToRenderCoord(pos);
//
//  //m_PropPicker->PickProp(mouse[0], mouse[1], getRenderer());
//#if 0
//  // Getting the prop from the renderer throws OpenGL errors
//  vtkAssemblyPath* path = getRenderer()->PickProp(mouse[0], mouse[1]);
//  vtkAssemblyNode* node = path->GetLastNode();
//  vtkProp* prop = node->GetViewProp();
//#endif
//
//#if 1
//  vtkProp3D* prop3D = nullptr;
//  VSAbstractFilter* filter = nullptr;
//  std::tie(prop3D, filter) = getInteractorStyle()->getFilterFromScreenCoords(mouse);
//  
//  if(filter)
//  {
//    m_ViewWidget->selectFilter(filter);
//  }
//  
//#endif
//
//  delete mouse;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlRenderWindow::createContextMenuCommand(QPoint point)
{
  m_CommandList.push(new VSContextMenuCommand(this, point));
}
