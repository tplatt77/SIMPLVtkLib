/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <QtCore/QEvent>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QStyledItemDelegate>

class VSAbstractFilter;
class VSFilterViewModel;

/**
 * @class VSFilterViewDelegate VSFilterViewDelegate.h SIMPLVtkLib/QtWidgets/VSFilterViewDelegate.h
 * @brief This class handles the displaying of VSAbstractFilter data in VSFilterView
 */
class VSFilterViewDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  explicit VSFilterViewDelegate(QObject* parent = 0);
  virtual ~VSFilterViewDelegate() = default;

protected:
  enum class ButtonState : unsigned char
  {
    Normal = 0,
    MouseOver,
    Pressed
  };

  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
  void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
  bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) Q_DECL_OVERRIDE;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const Q_DECL_OVERRIDE;
  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const Q_DECL_OVERRIDE;

  /**
   * @brief Returns the visualization filter for the given index
   * @param index
   * @return
   */
  VSAbstractFilter* getFilterFromIndex(const QModelIndex& index) const;

  /**
   * @brief Returns the corresponding visibility icon for the given filter and filter view model
   * @param filterViewModel
   * @param filter
   * @return
   */
  const QIcon getVisibilityIcon(const VSFilterViewModel* filterViewModel, VSAbstractFilter* filter) const;

  /**
   * @brief Returns the QRect where the visibility icon should be displayed
   * @param option
   * @param index
   * @return
   */
  QRect getVisibilityRect(const QStyleOptionViewItem& option, const QModelIndex& index) const;

  /**
   * @brief Returns the QRect where the filter text should be displayed
   * @param option
   * @param drawButtons
   * @return
   */
  QRect getTextRect(const QStyleOptionViewItem& option, const bool& drawButtons) const;

  /**
   * @brief Imports the visibility icons
   */
  static void SetupIcons();
  static QIcon s_VisibleIcon;
  static QIcon s_InvisibleIcon;

private:
  ButtonState m_VisibilityButtonState = ButtonState::Normal;
  QModelIndex m_MouseIndex;

  VSFilterViewDelegate(const VSFilterViewDelegate&) = delete; // Copy Constructor Not Implemented
  void operator=(const VSFilterViewDelegate&) = delete;       // Move assignment Not Implemented
};