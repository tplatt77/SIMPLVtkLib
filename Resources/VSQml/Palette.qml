import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

//import VSQml 1.0

ColumnLayout
{
  id: palette

  transformOrigin: Item.TopLeft
  clip: true

  property alias paletteTypeDestructible: titleBar.paletteTypeDestructible
  property alias paletteTypeCollapsible: titleBar.paletteTypeCollapsible
  property alias paletteType: titleBar.paletteType

  property alias title: titleBar.title
  property alias pinned: titleBar.pinned
  property alias collapsed: titleBar.collapsed

  property alias headerRadius: titleBar.radius
  property alias headerColor: titleBar.color
  property alias headerBorderColor: titleBar.borderColor
  property alias titleColor: titleBar.titleColor
  property alias headerHeight: titleBar.implicitHeight

  property alias backgroundRadius: backgroundRect.radius
  property alias backgroundBorderWidth: backgroundRect.borderWidth
  property alias backgroundBorderColor: backgroundRect.borderColor
  property alias backgroundColor: backgroundRect.color

  property alias titleMargin: titleBar.margin
  property int contentMargin: 5
  property int bottomMargin: 2

  property int baseZ: 2
  property int focusZ: 3

  // Changing the Z on active focus and the line parent = parent allows reordering the items even when focus is lost
  z: activeFocus ? focusZ : baseZ

  spacing: 4

  // If focus lost and not pinned, destroy the object
  function checkFocus() {
    if(!pinned && !activeFocus)
    {
      // Reinsert destroy() when a solution for combo boxes is figured out
      console.log("Placeholder: this palette should be deleted");
      //destroy();
    }
  }

  onActiveFocusChanged:
  {
    checkFocus()

    // This helps reorder the parent's children list to keep this item on top as if this was a window
    // This and [z: activeFocus ? ...] are the two parts required for the desired behavior
    if(activeFocus && parent)
    {
      // Keep this line as is!
      parent = parent
    }
  }

  Rectangle
  {
      id: backgroundRect
      anchors.fill: parent

      property color borderColor: "#bababa"
      property real borderWidth: 1

      radius: 8
      border.width: borderWidth
      border.color: borderColor
      color: "#e5e5e5"

      MouseArea
      {
          id: allMouseArea

          anchors.fill: parent
          onPressed: forceActiveFocus()
      }
  }

  PaletteTitleBar {
      id: titleBar
      Layout.fillWidth: true
      Layout.maximumHeight: palette.headerHeight
      Layout.minimumWidth: 200
      Layout.preferredHeight: palette.headerHeight
      Layout.preferredWidth: preferredWidth
  }

  onCollapsedChanged:
  {
      if(collapsed)
      {
          height = titleBar.height
      }
      else
      {
          height = implicitHeight
      }
  }
}
