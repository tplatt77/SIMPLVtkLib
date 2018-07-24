import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import VSQml 1.0

GridLayout {
  id: visibilityLayout
  
  property VSFilterViewSettings viewSettings: VSFilterViewSettings {}
  property alias viewAdvancedSettings : advancedSettings.checked

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
    
    model: ["Outline", "Points", "Wireframe", "Surface", "Surface with Edges"]
    currentIndex: 0 // Default value
    
    Connections{
      target: visibilityLayout.viewSettings
      onRepresentationChanged:{
        representationSelection.currentIndex = visibilityLayout.viewSettings.representation;
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
    text: qsTr("Array:")
  }
  
  ComboBox {
    id: arraySelection
    activeFocusOnTab: true
    Layout.columnSpan: 2
    Layout.fillWidth: true
    
    model: visibilityLayout.viewSettings.arrayNames
    //focus: true
    
    function updateArrayName()
    {
      var index = arraySelection.find(visibilityLayout.viewSettings.activeArrayName);
      if(index < 0)
      {
        index = 0;
      }
      
      arraySelection.currentIndex = index;
      visibilityLayout.showColors(index == 0);
      visibilityLayout.showComponents(visibilityLayout.viewSettings.componentNames.count > 1);
    }
    
    Connections{
      target: visibilityLayout.viewSettings
      onActiveArrayNameChanged:{
        var index = arraySelection.find(visibilityLayout.viewSettings.activeArrayName);
        if(index < 0)
        {
          index = 0;
        }
        
        arraySelection.currentIndex = index;
        visibilityLayout.showColors(index == 0);
        visibilityLayout.showComponents(visibilityLayout.viewSettings.componentNames.count > 1);
      }
    }
    onCurrentIndexChanged: {
      if(currentIndex == 0)
      {
        visibilityLayout.viewSettings.activeArrayName = ""
      }
      else
      {
        visibilityLayout.viewSettings.activeArrayName = currentText
      }
      
      parent.forceActiveFocus()
    }
  }
  
  Label {
    id: componentLabel
    text: qsTr("Component:")
  }
  
  ComboBox {
    id: componentSelection
    Layout.columnSpan: 2
    Layout.fillWidth: true
    
    model: visibilityLayout.viewSettings.componentNames
    //currentIndex: 1 // Default value
    
    onModelChanged: {
      showComponents(visibilityLayout.viewSettings.componentNames.length > 1);
    }
    
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
    
    visible: false
  }
  
  ColorButton {
    id: colorButton
    Layout.columnSpan: 2
    Layout.fillWidth: true
    Layout.preferredHeight: 23
    Layout.preferredWidth: 125
    
    visible: false
    
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
  }
  
  Label {
    id: label6
    text: qsTr("Map Scalars:")
  }
  
  ComboBox {
    id: mapScalarsSelection
    activeFocusOnTab: true
    Layout.fillWidth: true
    Layout.columnSpan: 2
    
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
    visible: visibilityLayout.viewAdvancedSettings
    
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

  function showComponents(show)
  {
      componentLabel.visible = show;
      componentSelection.visible = show;
  }

  function showColors(show)
  {
      colorLabel.visible = show;
      colorButton.visible = show;
  }

  onViewSettingsChanged:
  {
      if(viewSettings == null)
      {
          return
      }

      title = visibilityLayout.viewSettings.filterName + ": Visibility"

      visibilityCheckBox.checkedState = visibilityLayout.viewSettings.visibility ? Qt.Checked : Qt.Unchecked
      representationSelection.currentIndex = visibilityLayout.viewSettings.representation
      arraySelection.model = viewSettings.arrayNames
      arraySelection.updateArrayName()
      colorButton.colorProp = viewSettings.solidColor
      componentSelection.currentIndex = visibilityLayout.viewSettings.activeComponentIndex + 1
      mapScalarsSelection.currentIndex = viewSettings.mapColors
      showScalarsCheckBox.checkedState = visibilityLayout.viewSettings.showScalarBar ? Qt.Checked : Qt.Unchecked
      alphaSlider.value = visibilityLayout.viewSettings.alpha
      //transformItem.transform = visibilityLayout.viewSettings.transform
  }
}
