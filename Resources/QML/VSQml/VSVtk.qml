import QtQuick 2.6
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import VSQml 1.0


VSQmlVtkView {
  id: vtkView

  //anchors.fill: parent
  /*layer.enabled: true*/

  FilterTree
  {
      id: filterTree
      objectName: "filterTree"
      //title: "Filter View"

      x: 5
      y: 5
  }

//  Palette
//  {
//      id: palette

//      Transform {
//          Layout.fillWidth: true
//          Layout.minimumHeight: 10
//          Layout.minimumWidth: 240
//          Layout.margins: palette.contentMargin
//          Layout.bottomMargin: palette.contentMargin + palette.bottomMargin
//      }
//  }
}
