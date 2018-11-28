
set(${PROJECT_NAME}_Wizards_HDRS

)

set(${PROJECT_NAME}_Wizards_SRCS

)

set(${PROJECT_NAME}_Wizards_UIS

)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/Wizards" "${${PROJECT_NAME}_Wizards_HDRS}" "${${PROJECT_NAME}_Wizards_SRCS}" "0")

# Organize the Source files for things like Visual Studio and Xcode
cmp_IDE_GENERATED_PROPERTIES("${PROJECT_NAME}/Wizards/UI_Files" "${${PROJECT_NAME}_Wizards_UIS}" "")

# --------------------------------------------------------------------
# -- Run UIC on the necessary files
QT5_WRAP_UI( ${PROJECT_NAME}_Wizards_Generated_UI_HDRS ${${PROJECT_NAME}_QtWidgets_UIS} )
foreach(h ${${PROJECT_NAME}_Wizards_Generated_UI_HDRS})
  set_property(SOURCE ${h} PROPERTY SKIP_AUTOMOC ON)
endforeach()

cmp_IDE_SOURCE_PROPERTIES( "Generated/Qt_Uic" "${${PROJECT_NAME}_Wizards_Generated_UI_HDRS}" "" "0")

include_directories(${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/Wizards/UI_Files)