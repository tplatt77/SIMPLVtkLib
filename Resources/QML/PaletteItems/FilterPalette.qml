import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3

Palette {
    id: palette

    property alias changesWaiting: filterButtons.changesWaiting

    signal applyFilter()
    signal resetFilter()
    signal deleteFilter()

    FilterButtons
    {
        id: filterButtons

        Layout.fillWidth: true
        Layout.margins: palette.contentMargin
    }

    Component.onCompleted:
    {
        filterButtons.applyFilter.connect(applyFilter)
        filterButtons.resetFilter.connect(resetFilter)
        filterButtons.deleteFilter.connect(deleteFilter)
    }
}
