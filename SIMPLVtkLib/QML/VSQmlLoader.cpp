/* ============================================================================
* Copyright (c) 2009-2018 BlueQuartz Software, LLC
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

#include "VSQmlLoader.h"

#include "SIMPLVtkLib/QML/VSQmlVtkView.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h"
#include "SIMPLVtkLib/Visualization/Controllers/VSFilterViewModel.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSClipFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSCropFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSMaskFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSSliceFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSThresholdFilter.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSTransform.h"

#include "SIMPLVtkLib/Visualization/VtkWidgets/VSBoxWidget.h"
#include "SIMPLVtkLib/Visualization/VtkWidgets/VSPlaneWidget.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
VSQmlLoader::VSQmlLoader()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void VSQmlLoader::registerTypes()
{
  qmlRegisterType<VSQmlVtkView>("VSQml", 1, 0, "VSQmlVtkView");
  qmlRegisterType<VSFilterViewSettings>("VSQml", 1, 0, "VSFilterViewSettings");
  qmlRegisterType<VSFilterViewModel>("VSQml", 1, 0, "VSFilterViewModel");
  qmlRegisterType<VSFilterModel>("VSQml", 1, 0, "VSFilterModel");
  qmlRegisterType<VSTransform>("VSQml", 1, 0, "VSTransform");

  qmlRegisterType<VSClipFilter>("VSQml", 1, 0, "VSClipFilter");
  qmlRegisterType<VSCropFilter>("VSQml", 1, 0, "VSCropFilter");
  qmlRegisterType<VSMaskFilter>("VSQml", 1, 0, "VSMaskFilter");
  qmlRegisterType<VSSliceFilter>("VSQml", 1, 0, "VSSliceFilter");
  qmlRegisterType<VSThresholdFilter>("VSQml", 1, 0, "VSThresholdFilter");

  qmlRegisterType<VSBoxWidget>("VSQml", 1, 0, "VSBoxWidget");
  qmlRegisterType<VSPlaneWidget>("VSQml", 1, 0, "VSPlaneWidget");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl VSQmlLoader::GetVtkViewUrl()
{
  return QUrl("qrc:/QML/VSQml/VSVtk.qml");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl VSQmlLoader::GetPaletteUrl()
{
  return QUrl("qrc:/QML/VSQml/Palette.qml");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl VSQmlLoader::GetFilterUrl(VSAbstractFilter* filter)
{
  if(dynamic_cast<VSClipFilter*>(filter))
  {
    return QUrl("qrc:/QML/VSQml/ClipFilter.qml");
  }
  else if(dynamic_cast<VSCropFilter*>(filter))
  {
    return QUrl("qrc:/QML/VSQml/CropFilter.qml");
  }
  else if(dynamic_cast<VSSliceFilter*>(filter))
  {
    return QUrl("qrc:/QML/VSQml/SliceFilter.qml");
  }
  else if(dynamic_cast<VSThresholdFilter*>(filter))
  {
    return QUrl("qrc:/QML/VSQml/ThresholdFilter.qml");
  }

  return QUrl();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QUrl VSQmlLoader::GetVisibilitySettingsUrl()
{
  return QUrl("qrc:/QML/VSQml/VisibilitySettings.qml");
}
