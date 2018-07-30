import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

import "../InputTypes"

GridLayout {
    id: vtkPlaneWidget

    property alias normalX: normalXInput.value
    property alias normalY: normalYInput.value
    property alias normalZ: normalZInput.value

    property alias originX: originXInput.value
    property alias originY: originYInput.value
    property alias originZ: originZInput.value

    readonly property real inputWidth: 75
    readonly property int decimals: 2

    columns: 4
    columnSpacing: 4
    rowSpacing: 4

    normalX: 1
    normalY: 0
    normalZ: 0

    originX: 0
    originY: 0
    originZ: 0

    function getNormal()
    {
        var normal = [normalX, normalY, normalZ];
        return normal;
    }

    function getOrigin()
    {
        var origin = [originX, originY, originZ];
        return origin;
    }

    function setNormal(normal)
    {
        normalX = normal[0];
        normalY = normal[1];
        normalZ = normal[2];
    }

    function setOrigin(origin)
    {
        originX = origin[0];
        originY = origin[1];
        originZ = origin[2];
    }

    Label
    {
        id: normalLabel
        text: qsTr("Normal: ")
    }

    FloatField
    {
        id: normalXInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: normalYInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: normalZInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    Label
    {
        id: originLabel
        text: qsTr("Origin: ")
    }

    FloatField
    {
        id: originXInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: originYInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }

    FloatField
    {
        id: originZInput
        Layout.fillWidth: true
        implicitWidth: inputWidth
        Layout.preferredWidth: inputWidth

        decimals: decimals
    }
}
