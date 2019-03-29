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

#include "ImageSourceSelectionPage.h"
#include "PerformMontageWizard.h"

#include <QtCore/QDir>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "PerformMontage/PerformMontageConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageSourceSelectionPage::ImageSourceSelectionPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::ImageSourceSelectionPage)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImageSourceSelectionPage::~ImageSourceSelectionPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageSourceSelectionPage::setupGui()
{
  registerFields();

	connectSignalsSlots();

  // Set the default radio button selection
  m_Ui->datasetsRB->setChecked(true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageSourceSelectionPage::connectSignalsSlots()
{
  connect(m_Ui->datasetsRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(PerformMontage::FieldNames::ImageSource, PerformMontageWizard::ImageSource::Datasets);
    }
  });

  connect(m_Ui->dataContainersRB, &QRadioButton::toggled, [=] (bool checked) {
    if (checked)
    {
      setField(PerformMontage::FieldNames::ImageSource, PerformMontageWizard::ImageSource::DataContainers);
    }
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool ImageSourceSelectionPage::isComplete() const
{
  bool result = true;
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImageSourceSelectionPage::registerFields()
{
  registerField(PerformMontage::FieldNames::ImageSource, this, "ImageSource");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ImageSourceSelectionPage::nextId() const
{
  if (m_Ui->datasetsRB->isChecked())
  {
	emit m_Ui->datasetsRB->toggled(true);
    return PerformMontageWizard::WizardPages::DisplayTypeSelection;
  }
  else if (m_Ui->dataContainersRB->isChecked())
  {
	return PerformMontageWizard::WizardPages::DisplayTypeSelection;
  }
  else
  {
	return -1;
  }
}

