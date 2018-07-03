import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

//import VSQml 1.0

ColumnLayout
{
  id: palette

  transformOrigin: Item.TopLeft

  //z: 2

  //opacity: activeFocus ? 1 : 0.5
  opacity: 1

  //layer.enabled: true

  property string title: "Untitled"
  property bool pinned: false

  property real headerRadius: 2
  property color headerColor: "#4c8aff"
  property color titleColor: "#e2edff"
  property real headerHeight: 30

  property real backgroundRadius: 20
  property real backgroundBorderWidth: 1
  property color backgroundBorderColor: "#bababa"
  property color backgroundColor: "#e5e5e5"

  property int titleMargin: 3
  property int contentMargin: 5
  property int bottomMargin: 10

  spacing: 4

  signal removeObject()
  function remove() {
    console.log("Delete Palette")
    removeObject()
    destroy();
  }

  function checkFocus() {
    if(!pinned && !activeFocus)
    {
      console.log("Focus Lost")
      remove();
    }
  }

  // If focus lost and not pinned, destroy the object
  /*onActiveFocusChanged:
  {
    if(!pinned && !activeFocus)
    {
      //remove();
    }
  }*/

  Rectangle
  {
      radius: palette.backgroundRadius
      border.width: palette.backgroundBorderWidth
      border.color: palette.backgroundBorderColor
      color: palette.backgroundColor

      anchors.fill: parent
  }

  FocusScope
  {
      id: focusScope

      anchors.fill: parent
  }

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

          onClicked: palette.remove()
        }

        ToolButton
        {
          id: pinBtn

          Layout.fillWidth: false
          Layout.minimumWidth: titleBar.height
          Layout.maximumWidth: titleBar.height
          Layout.preferredHeight: titleBar.height
          //Layout.rightMargin: palette.titleMargin

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
}
