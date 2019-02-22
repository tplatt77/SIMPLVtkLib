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

#include "EnterGenericDataPage.h"
#include "ImportMontage/ImportMontageWizard.h"

#include <QtCore/QDir>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "ImportMontage/ImportMontageConstants.h"
#include "ImportMontageWizard.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterGenericDataPage::EnterGenericDataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::EnterGenericDataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterGenericDataPage::~EnterGenericDataPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterGenericDataPage::setupGui()
{
  connectSignalsSlots();

  updateOrderChoices(CollectionType::RowByRow);

  m_Ui->numOfRowsSB->setMinimum(1);
  m_Ui->numOfRowsSB->setMaximum(std::numeric_limits<int>().max());

  m_Ui->numOfColsSB->setMinimum(1);
  m_Ui->numOfColsSB->setMaximum(std::numeric_limits<int>().max());

  m_Ui->tileOverlapSB->setMinimum(0);
  m_Ui->tileOverlapSB->setMaximum(100);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterGenericDataPage::connectSignalsSlots()
{
  connect(m_Ui->numOfRowsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfColsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });

  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, [=] { emit completeChanged(); });

  connect(m_Ui->tileListWidget, &TileListWidget::inputDirectoryChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::fileOrderingChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::filePrefixChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::fileSuffixChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::fileExtensionChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::startIndexChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::endIndexChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::incrementIndexChanged, [=] { tileListWidgetChanged(); });
  connect(m_Ui->tileListWidget, &TileListWidget::paddingDigitsChanged, [=] { tileListWidgetChanged(); });

  connect(m_Ui->outputTextFileNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->collectionTypeCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] (int index) {
    updateOrderChoices(static_cast<CollectionType>(index));
  });

  connect(m_Ui->orderCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] (int index) {
    emit completeChanged();
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterGenericDataPage::tileListWidgetChanged()
{
  emit completeChanged();

  FileListInfo_t fileListInfo = m_Ui->tileListWidget->getFileListInfo();
  setFileListInfo(fileListInfo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterGenericDataPage::isComplete() const
{
  bool result = true;

  if (m_Ui->montageNameLE->isEnabled())
  {
    if (m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->numOfRowsSB->isEnabled())
  {
    if (m_Ui->numOfRowsSB->value() < m_Ui->numOfRowsSB->minimum() ||  m_Ui->numOfRowsSB->value() > m_Ui->numOfRowsSB->maximum())
    {
      result = false;
    }
  }

  if (m_Ui->numOfColsSB->isEnabled())
  {
    if (m_Ui->numOfColsSB->value() < m_Ui->numOfColsSB->minimum() ||  m_Ui->numOfColsSB->value() > m_Ui->numOfColsSB->maximum())
    {
      result = false;
    }
  }

  if (m_Ui->tileOverlapSB->isEnabled())
  {
    if (m_Ui->tileOverlapSB->value() < m_Ui->tileOverlapSB->minimum() ||  m_Ui->tileOverlapSB->value() > m_Ui->tileOverlapSB->maximum())
    {
      result = false;
    }
  }

  if (m_Ui->tileListWidget->isEnabled())
  {
    if (!m_Ui->tileListWidget->isComplete())
    {
      result = false;
    }
  }

  if (m_Ui->outputTextFileNameLE->isEnabled())
  {
    if (m_Ui->outputTextFileNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->collectionTypeCB->isEnabled())
  {
    if (m_Ui->collectionTypeCB->currentIndex() < 0 ||  m_Ui->collectionTypeCB->currentIndex() > m_Ui->collectionTypeCB->maxCount() - 1)
    {
      result = false;
    }
  }

  if (m_Ui->orderCB->isEnabled())
  {
    if (m_Ui->orderCB->currentIndex() < 0 ||  m_Ui->orderCB->currentIndex() > m_Ui->orderCB->maxCount() - 1)
    {
      result = false;
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterGenericDataPage::registerFields()
{
  registerField(ImportMontage::Generic::FieldNames::MontageName, m_Ui->montageNameLE);
  registerField(ImportMontage::Generic::FieldNames::NumberOfRows, m_Ui->numOfRowsSB);
  registerField(ImportMontage::Generic::FieldNames::NumberOfColumns, m_Ui->numOfColsSB);
  registerField(ImportMontage::Generic::FieldNames::TileOverlap, m_Ui->tileOverlapSB);
  registerField(ImportMontage::Generic::FieldNames::FileListInfo, this);
  registerField(ImportMontage::Generic::FieldNames::OutputFileName, m_Ui->outputTextFileNameLE);
  registerField(ImportMontage::Generic::FieldNames::MontageType, m_Ui->collectionTypeCB);
  registerField(ImportMontage::Generic::FieldNames::MontageOrder, m_Ui->orderCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterGenericDataPage::updateOrderChoices(CollectionType collectionType)
{
  m_Ui->orderCB->setEnabled(true);
  m_Ui->orderCB->clear();

  switch (collectionType)
  {
    case CollectionType::RowByRow:
    {
      m_Ui->orderCB->addItem("Right & Down");
      m_Ui->orderCB->addItem("Left & Down");
      m_Ui->orderCB->addItem("Right & Up");
      m_Ui->orderCB->addItem("Left & Up");
      break;
    }
    case CollectionType::ColumnByColumn:
    {
      m_Ui->orderCB->addItem("Down & Right");
      m_Ui->orderCB->addItem("Down & Left");
      m_Ui->orderCB->addItem("Up & Right");
      m_Ui->orderCB->addItem("Up & Left");
      break;
    }
    case CollectionType::SnakeByRows:
    {
      m_Ui->orderCB->addItem("Right & Down");
      m_Ui->orderCB->addItem("Left & Down");
      m_Ui->orderCB->addItem("Right & Up");
      m_Ui->orderCB->addItem("Left & Up");
      break;
    }
    case CollectionType::SnakeByColumns:
    {
      m_Ui->orderCB->addItem("Down & Right");
      m_Ui->orderCB->addItem("Down & Left");
      m_Ui->orderCB->addItem("Up & Right");
      m_Ui->orderCB->addItem("Up & Left");
      break;
    }
    case CollectionType::FilenameDefinedPosition:
    {
      m_Ui->orderCB->setDisabled(true);
      break;
    }
  }

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnterGenericDataPage::nextId() const
{
  return ImportMontageWizard::WizardPages::DataDisplayOptions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterGenericDataPage::cleanupPage()
{
  // Do not return the page to its original values if the user clicks back.
}

