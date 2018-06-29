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

#include "VSPalette.h"

#include <QtGui/QPainter>
#include <QtQuick/QSGRectangleNode>
#include <QtQuick/QSGSimpleRectNode>
#include <QtQuick/QSGEngine>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSPalette::VSPalette(QObject* parent)
: QQmlComponent(parent)
{
  //setFillColor(m_BackgroundColor);
  //setFlag(QQuickItem::ItemHasContents, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QQmlComponent* VSPalette::getContent()
{
  return m_Content;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::setContent(QQmlComponent* item)
{
  m_Content = item;
  item->setParent(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::close()
{
  deleteLater();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSPalette::isPinned() const
{
  return m_Pinned;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::setPinned(bool pinned)
{
  m_Pinned = pinned;

  emit pinnedStateChanged(pinned);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString VSPalette::getTitle() const
{
  return m_Title;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::setTitle(QString title)
{
  m_Title = title;

  emit titleChanged(title);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor VSPalette::getTitleColor() const
{
  return m_TitleColor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::setTitleColor(QColor titleColor)
{
  m_TitleColor = titleColor;

  emit titleColorChanged(titleColor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor VSPalette::getHeaderColor() const
{
  return m_HeaderColor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::setHeaderColor(QColor headerColor)
{
  m_HeaderColor = headerColor;

  emit headerColorChanged(headerColor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QColor VSPalette::getBackgroundColor() const
{
  return m_BackgroundColor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::setBackgroundColor(QColor bgColor)
{
  m_BackgroundColor = bgColor;

  emit backgroundColorChanged(bgColor);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::focusOutEvent(QFocusEvent* event)
{
  if(!m_Pinned)
  {
    close();
  }
}

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QSGNode* VSPalette::updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData*)
{
  QSGSimpleRectNode* node = static_cast<QSGSimpleRectNode*>(oldNode);
  if(!node) {
#if 0
    QSGEngine* engine = new QSGEngine(this);
    node = engine->createRectangleNode();

    if(!node) 
    {
      return nullptr;
    }
#else
    node = new QSGSimpleRectNode();
#endif
    node->setColor(Qt::red);
  }
  
  QSGFlatColorMaterial* material = new QSGFlatColorMaterial();
  material->setColor(getBackgroundColor());
  node->setMaterial(material);
  node->setFlag(QSGNode::OwnsMaterial);
  
  node->setColor(getBackgroundColor());
  node->setRect(boundingRect());
  node->markDirty(QSGNode::DirtyForceUpdate);
  return node;
}
#endif

#if 0
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSPalette::paint(QPainter* painter)
{
  QRect rect(0, 0, width(), height());
  int radius = 5;
  painter->setBrush(QBrush(getBackgroundColor()));
  painter->drawRoundedRect(rect, radius, radius);
}
#endif
