import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
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

        // Using palette property to ensure model type
        model: palette.filterModel

        alternatingRowColors: false
        rootIndex: filterModel.rootIndex();

        TableViewColumn
        {
            title: "Filters"
        }

        style: TreeViewStyle {
            backgroundColor: "white"
            alternateBackgroundColor: "white"
            itemDelegate: Item {
                readonly property string filterName: filterModel.getFilterText(styleData.index)
                readonly property bool filterCheckable: filterModel.getFilterCheckable(styleData.index)

                Label {
                    anchors.fill: parent
                    text: filterName
                    verticalAlignment: Text.AlignBottom
                    visible: !filterCheckable
                }
                CheckBox {
                    text: filterName
                    checkedState: filterModel.getFilterCheckState(styleData.index)
                    visible: filterCheckable
                }
            }
            branchDelegate: Image
            {
                scale: 0.65
                x: 5
                horizontalAlignment: Image.AlignHCenter
                source: styleData.isExpanded ? "qrc:/SIMPL/icons/images/navigate_close.png" : "qrc:/SIMPL/icons/images/navigate_right.png"
            }
        }
    }
}
