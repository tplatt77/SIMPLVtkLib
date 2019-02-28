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

#include "RobometListWidget.h"

//-- Qt Includes
#include <QtCore/QDir>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Utilities/FilePathGenerator.h"
#include "SIMPLib/Utilities/StringOperations.h"
#include "SIMPLib/Utilities/SIMPLDataPathValidator.h"

#include "SVWidgetsLib/Core/SVWidgetsLibConstants.h"
#include "SVWidgetsLib/QtSupport/QtSFileCompleter.h"
#include "SVWidgetsLib/QtSupport/QtSFileUtils.h"

#include "FilterParameterWidgetsDialogs.h"

// Initialize private static member variable
QString RobometListWidget::m_OpenDialogLastFilePath = "";

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RobometListWidget::RobometListWidget(QWidget* parent)
: QWidget(parent)
, m_Ui(new Ui::RobometListWidget)
{
  m_Ui->setupUi(this);
  setupGui();
  if(m_Ui->inputDir->text().isEmpty())
  {
    setInputDirectory(QDir::homePath());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RobometListWidget::~RobometListWidget() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::setWidgetListEnabled(bool b)
{
  foreach(QWidget* w, m_WidgetList)
  {
    w->setEnabled(b);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::setupGui()
{
  connectSignalsSlots();

  setupMenuField();

  m_Ui->numOfRows->setMaximum(std::numeric_limits<int>().max());
  m_Ui->numOfCols->setMaximum(std::numeric_limits<int>().max());
  m_Ui->slicePadding->setMaximum(std::numeric_limits<int>().max());
  m_Ui->rowColPadding->setMaximum(std::numeric_limits<int>().max());
  m_Ui->slice->setMaximum(std::numeric_limits<int>().max());

  m_Ui->absPathLabel->hide();

  m_WidgetList << m_Ui->inputDir << m_Ui->inputDirBtn;
  m_WidgetList << m_Ui->filePrefix << m_Ui->fileSuffix << m_Ui->fileExt << m_Ui->slice;
  m_WidgetList << m_Ui->numOfRows << m_Ui->numOfCols << m_Ui->slicePadding << m_Ui->rowColPadding;

  m_Ui->errorMessage->setVisible(false);

  // Update the widget when the data directory changes
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  connect(validator, &SIMPLDataPathValidator::dataDirectoryChanged, [=] {
    blockSignals(true);
    inputDir_textChanged(m_Ui->inputDir->text());
    blockSignals(false);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::connectSignalsSlots()
{
  // Connections for the various ui widgets
  connect(m_Ui->inputDirBtn, &QPushButton::clicked, this, &RobometListWidget::inputDirBtn_clicked);
  QtSFileCompleter* com = new QtSFileCompleter(this, true);
  m_Ui->inputDir->setCompleter(com);
  connect(com, static_cast<void (QtSFileCompleter::*)(const QString&)>(&QtSFileCompleter::activated), this, &RobometListWidget::inputDir_textChanged);
  connect(m_Ui->inputDir, &QtSLineEdit::textChanged, this, &RobometListWidget::inputDir_textChanged);

  connect(m_Ui->filePrefix, &QtSLineEdit::textChanged, this, [=] (const QString &filePrefix) {
    generateExampleInputFile();
    emit filePrefixChanged(filePrefix);
  });

  connect(m_Ui->fileSuffix, &QtSLineEdit::textChanged, this, [=] (const QString &fileSuffix) {
    generateExampleInputFile();
    emit fileSuffixChanged(fileSuffix);
  });

  connect(m_Ui->fileExt, &QtSLineEdit::textChanged, this, [=] (const QString &fileExtension) {
    generateExampleInputFile();
    emit fileExtensionChanged(fileExtension);
  });

  connect(m_Ui->slice, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int value) {
    generateExampleInputFile();
    emit sliceNumberChanged(value);
  });

  connect(m_Ui->numOfRows, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int value) {
    generateExampleInputFile();
    emit numberOfRowsChanged(value);
  });

  connect(m_Ui->numOfCols, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int value) {
    generateExampleInputFile();
    emit numberOfColumnsChanged(value);
  });

  connect(m_Ui->slicePadding, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int value) {
    generateExampleInputFile();
    emit slicePaddingChanged(value);
  });

  connect(m_Ui->rowColPadding, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [=] (int value) {
    generateExampleInputFile();
    emit rowColPaddingChanged(value);
  });
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::keyPressEvent(QKeyEvent* event)
{
  if(event->key() == Qt::Key_Escape)
  {
    m_Ui->inputDir->setText(m_CurrentText);
    m_Ui->inputDir->setStyleSheet("");
    m_Ui->inputDir->setToolTip("");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::setupMenuField()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  QFileInfo fi(inputPath);

  QMenu* lineEditMenu = new QMenu(m_Ui->inputDir);
  m_Ui->inputDir->setButtonMenu(QtSLineEdit::Left, lineEditMenu);
  QLatin1String iconPath = QLatin1String(":/SIMPL/icons/images/caret-bottom.png");

  m_Ui->inputDir->setButtonVisible(QtSLineEdit::Left, true);

  QPixmap pixmap(8, 8);
  pixmap.fill(Qt::transparent);
  QPainter painter(&pixmap);
  const QPixmap mag = QPixmap(iconPath);
  painter.drawPixmap(0, (pixmap.height() - mag.height()) / 2, mag);
  m_Ui->inputDir->setButtonPixmap(QtSLineEdit::Left, pixmap);

  {
    m_ShowFileAction = new QAction(lineEditMenu);
    m_ShowFileAction->setObjectName(QString::fromUtf8("showFileAction"));
#if defined(Q_OS_WIN)
    m_ShowFileAction->setText("Show in Windows Explorer");
#elif defined(Q_OS_MAC)
    m_ShowFileAction->setText("Show in Finder");
#else
    m_ShowFileAction->setText("Show in File System");
#endif
    lineEditMenu->addAction(m_ShowFileAction);
    connect(m_ShowFileAction, &QAction::triggered, [=] {
      showFileInFileSystem(m_Ui->inputDir->text());
    });
  }

  if(!inputPath.isEmpty() && fi.exists())
  {
    m_ShowFileAction->setEnabled(true);
  }
  else
  {
    m_ShowFileAction->setDisabled(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::showFileInFileSystem(const QString &filePath)
{
  QFileInfo fi(filePath);
  QString path;
  if (fi.isFile())
  {
    path = fi.absoluteFilePath();
  }
  else
  {
    path = fi.absolutePath();
  }

  QtSFileUtils::ShowPathInGui(this, fi.absoluteFilePath());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::validateInputFile()
{
  QString currentPath = m_Ui->inputDir->text();
  QFileInfo fi(currentPath);
  if(!currentPath.isEmpty() && !fi.exists())
  {
    QString defaultName = m_OpenDialogLastFilePath;

    QString title = QObject::tr("Select a replacement input file");

    QString file = QFileDialog::getExistingDirectory(this, title, getInputDirectory(), QFileDialog::ShowDirsOnly);
    if(file.isEmpty())
    {
      file = currentPath;
    }
    file = QDir::toNativeSeparators(file);
    // Store the last used directory into the private instance variable
    QFileInfo fi(file);

    setInputDirectory(fi.filePath());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::checkIOFiles()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->inputDir))
  {
    findMaxSliceAndPrefix();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::inputDirBtn_clicked()
{
  QString filter = tr("Robomet Configuration File (*.csv);;All Files (*.*)");
  QString title = "Select a Robomet configuration file";

  QString outputFile = QFileDialog::getOpenFileName(this, title,
    getInputDirectory(), filter);

  if(!outputFile.isNull())
  {
    m_Ui->inputDir->blockSignals(true);
    m_Ui->inputDir->setText(QDir::toNativeSeparators(outputFile));
    inputDir_textChanged(m_Ui->inputDir->text());
    setOpenDialogLastFilePath(outputFile);
    m_Ui->inputDir->blockSignals(false);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::inputDir_textChanged(const QString& text)
{
  Q_UNUSED(text)

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(text);

  QFileInfo fi(text);
  if (fi.isRelative())
  {
    m_Ui->absPathLabel->setText(inputPath);
    m_Ui->absPathLabel->show();
  }
  else
  {
    m_Ui->absPathLabel->hide();
  }

  m_Ui->inputDir->setToolTip("Absolute File Path: " + inputPath);

  if(QtSFileUtils::VerifyPathExists(inputPath, m_Ui->inputDir))
  {
    m_ShowFileAction->setEnabled(true);
    findMaxSliceAndPrefix();
    QDir dir(inputPath);
    QString dirname = dir.dirName();
    dir.cdUp();

    generateExampleInputFile();
    m_Ui->inputDir->blockSignals(true);
    m_Ui->inputDir->setText(QDir::toNativeSeparators(m_Ui->inputDir->text()));
    m_Ui->inputDir->blockSignals(false);
  }
  else
  {
    m_ShowFileAction->setEnabled(false);
    m_Ui->fileListView->clear();
  }

  emit inputDirectoryChanged(text);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::generateExampleInputFile()
{
  QString rowString = StringOperations::GeneratePaddedString(m_Ui->numOfRows->value() - 1, m_Ui->rowColPadding->value(), '0');
  QString colString = StringOperations::GeneratePaddedString(m_Ui->numOfCols->value() - 1, m_Ui->rowColPadding->value(), '0');
  QString sliceString = StringOperations::GeneratePaddedString(m_Ui->slice->value(), m_Ui->slicePadding->value(), '0');
  QFileInfo fi(m_Ui->inputDir->text());
  QString parentPath = tr("%1%2").arg(fi.path()).arg(QDir::separator());
  QString filename = QString("%1%2%3_%4_%5%6.%7").arg(parentPath).arg(m_Ui->filePrefix->text()).arg(sliceString).arg(rowString).arg(colString).arg(m_Ui->fileSuffix->text()).arg(m_Ui->fileExt->text());
  m_Ui->generatedFileNameExample->setText(filename);

  int sliceNumber = m_Ui->slice->value();
  int numOfRows = m_Ui->numOfRows->value();
  int numOfCols = m_Ui->numOfCols->value();
  QString prefix = m_Ui->filePrefix->text();
  QString suffix = m_Ui->fileSuffix->text();
  QString ext = m_Ui->fileExt->text();
  int slicePadding = m_Ui->slicePadding->value();
  int rowColPadding = m_Ui->rowColPadding->value();
  bool hasMissingFiles = false;

  fi.setFile(m_Ui->inputDir->text());
  QString slicePath = tr("%1%2%3%4").arg(fi.path()).arg(QDir::separator()).arg(m_Ui->filePrefix->text()).arg(sliceString);

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(slicePath);

  // Now generate all the file names the user is asking for and populate the table
  QVector<QString> fileList = generateFileList(sliceNumber, numOfRows, numOfCols, hasMissingFiles, inputPath, prefix, suffix, ext, slicePadding, rowColPadding);
  m_Ui->fileListView->clear();
  QIcon greenDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_green.png"));
  QIcon redDot = QIcon(QString(":/SIMPL/icons/images/bullet_ball_red.png"));
  for(QVector<QString>::size_type i = 0; i < fileList.size(); ++i)
  {
    QString filePath(fileList.at(i));
    QFileInfo fi(filePath);
    QListWidgetItem* item = new QListWidgetItem(filePath, m_Ui->fileListView);
    if(fi.exists())
    {
      item->setIcon(greenDot);
    }
    else
    {
      hasMissingFiles = true;
      item->setIcon(redDot);
    }
  }

  if(hasMissingFiles)
  {
    m_Ui->errorMessage->setVisible(true);
    m_Ui->errorMessage->setText("Alert: Red Dot File(s) on the list do NOT exist on the filesystem. Please make sure all files exist");
  }
  else
  {
    m_Ui->errorMessage->setVisible(true);
    m_Ui->errorMessage->setText("All files exist.");
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QString> RobometListWidget::generateFileList(int sliceNumber, int numberOfRows, int numberOfColumns, bool& hasMissingFiles, const QString& inputPath, const QString& filePrefix,
                                                     const QString& fileSuffix, const QString& fileExtension, int slicePaddingDigits, int rowColPaddingDigits)
{
  QVector<QString> fileList;
  QDir dir(inputPath);
  if(!dir.exists())
  {
    return fileList;
  }
  int index = 0;

  QString filename;
  bool missingFiles = false;
  for(int row = 0; row < numberOfRows; row++)
  {
    for(int col = 0; col < numberOfColumns; col++)
    {
      filename = QString("%1%2_%3_%4%5.%6").arg(filePrefix).arg(QString::number(sliceNumber), slicePaddingDigits, '0')
          .arg(QString::number(row), rowColPaddingDigits, '0').arg(QString::number(col), rowColPaddingDigits, '0')
          .arg(fileSuffix).arg(fileExtension);
      QString filePath = inputPath + QDir::separator() + filename;
      filePath = QDir::toNativeSeparators(filePath);

      QFileInfo fi(filePath);
      if(!fi.exists())
      {
        missingFiles = true;
      }

      fileList.push_back(filePath);
    }
  }

  hasMissingFiles = missingFiles;

  return fileList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::findMaxSliceAndPrefix()
{
  QString sliceString = StringOperations::GeneratePaddedString(m_Ui->slice->value(), m_Ui->slicePadding->value(), '0');
  QFileInfo fi(m_Ui->inputDir->text());
  QString slicePath = tr("%1%2%3%4").arg(fi.path()).arg(QDir::separator()).arg(m_Ui->filePrefix->text()).arg(sliceString);

  if(slicePath.length() == 0)
  {
    return;
  }

  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(slicePath);

  QDir dir(inputPath);

  // Final check to make sure we have a valid file extension
  if(m_Ui->fileExt->text().isEmpty())
  {
    return;
  }

  QString ext = "." + m_Ui->fileExt->text();
  QStringList filters;
  filters << "*" + ext;
  dir.setNameFilters(filters);
  QFileInfoList fiList = dir.entryInfoList();

  int minSlice = 0;
  int maxSlice = 0;
  int currValue = 0;
  bool flag = false;
  bool ok;
  QString fPrefix;
  QRegExp rx("(\\d+)");
  QStringList list;
  int pos = 0;
  int digitStart = 0;
  int digitEnd = 0;
  int totalOimFilesFound = 0;
  int minTotalDigits = 1000;
  foreach(QFileInfo fi, fiList)
  {
    if((fi.suffix().compare(ext) != 0) && fi.isFile())
    {
      pos = 0;
      list.clear();
      QString fn = fi.baseName();
      int length = fn.length();
      digitEnd = length - 1;
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        --digitEnd;
      }
      pos = digitEnd;

      digitStart = pos = rx.indexIn(fn, pos);
      digitEnd = digitStart;
      while((pos = rx.indexIn(fn, pos)) != -1)
      {
        list << rx.cap(0);
        fPrefix = fn.left(pos);
        pos += rx.matchedLength();
      }
      while(digitEnd >= 0 && fn[digitEnd] >= '0' && fn[digitEnd] <= '9')
      {
        ++digitEnd;
      }

      if(digitEnd - digitStart < minTotalDigits)
      {
        minTotalDigits = digitEnd - digitStart;
      }
      if(!list.empty())
      {
        currValue = list.front().toInt(&ok);
        if(!flag)
        {
          minSlice = currValue;
          flag = true;
        }
        if(currValue > maxSlice)
        {
          maxSlice = currValue;
        }
        if(currValue < minSlice)
        {
          minSlice = currValue;
        }
      }
      ++totalOimFilesFound;
    }
  }
  this->m_Ui->totalSlices->setText(QString::number(totalOimFilesFound));
  this->m_Ui->filePrefix->setText(fPrefix);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RobometListInfo_t RobometListWidget::getRobometListInfo()
{
  SIMPLDataPathValidator* validator = SIMPLDataPathValidator::Instance();
  QString inputPath = validator->convertToAbsolutePath(m_Ui->inputDir->text());

  RobometListInfo_t data;
  data.RowColPaddingDigits = m_Ui->rowColPadding->value();
  data.SlicePaddingDigits = m_Ui->slicePadding->value();
  data.NumberOfColumns = m_Ui->numOfCols->value();
  data.NumberOfRows = m_Ui->numOfRows->value();
  data.SliceNumber = m_Ui->slice->value();
  data.RobometFilePath = inputPath;
  data.ImagePrefix = m_Ui->filePrefix->text();
  data.ImageSuffix = m_Ui->fileSuffix->text();
  data.ImageExtension = m_Ui->fileExt->text();

  return data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool RobometListWidget::isComplete() const
{
  bool result = true;

  if (m_Ui->fileListView->count() <= 0)
  {
    result = false;
  }
  else
  {
    int fileCount = m_Ui->fileListView->count();
    for(int i = 0; i < fileCount; i++)
    {
      QListWidgetItem* item = m_Ui->fileListView->item(i);
      QString filePath = item->text();
      QFileInfo fi(filePath);
      if(!fi.exists())
      {
        result = false;
      }
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RobometListWidget::setInputDirectory(QString val)
{
  m_Ui->inputDir->setText(val);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString RobometListWidget::getInputDirectory()
{
  if(m_Ui->inputDir->text().isEmpty())
  {
    return QDir::homePath();
  }
  return m_Ui->inputDir->text();
}
