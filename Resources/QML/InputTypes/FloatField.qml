import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

GridLayout
{
    id: floatField

    property real max: 9999999
    property real min: -9999999
    property real decimals: 2

    property real value: 0

    onValueChanged: textField.text = value.toFixed(decimals)
    Connections
    {
        target: textField
        onEditingFinished:
        {
            if(floatField.value.toFixed(decimals) !== parseFloat(textField.text))
            {
                floatField.value = parseFloat(textField.text)
            }
        }
    }

    TextField
    {
        id: textField
        Layout.fillWidth: true
        Layout.fillHeight: true

        validator: DoubleValidator
        {
            top: floatField.max
            bottom: floatField.min
            decimals: floatField.decimals
        }

        verticalAlignment: Qt.AlignVCenter
        text: floatField.value.toFixed(decimals)
    }
}
