import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import VSQml 1.0

Palette {
    id: palette
    paletteType: paletteTypeCollapsible
    //title: "Filter View"

    property VSFilterModel filterModel: VSFilterModel { }

    ListModel
    {
        id: testModel

        ListElement {
            display: "Test1"
            tooltip: "foo"
        }
        ListElement {
            display: "Test2"
            tooltip: "Bar"
        }
    }

    TreeView
    {
        id: filterView

        Layout.fillWidth: true
        Layout.minimumHeight: 100
        Layout.minimumWidth: 222
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        model: palette.filterModel
        //model: testModel

        TableViewColumn
        {
            title: "Filters"
            role: "display"
            width: 200

            //delegate: Text{ text: "Filter: " + palette.filterModel.display }
        }
    }
}
