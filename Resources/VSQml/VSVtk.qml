import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2

import VSQml 1.0


Item {
  id: root;
  
  VSQmlVtkView {
    id: vtkView

    anchors.fill: parent
    /*layer.enabled: true*/

    /*
    height: 200
    width: 200
    x: 200
    y: 200
    */
  }
}
