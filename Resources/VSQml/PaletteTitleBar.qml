import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

//import VSQml 1.0

Rectangle
{
  id: titleBar
  
  radius: palette.headerRadius
  color: palette.headerColor
  border.color: palette.headerBorderColor
  border.width: 1
  
  MouseArea
  {
    id: titleArea
    
    anchors.fill: parent
    
    drag.target: palette
    drag.axis: Drag.XAndYAxis
    onPressed: forceActiveFocus()
  }
  
  RowLayout
  {
    id: titleBarLayout
    transformOrigin: Item.Top
    anchors.top: parent.top
    anchors.topMargin: 0
    anchors.right: parent.right
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    
    spacing: 4
    
    layoutDirection: Qt.RightToLeft
    
    ToolButton
    {
      id: closeBtn
      
      Layout.fillWidth: false
      Layout.minimumWidth: titleBar.height
      Layout.maximumWidth: titleBar.height
      Layout.preferredHeight: titleBar.height
      Layout.rightMargin: palette.titleMargin
      
      visible: paletteType == paletteTypeDestructible
      
      iconSource: "qrc:///SIMPL/icons/images/circle-x.png"
      
      onClicked: palette.remove()
    }
    
    ToolButton
    {
      id: pinBtn
      
      Layout.fillWidth: false
      Layout.minimumWidth: titleBar.height
      Layout.maximumWidth: titleBar.height
      Layout.preferredHeight: titleBar.height
      Layout.leftMargin: palette.titleMargin
      
      visible: paletteType == paletteTypeDestructible
      
      iconSource: "qrc:///SIMPL/icons/images/bookmark.png"
      
      checkable: true
      checked: palette.pinned
      onClicked: palette.pinned = checked
    }
    
    ToolButton
    {
      id: collapseBtn
      
      Layout.fillWidth: false
      Layout.minimumWidth: titleBar.height
      Layout.maximumWidth: titleBar.height
      Layout.preferredHeight: titleBar.height
      Layout.leftMargin: palette.titleMargin
      
      visible: paletteType == paletteTypeCollapsible
      
      iconSource: "qrc:///SIMPL/icons/images/navigate_close.png"
      
      checkable: true
      checked: !palette.collapsed
      onClicked: palette.collapsed = !checked
    }
    
    Text
    {
      id: paletteTitleText
      
      Layout.fillWidth: true
      Layout.leftMargin: palette.titleMargin
      
      font.bold: true
      font.pointSize: 12
      
      text: palette.title
      color: palette.titleColor
    }
  }
}
