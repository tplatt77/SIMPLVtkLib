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

#include "BatchProcessingFijiOptionsPage.h"
#include "ImportMontageWizard.h"

#include <QtCore/QDir>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "ImportMontage/ImportMontageConstants.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BatchProcessingFijiOptionsPage::BatchProcessingFijiOptionsPage(QWidget* parent)
  : QWizardPage(parent)
  , m_Ui(new Ui::BatchProcessingFijiOptionsPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
BatchProcessingFijiOptionsPage::~BatchProcessingFijiOptionsPage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BatchProcessingFijiOptionsPage::setupGui()
{
  connectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BatchProcessingFijiOptionsPage::connectSignalsSlots()
{
	connect(m_Ui->minTileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
	connect(m_Ui->maxTileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
	connect(m_Ui->stepTileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });

	connect(m_Ui->tileOverlapCB, &QCheckBox::stateChanged, this, &BatchProcessingFijiOptionsPage::constantTileOverlap_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BatchProcessingFijiOptionsPage::constantTileOverlap_stateChanged(int state)
{
	// Disable each of the spin boxes
	m_Ui->minTileOverlapSB->setEnabled(!state);
	m_Ui->maxTileOverlapSB->setEnabled(!state);
	m_Ui->stepTileOverlapSB->setEnabled(!state);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool BatchProcessingFijiOptionsPage::isComplete() const
{
  bool result = true;


  if(m_Ui->minTileOverlapSB->isEnabled())
  {
	  if(m_Ui->minTileOverlapSB->value() < m_Ui->minTileOverlapSB->minimum() || m_Ui->minTileOverlapSB->value() > m_Ui->minTileOverlapSB->maximum())
	  {
		  result = false;
	  }
  }


  if(m_Ui->maxTileOverlapSB->isEnabled())
  {
	  if(m_Ui->maxTileOverlapSB->value() < m_Ui->maxTileOverlapSB->minimum() || m_Ui->maxTileOverlapSB->value() > m_Ui->maxTileOverlapSB->maximum())
	  {
		  result = false;
	  }
  }



  if(m_Ui->stepTileOverlapSB->isEnabled())
  {
	  if(m_Ui->stepTileOverlapSB->value() < m_Ui->stepTileOverlapSB->minimum() || m_Ui->stepTileOverlapSB->value() > m_Ui->stepTileOverlapSB->maximum())
	  {
		  result = false;
	  }
  }
  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void BatchProcessingFijiOptionsPage::registerFields()
{
	registerField(ImportMontage::Fiji::FieldNames::ConstantTileOverlap, m_Ui->tileOverlapCB);
	registerField(ImportMontage::Fiji::FieldNames::MinTileOverlap, m_Ui->minTileOverlapSB);
	registerField(ImportMontage::Fiji::FieldNames::MaxTileOverlap, m_Ui->maxTileOverlapSB);
	registerField(ImportMontage::Fiji::FieldNames::StepTileOverlap, m_Ui->stepTileOverlapSB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int BatchProcessingFijiOptionsPage::nextId() const
{
  return ImportMontageWizard::WizardPages::DataDisplayOptions;
}

