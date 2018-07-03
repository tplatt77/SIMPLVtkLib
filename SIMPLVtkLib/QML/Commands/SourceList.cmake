
set(${PROJECT_NAME}_QML_Command_HDRS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QML/Commands/VSAbstractCommand.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QML/Commands/VSSelectCommand.h
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QML/Commands/VSContextMenuCommand.h
)

set(${PROJECT_NAME}_QML_Command_SRCS
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QML/Commands/VSAbstractCommand.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QML/Commands/VSSelectCommand.cpp
  ${${PROJECT_NAME}_SOURCE_DIR}/SIMPLVtkLib/QML/Commands/VSContextMenuCommand.cpp
)


cmp_IDE_SOURCE_PROPERTIES( "${PROJECT_NAME}/QML/Commands" "${${PROJECT_NAME}_QML_Command_HDRS}" "${${PROJECT_NAME}_QML_Command_SRCS}" "0")
