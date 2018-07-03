import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.0
import QtQuick.Layouts 1.2

Button
{
    id: button

    property color colorProp: "#00ff00"
    property string dialogTitle: "Select a Color"

    style: ButtonStyle {
        background: Rectangle {
            color: button.colorProp
            border.width: 1
            border.color: Qt.darker(button.colorProp, 2)
        }
    }

    signal updatedColor(color newColor)
    function setColor(color)
    {
        colorProp = color
        updatedColor(color)
    }

    ColorDialog {
        id: colorDialog
        title: button.dialogTitle

        onAccepted: {
            button.setColor(colorDialog.color)
        }
    }

    onClicked: colorDialog.open()
}
