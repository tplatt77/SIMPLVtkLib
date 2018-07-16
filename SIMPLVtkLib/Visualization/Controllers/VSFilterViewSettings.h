/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
 * contributors may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The code contained herein was partially funded by the followig contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <map>
#include <vector>

#include <QtGui/QIcon>
#include <QtCore/QObject>
#include <QtGui/QColor>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtQml/QQmlProperty>
#include <QtQml/QQmlPropertyValueSource>

#include <vtkAbstractMapper3D.h>
#include <vtkActor.h>
#include <vtkCubeAxesActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkOutlineFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

#include "SIMPLVtkLib/Visualization/Controllers/VSLookupTableController.h"
#include "SIMPLVtkLib/Visualization/VisualFilters/VSAbstractFilter.h"

#include "SIMPLVtkLib/SIMPLVtkLib.h"

class vtkDataSetMapper;
class vtkImageSliceMapper;
class vtkImageSlice;

/**
 * @class VSFilterViewSettings VSFilterViewSettings.h
 * SIMPLVtkLib/Visualization/Controllers/VSFilterViewSettings.h
 * @brief This class handles the visibility settings regarding a given VSVisualFilter
 * for a single VSViewController. This includes setting the vtkDataArray and component
 * to color by, whether or not to map values to a lookup table, and the visibility of
 * both the vtkActor and vtkScalarBarWidget.
 */
class SIMPLVtkLib_EXPORT VSFilterViewSettings : public QObject, public QQmlPropertyValueSource
{
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)

  Q_PROPERTY(QString filterName READ getFilterName)
  Q_PROPERTY(bool visibility READ isVisible WRITE setVisible NOTIFY visibilityChanged)
  Q_PROPERTY(bool gridVisibility READ isGridVisible WRITE setGridVisible NOTIFY gridVisibilityChanged)
  Q_PROPERTY(QString activeArrayName READ getActiveArrayName WRITE setActiveArrayName NOTIFY activeArrayNameChanged)
  Q_PROPERTY(int activeComponentIndex READ getActiveComponentIndex WRITE setActiveComponentIndex NOTIFY activeComponentIndexChanged)
  Q_PROPERTY(int pointSize READ getPointSize WRITE setPointSize NOTIFY pointSizeChanged)
  Q_PROPERTY(double alpha READ getAlpha WRITE setAlpha NOTIFY alphaChanged)
  Q_PROPERTY(bool showScalarBar READ isScalarBarVisible WRITE setScalarBarVisible NOTIFY showScalarBarChanged)
  Q_PROPERTY(QStringList arrayNames READ getArrayNames NOTIFY arrayNamesChanged)
  Q_PROPERTY(QStringList componentNames READ getComponentNames NOTIFY componentNamesChanged)
  Q_PROPERTY(double* solidColor READ getSolidColor WRITE setSolidColor NOTIFY solidColorChanged)

