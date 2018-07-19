import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

//import VSQml 1.0

Rectangle
{
  id: titleBar
  
  // PalettTypes
  readonly property int paletteTypeDestructible: 0
  readonly property int paletteTypeCollapsible: 1

  property int paletteType: paletteTypeDestructible

  readonly property bool isDestructible: paletteType == paletteTypeDestructible
  readonly property bool isCollapsible: paletteType == paletteTypeCollapsible

  property alias pinned: pinBtn.checked
  property bool collapsed: false
  property alias title: paletteTitleText.text
  property alias titleColor: paletteTitleText.color

  property color borderColor: "#2869e2"
  property real borderWidth: 1

  implicitHeight: 30
  property int margin: 3

  readonly property real preferredWidth: paletteTitleText.implicitWidth + (titleBar.margin + titleBarLayout.spacing * 2) + (pinBtn.width + closeBtn.width + titleBar.margin * 2);

  radius: 2
  color: "#4c8aff"
  border.color: borderColor
  border.width: borderWidth

  MouseArea
  {
    id: titleArea
    
    anchors.fill: parent
    
    drag.target: titleBar.parent
    drag.axis: Drag.XAndYAxis
    onPressed: forceActiveFocus()
    onPositionChanged: pinBtn.checked = true;
  }
  
  RowLayout
  {
    id: titleBarLayout
    transformOrigin: Item.Top
    anchors.fill: parent
    anchors.topMargin: 0
    
    spacing: 4
    
    layoutDirection: Qt.RightToLeft
    
    ToolButton
    {
      id: closeBtn
      
      Layout.fillWidth: false
      Layout.minimumWidth: titleBar.height
      Layout.maximumWidth: titleBar.height
      Layout.preferredHeight: titleBar.height
      Layout.rightMargin: titleBar.margin
      
      visible: isDestructible
      
      iconSource: "qrc:///SIMPL/icons/images/circle-x.png"
      
      onClicked: palette.destroy()
    }
    
    ToolButton
    {
      id: pinBtn
      
      Layout.fillWidth: false
      Layout.minimumWidth: titleBar.height
      Layout.maximumWidth: titleBar.height
      Layout.preferredHeight: titleBar.height
      Layout.leftMargin: titleBar.margin
      
      visible: isDestructible
      
      iconSource: "qrc:///SIMPL/icons/images/bookmark.png"
      
      checkable: true
      checked: false
      onClicked: titleBar.pinned = checked
    }
    
    ToolButton
    {
      id: collapseBtn
      
      Layout.fillWidth: false
      Layout.minimumWidth: titleBar.height
      Layout.maximumWidth: titleBar.height
      Layout.preferredHeight: titleBar.height
      Layout.leftMargin: titleBar.margin
      
      visible: isCollapsible
      
      iconSource: "qrc:///SIMPL/icons/images/navigate_close.png"
      
      checkable: true
      checked: !titleBar.collapsed
      onClicked: titleBar.collapsed = !checked
    }
    
    Label
    {
      id: paletteTitleText
      
      Layout.fillWidth: true
      Layout.leftMargin: titleBar.margin
      
      font.bold: true
      font.pointSize: 12
      
      text: "Untitled"
      color: "#e2edff"
    }
  }
}
