
set(${PROJECT_NAME}_QtWidgets_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSViewWidget.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisualizationWidget.h
)

set(${PROJECT_NAME}_QtWidgets_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSViewWidget.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QtWidgets/VSVisualizationWidget.cpp
)

cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/QtWidgets" "${${PROJECT_NAME}_QtWidgets_HDRS}" "${${PROJECT_NAME}_QtWidgets_SRCS}" "0")