public:
  using Map = std::map<VSAbstractFilter*, VSFilterViewSettings*>;

  enum class Representation : int
  {
    Invalid = -1,
    Outline,
    Points,
    Wireframe,
    Surface,
    SurfaceWithEdges,
    Default = Surface
  };

  enum class ActorType : int
  {
    Invalid = -1,
    DataSet = 0,
    Image2D = 1
  };

  enum class ColorMapping : int
  {
    Always = 0,
    NonColors,
    None
  };

  Q_ENUMS(Representation)
  Q_ENUMS(ActorType)
  Q_ENUMS(ColorMapping)

  Q_PROPERTY(Representation representation READ getRepresentation WRITE setRepresentation NOTIFY representationChanged)
  Q_PROPERTY(ColorMapping mapColors READ getMapColors WRITE setMapColors NOTIFY mapColorsChanged)

  VSFilterViewSettings();

  /**
   * @brief Constructor
   * @param filter
   */
  VSFilterViewSettings(VSAbstractFilter* filter);

  /**
   * @brief Copy constructor
   * @param copy
   */
  VSFilterViewSettings(const VSFilterViewSettings& copy);

  /**
   * @brief Deconstructor
   */
  virtual ~VSFilterViewSettings();

  /**
   * @brief Returns a pointer to the VSAbstractFilter
   * @return
   */
  VSAbstractFilter* getFilter();

  /**
   * @brief getFilterName required for filterName property.
   * Retrieves the value from the current filter.
   * @return
   */
  QString getFilterName() const;

  /**
   * @brief Returns true if the VSFilterViewSettings can be displayed and false otherwise
   * @return
   */
  bool isValid() const;

  /**
   * @brief Returns true if the filter is displayed for this view.  Returns false otherwise
   * @return
   */
  bool isVisible() const;

  /**
   * @brief Returns true if the axes grid is displayed for this view.  Returns false otherwise.
   */
  bool isGridVisible() const;

  /**
   * @brief Returns the active array name used to render the filter
   * @return
   */
  QString getActiveArrayName() const;

  /**
   * @brief Returns the active array component used to render the filter
   * @return
   */
  int getActiveComponentIndex() const;

  /**
   * @brief Returns the array names that can be used to color the data
   * @return
   */
  QStringList getArrayNames() const;

  /**
   * @brief Returns the component names available for the active array
   * @return
   */
  QStringList getComponentNames();

  /**
   * @brief Returns the number of components for the given array
   * @param arrayIndex
   * @return
   */
  int getNumberOfComponents(int arrayIndex);

  /**
   * @brief Returns the number of components for the given array
   * @param arrayIndex
   * @return
   */
  int getNumberOfComponents(QString name);

  /**
  * @brief Returns true if the active array has multiple components
  * @return
  */
  bool hasMulipleComponents();

  /**
   * @brief Returns the ColorMapping value for mapping filter values to a lookup table.
   * @return
   */
  ColorMapping getMapColors();

  /**
   * @brief Returns tha alpha transparency used for the displaying the actor
   */
  double getAlpha();

  /**
   * @brief Returns true if the ScalarBarWidget is visible.  Returns false otherwise
   * @return
   */
  bool isScalarBarVisible() const;

  /**
   * @brief Returns the vtkActor used to render the filter
   * @return
   */
  VTK_PTR(vtkProp3D) getActor();

  /**
   * @brief Returns the vtkScalarBarWidget used for the filter
   * @return
   */
  VTK_PTR(vtkScalarBarWidget) getScalarBarWidget();

  /**
   * @brief Returns the vtkCubeAxesActor used for showing the axes grid
   * @return
   */
  VTK_PTR(vtkCubeAxesActor) getCubeAxesActor();

  /**
   * @brief Returns the color used when no scalar data exists as a double*
   * @return
   */
  double* getSolidColor() const;

  /**
   * @brief Returns the color used when no scalar data exists as a QColor
   * @return
   */
  QColor getSolidQColor() const;

  /**
   * @brief Returns the actor property representation
   * @return
   */
  Representation getRepresentation() const;

  /**
   * @brief Returns the actor property representation as an integer
   * @return
   */
  int getRepresentationi() const;

  /**
   * @brief Returns the actor property representation as an enum
   * @return
   */
  ActorType getActorType() const;

  /**
   * @brief Returns true if the actor should be using point data instead of cell data.
   * Return false otherwise.
   * @return
   */
  bool isPointData();

  /**
   * @brief Returns true if the current actor is set to render points.
   * @return
   */
  bool isRenderingPoints();

  /**
   * @brief Returns the point size from the actor.
   * If ActorType is Image2D or Invalid, returns -1.
   * @return
   */
  int getPointSize() const;

  /**
   * @brief Sets the point size for the vtkActor. Does nothing for Image2D ActorTypes.
   * @param pointSize
   */
  void setPointSize(int pointSize);

  /**
   * @brief Returns true if the current actor renders points as spheres. Returns false otherwise.
   * @return
   */
  bool renderPointsAsSpheres() const;

  /**
   * @brief Sets whether or not points are rendered as spheres
   * @param renderSpheres
   */
  void setRenderPointsAsSpheres(bool renderSpheres);

  /**
   * @brief Copies another VSFilterViewSettings for everything but the active filter
   * @param filter
   */
  void copySettings(VSFilterViewSettings* other);

  /**
   * @brief Returns the data representation menu
   * @return
   */
  QMenu* getRepresentationMenu();

  /**
   * @brief Returns the ColorBy menu
   * @return
   */
  QMenu* getColorByMenu();

  /**
   * @brief Returns the ArrayComponent menu
   * @return
   */
  QMenu* getArrayComponentMenu();

  /**
   * @brief Returns the scalar mapping menu
   * @return
   */
  QMenu* getMapScalarsMenu();

  /**
   * @brief Returns an action allowing the user to pick the solid color.
   * @return
   */
  QAction* getSetColorAction();

  /**
   * @brief Returns an action allowing the user to set the object's opacity
   * @return
   */
  QAction* getSetOpacityAction();

  /**
   * @brief Returns an action allowing the user to turn off the scalar bar widget
   * @return
   */
  QAction* getToggleScalarBarAction();

  /**
   * @brief Returns the QIcon used for solid colors
   * @return
   */
  QIcon getSolidColorIcon();

  /**
   * @brief Returns the QIcon used for cell data
   * @return
   */
  QIcon getCellDataIcon();

  /**
   * @brief Returns the QIcon used for point data
   * @return
   */
  QIcon getPointDataIcon();

  /**
   * @brief Handle QML property value source
   * @param property
   */
  void setTarget(const QQmlProperty& property) override;

