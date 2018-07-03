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

#include "VSQmlLoader.h"

//#include "SIMPLVtkLib/QML/VSPalette.h"
#include "SIMPLVtkLib/QML/VSQmlVtkView.h"

VSQmlLoader* VSQmlLoader::m_Instance = nullptr;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlLoader::VSQmlLoader()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlLoader::Register()
{
  //QML_REGISTER(VSQmlVtkView);
  qmlRegisterType<VSQmlVtkView>("VSQml", SIMPLVtkLib::Version::Major().toInt(), SIMPLVtkLib::Version::Minor().toInt(), "VSQmlVtkView");
  //qmlRegisterType<VSPalette>("VSQml", SIMPLVtkLib::Version::Major().toInt(), SIMPLVtkLib::Version::Minor().toInt(), "VSPalette");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlLoader* VSQmlLoader::GetInstance()
{
  if(!m_Instance)
  {
    m_Instance = new VSQmlLoader();
    Register();
  }

  return m_Instance;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlLoader::setEngine(QQmlEngine* engine)
{
  m_Engine = engine;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlLoader::setRoot(QQuickItem* root)
{
  m_Root = root;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickItem* VSQmlLoader::loadItem(QUrl url)
{
  if(!m_Engine)
  {
    return nullptr;
  }

  QQmlComponent* component = new QQmlComponent(m_Engine);
  component->setObjectName("Component");
  component->loadUrl(url);
  QObject* obj = component->create(m_Engine->rootContext());
  QQuickItem* item = dynamic_cast<QQuickItem*>(obj);

  QList<QQmlError> errors = component->errors();
  for(QQmlError error : errors)
  {
    qDebug() << error.toString();
  }

  if(item)
  {
    QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
    item->setParentItem(m_Root);
    item->setParent(component);
  }
  
  return item;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickItem* VSQmlLoader::loadVtkView()
{
  return loadItem(GetVtkViewUrl());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQuickItem* VSQmlLoader::loadPalette()
{
  return loadItem(GetPaletteUrl());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl VSQmlLoader::GetVtkViewUrl()
{
  return QUrl("qrc:/VSQml/VSVtk.qml");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl VSQmlLoader::GetPaletteUrl()
{
  return QUrl("qrc:/VSQml/VSPalette.qml");
}
