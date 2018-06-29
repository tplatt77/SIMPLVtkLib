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

#pragma once

#include <QtGui/QColor>
#include <QtQml/QQmlComponent>

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class SIMPLVtkLib_EXPORT VSPalette: public QQmlComponent
{
  Q_OBJECT

  Q_PROPERTY(bool pinned READ isPinned WRITE setPinned NOTIFY pinnedStateChanged)
  Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
  Q_PROPERTY(QColor titleColor READ getTitleColor WRITE setTitleColor NOTIFY titleColorChanged)
  Q_PROPERTY(QColor headerColor READ getHeaderColor WRITE setHeaderColor NOTIFY headerColorChanged)
  Q_PROPERTY(QColor backgroundColor READ getBackgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)

public:
  VSPalette(QObject* parent = nullptr);
  virtual ~VSPalette() = default;

  QQmlComponent* getContent();
  void setContent(QQmlComponent* item);

  bool isPinned() const;

  QString getTitle() const;
  void setTitle(QString title);

  QColor getTitleColor() const;
  void setTitleColor(QColor titleColor);

  QColor getHeaderColor() const;
  void setHeaderColor(QColor headerColor);

  QColor getBackgroundColor() const;
  void setBackgroundColor(QColor bgColor);

  //void paint(QPainter* painter) override;
  
public slots:
  void close();
  void setPinned(bool pinned);

signals:
  void titleChanged(QString newTitle);
  void titleColorChanged(QColor newColor);
  void headerColorChanged(QColor newColor);
  void backgroundColorChanged(QColor newColor);
  void pinnedStateChanged(bool pinned);

protected:
  void focusOutEvent(QFocusEvent* event) override;
  QSGNode* updatePaintNode(QSGNode* oldNode, QQuickItem::UpdatePaintNodeData* ) override;

private:
  QString m_Title = "Untitled";
  QQmlComponent* m_Content = nullptr;
  bool m_Pinned = false;
  QColor m_TitleColor = QColor(206, 206, 206);
  QColor m_HeaderColor = QColor(84, 84, 84);
  QColor m_BackgroundColor = QColor(188, 188, 188);
};
