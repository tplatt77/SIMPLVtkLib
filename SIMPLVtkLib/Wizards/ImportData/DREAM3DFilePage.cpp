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

#include "DREAM3DFilePage.h"
#include "ImportDataWizard.h"

#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QFileDialog>

#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"
#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

// Initialize private static member variable
QString DREAM3DFilePage::m_OpenDialogLastDirectory = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DFilePage::DREAM3DFilePage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::DREAM3DFilePage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DFilePage::~DREAM3DFilePage() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFilePage::setupGui()
{
	connectSignalsSlots();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFilePage::connectSignalsSlots()
{
	connect(m_Ui->DREAM3DFileLE, &QLineEdit::textChanged, [=] { emit completeChanged(); });

	connect(m_Ui->selectButton, &QPushButton::clicked, this, &DREAM3DFilePage::selectBtn_clicked);

	QtSFileCompleter* com = new QtSFileCompleter(this, true);
	m_Ui->DREAM3DFileLE->setCompleter(com);
	connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &DREAM3DFilePage::dream3dFile_textChanged);
	connect(m_Ui->DREAM3DFileLE, &QLineEdit::textChanged, this, &DREAM3DFilePage::dream3dFile_textChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DREAM3DFilePage::isComplete() const
{
  bool result = true;

  if (m_Ui->DREAM3DFileLE->isEnabled())
  {
	  if (m_Ui->DREAM3DFileLE->text().isEmpty())
	  {
		  result = false;
	  }
  }
  return result;
}

void DREAM3DFilePage::registerFields()
{
	registerField("DREAM3DFile", m_Ui->DREAM3DFileLE);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFilePage::selectBtn_clicked()
{
	QMimeDatabase db;

	QMimeType pngType = db.mimeTypeForName("image/png");
	QStringList pngSuffixes = pngType.suffixes();
	QString pngSuffixStr = pngSuffixes.join(" *.");
	pngSuffixStr.prepend("*.");

	QMimeType tiffType = db.mimeTypeForName("image/tiff");
	QStringList tiffSuffixes = tiffType.suffixes();
	QString tiffSuffixStr = tiffSuffixes.join(" *.");
	tiffSuffixStr.prepend("*.");

	QMimeType jpegType = db.mimeTypeForName("image/jpeg");
	QStringList jpegSuffixes = jpegType.suffixes();
	QString jpegSuffixStr = jpegSuffixes.join(" *.");
	jpegSuffixStr.prepend("*.");

	QMimeType bmpType = db.mimeTypeForName("image/bmp");
	QStringList bmpSuffixes = bmpType.suffixes();
	QString bmpSuffixStr = bmpSuffixes.join(" *.");
	bmpSuffixStr.prepend("*.");

	// Open a file in the application
	QString filter = tr("Data Files (*.dream3d *.vtk *.vti *.vtp *.vtr *.vts *.vtu *.stl %1 %3 %3 %4);;"
		"DREAM.3D Files (*.dream3d);;"
		"Image Files (%1 %2 %3 %4);;"
		"VTK Files (*.vtk *.vti *.vtp *.vtr *.vts *.vtu);;"
		"STL Files (*.stl)")
		.arg(pngSuffixStr)
		.arg(tiffSuffixStr)
		.arg(jpegSuffixStr)
		.arg(bmpSuffixStr);

	QString outputFile = QFileDialog::getOpenFileName(this, tr("Select a DREAM3D File"), getInputDirectory(), filter);

	if (!outputFile.isNull())
	{
		m_Ui->DREAM3DFileLE->blockSignals(true);
		m_Ui->DREAM3DFileLE->setText(QDir::toNativeSeparators(outputFile));
		dream3dFile_textChanged(m_Ui->DREAM3DFileLE->text());
		setOpenDialogLastFilePath(outputFile);
		m_Ui->DREAM3DFileLE->blockSignals(false);
	}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFilePage::dream3dFile_textChanged(const QString& text)
{
	Q_UNUSED(text)

		SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
	QString inputPath = validator->convertToAbsolutePath(text);

	if (QtSFileUtils::VerifyPathExists(inputPath, m_Ui->DREAM3DFileLE))
	{
	}
	emit completeChanged();
	emit dream3dFileChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DFilePage::setInputDirectory(QString val)
{
	m_Ui->DREAM3DFileLE->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DREAM3DFilePage::getInputDirectory()
{
	if (m_Ui->DREAM3DFileLE->text().isEmpty())
	{
		return QDir::homePath();
	}
	return m_Ui->DREAM3DFileLE->text();
}

int DREAM3DFilePage::nextId() const
{
	return -1;
}

