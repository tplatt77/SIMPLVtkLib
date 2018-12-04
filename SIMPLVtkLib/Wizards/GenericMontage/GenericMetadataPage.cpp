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

#include "GenericMetadataPage.h"

#include <QtCore/QDir>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMetadataPage::GenericMetadataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::GenericMetadataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericMetadataPage::~GenericMetadataPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMetadataPage::setupGui()
{
//  m_Ui->gridLayout->addWidget(outputTextFileNameLE, 4, 1, 1, 3);

  connectSignalsSlots();

  m_Ui->numOfRowsSB->setMinimum(1);
  m_Ui->numOfRowsSB->setMaximum(std::numeric_limits<int>().max());

  m_Ui->numOfColsSB->setMinimum(1);
  m_Ui->numOfColsSB->setMaximum(std::numeric_limits<int>().max());

  m_Ui->tileOverlapSB->setMinimum(0);
  m_Ui->tileOverlapSB->setMaximum(100);

  m_Ui->firstFileIdxSB->setMinimum(0);
  m_Ui->firstFileIdxSB->setMaximum(std::numeric_limits<int>().max());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericMetadataPage::connectSignalsSlots()
{
  connect(m_Ui->numOfRowsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfColsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->firstFileIdxSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->tilesDirLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->outputTextFileNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->fusionMethodCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=] { emit completeChanged(); });

  connect(m_Ui->tilesDirSelectBtn, &QPushButton::clicked, [=] {
    QString filePath = QFileDialog::getExistingDirectory(this, "Select Tiles Directory", m_OpenDialogLastDirectory);
    if(filePath.isEmpty())
    {
      return;
    }
    m_OpenDialogLastDirectory = filePath;
    m_Ui->tilesDirLE->setText(filePath);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool GenericMetadataPage::isComplete() const
{
  bool result = true;

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

  if (m_Ui->firstFileIdxSB->isEnabled())
  {
    if (m_Ui->firstFileIdxSB->value() < m_Ui->firstFileIdxSB->minimum() ||  m_Ui->firstFileIdxSB->value() > m_Ui->firstFileIdxSB->maximum())
    {
      result = false;
    }
  }

  if (m_Ui->tilesDirLE->isEnabled())
  {
    if (m_Ui->tilesDirLE->text().isEmpty())
    {
      result = false;
    }

    QDir dir(m_Ui->tilesDirLE->text());
    if (!dir.exists())
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

  if (m_Ui->fusionMethodCB->isEnabled())
  {
    if (m_Ui->fusionMethodCB->currentIndex() < 0 || m_Ui->fusionMethodCB->currentIndex() > m_Ui->fusionMethodCB->maxCount() - 1)
    {
      result = false;
    }
  }

  return result;
}

void GenericMetadataPage::registerFields()
{
	registerField("numOfRows", numOfRowsSB);
	registerField("numOfCols", numOfColsSB);
	registerField("tileOverlap", tileOverlapSB);
	registerField("firstFileIndex", firstFileIdxSB);
	registerField("tilesDirectory", tilesDirLE);
	registerField("outputFileName", outputTextFileNameLE);
	registerField("fusionMethod", comboBox);
}

