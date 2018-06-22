import QtQuick 2.0

import VSQml 1.0


Rectangle {
  id: root;
	
  color:Qt.rgba(0.3, 0.3, 0.35, 1.0)
  
  VSQmlVtkView {
    id: vtkView

    anchors.fill: parent
    /*layer.enabled: true*/
  }
}
