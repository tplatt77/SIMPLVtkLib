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

#include "VSQmlFboRenderer.h"

#include <QOpenGLFramebufferObject>

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkWin32RenderWindowInteractor.h>
#include <vtkWin32OpenGLRenderWindow.h>
#include <QVTKInteractor.h>

#include "SIMPLVtkLib/QtWidgets/VSInteractorStyleFilterCamera.h"
#include "SIMPLVtkLib/QML/VSQmlRenderWindow.h"
#include "SIMPLVtkLib/QML/VSQmlVtkView.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlFboRenderer::VSQmlFboRenderer(VSQmlRenderWindow* QmlRenderWindow)
  : QQuickFramebufferObject::Renderer()
  , m_QmlRenderWindow(QmlRenderWindow)
{
  setupGui();
  QmlRenderWindow->Register(nullptr);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlFboRenderer::setupGui()
{
  if(!m_QmlRenderWindow->GetInteractor())
  {
    //m_Interactor = VTK_PTR(vtkWin32RenderWindowInteractor)::New();
    m_Interactor = QVTKInteractor::New();
    VTK_NEW(VSInteractorStyleFilterCamera, interactorStyle);

    m_Interactor->SetUseTDx(false);
    m_Interactor->SetRenderWindow(m_QmlRenderWindow);
    m_QmlRenderWindow->SetInteractor(m_Interactor);
    m_Interactor->Initialize();
    m_Interactor->SetInteractorStyle(interactorStyle);

    interactorStyle->SetDefaultRenderer(m_QmlRenderWindow->getRenderer());
  }

  m_QmlRenderWindow->Start();
  m_QmlRenderWindow->initializeAxes();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlFboRenderer::render()
{
  m_QmlRenderWindow->PushState();
  m_QmlRenderWindow->OpenGLInitState();
  m_QmlRenderWindow->internalRender();
  m_QmlRenderWindow->PopState();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlFboRenderer::synchronize(QQuickFramebufferObject* fbo)
{
  if(fbo)
  {
    VSQmlVtkView* view = dynamic_cast<VSQmlVtkView*>(fbo);
    view->init();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QOpenGLFramebufferObject* VSQmlFboRenderer::createFramebufferObject(const QSize& size)
{
  QOpenGLFramebufferObjectFormat format;
  format.setAttachment(QOpenGLFramebufferObject::Depth);
  //format.setSamples(m_QmlRenderWindow->getSamples());
  m_FBO = new QOpenGLFramebufferObject(size, format);

  m_QmlRenderWindow->setFramebufferObject(m_FBO);

  return m_FBO;
}