import QtQuick 2.6
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.2

import VSQml 1.0
import "../PaletteItems"
import "../InputTypes"

FilterPalette
{
    id: palette

    property VSClipFilter targetFilter: VSClipFilter {}
    property alias vtkBoxWidget: boxWidget.vtkWidget
    property alias vtkPlaneWidget: planeWidget.vtkWidget

    title: targetFilter.filterName

    changesWaiting: (clipTypeCombo.currentIndex !== targetFilter.lastClipType) || planeWidgetChanged || boxWidgetChanged || (isPlaneType ? insideOutCheckBox.checked !== targetFilter.lastPlaneInverted : insideOutCheckBox.check !== targetFitler.lastBoxInverted)
    onApplyFilter:
    {
        if(planeWidget.visible)
        {
            targetFilter.apply(planeWidget.getOrigin(), planeWidget.getNormal(), insideOutCheckBox.checked);
            vtkPlaneWidget.drawPlaneOff();
        }
        else
        {
            targetFilter.apply(boxWidget.getTranslation(), boxWidget.getRotation(), boxWidget.getScale(), insideOutCheckBox.checked);
        }
    }
    onResetFilter:
    {
        palette.resetFilterValues();
        if(planeWidget.visible)
        {
            vtkPlaneWidget.drawPlaneOff();
        }
    }
    onDeleteFilter:
    {
        targetFilter.deleteFilter();
        palette.destroy();
    }

    readonly property bool planeWidgetChanged: !((targetFilter.lastPlaneOrigin[0] == planeWidget.originX && targetFilter.lastPlaneOrigin[1] == planeWidget.originY && targetFilter.lastPlaneOrigin[2] == planeWidget.originZ) &&
                                                (targetFilter.lastPlaneNormal[0] == planeWidget.normalX && targetFilter.lastPlaneNormal[1] == planeWidget.normalY  && targetFilter.lastPlaneNormal[2] == planeWidget.normalZ))

    readonly property bool boxWidgetChanged: !((targetFilter.lastBoxTranslation[0] == boxWidget.translateX && targetFilter.lastBoxTranslation[1] == boxWidget.translateY && targetFilter.lastBoxTranslation[2] == boxWidget.translateY) &&
                                              (targetFilter.lastBoxRotation[0] == boxWidget.rotateX && targetFilter.lastBoxRotation[1] == boxWidget.rotateY && targetFilter.lastBoxRotation[2] == boxWidget.rotateZ) &&
                                              (targetFilter.lastBoxScale[0] == boxWidget.scaleX && targetFilter.lastBoxScale[1] == boxWidget.scaleY && targetFilter.lastBoxScale[2] == boxWidget.scaleZ))

    readonly property bool isPlaneType: clipTypeCombo.currentText === "Plane"
    readonly property bool isBoxType: clipTypeCombo.currentText === "Box"

    GridLayout
    {
        id: clipLayout

        columns: 2
        columnSpacing: 4
        rowSpacing: 4
        Layout.fillWidth: true
        Layout.minimumHeight: 10
        Layout.minimumWidth: 240
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

        Label
        {
            id: clipTypeLabel
            text: "Clip Type: "
        }
        ComboBox
        {
            id: clipTypeCombo
            Layout.fillWidth: true

            model: targetFilter.clipTypes

            onCurrentIndexChanged: enableVtkWidget();
        }
        CheckBox
        {
            id: insideOutCheckBox
            Layout.columnSpan: 2
            Layout.fillWidth: true

            text: "Inside Out"
        }

        VtkPlaneWidget
        {
            id: planeWidget
            Layout.columnSpan: 2
            Layout.fillWidth: true

            visible: clipTypeCombo.currentText == "Plane"
            vtkWidget.widgetEnabled: planeWidget.visible && palette.activeFocus
        }
        VtkBoxWidget
        {
            id: boxWidget
            Layout.columnSpan: 2
            Layout.fillWidth: true

            visible: clipTypeCombo.currentText == "Box"
            vtkWidget.widgetEnabled: boxWidget.visible && palette.activeFocus
        }
    }

    function resetFilterValues()
    {
        clipTypeCombo.currentIndex = targetFilter.lastClipType;

        var lastClipTypeInverted = targetFilter.lastClipType == 0 ? targetFilter.lastPlaneInverted : targetFilter.lastBoxInverted;
        insideOutCheckBox.checkedState = lastClipTypeInverted ? Qt.Checked : Qt.Unchecked;

        planeWidget.setOrigin(targetFilter.lastPlaneOrigin);
        planeWidget.setNormal(targetFilter.lastPlaneNormal);

        boxWidget.setTranslation(targetFilter.lastBoxTranslation);
        boxWidget.setRotation(targetFilter.lastBoxRotation);
        boxWidget.setScale(targetFilter.lastBoxScale);
    }

    onTargetFilterChanged:
    {
        resetFilterValues();
        vtkPlaneWidget.updateBounds(targetFilter);
        vtkBoxWidget.updateBounds(targetFilter);
        enableVtkWidget();
    }

    Keys.onReturnPressed: applyFilter();
    Keys.onDeletePressed: deleteFilter();

    function enableVtkWidget()
    {
        disableVtkWidget();

        switch(clipTypeCombo.currentIndex)
        {
        case 0:
            vtkPlaneWidget.widgetEnabled = true;
            break;
        case 1:
            vtkBoxWidget.widgetEnabled = true;
            break;
        default:
            vtkPlaneWidget.widgetEnabled = true;
            break;
        }
    }

    function disableVtkWidget()
    {
        vtkPlaneWidget.widgetEnabled = false;
        vtkBoxWidget.widgetEnabled = false;
    }
}
