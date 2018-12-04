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

#include "GenericCollectionTypePage.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericCollectionTypePage::GenericCollectionTypePage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::GenericCollectionTypePage)
{
  m_Ui->setupUi(this);

  setupGui();
  
  // Register fields
  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
GenericCollectionTypePage::~GenericCollectionTypePage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericCollectionTypePage::setupGui()
{
  connectSignalsSlots();

  updateOrderChoices(CollectionType::RowByRow);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GenericCollectionTypePage::connectSignalsSlots()
{
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
void GenericCollectionTypePage::updateOrderChoices(CollectionType collectionType)
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
bool GenericCollectionTypePage::isComplete() const
{
  bool result = true;

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

void GenericCollectionTypePage::registerFields()
{
	registerField("montageType", collectionTypeCB);
	registerField("montageOrder", orderCB);
}