public slots:
  /**
   * @brief Displays the vtkActor for this view
   */
  void show();

  /**
   * @brief Hides the vtkActor for this view
   */
  void hide();

  /**
   * @brief Sets the vtkActor's visibility for this view
   * @param visible
   */
  void setVisible(bool visible);

  /**
   * @brief Sets the vtkCubeAxesActor's visibility for this view
   * @param visible
   */
  void setGridVisible(bool visible);

  /**
   * @brief Updates the active array name for this view
   * @param name
   */
  void setActiveArrayName(QString name);

  /**
   * @brief Updates the active component index for this view
   * @param index
   */
  void setActiveComponentIndex(int index);

  /**
   * @brief Updates whether or not the data values are mapped to the lookup table for this view
   * @param mapColors
   */
  void setMapColors(ColorMapping mapColors);

  /**
   * @brief Sets the object's alpha transparency
   * @param alpha
   */
  void setAlpha(double alpha);

  /**
   * @brief Inverts the scalar bar so that colors at the higher end of the scale
   * are moved to the bottom and colors at the bottom are moved to the top.
   */
  void invertScalarBar();

  /**
   * @brief Set the preset colors for the VSLookupTableController
   * @param json
   */
  void loadPresetColors(const QJsonObject& json);

  /**
   * @brief Updates whether or not the vtkScalarBarWidget is visible for this view
   * @param visible
   */
  void setScalarBarVisible(bool visible);

  /**
   * @brief Sets the color to use when there are no scalar values to map
   * @param color
   */
  void setSolidColor(double* color);

  /**
   * @brief Sets the color to use when there are no scalar values to map
   * @param color
   */
  void setSolidQColor(QColor color);

  /**
   * @brief Sets the actor property representation
   * @param type
   */
  void setRepresentation(const Representation& type);

  /**
   * @brief Checks the imported data type, changes the representation, and renders the changes
   */
  void importedData();

  /**
   * @brief Updates the actors for the newly reloaded data type, resets the camera, and renders the changes
   */
  void reloadedData();

  /**
   * @brief Updates the actors for the current data type and renders the changes
   */
  void checkDataType();

  /**
   * @brief Updates the input connection for the vtkMapper
   * @param filter
   */
  void updateInputPort(VSAbstractFilter* filter);

  /**
   * @brief Updates the transformation for 2D image data
   */
  void updateTransform();

signals:
  void visibilityChanged(const bool&);
  void gridVisibilityChanged(const bool&);
  void activeArrayNameChanged(const QString&);
  void activeComponentIndexChanged(const int&);
  void pointSizeChanged(const int&);
  void alphaChanged(const double&);
  void showScalarBarChanged(const bool&);
  void arrayNamesChanged(const QStringList&);
  void componentNamesChanged(const QStringList&);

  void representationChanged(const Representation&);
  void mapColorsChanged(const ColorMapping&);

  void renderPointSpheresChanged(const bool&);
  void solidColorChanged(const double*);

  /*void visibilityChanged(VSFilterViewSettings*, bool);
  void gridVisibilityChanged(VSFilterViewSettings*, bool);
  void representationChanged(VSFilterViewSettings*, VSFilterViewSettings::Representation);
  void solidColorChanged(VSFilterViewSettings*, double*);
  void activeArrayNameChanged(VSFilterViewSettings*, QString);
  void activeComponentIndexChanged(VSFilterViewSettings*, int);
  void pointSizeChanged(VSFilterViewSettings*, int);
  void renderPointSpheresChanged(VSFilterViewSettings*, bool);
  void mapColorsChanged(VSFilterViewSettings*, ColorMapping);
  void alphaChanged(VSFilterViewSettings*, double);
  void showScalarBarChanged(VSFilterViewSettings*, bool); */

  void requiresRender();
  void actorsUpdated();
  void dataLoaded();
  void swappingActors(vtkProp3D* oldProp, vtkProp3D* newProp);

