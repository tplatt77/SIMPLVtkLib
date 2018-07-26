import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

import VSQml 1.0

Palette {
    id: palette
    paletteType: paletteTypeCollapsible
    title: "Filter View"

    property VSFilterViewModel filterViewModel: VSFilterViewModel { }

    TreeView
    {
        id: filterView

        Layout.fillWidth: true
        Layout.minimumHeight: 100
        Layout.minimumWidth: 202
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        // Using palette property to ensure model type
        model: palette.filterViewModel

        alternatingRowColors: false
        rootIndex: model.rootIndex;

        TableViewColumn
        {
            title: "Filters"
        }

        style: TreeViewStyle {
            backgroundColor: "white"
            alternateBackgroundColor: "white"
            itemDelegate: Item {
                readonly property VSFilterViewSettings viewSettings: filterViewModel.getFilterViewSettingsByIndex(styleData.index)
                readonly property string filterName: viewSettings.filterName
                readonly property bool filterCheckable: filterViewModel.getFilterCheckable(styleData.index)

                Label {
                    anchors.fill: parent
                    text: filterName
                    verticalAlignment: Text.AlignBottom
                    visible: !filterCheckable
                }
                CheckBox {
                    id: filterCheckBox
                    text: filterName
                    checkedState: viewSettings.visibility ? Qt.Checked : Qt.Unchecked
                    visible: filterCheckable
                    onCheckedStateChanged: viewSettings.visibility = (checkedState == Qt.Checked)
                    Connections {
                        target: viewSettings
                        onVisibilityChanged: filterCheckBox.checkedState = viewSettings.visibility ? Qt.Checked : Qt.Unchecked
                    }
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
