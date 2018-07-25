import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import VSQml 1.0

GridLayout {
  id: visibilityLayout
  
  property VSFilterViewSettings viewSettings: VSFilterViewSettings {}
  property alias viewAdvancedSettings : advancedSettings.checked
  readonly property bool arrayOptionsAvailable: viewSettings.fullyImported && viewSettings.representation != 0
  readonly property bool componentOptionsAvailable: arrayOptionsAvailable && viewSettings.componentNames.length > 1
  readonly property bool colorOptionsAvailable: arrayOptionsAvailable && viewSettings.activeArrayName == ""

  rowSpacing: 4
  columnSpacing: 4
  rows: 9
  columns: 3
  
  Label {
    id: visibilitySettingsTitle
    text: qsTr("Visibility Settings")
    Layout.fillWidth: true
    Layout.columnSpan: 2
    Layout.preferredHeight: 15
    Layout.preferredWidth: 218
    font.pointSize: 8
    font.bold: true
  }
  
  Button {
    id: advancedSettings
    //tooltip: "View Advanced Settings"
    Layout.columnSpan: 1
    Layout.preferredHeight: 17
    Layout.preferredWidth: 17
    
    checkable: true
    checked: false
    iconSource: "qrc:/SIMPL/icons/images/cog.png"
  }
  
  CheckBox {
    id: visibilityCheckBox
    Layout.columnSpan: 3
    Layout.fillWidth: true
    
    text: "Filter Visibility"
    checked: Qt.Unchecked
    visible: visibilityLayout.viewAdvancedSettings
    
    Connections{
      target: visibilityLayout.viewSettings
      onVisibilityChanged:{
        visibilityCheckBox.checkedState = visibilityLayout.viewSettings.visibility ? Qt.Checked : Qt.Unchecked
      }
    }
    onCheckedChanged:
    {
      visibilityLayout.viewSettings.visibility = (checkedState == Qt.Checked);
      parent.forceActiveFocus()
    }
  }
  
  Label {
    id: representationLabel
    text: qsTr("Representation:")
  }
  
  ComboBox {
    id: representationSelection
    Layout.columnSpan: 2
    Layout.fillWidth: true
    
    model: ["Outline"]
    currentIndex: 0 // Default value
    
    function setFullRepresentationModel()
    {
        model = ["Outline", "Points", "Wireframe", "Surface", "Surface with Edges"]
        representationSelection.currentIndex = visibilityLayout.viewSettings.representation;
    }
    function resetRepresentationModel()
    {
        representationSelection.currentIndex = 0;
        model = ["Outline"];
    }

    Connections{
      target: visibilityLayout.viewSettings
      onRepresentationChanged:{
        representationSelection.currentIndex = visibilityLayout.viewSettings.representation;
      }
      onDataLoaded: {
        if(representationSelection.model.length <= 1)
        {
          representationSelection.model = ["Outline", "Points", "Wireframe", "Surface", "Surface with Edges"]
        }
      }
    }
    onCurrentIndexChanged:
    {
      visibilityLayout.viewSettings.representation = currentIndex;
      visibilityLayout.forceActiveFocus()
    }
    
  }
  
  Label {
    id: arrayLabel
    visible: arrayOptionsAvailable
    text: qsTr("Array:")
  }
  
  ComboBox {
    id: arraySelection
    activeFocusOnTab: true
    Layout.columnSpan: 2
    Layout.fillWidth: true
    
    visible: arrayOptionsAvailable
    model: visibilityLayout.viewSettings.arrayNames
    //focus: true
    
    function updateArrayName()
    {
      if(visibilityLayout.viewSettings.arrayNames == undefined)
      {
        return;
      }

      var index = Math.max(0, arraySelection.find(visibilityLayout.viewSettings.activeArrayName));
      arraySelection.currentIndex = index;
    }
    
    Connections{
      target: visibilityLayout.viewSettings
      onActiveArrayNameChanged:{
        arraySelection.updateArrayName();
      }
      onDataLoaded: {
        arraySelection.updateArrayName();
      }
    }
    onCurrentIndexChanged: {
      visibilityLayout.viewSettings.activeArrayName = (currentIndex == 0) ? "" : currentText;
      parent.forceActiveFocus()
    }
    onModelChanged: {
      updateArrayName();
    }
  }
  
  Label {
    id: componentLabel
    visible: componentOptionsAvailable
    text: qsTr("Component:")
  }
  
  ComboBox {
    id: componentSelection
    Layout.columnSpan: 2
    Layout.fillWidth: true
    
    visible: componentOptionsAvailable
    model: visibilityLayout.viewSettings.componentNames
    
    Connections{
      target: visibilityLayout.viewSettings
      onActiveComponentIndexChanged:{
        componentSelection.currentIndex = visibilityLayout.viewSettings.activeComponentIndex + 1;
      }
    }
    onCurrentIndexChanged: {
      visibilityLayout.viewSettings.activeComponentIndex = currentIndex - 1;
      parent.forceActiveFocus()
    }
  }
  
  Label {
    id: colorLabel
    text: qsTr("Color:")
    Layout.preferredHeight: 13
    Layout.preferredWidth: 62
    
    visible: colorOptionsAvailable
  }
  
  ColorButton {
    id: colorButton
    Layout.columnSpan: 2
    Layout.fillWidth: true
    Layout.preferredHeight: 23
    Layout.preferredWidth: 125
    
    visible: colorOptionsAvailable
    
    Connections{
      target: visibilityLayout.viewSettings
      onSolidColorChanged:{
        colorButton.colorProp = visibilityLayout.viewSettings.solidColor;
      }
    }
    onColorPropChanged: {
      visibilityLayout.viewSettings.solidColor = colorButton.colorProp;
      parent.forceActiveFocus()
    }
  }
  
  Label {
    id: colorMappingTitle
    text: qsTr("Color Mapping")
    Layout.fillWidth: true
    Layout.columnSpan: 3
    Layout.preferredHeight: 13
    Layout.preferredWidth: 218
    font.bold: true
    visible: arrayOptionsAvailable
  }
  
  Label {
    id: label6
    text: qsTr("Map Scalars:")
    visible: arrayOptionsAvailable
  }
  
  ComboBox {
    id: mapScalarsSelection
    activeFocusOnTab: true
    Layout.fillWidth: true
    Layout.columnSpan: 2
    
    visible: arrayOptionsAvailable
    model: ["Always", "Non-Colors", "Never"]
    currentIndex: 1 // Default value
    
    Connections{
      target: visibilityLayout.viewSettings
      onMapColorsChanged:{
        mapScalarsSelection.currentIndex = visibilityLayout.viewSettings.mapColors
      }
    }
    onCurrentIndexChanged:
    {
      visibilityLayout.viewSettings.mapColors = currentIndex
      visibilityLayout.forceActiveFocus()
    }
  }
  
  CheckBox {
    id: showScalarsCheckBox
    text: qsTr("Show Scalar Bar")
    Layout.preferredHeight: 17
    Layout.preferredWidth: 210
    Layout.columnSpan: 3
    
    checkedState: Qt.Checked // Default value
    visible: visibilityLayout.viewAdvancedSettings && arrayOptionsAvailable
    
    Connections{
      target: visibilityLayout.viewSettings
      onShowScalarBarChanged:{
        showScalarsCheckBox.checkedState = visibilityLayout.viewSettings.showScalarBar ? Qt.Checked : Qt.Unchecked
      }
    }
    onCheckedChanged:
    {
      visibilityLayout.viewSettings.showScalarBar = (checkedState == Qt.Checked)
    }
    onClicked: forceActiveFocus()
  }
  
  Label {
    id: alphaLabel
    text: qsTr("Alpha:")
  }
  
  Slider {
    id: alphaSlider
    Layout.fillWidth: true
    Layout.columnSpan: 2
    Layout.preferredHeight: 22
    Layout.preferredWidth: 148
    value: 1 // Default value
    
    Connections{
      target: visibilityLayout.viewSettings
      onAlphaChanged:{
        alphaSlider.value = visibilityLayout.viewSettings.alpha;
      }
    }
    
    onValueChanged: {
      visibilityLayout.viewSettings.alpha = value
    }
  }

  onViewSettingsChanged:
  {
      if(viewSettings == null)
      {
          return
      }

      title = visibilityLayout.viewSettings.filterName + ": Visibility"

      visibilityCheckBox.checkedState = visibilityLayout.viewSettings.visibility ? Qt.Checked : Qt.Unchecked
      visibilityLayout.viewSettings.fullyImported ? representationSelection.setFullRepresentationModel() : representationSelection.resetRepresentationModel();
      arraySelection.updateArrayName()
      colorButton.colorProp = viewSettings.solidColor
      componentSelection.currentIndex = visibilityLayout.viewSettings.activeComponentIndex + 1
      mapScalarsSelection.currentIndex = viewSettings.mapColors
      showScalarsCheckBox.checkedState = visibilityLayout.viewSettings.showScalarBar ? Qt.Checked : Qt.Unchecked
      alphaSlider.value = visibilityLayout.viewSettings.alpha
      //transformItem.transform = visibilityLayout.viewSettings.transform
  }
}
