/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "VSVtkQmlWidget.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSVtkQMLWidget::VSVtkQMLWidget(QWidget* parent, int numRenderers)
: QQuickWidget(parent)
{
  setupRenderWindow(numRenderers);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVtkQMLWidget::setupRenderWindow(int numRenderers)
{
  // Render Window
  vtkRenderWindow* renWin = vtkGenericOpenGLRenderWindow::New();
  renWin->SetMapped(1);
  renWin->SetSize(width(), height());
  renWin->SetPosition(x(), y());

  // Interactor
  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  iren->SetUseTDx(false);
  renWin->SetInteractor(iren);
  iren->Initialize();

  // Style
  vtkInteractorStyle* style = vtkInteractorStyleTrackballCamera::New();
  iren->SetInteractorStyle(style);

  iren->Delete();
  style->Delete();

  renWin->SetNumberOfLayers(numRenderers);

  for(int i = 0; i < numRenderers; i++)
  {
    // Renderer
    VTK_PTR(vtkRenderer) renderer = VTK_PTR(vtkRenderer)::New();
    renderer->SetLayer(i);

    // Only the first item has a background
    if(i == 0)
    {
      double bgColor[3] = {0.3, 0.3, 0.35};
      renderer->SetBackground(bgColor);
    }
    
    renWin->AddRenderer(renderer);
  }
  

  // Apply Render Window
  setRenderWindow(renWin);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVtkQMLWidget::setNumberOfRenderers(int num)
{
  VTK_PTR(vtkRenderWindow) renWin = getRenderWindow();
  if(nullptr == renWin)
  {
    return;
  }

  renWin->SetNumberOfLayers(num);

  for(int i = getNumberOfRenderers(); i < num; i++)
  {
    VTK_PTR(vtkRenderer) renderer = VTK_PTR(vtkRenderer)::New();
    renderer->SetLayer(i);
    renWin->AddRenderer(renderer);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int VSVtkQMLWidget::getNumberOfRenderers()
{
  if(nullptr == getRenderWindow())
  {
    return -1;
  }

  return getRenderWindow()->GetNumberOfLayers();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderer* VSVtkQMLWidget::getRenderer(int index)
{
  if(nullptr == getRenderWindow())
  {
    return nullptr;
  }

  if(index > getNumberOfRenderers() || index < 0)
  {
    return nullptr;
  }

  vtkObject* renObj = getRenderWindow()->GetRenderers()->GetItemAsObject(index);
  return vtkRenderer::SafeDownCast(renObj);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
vtkRenderWindow* VSVtkQMLWidget::getRenderWindow()
{
  return m_RenderWindow;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSVtkQMLWidget::setRenderWindow(vtkRenderWindow* renWin)
{
  m_RenderWindow = renWin;
}
