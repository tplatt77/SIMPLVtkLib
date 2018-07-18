import QtQuick 2.0
import QtQuick.Controls 1.2
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
}
