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

#include "ImportDataWizard.h"

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
  DREAM3DFileTreeModel* model = new DREAM3DFileTreeModel();
  connect(model, &DREAM3DFileTreeModel::dataChanged, this, &LoadHDF5DataPage::modelDataChanged);

  connect(m_Ui->selectAllCB, &QCheckBox::stateChanged, this, &LoadHDF5DataPage::selectAllStateChanged);

  m_Ui->treeView->setModel(model);

  m_LoadingMovie = new QMovie(":/gifs/loading.gif");
  m_Ui->loadingLabel->setMovie(m_LoadingMovie);

  connect(&m_ProxyInitWatcher, SIGNAL(finished()), this, SLOT(proxyInitFinished()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::initializePage()
{
  m_Ui->errLabel->hide();
  m_Ui->loadingLabel->hide();

  m_Ui->loadingLabel->show();
  m_LoadingMovie->start();

  QString filePath = field("DataFilePath").toString();

  QFuture<DataContainerArrayProxy> future = QtConcurrent::run(this, &LoadHDF5DataPage::readDCAProxy, filePath);
  m_ProxyInitWatcher.setFuture(future);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::proxyInitFinished()
{
  QFuture<DataContainerArrayProxy> future = m_ProxyInitWatcher.future();
  DataContainerArrayProxy proxy = future.result();

  setProxy(proxy);

  m_Ui->loadingLabel->hide();
  m_LoadingMovie->stop();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy LoadHDF5DataPage::readDCAProxy(const QString& filePath)
{
  QFileInfo fi(filePath);

  SIMPLH5DataReader reader;
  connect(&reader, &SIMPLH5DataReader::errorGenerated, [=] (const QString &title, const QString &msg, const int &code) {
    m_Ui->errLabel->setText(tr("%1").arg(msg));
    m_Ui->errLabel->show();
  });

  bool success = reader.openFile(filePath);
  if(!success)
  {
    return DataContainerArrayProxy();
  }

  int err = 0;
  SIMPLH5DataReaderRequirements req(SIMPL::Defaults::AnyPrimitive, SIMPL::Defaults::AnyComponentSize, AttributeMatrix::Type::Any, IGeometry::Type::Any);
  DataContainerArrayProxy proxy = reader.readDataContainerArrayStructure(&req, err);
  if(proxy.dataContainers.isEmpty())
  {
    return DataContainerArrayProxy();
  }

  QStringList dcNames = proxy.dataContainers.keys();
  for(int i = 0; i < dcNames.size(); i++)
  {
    QString dcName = dcNames[i];
    DataContainerProxy dcProxy = proxy.dataContainers[dcName];

    // We want only data containers with geometries displayed
    if(dcProxy.dcType == static_cast<unsigned int>(DataContainer::Type::Unknown))
    {
      proxy.dataContainers.remove(dcName);
    }
    else
    {
      QStringList amNames = dcProxy.attributeMatricies.keys();
      for(int j = 0; j < amNames.size(); j++)
      {
        QString amName = amNames[j];
        AttributeMatrixProxy amProxy = dcProxy.attributeMatricies[amName];

        // We want only cell attribute matrices displayed
        if(amProxy.amType != AttributeMatrix::Type::Cell)
        {
          dcProxy.attributeMatricies.remove(amName);
          proxy.dataContainers[dcName] = dcProxy;
        }
      }
    }
  }

  if(proxy.dataContainers.size() <= 0)
  {
    m_Ui->errLabel->setText("Failed to load DREAM3D file '%1' because the file does not contain any data containers with a supported geometry.");
    m_Ui->errLabel->show();
    return DataContainerArrayProxy();
  }

  return proxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::selectAllStateChanged(int state)
{
  DREAM3DFileTreeModel* model = dynamic_cast<DREAM3DFileTreeModel*>(m_Ui->treeView->model());
  if (model == nullptr)
  {
    return;
  }

  Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
  if(checkState == Qt::PartiallyChecked)
  {
    m_Ui->selectAllCB->setCheckState(Qt::Checked);
    return;
  }

  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex dcIndex = model->index(i, DREAM3DFileItem::Name);
    model->setData(dcIndex, checkState, Qt::CheckStateRole);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>())
{
  DREAM3DFileTreeModel* model = dynamic_cast<DREAM3DFileTreeModel*>(m_Ui->treeView->model());
  if (model == nullptr)
  {
    return;
  }

  bool allChecked = true;
  Qt::CheckState checkState = Qt::Unchecked;
  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex dcIndex = model->index(i, DREAM3DFileItem::Name);
    if(model->getCheckState(dcIndex) == Qt::Checked)
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

  m_Ui->selectAllCB->blockSignals(true);
  m_Ui->selectAllCB->setCheckState(checkState);
  m_Ui->selectAllCB->blockSignals(false);

  m_Proxy = model->getModelProxy();
  emit proxyChanged(m_Proxy);

  emit completeChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool LoadHDF5DataPage::isComplete() const
{
  DREAM3DFileTreeModel* model = dynamic_cast<DREAM3DFileTreeModel*>(m_Ui->treeView->model());
  if (model == nullptr)
  {
    return false;
  }

  bool allChecked = true;
  Qt::CheckState checkState = Qt::Unchecked;
  for(int i = 0; i < model->rowCount(); i++)
  {
    QModelIndex dcIndex = model->index(i, DREAM3DFileItem::Name);
    if(model->getCheckState(dcIndex) == Qt::Checked)
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
  else
  {
    return true;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::registerFields()
{
  registerField("DREAM3DProxy", this, "Proxy", "proxyChanged");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void LoadHDF5DataPage::setProxy(DataContainerArrayProxy proxy)
{
  DREAM3DFileTreeModel* model = static_cast<DREAM3DFileTreeModel*>(m_Ui->treeView->model());
  if(model != nullptr)
  {
    model->populateTreeWithProxy(proxy);
    m_Ui->selectAllCB->setChecked(true);

    QModelIndexList indexes = model->match(model->index(0, 0), Qt::DisplayRole, "*", -1, Qt::MatchWildcard | Qt::MatchRecursive);
    for(int i = 0; i < indexes.size(); i++)
    {
      QModelIndex index = indexes[i];
      m_Ui->treeView->expand(index);
    }
  }

  m_Proxy = proxy;
  emit proxyChanged(proxy);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArrayProxy LoadHDF5DataPage::getProxy()
{
  return m_Proxy;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int LoadHDF5DataPage::nextId() const
{
  if (m_Proxy.dataContainers.size() > 1)
  {
    return ImportDataWizard::WizardPages::DataDisplayOptions;
  }
  else
  {
    return -1;
  }
}
