import QtQuick 2.6
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import VSQml 1.0

ColumnLayout {
    id: transformItem
    spacing: 6

    //property VSTransform transform: VSTransform {}

    readonly property real labelHeight: 45
    readonly property real labelPadding: 2
    readonly property real labelMargins: 2
    readonly property real labelLeftPadding: 4
    readonly property real minimumItemWidth: width / 4.5

    Component {
        id: transformBGComp

        Rectangle {
            color: "#c4c4c4"
            border.width: 1
            border.color: "#adadad"
            anchors.margins: labelMargins
            radius: 4
        }
    }

    // Local Transform
    Label {
        id: localTransformLabel
        font.bold: true
        text: "Local Transform"
    }
    GridLayout {
        id: localTransformLayout

        rowSpacing: 4
        columnSpacing: 4
        rows: 3
        columns: 4
        Layout.fillWidth: true

        Label {
            id: localTransformPosition
            Layout.fillWidth: false
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8
            font.bold: false

            verticalAlignment: Qt.AlignVCenter
            leftPadding: labelLeftPadding
            text: qsTr("Position")
        }

        TextField {
            id: localPositionX
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            Layout.columnSpan: 1
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.positionX
        }

        TextField {
            id: localPositionY
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            Layout.columnSpan: 1
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.positionY
        }

        TextField {
            id: localPositionZ
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            Layout.columnSpan: 1
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.positionZ
        }

        Label {
            id: localTransformRotation
            Layout.fillWidth: false
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8
            font.bold: false

            verticalAlignment: Qt.AlignVCenter
            leftPadding: labelLeftPadding
            text: qsTr("Rotation")
        }

        TextField {
            id: localRotationX
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.rotationX
        }

        TextField {
            id: localRotationY
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.rotationY
        }

        TextField {
            id: localRotationZ
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.rotationZ
        }

        Label {
            id: localTransformScale
            Layout.fillWidth: false
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8
            font.bold: false

            verticalAlignment: Qt.AlignVCenter
            leftPadding: labelLeftPadding
            text: qsTr("Scale")
        }

        TextField {
            id: localScaleX
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.scaleX
        }

        TextField {
            id: localScaleY
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.scaleY
        }

        TextField {
            id: localScaleZ
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8

            validator: DoubleValidator {}
            //text: transformItem.transform.scaleZ
        }
    }

    // Global Transform
    Label {
        id: globalTransformLabel
        font.bold: true
        text: "Global Transform"
    }
    GridLayout {
        id: globalTransformLayout

        rowSpacing: 4
        columnSpacing: 4
        rows: 3
        columns: 4
        Layout.fillWidth: true

        Label {
            id: globalTransformPosition
            Layout.fillWidth: false
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8
            font.bold: false

            verticalAlignment: Qt.AlignVCenter
            leftPadding: labelLeftPadding
            text: qsTr("Position")
        }

        Label {
            id: globalPositionX
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.positionX
        }

        Label {
            id: globalPositionY
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.positionY
        }

        Label {
            id: globalPositionZ
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.positionZ
        }

        Label {
            id: globalTransformRotation
            Layout.fillWidth: false
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8
            font.bold: false

            verticalAlignment: Qt.AlignVCenter
            leftPadding: labelLeftPadding
            text: qsTr("Rotation")
        }

        Label {
            id: globalRotationX
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.rotationX
        }

        Label {
            id: globalRotationY
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.rotationY
        }

        Label {
            id: globalRotationZ
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.rotationZ
        }

        Label {
            id: globalTransformScale
            Layout.fillWidth: false
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.preferredWidth: minimumItemWidth
            font.pointSize: 8
            font.bold: false

            verticalAlignment: Qt.AlignVCenter
            leftPadding: labelLeftPadding
            text: qsTr("Scale")
        }

        Label {
            id: globalScaleX
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.scaleX
        }

        Label {
            id: globalScaleY
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.scaleY
        }

        Label {
            id: globalScaleZ
            Layout.columnSpan: 1
            Layout.preferredHeight: labelHeight
            Layout.minimumWidth: minimumItemWidth
            font.pointSize: 8

            padding: labelMargins * 2
            verticalAlignment: Qt.AlignVCenter
            background: Loader { sourceComponent: transformBGComp }
            //text: transformItem.transform.scaleZ
        }
    }

//    Connections {
//        source: transform
//        onUpdatedLocalPosition:
//        {
//            localPositionX.text = transform.localPosition[0]
//            localPositionY.text = transform.localPosition[1]
//            localPositionZ.text = transform.localPosition[2]
//        }
//        onUpdatedLocalRotation:
//        {
//            localRotationX.text = transform.localRotation[0]
//            localRotationY.text = transform.localRotation[1]
//            localRotationZ.text = transform.localRotation[2]
//        }
//        onUpdatedLocalScale:
//        {
//            localScaleX.text = transform.localScale[0]
//            localScaleY.text = transform.localScale[1]
//            localScaleZ.text = transform.localScale[2]
//        }
//        onUpdatedGlobalPosition:
//        {
//            globalPositionX.text = transform.globalPosition[0]
//            globalPositionY.text = transform.globalPosition[1]
//            globalPositionZ.text = transform.globalPosition[2]
//        }
//        onUpdatedGlobalRotation:
//        {
//            globalRotationX.text = transform.globalRotation[0]
//            globalRotationY.text = transform.globalRotation[1]
//            globalRotationZ.text = transform.globalRotation[2]
//        }
//        onUpdatedGlobalScale:
//        {
//            globalScaleX.text = transform.globalScale[0]
//            globalScaleY.text = transform.globalScale[1]
//            globalScaleZ.text = transform.globalScale[2]
//        }
//    }

//    onTransformChanged:
//    {
//        localPositionX.text = transform.localPosition[0]
//        localPositionY.text = transform.localPosition[1]
//        localPositionZ.text = transform.localPosition[2]

//        localRotationX.text = transform.localRotation[0]
//        localRotationY.text = transform.localRotation[1]
//        localRotationZ.text = transform.localRotation[2]

//        localScaleX.text = transform.localScale[0]
//        localScaleY.text = transform.localScale[1]
//        localScaleZ.text = transform.localScale[2]

//        globalPositionX.text = transform.globalPosition[0]
//        globalPositionY.text = transform.globalPosition[1]
//        globalPositionZ.text = transform.globalPosition[2]

//        globalRotationX.text = transform.globalRotation[0]
//        globalRotationY.text = transform.globalRotation[1]
//        globalRotationZ.text = transform.globalRotation[2]

//        globalScaleX.text = transform.globalScale[0]
//        globalScaleY.text = transform.globalScale[1]
//        globalScaleZ.text = transform.globalScale[2]
//    }
}
