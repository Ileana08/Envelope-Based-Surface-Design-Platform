#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QErrorMessage>
#include <QComboBox>

#include "CubicBezier2D.h"
#include "envelope.h"

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class is the main application window. Among other
 * things, it handles the inputs from the screen widgets.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

    QErrorMessage error;
 public:
  Ui::MainWindow *ui;

  explicit MainWindow(QWidget *parent = nullptr);
  void renderToFile();
  ~MainWindow() override;

 private:
  void SetComboBoxItemEnabled(QComboBox *comboBox, int index, bool enabled);
  void SetComboBoxItemEnabled(QComboBox *comboBox, const Envelope *env, bool enabled);
  QString QVectorToString(const QVector3D &v);
  void addEnvToSelectorMenus(const Envelope *env);
  void removeEnvFromSelectorMenus(const Envelope *env);

  void updateUI();
  void updateUI(int prevIdx);
  // void updateControlPointBoxes(CubicBezier2D bezier);

private slots:
  // Envelope Menu
  void on_envelopeSelectBox_currentIndexChanged(int index);
  void on_envelopeActiveCheckBox_toggled(bool checked);
  void on_constraintA0SelectBox_currentIndexChanged(int index);
  void on_constraintA1SelectBox_currentIndexChanged(int index);
  void on_tanContCheckBox_toggled(bool checked);

  void on_newEnvelopeButton_clicked();

  // Tool menu
  void on_radiusSpinBox_valueChanged(double value);
  void on_drumRadiusSpinBox_valueChanged(double value);
  void on_angleSpinBox_valueChanged(double value);
  void on_heightSpinBox_valueChanged(double value);
  void on_bezierRadiusSpinBox_valueChanged(double value);
  void on_toolBox_currentIndexChanged(int index);


  // void on_controlPointChanged();
  void on_bezierToolChanged(CubicBezier2D bezier);

  // Render menu
  void on_envelopeCheckBox_toggled(bool checked);
  void on_toolCheckBox_toggled(bool checked);
  void on_grazCurveCheckBox_toggled(bool checked);
  void on_pathCheckBox_toggled(bool checked);
  void on_controlPointsCheckBox_toggled(bool checked);
  void on_orientationCPsCheckBox_toggled(bool checked);
  void on_toolAxisCheckBox_toggled(bool checked);
  void on_normalsCheckBox_toggled(bool checked);
  void on_sphereCheckBox_toggled(bool checked);
  void on_reflecLinesCheckBox_toggled(bool checked);
  void on_freqReflSpinBox_valueChanged(int value);
  void on_fracReflSpinBox_valueChanged(double value);
  void on_axisSectorsSpinBox_valueChanged(int value);
  void on_timeSectorsSpinBox_valueChanged(int value);

  // General Side menu
  void on_TimeSlider_sliderMoved(int value);
  void on_aSlider_sliderMoved(int value);

  void on_ResetRotationButton_clicked();
  // void on_RotationDialX_sliderMoved(int value);
  // void on_RotationDialY_sliderMoved(int value);
  // void on_RotationDialZ_sliderMoved(int value);

  void on_ResetScaleButton_clicked();
  // void on_ScaleSlider_sliderMoved(int value);
};

#endif  // MAINWINDOW_H
