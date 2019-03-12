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

#include "EnterDREAM3DDataPage.h"

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

#include "SIMPLVtkLib/Wizards/ImportMontage/DataDisplayOptionsPage.h"

#include "ImportMontage/ImportMontageConstants.h"

#include "ImportMontageWizard.h"

// Initialize private static member variable
QString EnterDREAM3DDataPage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterDREAM3DDataPage::EnterDREAM3DDataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::EnterDREAM3DDataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EnterDREAM3DDataPage::~EnterDREAM3DDataPage()
{
	delete m_LoadingMovie;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::setupGui()
{
	m_Ui->loadHDF5DataWidget->setNavigationButtonsVisibility(false);
	
	connectSignalsSlots();
	
	m_Ui->loadHDF5DataWidget->setNavigationButtonsVisibility(false);

	m_Ui->numOfRowsSB->setMinimum(1);
	m_Ui->numOfRowsSB->setMaximum(std::numeric_limits<int>().max());

	m_Ui->numOfColsSB->setMinimum(1);
	m_Ui->numOfColsSB->setMaximum(std::numeric_limits<int>().max());

	m_Ui->tileOverlapSB->setMinimum(0);
	m_Ui->tileOverlapSB->setMaximum(100);
	
	m_Ui->originX->setValidator(new QDoubleValidator);
	m_Ui->originY->setValidator(new QDoubleValidator);
	m_Ui->originZ->setValidator(new QDoubleValidator);
	m_Ui->spacingX->setValidator(new QDoubleValidator);
	m_Ui->spacingY->setValidator(new QDoubleValidator);
	m_Ui->spacingZ->setValidator(new QDoubleValidator);

	// Store the advancedGridLayout inside the QtSDisclosableWidget
	QtSDisclosableWidget* advancedGB = new QtSDisclosableWidget(this);
	advancedGB->setTitle("Advanced");
	QLayout* advancedGridLayout = m_Ui->advancedGridLayout;
	advancedGridLayout->setParent(nullptr);
	advancedGB->setContentLayout(advancedGridLayout);

	m_Ui->gridLayout->addWidget(advancedGB, 9, 0, 1, 4);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::connectSignalsSlots()
{
  connect(m_Ui->loadHDF5DataWidget, &VSLoadHDF5DataWidget::proxyChanged, this, &EnterDREAM3DDataPage::proxyChanged);
  connect(m_Ui->selectButton, &QPushButton::clicked, this, &EnterDREAM3DDataPage::selectBtn_clicked);

	QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->dataFileLE->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &EnterDREAM3DDataPage::dataFile_textChanged);
  connect(m_Ui->dataFileLE, &QLineEdit::textChanged, this, &EnterDREAM3DDataPage::dataFile_textChanged);

  connect(m_Ui->montageNameLE, &QLineEdit::textChanged, this, [=] { emit completeChanged(); });

  connect(m_Ui->imageDataContainerPrefixLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->cellAttrMatrixNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->imageArrayNameLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->tileOverlapSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfRowsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });
  connect(m_Ui->numOfColsSB, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=] { emit completeChanged(); });

  connect(m_Ui->loadHDF5DataWidget, &VSLoadHDF5DataWidget::proxyChanged, this, &EnterDREAM3DDataPage::proxyChanged);

  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, [=] {
    bool isChecked = m_Ui->changeOriginCB->isChecked();
    m_Ui->originX->setEnabled(isChecked);
    m_Ui->originY->setEnabled(isChecked);
    m_Ui->originZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeOriginCB, &QCheckBox::stateChanged, this, &EnterDREAM3DDataPage::changeOrigin_stateChanged);
  connect(m_Ui->originX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->originZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });

  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, [=] {
    bool isChecked = m_Ui->changeSpacingCB->isChecked();
    m_Ui->spacingX->setEnabled(isChecked);
    m_Ui->spacingY->setEnabled(isChecked);
    m_Ui->spacingZ->setEnabled(isChecked);
  });
  connect(m_Ui->changeSpacingCB, &QCheckBox::stateChanged, this, &EnterDREAM3DDataPage::changeSpacing_stateChanged);
  connect(m_Ui->spacingX, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingY, &QLineEdit::textChanged, [=] { emit completeChanged(); });
  connect(m_Ui->spacingZ, &QLineEdit::textChanged, [=] { emit completeChanged(); });
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::changeOrigin_stateChanged(int state)
{
  m_Ui->originX->setEnabled(state);
  m_Ui->originY->setEnabled(state);
  m_Ui->originZ->setEnabled(state);
  if(state == false)
  {
    m_Ui->originX->setText("0");
    m_Ui->originY->setText("0");
    m_Ui->originZ->setText("0");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::changeSpacing_stateChanged(int state)
{
  m_Ui->spacingX->setEnabled(state);
  m_Ui->spacingY->setEnabled(state);
  m_Ui->spacingZ->setEnabled(state);
  if(state == false)
  {
    m_Ui->spacingX->setText("1");
    m_Ui->spacingY->setText("1");
    m_Ui->spacingZ->setText("1");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::proxyChanged(DataContainerArrayProxy proxy)
{
	size_t checkCount = 0;
	QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
	
	int rowCount = 0;
	int colCount = 0;

	bool variablesSet = false;

	for(QMap<QString, DataContainerProxy>::iterator iter = dataContainers.begin(); iter != dataContainers.end(); iter++)
	{
		DataContainerProxy dcProxy = iter.value();
		if(dcProxy.getFlag() == Qt::Checked ||
			dcProxy.getFlag() == Qt::PartiallyChecked)
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

		if(!variablesSet)
		{
		  // Automatically set the data array path variables
		  QMap<QString, AttributeMatrixProxy>& attributeMatricies = dcProxy.getAttributeMatricies();
		  AttributeMatrixProxy attributeMatrixProxy = attributeMatricies.first();
		  QMap<QString, DataArrayProxy>& dataArrays = attributeMatrixProxy.getDataArrays();
		  DataArrayProxy dataArrayProxy = dataArrays.first();
		  m_Ui->imageDataContainerPrefixLE->setText(dcProxy.getName().split("_").first());
		  m_Ui->cellAttrMatrixNameLE->setText(attributeMatrixProxy.getName());
		  m_Ui->imageArrayNameLE->setText(dataArrayProxy.getName());
		  variablesSet = true;
		}
	}

	m_Ui->numOfRowsSB->setValue(rowCount);
	m_Ui->numOfColsSB->setValue(colCount);

	setFinalPage(checkCount <= 1);

	emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EnterDREAM3DDataPage::isComplete() const
{
  bool result = true;

  if (m_Ui->montageNameLE->isEnabled())
  {
    if (m_Ui->montageNameLE->text().isEmpty())
    {
      result = false;
    }
  }

  if (m_Ui->dataFileLE->isEnabled())
  {
    if (m_Ui->dataFileLE->text().isEmpty())
    {
      result = false;
    }
  }

  QFileInfo fi(m_Ui->dataFileLE->text());
  if (fi.completeSuffix() != "dream3d")
  {
    result = false;
  }

  if (m_Ui->numOfRowsSB->isEnabled())
  {
    if (m_Ui->numOfRowsSB->value() < m_Ui->numOfRowsSB->minimum() || m_Ui->numOfRowsSB->value() > m_Ui->numOfRowsSB->maximum())
    {
      result = false;
    }
  }

  if (m_Ui->numOfColsSB->isEnabled())
  {
    if (m_Ui->numOfColsSB->value() < m_Ui->numOfColsSB->minimum() || m_Ui->numOfColsSB->value() > m_Ui->numOfColsSB->maximum())
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

  DataContainerArrayProxy proxy = getProxy();

  bool allChecked = true;
  Qt::CheckState checkState = Qt::Unchecked;
  QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
  for(QMap<QString, DataContainerProxy>::iterator iter = dataContainers.begin(); iter != dataContainers.end(); iter++)
  {
	  DataContainerProxy dcProxy = iter.value();
	  if(dcProxy.getFlag() == Qt::Checked ||
		  dcProxy.getFlag() == Qt::PartiallyChecked)
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
	  return false;
  }

  if(m_LoadingProxy)
  {
	  return false;
  }

  if(m_Ui->originX->isEnabled())
  {
    if(m_Ui->originX->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->originY->isEnabled())
  {
    if(m_Ui->originY->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->originZ->isEnabled())
  {
    if(m_Ui->originZ->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->spacingX->isEnabled())
  {
    if(m_Ui->spacingX->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->spacingY->isEnabled())
  {
    if(m_Ui->spacingY->text().isEmpty())
    {
      result = false;
    }
  }

  if(m_Ui->spacingZ->isEnabled())
  {
    if(m_Ui->spacingZ->text().isEmpty())
    {
      result = false;
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::initializePage()
{
	QString filePath = m_Ui->dataFileLE->text();

	m_Ui->loadHDF5DataWidget->initialize(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::registerFields()
{
  registerField(ImportMontage::DREAM3D::FieldNames::MontageName, m_Ui->montageNameLE);
  registerField(ImportMontage::DREAM3D::FieldNames::DataFilePath, m_Ui->dataFileLE);
  registerField(ImportMontage::DREAM3D::FieldNames::NumberOfRows, m_Ui->numOfRowsSB);
  registerField(ImportMontage::DREAM3D::FieldNames::NumberOfColumns, m_Ui->numOfColsSB);
  registerField(ImportMontage::DREAM3D::FieldNames::DataContainerPrefix, m_Ui->imageDataContainerPrefixLE);
  registerField(ImportMontage::DREAM3D::FieldNames::CellAttributeMatrixName, m_Ui->cellAttrMatrixNameLE);
  registerField(ImportMontage::DREAM3D::FieldNames::ImageArrayName, m_Ui->imageArrayNameLE);
  registerField(ImportMontage::DREAM3D::FieldNames::TileOverlap, m_Ui->tileOverlapSB);
  registerField(ImportMontage::DREAM3D::FieldNames::Proxy, m_Ui->loadHDF5DataWidget, "Proxy");
  registerField(ImportMontage::DREAM3D::FieldNames::ChangeOrigin, m_Ui->changeOriginCB);
  registerField(ImportMontage::DREAM3D::FieldNames::ChangeSpacing, m_Ui->changeSpacingCB);
  registerField(ImportMontage::DREAM3D::FieldNames::SpacingX, m_Ui->spacingX);
  registerField(ImportMontage::DREAM3D::FieldNames::SpacingY, m_Ui->spacingY);
  registerField(ImportMontage::DREAM3D::FieldNames::SpacingZ, m_Ui->spacingZ);
  registerField(ImportMontage::DREAM3D::FieldNames::OriginX, m_Ui->originX);
  registerField(ImportMontage::DREAM3D::FieldNames::OriginY, m_Ui->originY);
  registerField(ImportMontage::DREAM3D::FieldNames::OriginZ, m_Ui->originZ);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::selectBtn_clicked()
{
  QString filter = tr("DREAM3D File (*.dream3d);;All Files (*.*)");
  QString title = "Select a DREAM3D file";

  QString outputFile = QFileDialog::getOpenFileName(this, title,
		getInputDirectory(), filter);

	if (!outputFile.isNull())
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
void EnterDREAM3DDataPage::dataFile_textChanged(const QString& text)
{
	Q_UNUSED(text)

		SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
	QString inputPath = validator->convertToAbsolutePath(text);

  if (QtSFileUtils::VerifyPathExists(inputPath, m_Ui->dataFileLE))
	{
	}

  if (!inputPath.isEmpty())
  {
    QFileInfo fi(inputPath);
    QString ext = fi.completeSuffix();
    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForFile(inputPath, QMimeDatabase::MatchContent);

    setFinalPage(false);
  }

  QString filePath = m_Ui->dataFileLE->text();
  
  m_Ui->loadHDF5DataWidget->initialize(filePath);

	emit completeChanged();
  emit dataFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::setInputDirectory(QString val)
{
  m_Ui->dataFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EnterDREAM3DDataPage::getInputDirectory()
{
  if (m_Ui->dataFileLE->text().isEmpty())
	{
		return QDir::homePath();
	}
  return m_Ui->dataFileLE->text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int EnterDREAM3DDataPage::nextId() const
{
	DataContainerArrayProxy proxy = getProxy();

	size_t checkCount = 0;
	QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
	for(QMap<QString, DataContainerProxy>::iterator iter = dataContainers.begin(); iter != dataContainers.end(); iter++)
	{
		DataContainerProxy dcProxy = iter.value();
		if(dcProxy.getFlag() == Qt::Checked)
		{
			checkCount++;
		}
	}

	if(checkCount > 1)
	{
		QString filePath = m_Ui->dataFileLE->text();

		m_Ui->loadHDF5DataWidget->initialize(filePath);
		return ImportMontageWizard::WizardPages::DataDisplayOptions;
	}

	return -1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::cleanupPage()
{
  // Do not return the page to its original values if the user clicks back.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EnterDREAM3DDataPage::setProxy(DataContainerArrayProxy proxy)
{
	m_Ui->loadHDF5DataWidget->setProxy(proxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy EnterDREAM3DDataPage::getProxy() const
{
	return m_Ui->loadHDF5DataWidget->getProxy();
}
