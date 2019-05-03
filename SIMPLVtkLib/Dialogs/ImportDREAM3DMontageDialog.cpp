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

#include "ImportDREAM3DMontageDialog.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtGui/QMovie>

#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileSystemModel>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSDisclosableWidget.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

// Initialize private static member variable
QString ImportDREAM3DMontageDialog::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDREAM3DMontageDialog::ImportDREAM3DMontageDialog(QWidget* parent)
: AbstractImportMontageDialog(parent)
, m_Ui(new Ui::ImportDREAM3DMontageDialog)
{
  m_Ui->setupUi(this);

  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDREAM3DMontageDialog::~ImportDREAM3DMontageDialog()
{
  delete m_LoadingMovie;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ImportDREAM3DMontageDialog::Pointer ImportDREAM3DMontageDialog::New(QWidget* parent)
{
  Pointer sharedPtr(new ImportDREAM3DMontageDialog(parent));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::setupGui()
{
  m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Import");

  m_Ui->loadHDF5DataWidget->setNavigationButtonsVisibility(false);

  connectSignalsSlots();

  m_Ui->loadHDF5DataWidget->setNavigationButtonsVisibility(false);

  m_Ui->numOfRowsSB->setMinimum(1);
  m_Ui->numOfRowsSB->setMaximum(std::numeric_limits<int>().max());

  m_Ui->numOfColsSB->setMinimum(1);
  m_Ui->numOfColsSB->setMaximum(std::numeric_limits<int>().max());

  setDisplayType(AbstractImportMontageDialog::DisplayType::Outline);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::connectSignalsSlots()
{
  connect(m_Ui->dataDisplayTypeCB, qOverload<int>(&QComboBox::currentIndexChanged), [=](int index) { setDisplayType(static_cast<AbstractImportMontageDialog::DisplayType>(index)); });

  connect(m_Ui->loadHDF5DataWidget, &VSLoadHDF5DataWidget::proxyChanged, this, &ImportDREAM3DMontageDialog::proxyChanged);
  connect(m_Ui->selectButton, &QPushButton::clicked, this, &ImportDREAM3DMontageDialog::selectBtn_clicked);

  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &ImportDREAM3DMontageDialog::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &ImportDREAM3DMontageDialog::dataFile_textChanged);

  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, [=] { checkComplete(); });

  connect(m_Ui->numOfRowsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { checkComplete(); });
  connect(m_Ui->numOfColsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { checkComplete(); });

  connect(m_Ui->loadHDF5DataWidget, &VSLoadHDF5DataWidget::proxyChanged, this, &ImportDREAM3DMontageDialog::proxyChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::proxyChanged(DataContainerArrayProxy proxy)
{
  size_t checkCount = 0;
  QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();

  int rowCount = 0;
  int colCount = 0;

  for(QMap<QString, DataContainerProxy>::iterator iter = dataContainers.begin(); iter != dataContainers.end(); iter++)
  {
    DataContainerProxy dcProxy = iter.value();
    if(dcProxy.getFlag() == Qt::Checked || dcProxy.getFlag() == Qt::PartiallyChecked)
    {
      checkCount++;
    }

    QString dcName = dcProxy.getName();
    QString rowColString = dcName.split("_").last();
    QString rowString = rowColString.split("c").first().replace("r", "");
    QString colString = rowColString.split("c").last();
    int rowNumber = rowString.toInt();
    if(rowNumber >= rowCount)
    {
      rowCount = rowNumber + 1;
    }
    int colNumber = colString.toInt();
    if(colNumber >= colCount)
    {
      colCount = colNumber + 1;
    }
  }

  m_Ui->numOfRowsSB->setValue(rowCount);
  m_Ui->numOfColsSB->setValue(colCount);

  checkComplete();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::checkComplete() const
{
  bool result = true;

  if(m_Ui->montageNameLE->isEnabled())
  {
    if(m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->dataFileLE->isEnabled())
  {
    if(m_Ui->dataFileLE->text().isEmpty())
    {
      result = false;
    }
  }

  QFileInfo fi(m_Ui->dataFileLE->text());
  if(fi.completeSuffix() != "dream3d")
  {
    result = false;
  }

  if(m_Ui->numOfRowsSB->isEnabled())
  {
    if(m_Ui->numOfRowsSB->value() < m_Ui->numOfRowsSB->minimum() || m_Ui->numOfRowsSB->value() > m_Ui->numOfRowsSB->maximum())
    {
      result = false;
    }
  }

  if(m_Ui->numOfColsSB->isEnabled())
  {
    if(m_Ui->numOfColsSB->value() < m_Ui->numOfColsSB->minimum() || m_Ui->numOfColsSB->value() > m_Ui->numOfColsSB->maximum())
    {
      result = false;
    }
  }

  DataContainerArrayProxy proxy = getProxy();

  bool allChecked = true;
  Qt::CheckState checkState = Qt::Unchecked;
  QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
  for(QMap<QString, DataContainerProxy>::iterator iter = dataContainers.begin(); iter != dataContainers.end(); iter++)
  {
    DataContainerProxy dcProxy = iter.value();
    if(dcProxy.getFlag() == Qt::Checked || dcProxy.getFlag() == Qt::PartiallyChecked)
    {
      checkState = Qt::PartiallyChecked;
    }
    else
    {
      allChecked = false;
    }
  }

  if(allChecked == true)
  {
    checkState = Qt::Checked;
  }

  if(checkState == Qt::Unchecked)
  {
    result = false;
  }

  if(m_LoadingProxy)
  {
    result = false;
  }

  QPushButton* okBtn = m_Ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok);
  if(!okBtn)
  {
    throw InvalidOKButtonException();
  }

  okBtn->setEnabled(result);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::initializePage()
{
  QString filePath = m_Ui->dataFileLE->text();

  m_Ui->loadHDF5DataWidget->initialize(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::selectBtn_clicked()
{
  QString filter = tr("DREAM3D File (*.dream3d);;All Files (*.*)");
  QString title = "Select a DREAM3D file";

  QString outputFile = QFileDialog::getOpenFileName(this, title, getInputDirectory(), filter);

  if(!outputFile.isNull())
  {
    m_Ui->dataFileLE->blockSignals(true);
    m_Ui->dataFileLE->setText(QDir::toNativeSeparators(outputFile));
    dataFile_textChanged(m_Ui->dataFileLE->text());
    setOpenDialogLastFilePath(outputFile);
    m_Ui->dataFileLE->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::dataFile_textChanged(const QString& text)
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->dataFileLE))
  {
  }

  if(!inputPath.isEmpty())
  {
    QFileInfo fi(inputPath);
    QString ext = fi.completeSuffix();
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(inputPath, QMimeDatabase::MatchContent);
  }

  QString filePath = m_Ui->dataFileLE->text();

  m_Ui->loadHDF5DataWidget->initialize(filePath);

  checkComplete();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportDREAM3DMontageDialog::getInputDirectory()
{
  if(m_Ui->dataFileLE->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ImportDREAM3DMontageDialog::setProxy(DataContainerArrayProxy proxy)
{
  m_Ui->loadHDF5DataWidget->setProxy(proxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy ImportDREAM3DMontageDialog::getProxy() const
{
  return m_Ui->loadHDF5DataWidget->getProxy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportDREAM3DMontageDialog::getMontageName()
{
  return m_Ui->montageNameLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::tuple<int, int> ImportDREAM3DMontageDialog::getMontageDimensions()
{
  std::tuple<int, int> dims = std::make_tuple(m_Ui->numOfRowsSB->value(), m_Ui->numOfColsSB->value());
  return dims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportDREAM3DMontageDialog::getDataFilePath()
{
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportDREAM3DMontageDialog::getAttributeMatrixName()
{
  return m_Ui->cellAttrMatrixNameLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ImportDREAM3DMontageDialog::getDataArrayName()
{
  return m_Ui->imageArrayNameLE->text();
}
