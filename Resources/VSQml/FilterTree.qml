import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import VSQml 1.0

Palette {
    id: palette
    paletteType: paletteTypeCollapsible
    title: "Filter View"

    //property VSFilterModel filterModel: VSFilterModel { }
    property alias filterModel: filterView.model

    TreeView
    {
        id: filterView

        Layout.fillWidth: true
        Layout.minimumHeight: 100
        Layout.minimumWidth: 202
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        alternatingRowColors: false

        TableViewColumn
        {
            title: "Filters"
            role: "display"
            width: 200
        }

        // Using QML context property
        //model: filterModel
    }

    onFilterModelChanged:
    {
        console.log("Model Rows: " + filterModel.rowCount())
        filterView.rootIndex = filterModel.rootIndex()
    }
}
