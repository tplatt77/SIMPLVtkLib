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

#include "FileTypeSelectionPage.h"
#include "ImportMontageWizard.h"

#include <QtCore/QDir>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "ImportMontage/ImportMontageConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileTypeSelectionPage::FileTypeSelectionPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::FileTypeSelectionPage)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FileTypeSelectionPage::~FileTypeSelectionPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileTypeSelectionPage::setupGui()
{
  registerFields();

	connectSignalsSlots();

  // Set the default radio button selection
  m_Ui->imageFileListRB->setChecked(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileTypeSelectionPage::connectSignalsSlots()
{
  connect(m_Ui->imageFileListRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(ImportMontage::FieldNames::InputType, ImportMontageWizard::InputType::Generic);
    }
  });

  connect(m_Ui->dream3dFileRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(ImportMontage::FieldNames::InputType, ImportMontageWizard::InputType::DREAM3D);
    }
  });

  connect(m_Ui->zeissFileRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(ImportMontage::FieldNames::InputType, ImportMontageWizard::InputType::Zeiss);
    }
  });

  connect(m_Ui->fijiFileRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(ImportMontage::FieldNames::InputType, ImportMontageWizard::InputType::Fiji);
    }
  });

  connect(m_Ui->robometFileRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(ImportMontage::FieldNames::InputType, ImportMontageWizard::InputType::Robomet);
    }
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool FileTypeSelectionPage::isComplete() const
{
  bool result = true;
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FileTypeSelectionPage::registerFields()
{
  registerField(ImportMontage::FieldNames::InputType, this, "InputType");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FileTypeSelectionPage::nextId() const
{
  if (m_Ui->imageFileListRB->isChecked())
  {
	  emit m_Ui->imageFileListRB->toggled(true);
    return ImportMontageWizard::WizardPages::GenericMetadata;
	}
  else if (m_Ui->dream3dFileRB->isChecked())
	{
    return ImportMontageWizard::WizardPages::EnterDREAM3DFile;
	}
  else if (m_Ui->fijiFileRB->isChecked())
  {
    return ImportMontageWizard::WizardPages::EnterFijiFile;
  }
  else if (m_Ui->robometFileRB->isChecked())
  {
    return ImportMontageWizard::WizardPages::EnterRobometFile;
  }
  else if(m_Ui->zeissFileRB->isChecked())
  {
    return ImportMontageWizard::WizardPages::EnterZeissFile;
  }
	else
	{
		return -1;
	}
}

