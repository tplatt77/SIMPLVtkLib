set(${PROJECT_NAME}_ImportMontage_HDRS
  ${WIZARDS_SOURCE_DIR}/ImportMontage/DataDisplayOptionsPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/FileTypeSelectionPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/GenericMetadataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImportMontageConstants.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImportMontageWizard.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/LoadHDF5DataPage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/MontageSettings.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/MontageWorker.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterDataFilePage.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileConfigFileGenerator.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileListWidget.h
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ZeissImportPage.h
)

set(${PROJECT_NAME}_ImportMontage_SRCS
  ${WIZARDS_SOURCE_DIR}/ImportMontage/DataDisplayOptionsPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/FileTypeSelectionPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/GenericMetadataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ImportMontageWizard.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/LoadHDF5DataPage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/MontageSettings.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/MontageWorker.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/EnterDataFilePage.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileConfigFileGenerator.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/TileListWidget.cpp
  ${WIZARDS_SOURCE_DIR}/ImportMontage/ZeissImportPage.cpp
)

set(${PROJECT_NAME}_ImportMontage_UIS
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/DataDisplayOptionsPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/FileTypeSelectionPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/GenericMetadataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/LoadHDF5DataPage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/EnterDataFilePage.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/TileListWidget.ui
  ${WIZARDS_SOURCE_DIR}/ImportMontage/UI_Files/ZeissImportPage.ui
  
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