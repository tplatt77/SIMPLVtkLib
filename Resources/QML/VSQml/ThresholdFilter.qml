import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import VSQml 1.0
import "../PaletteItems"
import "../InputTypes"

FilterPalette
{
    id: palette

    property VSThresholdFilter targetFilter: VSThresholdFilter {}
    readonly property alias arrayName: arrayComboBox.currentText
    readonly property real arrayMin: targetFilter.getArrayMinValue(arrayName)
    readonly property real arrayMax: targetFilter.getArrayMaxValue(arrayName)
    readonly property real arrayRangeLength: arrayMax - arrayMin;
    readonly property real minValue: rangeSlider.minValue * arrayRangeLength + arrayMin
    readonly property real maxValue: rangeSlider.maxValue * arrayRangeLength + arrayMin
    property int decimals: 2

    title: targetFilter.filterName

    changesWaiting: !((arrayComboBox.currentText === targetFilter.lastArrayName) && (minValue === targetFilter.lastMinValue) && (maxValue === targetFilter.lastMaxValue)) && (arrayComboBox.currentIndex !== -1)
    onApplyFilter:
    {
        targetFilter.apply(arrayName, minValue, maxValue)
    }
    onResetFilter:
    {
        palette.resetFilterValues();
    }
    onDeleteFilter:
    {
        targetFilter.deleteFilter();
        palette.destroy();
    }

    GridLayout
    {
        id: thresholdLayout
        columns: 2
        rows: 4
        columnSpacing: 4
        rowSpacing: 4
        Layout.fillWidth: true
        Layout.minimumHeight: 10
        Layout.minimumWidth: 240
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        Label
        {
            id: arrayLabel
            text: qsTr("Array: ");
        }
        ComboBox
        {
            id: arrayComboBox
            Layout.fillWidth: true

            model: targetFilter.scalarNames
        }
        Label
        {
            id: rangeLabel
            text: qsTr("Range: ")
        }

        WrappedRangeSlider
        {
            id: rangeSlider
            Layout.fillWidth: true
            Layout.rowSpan: 2

            enabled: arrayComboBox.currentIndex >= 0
            from: 0
            to: 1
        }
        Item {}
        Label
        {
            id: rangeValueLabel
            Layout.fillWidth: true
            Layout.column: 1
            Layout.row: 3

            text: qsTr("( %1 , %2 )".arg(minValue.toFixed(decimals)).arg(maxValue.toFixed(decimals)))
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
    }

    function resetFilterValues()
    {
        // Last Min/Max will not exist if the array was never set
        if(targetFilter.lastArrayName === "")
        {
            return;
        }

        arrayComboBox.currentIndex = arrayComboBox.find(targetFilter.lastArrayName)
        rangeSlider.minValue = (targetFilter.lastMinValue - arrayMin) / arrayRangeLength
        rangeSlider.maxValue = (targetFilter.lastMaxValue - arrayMin) / arrayRangeLength
    }

    onTargetFilterChanged:
    {
        palette.resetFilterValues();
    }

    Component.onCompleted:
    {
        // Default state
        if(targetFilter.lastArrayName === "")
        {
            console.log("Initialize Threshold: " + rangeSlider.from + ", " + rangeSlider.to);
            rangeSlider.minValue = rangeSlider.from
            rangeSlider.maxValue = rangeSlider.to
        }

        //resetFilterValues();
    }

    Keys.onReturnPressed: applyFilter();
    Keys.onDeletePressed: deleteFilter();
}