protected:
  /**
   * @brief Creates the static icons used used.
   */
  void setupStaticIcons();

  /**
   * @brief Performs initial setup commands for any actors used in the view settings
   */
  void setupActors(bool outline = true);

  /**
   * @brief Creates a vtkImageSliceMapper and vtkImageSlice for displaying 2D Image data
   */
  void setupImageActors();

  /**
   * @brief Creates a vtkDataSetMapper and vtkActor for displaying generic vtkDataSets
   */
  void setupDataSetActors();

  /**
   * @brief Creates a vtkAxisActor for displaying the axis grid
   */
  void setupCubeAxesActor();

  /**
   * @brief Creates and initializes QActions
   */
  void setupActions();

  /**
   * @brief Returns the vtkDataSetMapper if the ActorType is DataSet and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkDataSetMapper* getDataSetMapper() const;

  /**
   * @brief Returns the vtkActor if the ActorType is DataSet and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkActor* getDataSetActor() const;

  /**
   * @brief Returns the vtkImageSliceMapper if the ActorType is Image2D and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkImageSliceMapper* getImageMapper() const;

  /**
   * @brief Returns the vtkImageSlice if the ActorType is Image2D and the settings are valid.
   * Returns nullptr otherwise.
   * @return
   */
  vtkImageSlice* getImageSliceActor() const;

  /**
   * @brief Returns true if data set is a 2D image.  Returns false otherwise.
   * @return
   */
  bool isFlatImage();

  /**
   * @brief Returns true if there is only a single array in the filter output's point data.
   * Returns false if there are no arrays or more than one.
   */
  bool hasSinglePointArray();

  /**
   * @brief Updates the alpha for DataSet actors
   */
  void updateDataSetAlpha();

  /**
   * @breif Updates the alpha for 2D Image actors
   */
  void updateImageAlpha();

  /**
   * @brief Sets the visual filter makes any Qt connections required
   * @param filter
   */
  void connectFilter(VSAbstractFilter* filter);

  /**
   * @brief Handles the target filter being deleted.
   */
  void filterDeleted();

  /**
   * @brief Returns the array name based on the given index
   * @param index
   * @return
   */
  QString getArrayNameByIndex(int index);

  /**
   * @brief Returns the array at the given index
   * @param index
   * @return
   */
  vtkDataArray* getArrayAtIndex(int index);

  /**
   * @brief Returns the array with the given name
   * @param name
   * @return
   */
  vtkDataArray* getArrayByName(QString name);

  /**
   * @brief Updates the mapper color mode to match variable values
   */
  void updateColorMode();

  /**
   * @brief Returns true if the given array is a 3-Component color array
   * @param array
   * @return
   */
  bool isColorArray(vtkDataArray* array);

  /**
   * @brief Returns the current vtkDataArray specified by the given array index
   */
  vtkDataArray* getDataArray();

private:
  VSAbstractFilter* m_Filter = nullptr;
  ActorType m_ActorType = ActorType::Invalid;
  VTK_PTR(vtkDataSetSurfaceFilter) m_DataSetFilter = nullptr;
  bool m_ShowFilter = true;
  QString m_ActiveArrayName;
  int m_ActiveComponent = -1;
  ColorMapping m_MapColors = ColorMapping::NonColors;
  Representation m_Representation = Representation::Default;
  VTK_PTR(vtkAbstractMapper3D) m_Mapper = nullptr;
  VTK_PTR(vtkProp3D) m_Actor = nullptr;
  VTK_PTR(vtkOutlineFilter) m_OutlineFilter = nullptr;
  VSLookupTableController* m_LookupTable = nullptr;
  double m_Alpha = 1.0;
  VTK_PTR(vtkScalarBarActor) m_ScalarBarActor = nullptr;
  VTK_PTR(vtkScalarBarWidget) m_ScalarBarWidget = nullptr;
  bool m_HadNoArrays = false;
  VTK_PTR(vtkCubeAxesActor) m_CubeAxesActor = nullptr;
  bool m_GridVisible = false;
  QQmlProperty m_TargetProperty;

  QAction* m_SetColorAction = nullptr;
  QAction* m_SetOpacityAction = nullptr;
  QAction* m_ToggleScalarBarAction = nullptr;
  static QIcon* m_SolidColorIcon;
  static QIcon* m_CellDataIcon;
  static QIcon* m_PointDataIcon;

  static double* NULL_COLOR;
};

Q_DECLARE_METATYPE(VSFilterViewSettings)
