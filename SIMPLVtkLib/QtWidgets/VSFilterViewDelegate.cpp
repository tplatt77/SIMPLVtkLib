/* ============================================================================
 * Copyright (c) 2009-2017 BlueQuartz Software, LLC
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

#include "VSFilterViewDelegate.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QPainter>
#include <QtWidgets/QLineEdit>

#include "SVWidgetsLib/Widgets/SVStyle.h"

#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

namespace
{
const int k_ButtonOffset = 2;
}

QIcon VSFilterViewDelegate::s_VisibleIcon;
QIcon VSFilterViewDelegate::s_InvisibleIcon;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewDelegate::SetupIcons()
{
  s_VisibleIcon = QIcon(":/icons/pqEyeball.png");
  s_InvisibleIcon = QIcon(":/icons/pqEyeballClosed.png");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSFilterViewDelegate::VSFilterViewDelegate(QObject* parent)
: QStyledItemDelegate(parent)
{
  SetupIcons();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSAbstractFilter* VSFilterViewDelegate::getFilterFromIndex(const QModelIndex& index) const
{
  const VSFilterViewModel* filterViewModel = dynamic_cast<const VSFilterViewModel*>(index.model());
  if(nullptr == filterViewModel)
  {
    return nullptr;
  }

  return filterViewModel->getFilterFromIndex(index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QIcon VSFilterViewDelegate::getVisibilityIcon(const VSFilterViewModel* filterViewModel, VSAbstractFilter* filter) const
{
  if(nullptr == filterViewModel)
  {
    return QIcon();
  }

  VSFilterViewSettings* viewSettings = filterViewModel->getFilterViewSettings(filter);
  if(viewSettings)
  {
    return viewSettings->isVisible() ? s_VisibleIcon : s_InvisibleIcon;
  }

  return QIcon();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRect VSFilterViewDelegate::getVisibilityRect(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  VSAbstractFilter* filter = getFilterFromIndex(index);
  if(nullptr == filter)
  {
    return QRect();
  }

  const int buttonSize = option.rect.height();
  const int buttonX = option.rect.x() + option.rect.width() - (buttonSize + ::k_ButtonOffset);
  const int buttonY = option.rect.y();
  return QRect(buttonX, buttonY, buttonSize, buttonSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QRect VSFilterViewDelegate::getTextRect(const QStyleOptionViewItem& option, const bool& drawButtons) const
{
  QRect textRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height());

  if(drawButtons)
  {
    QModelIndex index = option.index;
    VSAbstractFilter* filter = getFilterFromIndex(index);
    if(filter && filter->isCheckable())
    {
      const int widthLost = option.rect.height();
      textRect.setWidth(textRect.width() - widthLost);
    }
  }

  return textRect;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* VSFilterViewDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QLineEdit* editor = new QLineEdit(parent);
  return editor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
  QString value = index.model()->data(index, Qt::DisplayRole).toString();
  QLineEdit* line = static_cast<QLineEdit*>(editor);
  line->setText(value);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool VSFilterViewDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
  // Looking for click in the visibility button area
  QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
  if(mouseEvent != nullptr)
  {
    QRect buttonRect = getVisibilityRect(option, index);
    if(buttonRect.contains(mouseEvent->pos()))
    {
      switch(event->type())
      {
      case QEvent::MouseMove:
      {
        if(buttonRect.contains(mouseEvent->pos()))
        {
          // Set the current index
          m_MouseIndex = index;
          if(m_VisibilityButtonState == ButtonState::Normal)
          {
            m_VisibilityButtonState = ButtonState::MouseOver;
          }
          // Do not call parent class
          return true;
        }
        else
        {
          m_VisibilityButtonState = ButtonState::Normal;
        }
        break;
      }
      case QEvent::MouseButtonPress:
      {
        // MouseOver state should already be checked on MouseMove
        if(m_VisibilityButtonState == ButtonState::MouseOver)
        {
          if(mouseEvent->button() == Qt::LeftButton)
          {
            m_VisibilityButtonState = ButtonState::Pressed;
          }
          else
          {
            m_VisibilityButtonState = ButtonState::Normal;
          }
          return true;
        }
        break;
      }
      case QEvent::MouseButtonRelease:
      {
        if(mouseEvent->button() == Qt::LeftButton)
        {
          const VSFilterViewModel* filterViewModel = dynamic_cast<const VSFilterViewModel*>(index.model());
          VSFilterViewSettings* viewSettings = filterViewModel->getFilterViewSettingsByIndex(index);
          viewSettings->setVisible(!viewSettings->isVisible());
          return true;
        }
        break;
      }
      default:
        break;
      }
      return true;
    }
  }

  return QStyledItemDelegate::editorEvent(event, model, option, index);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
  VSFilterViewModel* filterViewModel = qobject_cast<VSFilterViewModel*>(model);

  QLineEdit* line = static_cast<QLineEdit*>(editor);
  QString value = line->text();

  if(value.isEmpty() == false)
  {
    filterViewModel->setData(index, value, Qt::DisplayRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  editor->setGeometry(option.rect);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSFilterViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QStyleOptionViewItem op(option);
  painter->setPen(QPen());
  painter->setBrush(QBrush());
  painter->setFont(op.font);

  // Get Text / Background Color
  QColor textColor;
  QColor backgroundColor;
  if(option.state & QStyle::State_Selected)
  {
    if(option.state & QStyle::State_Active)
    {
      textColor = SVStyle::Instance()->getQTreeViewItemSelectedActive_color();
      backgroundColor = SVStyle::Instance()->getQTreeViewItemSelectedActive_background_color();
    }
    else
    {
      textColor = SVStyle::Instance()->getQTreeViewItemSelectedNotActive_color();
      backgroundColor = SVStyle::Instance()->getQTreeViewItemSelectedNotActive_background_color();
    }
  }
  else if((option.state & QStyle::State_MouseOver))
  {
    textColor = SVStyle::Instance()->getQTreeViewItemHover_color();
    backgroundColor = SVStyle::Instance()->getQTreeViewItemHover_background_color();
  }
  else
  {
    textColor = SVStyle::Instance()->getQTreeViewItem_color();
    backgroundColor = SVStyle::Instance()->getQTreeViewItem_background_color();
  }

  if(!backgroundColor.isValid())
  {
    if(option.state & QStyle::State_Selected)
    {
      backgroundColor = option.palette.color(QPalette::Highlight);
    }
    else if(option.features & QStyleOptionViewItem::Alternate)
    {
      backgroundColor = option.palette.color(QPalette::AlternateBase);
    }
    else
    {
      backgroundColor = option.palette.color(QPalette::Base);
    }
  }

  // Fill Background
  painter->fillRect(option.rect, backgroundColor);

  // Get Filter
  VSAbstractFilter* filter = getFilterFromIndex(index);
  bool hasVisibilityButton = filter->isCheckable();
  const VSFilterViewModel* filterViewModel = dynamic_cast<const VSFilterViewModel*>(index.model());
  QRect textRect = getTextRect(option, !hasVisibilityButton);

  // Draw Text
  painter->setPen(textColor);
  painter->drawText(textRect, filter->getFilterName());

  // Draw Visibility Button
  if(hasVisibilityButton)
  {
    QRect buttonRect = getVisibilityRect(option, index);
    getVisibilityIcon(filterViewModel, filter).paint(painter, buttonRect);
  }
}
