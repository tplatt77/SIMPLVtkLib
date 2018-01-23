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

#ifndef _vsinfowidget_h_
#define _vsinfowidget_h_

#include <QtWidgets/QWidget>

#include "SIMPLVtkLib/Visualization/Controllers/VSViewController.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

/**
* @class VSInfoWidget VSInfoWidget.h SIMPLVtkLib/QtWidgets/VSInfoWidget.h
* @brief This class handles displaying information about a visual filter and 
* its view settings in the active VSViewController
*/
class VSInfoWidget : public QWidget
{
  Q_OBJECT

public:
  /**
  * @brief Constructor
  * @param parent
  */
  VSInfoWidget(QWidget* parent = nullptr);

public slots:
  /**
  * @brief Changes the filter being displayed
  * @param filter
  */
  void setFilter(VSAbstractFilter* filter);

  /**
  * @brief Changes the VSViewController used for fetching VSFilterViewSettings
  * @param viewController
  */
  void setViewController(VSViewController* viewController);

protected:
  /**
  * @brief Updates the information on the visual filter
  */
  void updateFilterInfo();

  /**
  * @brief Updates the information on the VSFilterViewSettings
  */
  void updateViewSettingInfo();

private:
  VSAbstractFilter* m_Filter = nullptr;
  VSViewController* m_ViewController = nullptr;
};

#endif
