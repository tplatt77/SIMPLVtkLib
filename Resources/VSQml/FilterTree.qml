import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import VSQml 1.0

Palette {
    id: palette
    paletteType: paletteTypeCollapsible
    title: "Filter View"

    property VSFilterModel filterModel: VSFilterModel { }

    TreeView
    {
        id: filterView

        Layout.fillWidth: true
        Layout.minimumHeight: 100
        Layout.minimumWidth: 202
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        // Using QML context property
        model: filterModel

        TableViewColumn
        {
            title: "Filters"
            //role: "display"
            width: 200

            delegate: Text { text: "Filter" }
        }
    }

    onFilterModelChanged:
    {
        console.log("Model Rows: " + filterModel.rowCount())
    }
}
