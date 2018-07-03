import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

Palette
{
    id: palette

    GridLayout {
        id: visibilityLayout
        x: 0
        y: 19
        width: 240
        height: 208
        rowSpacing: 4
        columnSpacing: 4
        rows: 9
        columns: 3

        Layout.fillWidth: true
        Layout.minimumHeight: 10
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        Label {
            id: visibilitySettingsTitle
            text: qsTr("Visibility Settings")
            Layout.fillWidth: true
            Layout.columnSpan: 3
            Layout.preferredHeight: 13
            Layout.preferredWidth: 218
            font.pointSize: 8
            font.bold: true
        }

        Label {
            id: arrayLabel
            text: qsTr("Array:")
        }

        ComboBox {
            id: arraySelection
            Layout.columnSpan: 2
            Layout.fillWidth: true

            model: ["ColorArray"]
        }

        Label {
            id: componentLabel
            text: qsTr("Component:")
        }

        ComboBox {
            id: componentSelection
            Layout.columnSpan: 2
            Layout.fillWidth: true

            model: ["Magnitude"]
        }

        Label {
            id: colorLabel
            text: qsTr("Color:")
            Layout.preferredHeight: 13
            Layout.preferredWidth: 62

            visible: false
        }

        ColorButton {
            id: colorButton
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.preferredHeight: 23
            Layout.preferredWidth: 125

            visible: false
        }

        Label {
            id: representationLabel
            text: qsTr("Representation:")
        }

        ComboBox {
            id: representationSelection
            Layout.columnSpan: 2
            Layout.fillWidth: true

            model: ["Surface"]
        }

        Label {
            id: colorMappingTitle
            text: qsTr("Color Mapping")
            Layout.fillWidth: true
            Layout.columnSpan: 3
            Layout.preferredHeight: 13
            Layout.preferredWidth: 218
            font.bold: true
        }

        Label {
            id: label6
            text: qsTr("Map Scalars:")
        }

        ComboBox {
            id: mapScalarsSelection
            Layout.fillWidth: true
            Layout.columnSpan: 2

            model: ["Non-Colors"]
        }

        CheckBox {
            id: showScalarsCheckBox
            text: qsTr("Show Scalar Bar")
            Layout.preferredHeight: 17
            Layout.preferredWidth: 210
            Layout.columnSpan: 3

            checkedState: Qt.Checked
        }

        Label {
            id: alphaLabel
            text: qsTr("Alpha:")
        }

        Slider {
            id: alphaSlider
            Layout.fillWidth: true
            Layout.columnSpan: 2
            Layout.preferredHeight: 22
            Layout.preferredWidth: 148
            value: 1
        }
    }

    function showComponents(show)
    {
        componentLabel.visible(show)
        componentSelection.visible(show)
    }

    function showColors(show)
    {
        colorLabel.visible(show)
        colorButton.visible(show)
    }
}
