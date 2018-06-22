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

#include <vtkAxesActor.h>
#include <vtkPointPicker.h>
#include <vtkRendererCollection.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlRenderWindow::VSQmlRenderWindow()
 : vtkExternalOpenGLRenderWindow()
, m_FboRenderer(nullptr)
{
  OffScreenRenderingOn();
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
  vtkExternalOpenGLRenderWindow::OpenGLInitState();

  //glUseProgram(0);
  //if(getRenderer())
  //{
  //  double* bgColor = getRenderer()->GetBackground();
  //  glClearColor(bgColor[0], bgColor[0], bgColor[0], 1.0);
  //  //glClearColor(0.3, 0.3, 0.35, 1.0);
  //}
  //
  //glEnable(GL_BLEND);
  //glHint(GL_CLIP_VOLUME_CLIPPING_HINT_EXT, GL_FASTEST);
  //glDepthMask(GL_TRUE);
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

    VTK_PTR(vtkPointPicker) pointPicker = VTK_PTR(vtkPointPicker)::New();
    pointPicker->SetTolerance(0.00025);

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

    GetInteractor()->SetPicker(pointPicker);
    //GetRenderWindow()->SetNumberOfLayers(m_NumRenderLayers);
    //GetRenderWindow()->AddRenderer(getRenderer());
  }

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
