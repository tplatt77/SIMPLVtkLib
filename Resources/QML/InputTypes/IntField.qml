import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

GridLayout
{
    id: intField

    property alias implicitWidth: textField.implicitWidth
    property alias implicitHeight: textField.implicitHeight

    property real max: 9999999999
    property real min: -9999999999

    property int value: 0

    onValueChanged: textField.text = value
    Connections
    {
        target: textField
        onEditingFinished:
        {
            intField.value = parseInt(textField.text)
        }
    }

    TextField
    {
        id: textField
        Layout.fillWidth: true
        Layout.fillHeight: true

        validator: IntValidator
        {
            top: intField.max
            bottom: intField.min
        }

        verticalAlignment: Qt.AlignVCenter
        text: qsTr("0")
    }
}
