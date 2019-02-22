/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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

#include "LoadHDF5DataPage.h"

#include <QtConcurrent>

#include <QtGui/QMovie>

#include <QtWidgets/QMessageBox>

#include "SIMPLib/Utilities/SIMPLH5DataReader.h"
#include "SIMPLib/Utilities/SIMPLH5DataReaderRequirements.h"

#include "SIMPLVtkLib/Dialogs/Utilities/DREAM3DFileTreeModel.h"
#include "SIMPLVtkLib/Wizards/ImportMontage/DataDisplayOptionsPage.h"

#include "ImportMontage/ImportMontageConstants.h"
#include "ImportMontageWizard.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadHDF5DataPage::LoadHDF5DataPage(QWidget* parent)
: QWizardPage(parent)
, m_Ui(new Ui::LoadHDF5DataPage)
{
  m_Ui->setupUi(this);

  setupGui();

  registerFields();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
LoadHDF5DataPage::~LoadHDF5DataPage()
{
  delete m_LoadingMovie;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::setupGui()
{
  m_Ui->loadHDF5DataWidget->setNavigationButtonsVisibility(false);

  connect(m_Ui->loadHDF5DataWidget, &VSLoadHDF5DataWidget::proxyChanged, this, &LoadHDF5DataPage::proxyChanged);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::initializePage()
{
  QString filePath = field(ImportMontage::DREAM3D::FieldNames::DataFilePath).toString();

  m_Ui->loadHDF5DataWidget->initialize(filePath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::cleanupPage()
{
  // Do not clean up the page.  We want the proxy to stay around until the file changes.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::proxyChanged(DataContainerArrayProxy proxy)
{
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

  setFinalPage(checkCount <= 1);

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool LoadHDF5DataPage::isComplete() const
{
  DataContainerArrayProxy proxy = getProxy();

  bool allChecked = true;
  Qt::CheckState checkState = Qt::Unchecked;
  QMap<QString, DataContainerProxy>& dataContainers = proxy.getDataContainers();
  for(QMap<QString, DataContainerProxy>::iterator iter = dataContainers.begin(); iter != dataContainers.end(); iter++)
  {
    DataContainerProxy dcProxy = iter.value();
    if(dcProxy.getFlag() == Qt::Checked)
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

  if (checkState == Qt::Unchecked)
  {
    return false;
  }

  if (m_LoadingProxy)
  {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::registerFields()
{
  registerField(ImportMontage::DREAM3D::FieldNames::Proxy, m_Ui->loadHDF5DataWidget);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::setProxy(DataContainerArrayProxy proxy)
{
  m_Ui->loadHDF5DataWidget->setProxy(proxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy LoadHDF5DataPage::getProxy() const
{
  return m_Ui->loadHDF5DataWidget->getProxy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LoadHDF5DataPage::nextId() const
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

  if (checkCount > 1)
  {
    return ImportMontageWizard::WizardPages::DataDisplayOptions;
  }

  return -1;
}
