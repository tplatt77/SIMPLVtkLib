set(${PROJECT_NAME}_ImportMontage_HDRS
  ${WIZARDS_SOURCE_DIR}/ImportMontage/DataDisplayOptionsPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterDREAM3DDataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterFijiDataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterRobometDataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterZeissDataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/FijiListWidget.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/FileTypeSelectionPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterGenericDataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImportMontageConstants.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImportMontageWizard.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/MontageSettings.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImporterWorker.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/RobometListWidget.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileConfigFileGenerator.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileListWidget.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ZeissListWidget.h
)

set(${PROJECT_NAME}_ImportMontage_SRCS
  ${WIZARDS_SOURCE_DIR}/ImportMontage/DataDisplayOptionsPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterDREAM3DDataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterFijiDataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterRobometDataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterZeissDataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/FijiListWidget.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/FileTypeSelectionPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterGenericDataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImportMontageWizard.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/MontageSettings.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImporterWorker.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/RobometListWidget.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileConfigFileGenerator.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileListWidget.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ZeissListWidget.cpp
)

set(${PROJECT_NAME}_ImportMontage_UIS
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/DataDisplayOptionsPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/EnterDREAM3DDataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/EnterFijiDataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/EnterRobometDataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/EnterZeissDataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/FijiListWidget.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/FileTypeSelectionPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/EnterGenericDataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/RobometListWidget.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/TileListWidget.ui
  
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/ZeissListWidget.ui
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Wizards" "${${PROJECT_NAME}_ImportMontage_HDRS}" "${${PROJECT_NAME}_ImportMontage_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files" "${${PROJECT_NAME}_ImportMontage_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_ImportMontage_Generated_UI_HDRS ${${PROJECT_NAME}_ImportMontage_UIS} )
foreach(h ${${PROJECT_NAME}_ImportMontage_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_ImportMontage_Generated_UI_HDRS}" "" "0")

include_directories(${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files)