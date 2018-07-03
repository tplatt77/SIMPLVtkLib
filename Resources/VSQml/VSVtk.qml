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
  }

  VSPalette
  {
    id: test

    x: 50
    y: 50
  }
}
