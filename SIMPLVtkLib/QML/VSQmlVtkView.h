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

#pragma once

//#include "quickQmlRegister.hpp"

#include <QtQuick/QQuickFramebufferObject>
#include <QtGui/QMouseEvent>

#include <QVTKInteractorAdapter.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>

#include "SIMPLVtkLib/QML/VSQmlFboRenderer.h"
#include "SIMPLVtkLib/QML/VSQmlRenderWindow.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class SIMPLVtkLib_EXPORT VSQmlVtkView : public QQuickFramebufferObject
{
  Q_OBJECT

public:
  //static Qml::Register::Symbol::Class<VSQMLRenderWindow> Register;

  VSQmlVtkView(QQuickItem* parent = nullptr);
  virtual ~VSQmlVtkView() = default;

  void init();
  void update();

  Renderer* createRenderer() const override;
  VSQmlRenderWindow* GetRenderWindow() const;
  QVTKInteractorAdapter* GetInteractorAdapter();

  vtkRenderer* getRenderer();
  vtkCamera* getCamera();

  QQuickItem* createPalette(QPoint point);

  QQuickItem* createViewSettingPalette(QPoint point, VSAbstractFilter* filter);

  void setViewWidget(VSAbstractViewWidget* viewWidget);

signals:
  void rendererCreated() const;

protected:
  void renderVtk();

  bool event(QEvent* evt) Q_DECL_OVERRIDE;

  /**
  * @brief Overrides the mousePressEvent
  * @param event
  */
  void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  void mouseDoubleClickEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

  void passMouseEventToVtk(QMouseEvent* event);

private:
  mutable VTK_PTR(VSQmlRenderWindow) m_RenderWindow = nullptr;
  mutable VSQmlFboRenderer* m_FBO = nullptr;
  mutable VTK_PTR(vtkRenderer) m_Renderer = nullptr;
  QVTKInteractorAdapter* m_InteractorAdapter = nullptr;
  VSAbstractViewWidget* m_ViewWidget = nullptr;
};