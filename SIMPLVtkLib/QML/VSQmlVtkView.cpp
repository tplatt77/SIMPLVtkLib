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

#include "VSQmlVtkView.h"

#include <vtkRenderer.h>
#include <vtkExternalOpenGLCamera.h>

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkMapper.h>
#include <vtkConeSource.h>

#include "SIMPLVtkLib/QML/VSQmlLoader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlVtkView::VSQmlVtkView(QQuickItem* parent)
 : QQuickFramebufferObject(parent)
{
  m_RenderWindow = VTK_PTR(VSQmlRenderWindow)::New();
  setMirrorVertically(true);

  setActiveFocusOnTab(true);
  setAcceptedMouseButtons(Qt::MouseButton::LeftButton | Qt::MouseButton::RightButton);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::init()
{
  // If any additional work is required before rendering, insert it here

  update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::update()
{
  QQuickFramebufferObject::update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlRenderWindow* VSQmlVtkView::GetRenderWindow() const
{
  return m_RenderWindow.Get();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickFramebufferObject::Renderer* VSQmlVtkView::createRenderer() const
{
  if(m_RenderWindow->getRenderer())
  {
    m_Renderer = m_RenderWindow->getRenderer();
  }
  else
  {
    m_Renderer = VTK_PTR(vtkRenderer)::New();
    m_Renderer->SetActiveCamera(vtkExternalOpenGLCamera::New());
    m_Renderer->SetLayer(0);
    m_RenderWindow->AddRenderer(m_Renderer);
  }

  double bgColor[3] = { 0.3, 0.3, 0.35 };
  m_Renderer->SetBackground(bgColor);

  m_FBO = new VSQmlFboRenderer(m_RenderWindow);
  
#if 0
  VSInteractorStyleFilterCamera* camera = m_RenderWindow->getInteractorStyle();
  if(camera)
  {
    camera->setViewWidget(m_ViewWidget);
  }
#endif

  emit rendererCreated();
  return m_FBO;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderer* VSQmlVtkView::getRenderer()
{
  if(m_Renderer)
  {
    return m_Renderer.Get();
  }
  
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkCamera* VSQmlVtkView::getCamera()
{
  return nullptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::mouseDoubleClickEvent(QMouseEvent* event)
{
  if(m_RenderWindow)
  {
    //createPalette(event->pos());
    m_RenderWindow->createSelectionCommand(this, event->pos());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickItem* VSQmlVtkView::createPalette(QPoint point)
{
  QQmlEngine* engine = qmlEngine(this);
  QQmlComponent component(engine, VSQmlLoader::GetPaletteUrl());
  QQuickItem* childItem = dynamic_cast<QQuickItem*>(component.create());
  childItem->setParentItem(this);

  QList<QQmlError> errors = component.errors();
  for(QQmlError error : errors)
  {
    qDebug() << error.toString();
  }

  childItem->setPosition(point);
  childItem->setVisible(true);

  connect(childItem, SIGNAL(removeObject()), childItem, SLOT(deleteLater()));

  return childItem;
}
