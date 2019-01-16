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
#include "MontageWorker.h"

#include <QtConcurrent>

#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>

#include <SIMPLib/Utilities/SIMPLH5DataReader.h>

#include "SIMPLib/FilterParameters/IntVec3FilterParameter.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "SIMPLVtkLib/Wizards/ImportMontage/TileConfigFileGenerator.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageWorker::MontageWorker(FilterPipeline::Pointer pipeline,
	AbstractFilter::Pointer itkMontageFilter)
{
	m_itkMontageFilter = itkMontageFilter;
	m_pipeline = pipeline;
	m_configFile = false;
	m_readDream3dFile = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageWorker::MontageWorker(FilterPipeline::Pointer pipeline,
	AbstractFilter::Pointer inputFilter,
	AbstractFilter::Pointer itkMontageFilter,
	bool readDream3dFile)
{

	if (readDream3dFile)
	{
		m_dream3dFileReader = inputFilter;
	}
	else
	{
		m_importConfigFileFilter = inputFilter;
	}
	m_itkMontageFilter = itkMontageFilter;
	m_pipeline = pipeline;
	m_configFile = !readDream3dFile;
	m_readDream3dFile = readDream3dFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MontageWorker::~MontageWorker()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MontageWorker::process()
{
	if (m_itkMontageFilter == nullptr)
	{
		return;
	}
	if (m_configFile)
	{
		if (m_importConfigFileFilter == nullptr)
		{
			return;
		}
		m_pipeline->pushBack(m_importConfigFileFilter);
		m_importConfigFileFilter->preflight();
		DataContainerArray::Pointer dca = m_importConfigFileFilter->getDataContainerArray();
		QStringList dcNames = dca->getDataContainerNames();
		QVariant var;
		bool propWasSet = false;

		// Set the list of image data containers
		var.setValue(dcNames);
		propWasSet = m_itkMontageFilter->setProperty("ImageDataContainers", var);
		if (!propWasSet)
		{
			qDebug() << "Property was not set for ImageDataContainers";
			return;
		}
	}

	if (m_readDream3dFile)
	{
		if (m_dream3dFileReader == nullptr)
		{
			return;
		}

		m_pipeline->pushBack(m_dream3dFileReader);
	}

	m_pipeline->pushBack(m_itkMontageFilter);
	m_pipeline->execute();

	int err = m_pipeline->getErrorCondition();
	qInfo() << "Pipeline err condition: " << err;

	emit resultReady(err);
}