import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

import VSQml 1.0
import "../PaletteItems"
import "../InputTypes"

Palette
{
    id: palette
    property VSCropFilter targetFilter: VSCropFilter {}
    title: targetFilter.filterName

    property alias voiBoundXMin: voiXMin.bottom
    property alias voiBoundXMax: voiXMax.top
    property alias voiBoundYMin: voiYMin.bottom
    property alias voiBoundYMax: voiYMax.top
    property alias voiBoundZMin: voiZMin.bottom
    property alias voiBoundZMax: voiZMax.top

    property alias sampleRateBoundIMin: sampleRateIInput.bottom
    property alias sampleRateBoundIMax: sampleRateIInput.top
    property alias sampleRateBoundJMin: sampleRateJInput.bottom
    property alias sampleRateBoundJMax: sampleRateJInput.top
    property alias sampleRateBoundKMin: sampleRateKInput.bottom
    property alias sampleRateBoundKMax: sampleRateKInput.top

    property alias voiValueXMin: voiXMin.value
    property alias voiValueXMax: voiXMax.value
    property alias voiValueYMin: voiYMin.value
    property alias voiValueYMax: voiYMax.value
    property alias voiValueZMin: voiZMin.value
    property alias voiValueZMax: voiZMax.value

    property alias sampleRateValueI: sampleRateIInput.value
    property alias sampleRateValueJ: sampleRateJInput.value
    property alias sampleRateValueK: sampleRateKInput.value

    FilterButtons
    {
        id: filterButtons

        Layout.fillWidth: true
        Layout.margins: palette.contentMargin
        //changesWaiting: !((arrayComboBox.currentText === targetFilter.lastArrayName) && (minValue === targetFilter.lastMinValue) && (maxValue === targetFilter.lastMaxValue)) && (arrayComboBox.currentIndex !== -1)

        onApplyFilter:
        {
            var voi = [voiValueXMin, voiValueXMax, voiValueYMin, voiValueYMax, voiValueZMin, voiValueZMax]
            var sampleRate = [sampleRateValueI, sampleRateValueJ, sampleRateValueK]

            targetFilter.apply(voi, sampleRate)
        }
        onResetFilter:
        {
            palette.updateVoi();
            palette.updateSampleRate();
        }
        onDeleteFilter:
        {
            targetFilter.deleteFilter();
            palette.destroy();
        }
    }

    GridLayout
    {
        id: cropLayout

        columns: 3
        columnSpacing: 4
        rowSpacing: 4

        Layout.fillWidth: true
        Layout.minimumHeight: 10
        Layout.minimumWidth: 240
        Layout.preferredWidth: 240
        Layout.margins: palette.contentMargin

        readonly property real inputWidth: 100

        Label
        {
            id: volumeOfInterestLabel
            Layout.rowSpan: 3

            text: qsTr("VOI: ");
        }

        IntField
        {
            id: voiXMin
            Layout.fillWidth: true
            Layout.preferredWidth: inputWidth

            max: voiBoundXMax
            min: 0
        }

        IntField
        {
            id: voiXMax
            Layout.fillWidth: true
            Layout.preferredWidth: inputWidth

            max: 0
            min: voiBoundXMin
        }

        IntField
        {
            id: voiYMin
            Layout.fillWidth: true
            Layout.preferredWidth: inputWidth

            max: voiBoundYMax
            min: 0
        }

        IntField
        {
            id: voiYMax
            Layout.fillWidth: true
            Layout.preferredWidth: inputWidth

            max: 0
            min: voiBoundYMin
        }

        IntField
        {
            id: voiZMin
            Layout.fillWidth: true
            Layout.preferredWidth: inputWidth

            max: voiBoundZMax
            min: 0
        }

        IntField
        {
            id: voiZMax
            Layout.fillWidth: true
            Layout.preferredWidth: inputWidth

            max: 0
            min: voiBoundZMin
        }

        Label
        {
            id: sampleRateILabel
            text: qsTr("Sample Rate I: ");
        }

        IntField
        {
            id: sampleRateIInput
            Layout.fillWidth: true
            Layout.columnSpan: 2
            Layout.preferredWidth: inputWidth * 2

            min: 0
        }

        Label
        {
            id: sampleRateJLabel
            text: qsTr("Sample Rate J: ");
        }

        IntField
        {
            id: sampleRateJInput
            Layout.fillWidth: true
            Layout.columnSpan: 2
            Layout.preferredWidth: inputWidth * 2

            min: 0
        }

        Label
        {
            id: sampleRateKLabel
            text: qsTr("Sample Rate K: ");
        }

        IntField
        {
            id: sampleRateKInput
            Layout.fillWidth: true
            Layout.columnSpan: 2
            Layout.preferredWidth: inputWidth * 2

            min: 0
        }
    }

    function updateVoi()
    {
        voiXMin.value = targetFilter.voi[0]
        voiXMax.value = targetFilter.voi[1]
        voiYMin.value = targetFilter.voi[2]
        voiYMax.value = targetFilter.voi[3]
        voiZMin.value = targetFilter.voi[4]
        voiZMax.value = targetFilter.voi[5]
    }

    function updateSampleRate()
    {
        sampleRateIInput.value = targetFilter.sampleRate[0]
        sampleRateJInput.value = targetFilter.sampleRate[1]
        sampleRateKInput.value = targetFilter.sampleRate[2]
    }

    Connections
    {
        target: targetFilter
        onVoiChanged:
        {
            updateVoi();
        }
        onSampleRateChanged:
        {
            updateSampleRate();
        }
    }
}
