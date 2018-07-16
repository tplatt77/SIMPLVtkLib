import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

//import VSQml 1.0

ColumnLayout
{
  id: palette

  transformOrigin: Item.TopLeft

  //opacity: 1

  property string title: "Untitled"
  property bool pinned: false
  //property real contentOpacity: activeFocus ? 1 : 0.5

  property real headerRadius: 2
  property color headerColor: "#4c8aff"
  property color headerBorderColor: "#2869e2"
  property color titleColor: "#e2edff"
  property real headerHeight: 30

  property real backgroundRadius: 20
  property real backgroundBorderWidth: 1
  property color backgroundBorderColor: "#bababa"
  property color backgroundColor: "#e5e5e5"

  property int titleMargin: 3
  property int contentMargin: 5
  property int bottomMargin: 10

  property int baseZ: 2
  property int focusZ: 3

  // Changing the Z on active focus and the line parent = parent allows reordering the items even when focus is lost
  z: activeFocus ? focusZ : baseZ

  spacing: 4

  // removeObject signal is important for cases when ownership is set to C++ and not JavaScript
  // A C++ owned item cannot be deleted from the QML side using destroy()
  signal removeObject()
  function remove() {
    removeObject()
    destroy();
  }

  // If focus lost and not pinned, destroy the object
  function checkFocus() {
    if(!pinned && !activeFocus)
    {
        console.log("Focus Lost on: " + palette.title);
      //remove();
    }
  }

  onActiveFocusChanged:
  {
    console.log("Active Focus Changed")
    checkFocus()

      if(activeFocus && parent)
      {
          parent = parent
      }
  }

//  onContentOpacityChanged:
//  {
//    for(var i = 0; i < children.length; i++)
//    {
//      if(children[i] != titleBar)
//      {
//        children[i].opacity = contentOpacity;
//      }
//    }
//  }

  Rectangle
  {
      id: backgroundRect

      anchors.fill: parent

      //opacity: parent.activeFocus ? 1 : 0.5
      radius: palette.backgroundRadius
      border.width: palette.backgroundBorderWidth
      border.color: palette.backgroundBorderColor
      color: palette.backgroundColor

      MouseArea
      {
          id: allMouseArea

          anchors.fill: parent
          onPressed: forceActiveFocus()
      }
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

    // Adjust minimumWidth with title length and button sizes
    onTitleChanged:
    {
        var minimumWidth = pinBtn.width + closeBtn.width + paletteTitleText.width + (palette.spacing * 2 + palette.titleMargin * 3);
        if(palette.width < minimumWidth)
        {
            palette.width = minimumWidth
        }
    }
}
