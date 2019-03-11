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

#include <QtWidgets/QWidget>

#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/Filtering/FilterPipeline.h"

#include "SIMPLVtkLib/QtWidgets/VSAbstractImporter.h"

#include "ui_VSQueueWidget.h"

/**
 * @class VSQueueWidget VSQueueWidget.h SIMPLVtkLib/QtWidgets/VSQueueWidget.h
 */
class VSQueueWidget : public QWidget
{
  Q_OBJECT

public:
  SIMPL_SHARED_POINTERS(VSQueueWidget)

  /**
   * @brief Deconstructor
   */
  virtual ~VSQueueWidget() = default;

  /**
   * @brief New
   * @param parent
   * @return
   */
  static Pointer New(QWidget* parent = nullptr);

public slots:
  /**
   * @brief addImporterToQueue
   * @param name
   * @param importer
   */
  void addImporterToQueue(const QString &name, VSAbstractImporter::Pointer importer);

  /**
   * @brief removeImporterFromQueue
   * @param name
   * @param importer
   */
  void removeImporterFromQueue(const QString &name, VSAbstractImporter::Pointer importer);

protected:
  /**
   * @brief Constructor
   * @param parent
   */
  VSQueueWidget(QWidget* parent);

  /**
   * @brief Initializes some of the GUI elements with selections or other GUI related items
   */
  void setupGui();

private:
  QSharedPointer<Ui::VSQueueWidget> m_Ui;

  enum Roles
  {
    ImporterRole = Qt::UserRole + 1
  };

  /**
   * @brief connectSignalsSlots
   */
  void connectSignalsSlots();

};
