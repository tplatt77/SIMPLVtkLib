import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

import VSQml 1.0

Palette
{
    id: palette

    property VSFilterViewSettings viewSettings: VSFilterViewSettings {}
    property alias viewAdvancedSettings : advancedSettings.checked

    GridLayout {
        id: visibilityLayout

        rowSpacing: 4
        columnSpacing: 4
        rows: 9
        columns: 3

        Layout.fillWidth: true
        Layout.minimumHeight: 10
        Layout.minimumWidth: 240
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        Label {
            id: visibilitySettingsTitle
            text: qsTr("Visibility Settings")
            Layout.fillWidth: true
            Layout.columnSpan: 2
            Layout.preferredHeight: 15
            Layout.preferredWidth: 218
            font.pointSize: 8
            font.bold: true
        }

        Button {
            id: advancedSettings
            tooltip: "View Advanced Settings"
            Layout.columnSpan: 1
            Layout.preferredHeight: 17
            Layout.preferredWidth: 17

            checkable: true
            checked: false
            iconSource: "qrc:/SIMPL/icons/images/cog.png"
        }

        CheckBox {
            id: visibilityCheckBox
            Layout.columnSpan: 3
            Layout.fillWidth: true

            text: "Filter Visibility"
            checked: Qt.Unchecked
            visible: palette.viewAdvancedSettings

            Connections{
                target: palette.viewSettings
                onVisibilityChanged:{
                    visibilityCheckBox.checkedState = palette.viewSettings.visibility ? Qt.Checked : Qt.Unchecked
                }
            }
            onCheckedChanged:
            {
                palette.viewSettings.visibility = (checkedState == Qt.Checked);
                parent.forceActiveFocus()
            }
        }

        Label {
            id: representationLabel
            text: qsTr("Representation:")
        }

        ComboBox {
            id: representationSelection
            Layout.columnSpan: 2
            Layout.fillWidth: true

            model: ["Outline", "Points", "Wireframe", "Surface", "Surface with Edges"]
            currentIndex: 0 // Default value

            Connections{
                target: palette.viewSettings
                onRepresentationChanged:{
                    representationSelection.currentIndex = palette.viewSettings.representation;
                }
            }
            onCurrentIndexChanged:
            {
                palette.viewSettings.representation = currentIndex;
                parent.forceActiveFocus()
            }

        }

        Label {
            id: arrayLabel
            text: qsTr("Array:")
        }

        ComboBox {
            id: arraySelection
            activeFocusOnTab: true
            Layout.columnSpan: 2
            Layout.fillWidth: true

            model: palette.viewSettings.arrayNames
            //focus: true

            function updateArrayName()
            {
                var index = arraySelection.find(palette.viewSettings.activeArrayName);
                if(index < 0)
                {
                    index = 0;
                }

                arraySelection.currentIndex = index;
                palette.showColors(index == 0);
                palette.showComponents(palette.viewSettings.componentNames.count > 1);
            }

            Connections{
                target: palette.viewSettings
                onActiveArrayNameChanged:{
                    var index = arraySelection.find(palette.viewSettings.activeArrayName);
                    if(index < 0)
                    {
                        index = 0;
                    }

                    arraySelection.currentIndex = index;
                    palette.showColors(index == 0);
                    palette.showComponents(palette.viewSettings.componentNames.count > 1);
                }
            }
            onCurrentIndexChanged: {
                if(currentIndex == 0)
                {
                    palette.viewSettings.activeArrayName = ""
                }
                else
                {
                    palette.viewSettings.activeArrayName = currentText
                }

                parent.forceActiveFocus()
            }
        }

        Label {
            id: componentLabel
            text: qsTr("Component:")
        }

        ComboBox {
            id: componentSelection
            Layout.columnSpan: 2
            Layout.fillWidth: true

            model: palette.viewSettings.componentNames
            //currentIndex: 1 // Default value

            onModelChanged: {
                showComponents(palette.viewSettings.componentNames.length > 1);
            }

            Connections{
                target: palette.viewSettings
                onActiveComponentIndexChanged:{
                    componentSelection.currentIndex = palette.viewSettings.activeComponentIndex + 1;
                }
            }
            onCurrentIndexChanged: {
                palette.viewSettings.activeComponentIndex = currentIndex - 1;
                parent.forceActiveFocus()
            }
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

            Connections{
                target: palette.viewSettings
                onSolidColorChanged:{
                    colorButton.colorProp = palette.viewSettings.solidColor;
                }
            }
            onColorPropChanged: {
                palette.viewSettings.solidColor = colorButton.colorProp;
                parent.forceActiveFocus()
            }
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
            activeFocusOnTab: true
            Layout.fillWidth: true
            Layout.columnSpan: 2

            model: ["Always", "Non-Colors", "Never"]
            currentIndex: 1 // Default value

            Connections{
                target: palette.viewSettings
                onMapColorsChanged:{
                    mapScalarsSelection.currentIndex = palette.viewSettings.mapColors
                }
            }
            onCurrentIndexChanged:
            {
                palette.viewSettings.mapColors = currentIndex
                parent.forceActiveFocus()
            }
        }

        CheckBox {
            id: showScalarsCheckBox
            text: qsTr("Show Scalar Bar")
            Layout.preferredHeight: 17
            Layout.preferredWidth: 210
            Layout.columnSpan: 3

            checkedState: Qt.Checked // Default value
            visible: palette.viewAdvancedSettings

            Connections{
                target: palette.viewSettings
                onShowScalarBarChanged:{
                    showScalarsCheckBox.checkedState = palette.viewSettings.showScalarBar ? Qt.Checked : Qt.Unchecked
                }
            }
            onCheckedChanged:
            {
                palette.viewSettings.showScalarBar = (checkedState == Qt.Checked)
            }
            onClicked: forceActiveFocus()
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
            value: 1 // Default value
            activeFocusOnPress: true

            Connections{
                target: palette.viewSettings
                onAlphaChanged:{
                    alphaSlider.value = palette.viewSettings.alpha;
                }
            }

            onValueChanged: {
                palette.viewSettings.alpha = value
            }
        }
    }

    function showComponents(show)
    {
        componentLabel.visible = show;
        componentSelection.visible = show;
    }

    function showColors(show)
    {
        colorLabel.visible = show;
        colorButton.visible = show;
    }

    onViewSettingsChanged:
    {
        if(viewSettings == null)
        {
            return
        }

        title = palette.viewSettings.filterName + ": Visibility"

        visibilityCheckBox.checkedState = palette.viewSettings.visibility ? Qt.Checked : Qt.Unchecked
        representationSelection.currentIndex = palette.viewSettings.representation
        arraySelection.model = viewSettings.arrayNames
        arraySelection.updateArrayName()
        colorButton.colorProp = viewSettings.solidColor
        componentSelection.currentIndex = palette.viewSettings.activeComponentIndex + 1
        mapScalarsSelection.currentIndex = viewSettings.mapColors
        showScalarsCheckBox.checkedState = palette.viewSettings.showScalarBar ? Qt.Checked : Qt.Unchecked
        alphaSlider.value = palette.viewSettings.alpha
    }
}
