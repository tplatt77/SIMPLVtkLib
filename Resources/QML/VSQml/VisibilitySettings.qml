import QtQuick 2.6
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import VSQml 1.0
import "../PaletteItems"

Palette
{
    id: palette

    property alias viewSettings: visibilityLayout.viewSettings

    VisibilityLayout {
        id: visibilityLayout
        Layout.fillWidth: true
        Layout.minimumHeight: 10
        Layout.minimumWidth: 240
        Layout.margins: palette.contentMargin
        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin
    }

//    SwipeView
//    {
//        id: viewSettingsSwipeView
//        Layout.fillWidth: true
//        Layout.minimumHeight: Math.max(100, visbilityLayout.height, transformItem.height)
//        Layout.minimumWidth: 240
//        Layout.margins: palette.contentMargin
//        //Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

//        VisibilityLayout {
//            id: visibilityLayout
//        }

//        Transform {
//            id: transformItem
//        }
//    }

//    PageIndicator {
//        id: swipeIndicator
//        Layout.fillWidth: true
//        Layout.minimumHeight: 10
//        Layout.minimumWidth: 240
//        Layout.margins: palette.contentMargin
//        Layout.bottomMargin: palette.contentMargin + palette.bottomMargin

//        count: viewSettingsSwipeView.count
//        currentIndex: viewSettingsSwipeView.currentIndex
//    }
}
