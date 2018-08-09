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

#include <QtCore/QVariant>
#include <QtQml/QQmlEngine>

#include <vtkRenderer.h>
#include <vtkExternalOpenGLCamera.h>

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkMapper.h>
#include <vtkConeSource.h>

#include "SIMPLVtkLib/QML/VSQmlLoader.h"
#include "SIMPLVtkLib/QtWidgets/VSAbstractViewWidget.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlVtkView::VSQmlVtkView(QQuickItem* parent)
 : QQuickFramebufferObject(parent)
{
  m_RenderWindow = VTK_PTR(VSQmlRenderWindow)::New();
  setMirrorVertically(true);
  setTextureFollowsItemSize(true);

  setActiveFocusOnTab(true);
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::AllButtons);
  setFlags(flags() | QQuickItem::ItemHasContents | QQuickItem::ItemIsFocusScope | QQuickItem::ItemAcceptsInputMethod);

  // Create the QVTKInteractorAdapter so that Qt and VTK can interact
  m_InteractorAdapter = new QVTKInteractorAdapter(this);
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
QVTKInteractorAdapter* VSQmlVtkView::GetInteractorAdapter()
{
  return m_InteractorAdapter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::updateDevicePixelRatio(const int& ratio)
{
  m_InteractorAdapter->SetDevicePixelRatio(ratio);
  const QSizeF itemSize = size() * ratio;
  GetRenderWindow()->GetInteractor()->SetSize(itemSize.width(), itemSize.height());
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

  delete m_FBO;
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
void VSQmlVtkView::renderVtk()
{
  if(m_FBO)
  {
    m_FBO->update();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSQmlVtkView::event(QEvent* evt)
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
    m_InteractorAdapter->ProcessEvent(evt, GetRenderWindow()->GetInteractor());
    break;
  }

  renderVtk();
  return QQuickFramebufferObject::event(evt);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::mousePressEvent(QMouseEvent* event)
{
  forceActiveFocus();

  passMouseEventToVtk(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::mouseMoveEvent(QMouseEvent* event)
{
  passMouseEventToVtk(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::mouseReleaseEvent(QMouseEvent* event)
{
  passMouseEventToVtk(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::mouseDoubleClickEvent(QMouseEvent* event)
{
  forceActiveFocus();

  // Filter Selection
  if(m_RenderWindow)
  {
    //createPalette(event->pos());
    m_RenderWindow->createSelectionCommand(this, event->pos());
  }

  // QVTKInteractor
  passMouseEventToVtk(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::passMouseEventToVtk(QMouseEvent* event)
{
  if(GetRenderWindow() && GetRenderWindow()->GetInteractor())
  {
    m_InteractorAdapter->ProcessEvent(event,
      GetRenderWindow()->GetInteractor());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::setVtkInteractor(QQuickItem* item)
{
  if(nullptr == item)
  {
    return;
  }

  QVariant var = item->property("vtkPlaneWidget");
  if(var.isValid())
  {
    VSPlaneWidget* planeWidget = var.value<VSPlaneWidget*>();
    if(planeWidget)
    {
      planeWidget->setInteractor(m_RenderWindow->GetInteractor());
    }
  }

  var = item->property("vtkBoxWidget");
  if(var.isValid())
  {
    VSBoxWidget* boxWidget = var.value<VSBoxWidget*>();
    if(boxWidget)
    {
      boxWidget->setInteractor(m_RenderWindow->GetInteractor());
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickItem* VSQmlVtkView::createFilterPalette(QPoint point, VSAbstractFilter* filter)
{
  QUrl paletteUrl = VSQmlLoader::GetFilterUrl(filter);
  if(false == paletteUrl.isValid())
  {
    return nullptr;
  }

  QQmlEngine* engine = qmlEngine(this);
  QQmlComponent component(engine, paletteUrl);
  QQuickItem* paletteItem = dynamic_cast<QQuickItem*>(component.create());
  if(nullptr == paletteItem)
  {
    QList<QQmlError> errors = component.errors();
    for(QQmlError error : errors)
    {
      qDebug() << error.toString();
    }
    return nullptr;
  }

  paletteItem->setParentItem(this);
  setVtkInteractor(paletteItem);

  QList<QQmlError> errors = component.errors();
  for(QQmlError error : errors)
  {
    qDebug() << error.toString();
  }

  paletteItem->setPosition(point);
  QQmlEngine::setObjectOwnership(paletteItem, QQmlEngine::ObjectOwnership::JavaScriptOwnership);

  paletteItem->setProperty("targetFilter", QVariant::fromValue(filter));

  return paletteItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickItem* VSQmlVtkView::createViewSettingPalette(QPoint point, VSAbstractFilter* filter)
{
  if(nullptr == m_ViewWidget)
  {
    return nullptr;
  }

  VSFilterViewSettings* filterViewSettings = m_ViewWidget->getFilterViewSettings(filter);

  // Cannot display null VSFilterViewSettings
  if(nullptr == filterViewSettings)
  {
    return nullptr;
  }

  QQmlEngine* engine = qmlEngine(this);
  QQmlComponent component(engine, VSQmlLoader::GetVisibilitySettingsUrl());
  QQuickItem* paletteItem = dynamic_cast<QQuickItem*>(component.create());

  QList<QQmlError> errors = component.errors();
  for(QQmlError error : errors)
  {
    qDebug() << error.toString();
  }

  paletteItem->setPosition(point);
  paletteItem->setFlags(paletteItem->flags() | QQuickItem::Flag::ItemHasContents | QQuickItem::Flag::ItemIsFocusScope);
  QQmlEngine::setObjectOwnership(paletteItem, QQmlEngine::ObjectOwnership::JavaScriptOwnership);
  paletteItem->setParentItem(this);
  paletteItem->forceActiveFocus();
  
  // Set View Settings
  QVariant viewSettings;
  viewSettings.setValue(filterViewSettings);
  paletteItem->setProperty("viewSettings", viewSettings);

  return paletteItem;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::setViewWidget(VSAbstractViewWidget* viewWidget)
{
  m_ViewWidget = viewWidget;

  if(GetRenderWindow())
  {
    GetRenderWindow()->setViewWidget(viewWidget);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
  // TODO: Recreate FBO

  QQuickFramebufferObject::geometryChanged(newGeometry, oldGeometry);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlVtkView::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value)
{
  if(change == QQuickItem::ItemChange::ItemDevicePixelRatioHasChanged)
  {
    int ratio = value.realValue;
    updateDevicePixelRatio(ratio);
  }

  QQuickFramebufferObject::itemChange(change, value);
}
