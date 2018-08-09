import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

import VSQml 1.0
import "../PaletteItems"
import "../InputTypes"

FilterPalette
{
    id: palette

    property VSSliceFilter targetFilter: VSSliceFilter {}
    property alias vtkPlaneWidget: planeWidget.vtkWidget

    title: targetFilter.filterName

    changesWaiting: planeWidgetChanged
    onApplyFilter:
    {
        targetFilter.apply(planeWidget.getOrigin(), planeWidget.getNormal());
        vtkPlaneWidget.widgetEnabled = true
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

    readonly property bool planeWidgetChanged: !((targetFilter.lastOrigin[0] == planeWidget.originX && targetFilter.lastOrigin[1] == planeWidget.originY && targetFilter.lastOrigin[2] == planeWidget.originZ) &&
                                                (targetFilter.lastNormal[0] == planeWidget.normalX && targetFilter.lastNormal[1] == planeWidget.normalY  && targetFilter.lastNormal[2] == planeWidget.normalZ))


    GridLayout
    {
        id: sliceLayout

        columns: 2
        columnSpacing: 4
        rowSpacing: 4
        Layout.fillWidth: true
        Layout.minimumHeight: 10
        Layout.minimumWidth: 240
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        VtkPlaneWidget
        {
            id: planeWidget
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }
    }

    function resetFilterValues()
    {
        insideOutCheckBox.checkedState = lastClipTypeInverted ? Qt.Checked : Qt.Unchecked;

        planeWidget.setOrigin(targetFilter.lastPlaneOrigin);
        planeWidget.setNormal(targetFilter.lastPlaneNormal);
    }

    onTargetFilterChanged:
    {
        resetFilterValues();
    }

    Keys.onReturnPressed: applyFilter();
    Keys.onDeletePressed: deleteFilter();
}
