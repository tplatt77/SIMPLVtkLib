import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

RowLayout
{
    id: buttonLayout
    spacing: 6

    property bool changesWaiting: false

    signal applyFilter()
    signal resetFilter()
    signal deleteFilter()

    Button
    {
        id: applyBtn
        Layout.fillWidth: true

        text: qsTr("Apply")
        enabled: changesWaiting
        isDefault: true
    }

    Button
    {
        id: resetBtn
        Layout.fillWidth: true

        text: qsTr("Reset")
        enabled: changesWaiting
    }

    Button
    {
        id: deleteBtn
        Layout.fillWidth: true

        text: qsTr("Delete")
    }

    Component.onCompleted:
    {
        applyBtn.clicked.connect(applyFilter)
        resetBtn.clicked.connect(resetFilter)
        deleteBtn.clicked.connect(deleteFilter)
    }
}
