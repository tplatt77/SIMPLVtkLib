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

#include "VSMontageImporter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMontageImporter::VSMontageImporter(FilterPipeline::Pointer pipeline)
: VSAbstractImporter()
, m_Pipeline(pipeline)
{
  pipeline->addMessageReceiver(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMontageImporter::~VSMontageImporter() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSMontageImporter::Pointer VSMontageImporter::New(FilterPipeline::Pointer pipeline)
{
  VSMontageImporter::Pointer sharedPtr(new VSMontageImporter(pipeline));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMontageImporter::processPipelineMessage(const PipelineMessage& pipelineMsg)
{
  if(pipelineMsg.getType() == PipelineMessage::MessageType::StatusMessage)
  {
    QString str = pipelineMsg.generateStatusString();
    notifyStatusMessage(str);
  }
  else if (pipelineMsg.getType() == PipelineMessage::MessageType::Error)
  {
    notifyErrorMessage(pipelineMsg.generateErrorString(), pipelineMsg.getCode());
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSMontageImporter::execute()
{
  emit started();

  m_Pipeline->execute();

  int err = m_Pipeline->getErrorCondition();

//  qInfo() << "Pipeline err condition: " << err;
//  // For now, quit after an error condition
//  // However, may want to consider returning
//  // list of errors associated with each pipeline
//  if(err < 0)
//  {
//    break;
//  }

  emit resultReady(m_Pipeline, err);
  emit finished();
}
