import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

//import VSQml 1.0

Rectangle
{
  id: palette

  transformOrigin: Item.TopLeft

  z: 2

  //opacity: activeFocus ? 1 : 0.5

  property string title: "Untitled"
  property bool pinned: false

  property real headerRadius: 10
  property color headerColor: "#002c75"
  property color titleColor: "#cee0ff"
  property real headerHeight: 30

  property real backgroundRadius: 20
  property real backgroundBorderWidth: 2
  property color backgroundBorderColor: "#515151"
  //property color backgroundColor: "#bfbfbf"
  property color backgroundColor: "#101010"

  property int titleMargin: 10
  property int contentMargin: 5

  // If focus lost and not pinned, destroy the object
  onActiveFocusChanged:
  {
    if(!pinned && !activeFocus)
    {
      destroy();
    }
  }

  radius: palette.backgroundRadius
  border.width: palette.backgroundBorderWidth
  border.color: palette.backgroundBorderColor
  color: palette.backgroundColor

  ColumnLayout
  {
    id: paletteLayout
    transformOrigin: Item.TopLeft

    //anchors.fill: parent
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right

    spacing: 4

    Rectangle
    {
      id: titleBar

      Layout.fillWidth: true
      Layout.minimumWidth: 200
      Layout.preferredHeight: palette.headerHeight
      Layout.maximumHeight: palette.headerHeight

      radius: palette.headerRadius
      color: palette.headerColor

      MouseArea
      {
        id: titleArea

        anchors.fill: parent

        drag.target: palette
        drag.axis: Drag.XAndYAxis
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
          Layout.rightMargin: palette.titleMargin

          iconSource: "qrc:///SIMPL/icons/images/bookmark.png"

          checkable: true
          checked: palette.pinned
          onClicked: palette.pinned = checked
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


    Text
    {
      id: contentsContainer

      Layout.fillWidth: true
      Layout.minimumHeight: 10
      Layout.margins: palette.contentMargin

      color: "#ff0000"
      text: "Hello, World"
    }

    height: paletteLayout.height
  }
}
