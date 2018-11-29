
set(${PROJECT_NAME}_GenericMontage_HDRS
  ${WIZARDS_SOURCE_DIR}/GenericMontage/GenericMontageWizard.h
  ${WIZARDS_SOURCE_DIR}/GenericMontage/GenericMontageSettings.h
  ${WIZARDS_SOURCE_DIR}/GenericMontage/GenericCollectionTypePage.h
)

set(${PROJECT_NAME}_GenericMontage_SRCS
  ${WIZARDS_SOURCE_DIR}/GenericMontage/GenericMontageWizard.cpp
  ${WIZARDS_SOURCE_DIR}/GenericMontage/GenericMontageSettings.cpp
  ${WIZARDS_SOURCE_DIR}/GenericMontage/GenericCollectionTypePage.cpp
)

set(${PROJECT_NAME}_GenericMontage_UIS
  ${WIZARDS_SOURCE_DIR}/GenericMontage/UI_Files/GenericCollectionTypePage.ui
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Wizards" "${${PROJECT_NAME}_GenericMontage_HDRS}" "${${PROJECT_NAME}_GenericMontage_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${WIZARDS_SOURCE_DIR}/GenericMontage/UI_Files" "${${PROJECT_NAME}_GenericMontage_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_GenericMontage_Generated_UI_HDRS ${${PROJECT_NAME}_GenericMontage_UIS} )
foreach(h ${${PROJECT_NAME}_GenericMontage_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_GenericMontage_Generated_UI_HDRS}" "" "0")

include_directories(${WIZARDS_SOURCE_DIR}/GenericMontage/UI_Files)