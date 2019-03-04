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

#include "EnterRobometDataPage.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"

#include "ImportMontage/ImportMontageConstants.h"

#include "ImportMontageWizard.h"

// Initialize private static member variable
QString EnterRobometDataPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterRobometDataPage::EnterRobometDataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::EnterRobometDataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterRobometDataPage::~EnterRobometDataPage()
{
  disconnectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::setupGui()
{
	connectSignalsSlots();

  // Store the advancedGridLayout inside the QtSDisclosableWidget
  QtSDisclosableWidget* advancedGB = new QtSDisclosableWidget(this);
  advancedGB->setTitle("Advanced");
  QLayout* advancedGridLayout = m_Ui->advancedGridLayout;
  advancedGridLayout->setParent(nullptr);
  advancedGB->setContentLayout(advancedGridLayout);

  m_Ui->gridLayout->addWidget(advancedGB, 9, 0, 1, 4);
}

QPair<int,int> gridPosition(QWidget* widget)
{
  auto gp = qMakePair(-1,-1);
  if (!widget->parentWidget())
  {
    return gp;
  }

  auto layout = dynamic_cast<QGridLayout*>(widget->parentWidget()->layout());
  if (!layout)
  {
    return gp;
  }

  int index = layout->indexOf(widget);
  if (index < 0)
  {
    return gp;
  }

  int rs,cs;
  layout->getItemPosition(index, &gp.first, &gp.second, &rs, &cs);
  return gp;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::connectSignalsSlots()
{
  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, &EnterRobometDataPage::montageName_textChanged);

  connect(m_Ui->imageDataContainerPrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->cellAttrMatrixNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageArrayNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });

  connect(m_Ui->robometListWidget, &RobometListWidget::inputDirectoryChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::filePrefixChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::fileSuffixChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::fileExtensionChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::sliceMinChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::sliceMaxChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::numberOfRowsChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::numberOfColumnsChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::slicePaddingChanged, this, &EnterRobometDataPage::robometListWidgetChanged);
  connect(m_Ui->robometListWidget, &RobometListWidget::rowColPaddingChanged, this, &EnterRobometDataPage::robometListWidgetChanged);

  connect(m_Ui->changeTileOverlapCB, &QCheckBox::stateChanged, this, &EnterRobometDataPage::changeTileOverlap_stateChanged);
  connect(m_Ui->manualDCAElementNamesCB, &QCheckBox::stateChanged, this, &EnterRobometDataPage::manualDCAElementNames_stateChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::disconnectSignalsSlots()
{
  disconnect(m_Ui->imageDataContainerPrefixLE, &QLineEdit::textChanged, 0, 0);
  disconnect(m_Ui->cellAttrMatrixNameLE, &QLineEdit::textChanged, 0, 0);
  disconnect(m_Ui->imageArrayNameLE, &QLineEdit::textChanged, 0, 0);
  disconnect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), 0, 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::robometListWidgetChanged()
{
  RobometListInfo_t robometListInfo = m_Ui->robometListWidget->getRobometListInfo();
  setRobometListInfo(robometListInfo);

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::changeTileOverlap_stateChanged(int state)
{
  m_Ui->tileOverlapSB->setEnabled(state);
  if (state == false)
  {
    m_Ui->tileOverlapSB->setValue(15);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::manualDCAElementNames_stateChanged(int state)
{
  bool isChecked = m_Ui->manualDCAElementNamesCB->isChecked();
  m_Ui->imageDataContainerPrefixLE->setEnabled(isChecked);
  m_Ui->cellAttrMatrixNameLE->setEnabled(isChecked);
  m_Ui->imageArrayNameLE->setEnabled(isChecked);

  montageName_textChanged(m_Ui->montageNameLE->text());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::montageName_textChanged(const QString &text)
{
  if (!m_Ui->manualDCAElementNamesCB->isChecked())
  {
    m_Ui->imageDataContainerPrefixLE->setText(tr("%1_").arg(text));
    m_Ui->cellAttrMatrixNameLE->setText(tr("Cell Attribute Matrix"));
    m_Ui->imageArrayNameLE->setText(tr("Image Data"));
  }

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterRobometDataPage::isComplete() const
{
  bool result = true;

  if (m_Ui->montageNameLE->isEnabled())
  {
    if (m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->robometListWidget->isEnabled())
  {
    if (!m_Ui->robometListWidget->isComplete())
    {
      result = false;
    }
  }

  if (m_Ui->imageDataContainerPrefixLE->isEnabled())
  {
    if (m_Ui->imageDataContainerPrefixLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->cellAttrMatrixNameLE->isEnabled())
  {
    if (m_Ui->cellAttrMatrixNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->imageArrayNameLE->isEnabled())
  {
    if (m_Ui->imageArrayNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->tileOverlapSB->isEnabled())
  {
    if (m_Ui->tileOverlapSB->value() < m_Ui->tileOverlapSB->minimum() || m_Ui->tileOverlapSB->value() > m_Ui->tileOverlapSB->maximum())
    {
      result = false;
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::registerFields()
{
  registerField(ImportMontage::Robomet::FieldNames::MontageName, m_Ui->montageNameLE);
  registerField(ImportMontage::Robomet::FieldNames::RobometListInfo, this, "RobometListInfo");
  registerField(ImportMontage::Robomet::FieldNames::DataContainerPrefix, m_Ui->imageDataContainerPrefixLE);
  registerField(ImportMontage::Robomet::FieldNames::CellAttributeMatrixName, m_Ui->cellAttrMatrixNameLE);
  registerField(ImportMontage::Robomet::FieldNames::ImageArrayName, m_Ui->imageArrayNameLE);
  registerField(ImportMontage::Robomet::FieldNames::TileOverlap, m_Ui->tileOverlapSB);
  registerField(ImportMontage::Robomet::FieldNames::ChangeTileOverlap, m_Ui->changeTileOverlapCB);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnterRobometDataPage::nextId() const
{
  return ImportMontageWizard::WizardPages::DataDisplayOptions;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterRobometDataPage::cleanupPage()
{
  // Do not return the page to its original values if the user clicks back.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterRobometDataPage::validatePage()
{
  return true;
}
