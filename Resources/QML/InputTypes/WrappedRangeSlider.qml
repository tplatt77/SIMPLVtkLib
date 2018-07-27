import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

ColumnLayout {
    id: groupedRangeSlider

    property real from: 0
    property real to: 1
    property alias minValue: minSlider.value
    property alias maxValue: maxSlider.value

    Slider
    {
        id: minSlider
        Layout.fillWidth: true

        minimumValue: groupedRangeSlider.from
        maximumValue: groupedRangeSlider.to

        onValueChanged:
        {
            if(value > maxSlider.value)
            {
                maxSlider.value = value;
            }
        }
    }
    Slider
    {
        id: maxSlider
        Layout.fillWidth: true

        minimumValue: groupedRangeSlider.from
        maximumValue: groupedRangeSlider.to

        onValueChanged:
        {
            if(value < minSlider.value)
            {
                minSlider.value = value;
            }
        }
    }
}
