#include "mainwindow.h"

#include "ui_mainwindow.h"
#include <QStandardItemModel>

/**
 * @brief MainWindow::MainWindow Constructs a new main window.
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->envelopeSelectBox->setItemData(0, QVariant(-1));
  ui->constraintA0SelectBox->setItemData(0, QVariant(-1));
  ui->constraintA1SelectBox->setItemData(0, QVariant(-1));

  ui->aSlider->setMaximum(ui->mainView->settings.aSectors);
  ui->TimeSlider->setMaximum(ui->mainView->settings.tSectors);

  // connect(ui->p0x, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);
  // connect(ui->p0y, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);
  // connect(ui->p1x, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);
  // connect(ui->p1y, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);
  // connect(ui->p2x, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);
  // connect(ui->p2y, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);
  // connect(ui->p3x, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);
  // connect(ui->p3y, &QDoubleSpinBox::editingFinished, this, &MainWindow::on_controlPointChanged);

  connect(ui->bezierToolView, &BezierToolView::bezierToolChanged,
    this, &MainWindow::on_bezierToolChanged);

  updateUI();
}

/**
 * @brief MainWindow::~MainWindow Destructor.
 */
MainWindow::~MainWindow() { delete ui; }

/**
 * @brief MainWindow::addEnvToSelectorMenus Adds an envelope to all the selector menus
 * @param env
 */
void MainWindow::addEnvToSelectorMenus(const Envelope* env)
{
  int idx = env->getIndex();
  QString text = "Envelope " + QString::number(idx);
  QVariant data = QVariant(idx);

  ui->envelopeSelectBox->addItem(text, data);
  ui->constraintA0SelectBox->addItem(text, data);
  ui->constraintA1SelectBox->addItem(text, data);
}


void MainWindow::removeEnvFromSelectorMenus(const Envelope* env)
{
  QVariant data = QVariant(env->getIndex());
  int i = ui->envelopeSelectBox->findData(data);
  ui->envelopeSelectBox->removeItem(i);
  ui->constraintA0SelectBox->removeItem(i);
  ui->constraintA1SelectBox->removeItem(i);
}

/**
 * @brief SetComboBoxItemEnabled Changes the visibility of an item in a combobox. Thanks to https://stackoverflow.com/questions/38915001/disable-specific-items-in-qcombobox/62261745
 * @param comboBox
 * @param index
 * @param enabled
 */
void MainWindow::SetComboBoxItemEnabled(QComboBox* comboBox, int index, bool enabled)
{
  auto* model = qobject_cast<QStandardItemModel*>(comboBox->model());
  assert(model);
  if (!model) return;

  auto* item = model->item(index);
  assert(item);
  if (!item) return;
  item->setEnabled(enabled);
}

/**
 * @brief SetComboBoxItemEnabled Changes the visibility of an item in a combobox. Thanks to https://stackoverflow.com/questions/38915001/disable-specific-items-in-qcombobox/62261745
 * @param comboBox
 * @param env
 * @param enabled
 */
void MainWindow::SetComboBoxItemEnabled(QComboBox* comboBox, const Envelope* env, bool enabled)
{
  int idx = comboBox->findData(QVariant(env->getIndex()));
  SetComboBoxItemEnabled(comboBox, idx, enabled);
}

QString MainWindow::QVectorToString(const QVector3D& v)
{
  return QString("(%1,%2,%3)").arg(v.x()).arg(v.y()).arg(v.z());
}

void MainWindow::updateUI()
{
  updateUI(ui->mainView->settings.selectedIdx);
}

void MainWindow::updateUI(int prevIdx)
{
  QSignalBlocker b1(ui->envelopeActiveCheckBox);
  QSignalBlocker b2(ui->constraintA0SelectBox);
  QSignalBlocker b3(ui->constraintA1SelectBox);
  QSignalBlocker b4(ui->tanContCheckBox);
  QSignalBlocker b9(ui->radiusSpinBox);
  QSignalBlocker b10(ui->drumRadiusSpinBox);
  QSignalBlocker b11(ui->angleSpinBox);
  QSignalBlocker b12(ui->heightSpinBox);
  QSignalBlocker b13(ui->toolBox);

  int idx = ui->mainView->settings.selectedIdx;
  // if (idx == prevIdx) {
  //     qDebug() << ":: ERROR -- UI update for unchanged selection";
  //     return;
  // }
  QVector<Envelope*> envelopes = ui->mainView->envelopes;

  // Update the constraint selectors content
  QList<int> dependents = idx == -1 ? QList<int>() : envelopes[idx]->getAllDependents().values();
  for (int i = 0; i < envelopes.size(); i++)
  {
    if (!ui->mainView->indicesUsed[i]) continue;
    bool showInConstraintBox = !dependents.contains(i);
    SetComboBoxItemEnabled(ui->constraintA0SelectBox, envelopes[i], showInConstraintBox);
    SetComboBoxItemEnabled(ui->constraintA1SelectBox, envelopes[i], showInConstraintBox);
  }
  if (idx != -1 && envelopes[idx]->isPositContinuous())
  {
    SetComboBoxItemEnabled(ui->constraintA1SelectBox, envelopes[idx]->getAdjA0Envelope(), false);
  }

  // Enable tabs if an envelope is selected
  ui->envelopeActiveCheckBox->setEnabled(idx != -1);
  ui->constraintsGroupBox->setEnabled(idx != -1);
  ui->SettingsTabMenu->setTabEnabled(2, idx != -1);
  ui->SettingsTabMenu->setTabEnabled(3, idx != -1);

  if (idx != -1)
  {
    Envelope* env = envelopes[idx];

    // Envelope Settings
    bool envelopeActive = env->isActive();
    ui->envelopeActiveCheckBox->setChecked(envelopeActive);

    int a0Idx = env->isPositContinuous() ? env->getAdjA0Envelope()->getIndex() + 1 : 0;
    ui->constraintA0SelectBox->setCurrentIndex(a0Idx);

    bool tanCont = env->getTanContinuity();
    ui->tanContCheckBox->setChecked(tanCont);
    ui->tanContCheckBox->setEnabled(env->isPositContinuous());

    int a1Idx = env->isAxisConstrained() ? env->getAdjA1Envelope()->getIndex() + 1 : 0;
    ui->constraintA1SelectBox->setCurrentIndex(a1Idx);
    ui->constraintA1SelectBox->setEnabled(env->isPositContinuous());

    // Tool settings

    Cylinder* cyl = ui->mainView->cylinders[idx];
    Drum* drum = ui->mainView->drums[idx];
    BezierTool* bezierTool = ui->mainView->bezierTools[idx];
    ui->radiusSpinBox->setValue(cyl->getRadius());
    ui->drumRadiusSpinBox->setValue(drum->getCurvatureRadius());
    ui->angleSpinBox->setValue(cyl->getAngle());
    ui->heightSpinBox->setValue(cyl->getHeight());

    int toolTypeIdx = (int)env->getTool()->getType();
    ui->toolBox->setCurrentIndex(toolTypeIdx);

    switch (toolTypeIdx)
    {
    case ToolType::Tool_Cylinder:
      ui->angleSpinBox->setEnabled(true);
      ui->drumRadiusSpinBox->setEnabled(false);
      ui->bezierToolView->setEnabled(false);
      break;
    case ToolType::Tool_Drum:
      ui->angleSpinBox->setEnabled(false);
      ui->drumRadiusSpinBox->setEnabled(true);
      ui->bezierToolView->setEnabled(false);
      break;
    case ToolType::Tool_Bezier:
      ui->angleSpinBox->setEnabled(false);
      ui->drumRadiusSpinBox->setEnabled(false);
      ui->bezierToolView->setEnabled(true);
      break;
    default:
      ui->angleSpinBox->setEnabled(false);
      ui->drumRadiusSpinBox->setEnabled(false);
      ui->bezierToolView->setEnabled(false);
      break;
    }
  }
  else
  {
    // if no envelope is selected, clear the constraint boxes
    ui->constraintA0SelectBox->setCurrentIndex(0);
    ui->constraintA1SelectBox->setCurrentIndex(0);
    ui->tanContCheckBox->setChecked(false);
  }
  qDebug() << "done updating ui";
}

// void MainWindow::updateControlPointBoxes(CubicBezier2D bezier)
// {
//   ui->p0x->setValue(bezier.getP0().x());
//   ui->p0y->setValue(bezier.getP0().y());
//   ui->p1x->setValue(bezier.getP1().x());
//   ui->p1y->setValue(bezier.getP1().y());
//   ui->p2x->setValue(bezier.getP2().x());
//   ui->p2y->setValue(bezier.getP2().y());
//   ui->p3x->setValue(bezier.getP3().x());
//   ui->p3y->setValue(bezier.getP3().y());
// }

/***********************************************************/
/********************** Envelope Menu **********************/
/***********************************************************/

void MainWindow::on_envelopeSelectBox_currentIndexChanged(int index)
{
  qDebug() << ":: on_envelopeSelectBox_currentIndexChanged";
  int idx = ui->envelopeSelectBox->itemData(index).value<int>();
  int prevIdx = ui->mainView->settings.selectedIdx;
  qDebug() << "Selected envelope" << idx;
  ui->mainView->settings.selectedIdx = idx;

  updateUI(prevIdx);
}

void MainWindow::on_envelopeActiveCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_envelopeActiveCheckBox_toggled";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  if (ui->mainView->envelopes[idx]->isActive() == checked) return;

  qDebug() << "Changed active state of envelope" << idx;
  ui->mainView->envelopes[idx]->setActive(checked);

  ui->mainView->update();
}


void MainWindow::on_constraintA0SelectBox_currentIndexChanged(int index)
{
  qDebug() << ":: on_constraintA0SelectBox_currentIndexChanged";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  Envelope* envelope = ui->mainView->envelopes[idx];
  // Unhide previous adjacent envelope in a1 constraint
  if (envelope->isPositContinuous())
  {
    SetComboBoxItemEnabled(ui->constraintA1SelectBox, envelope->getAdjA0Envelope(), true);
  }

  // Set adjacent envelope
  Envelope* adjEnv = (index == 0) ? nullptr : ui->mainView->envelopes[index - 1];
  if (envelope->getAdjA0Envelope() != adjEnv)
  {
    envelope->setAdjacentA0Envelope(adjEnv);

    QSet<int> depEnvs = envelope->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;

    ui->mainView->envelopeMeshUpdates += idx;
    ui->mainView->toolTransfUpdates += idx;

    ui->mainView->update();
  }

  // Hide new adjacent envelope in a1 constraint, or if its no longer adjacent to any, disable the tangent and a1 constraint
  if (adjEnv != nullptr)
  {
    SetComboBoxItemEnabled(ui->constraintA1SelectBox, adjEnv, false);
  }
  ui->tanContCheckBox->setEnabled(adjEnv != nullptr);
  ui->constraintA1SelectBox->setEnabled(adjEnv != nullptr);
  // If adjacent, disable the path menu
  ui->SettingsTabMenu->setTabEnabled(3, adjEnv == nullptr);
}

void MainWindow::on_constraintA1SelectBox_currentIndexChanged(int index)
{
  qDebug() << ":: on_constraintA1SelectBox_currentIndexChanged";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  Envelope* envelope = ui->mainView->envelopes[idx];

  // Set adjacent envelope
  Envelope* adjEnv = (index == 0) ? nullptr : ui->mainView->envelopes[index - 1];
  if (envelope->getAdjA1Envelope() != adjEnv)
  {
    envelope->setAdjacentA1Envelope(adjEnv);

    QSet<int> depEnvs = envelope->getAllDependents();
    ui->mainView->envelopeMeshUpdates += depEnvs;
    ui->mainView->toolTransfUpdates += depEnvs;
    ui->mainView->envelopeMeshUpdates += idx;
    ui->mainView->toolTransfUpdates += idx;
    ui->mainView->update();
  }
  ui->tanContCheckBox->setEnabled(adjEnv == nullptr);
}

/**
 * @brief MainWindow::on_tanContCheckBox_toggled Updates the tangetial continuity settings of the envelopes.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_tanContCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_tanContCheckBox_toggled";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  ui->mainView->envelopes[idx]->setTanContinuity(checked);

  QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  ui->mainView->envelopeMeshUpdates += depEnvs;
  ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->update();

  updateUI();
}

void MainWindow::on_newEnvelopeButton_clicked()
{
  qDebug() << ":: on_newEnvelopeButton_clicked";
  Envelope* env = ui->mainView->addNewEnvelope();
  if (env == nullptr)
  {
    qDebug() << "Maximum number of envelopes reached";
    return;
  }
  int idx = env->getIndex();
  addEnvToSelectorMenus(env);

  // QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  // ui->mainView->envelopeMeshUpdates += depEnvs;
  // ui->mainView->toolMeshUpdates += depEnvs;
  // ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->updateAllUniforms = true;
  ui->mainView->update();
}

/***********************************************************/
/************************ Tool Menu ************************/
/***********************************************************/


/**
 * @brief MainWindow::on_radiusSpinBox_valueChanged Updates the radius of the tool.
 * @param value new radius.
 */
void MainWindow::on_radiusSpinBox_valueChanged(double value)
{
  qDebug() << ":: on_radiusSpinBox_valueChanged";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  ui->mainView->cylinders[idx]->setRadius(value);
  ui->mainView->drums[idx]->setRadius(value);
  ui->mainView->bezierTools[idx]->setInnerRadius(value);

  // QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  // ui->mainView->toolMeshUpdates += depEnvs;
  // ui->mainView->envelopeMeshUpdates += depEnvs;
  // ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_drumRadiusSpinBox_valueChanged Updates the inner radius of the drum.
 * @param value new inner radius.
 */
void MainWindow::on_drumRadiusSpinBox_valueChanged(double value)
{
  qDebug() << ":: on_drumRadiusSpinBox_valueChanged";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  ui->mainView->drums[idx]->setCurvatureRadius(value);

  // QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  // ui->mainView->toolMeshUpdates += depEnvs;
  // ui->mainView->envelopeMeshUpdates += depEnvs;
  // ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_angleSpinBox_valueChanged Updates the opening angle of the cylinder.
 * @param value new opening angle.
 */
void MainWindow::on_angleSpinBox_valueChanged(double value)
{
  qDebug() << ":: on_angleSpinBox_valueChanged";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  ui->mainView->cylinders[idx]->setAngle(value);

  // QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  // ui->mainView->toolMeshUpdates += depEnvs;
  // ui->mainView->envelopeMeshUpdates += depEnvs;
  // ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_heightSpinBox_valueChanged Updates the height (length) of the tool.
 * @param value new height.
 */
void MainWindow::on_heightSpinBox_valueChanged(double value)
{
  qDebug() << ":: on_heightSpinBox_valueChanged";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;
  ui->mainView->cylinders[idx]->setHeight(value);
  ui->mainView->drums[idx]->setHeight(value);
  ui->mainView->bezierTools[idx]->setHeight(value);


  // QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  // ui->mainView->toolMeshUpdates += depEnvs;
  // ui->mainView->envelopeMeshUpdates += depEnvs;
  // ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_toolBox_currentIndexChanged Updates the tool type. Activates/Deactivates their corresponding input fields.
 * @param index The index of the tool.
 */
void MainWindow::on_toolBox_currentIndexChanged(int index)
{
  qDebug() << ":: on_toolBox_currentIndexChanged";
  int idx = ui->mainView->settings.selectedIdx;
  if (idx == -1) return;

  qDebug() << "tool index" << index;
  Tool* tool = nullptr;
  switch (index)
  {
  case ToolType::Tool_Cylinder:
    ui->angleSpinBox->setEnabled(true);
    ui->drumRadiusSpinBox->setEnabled(false);
    ui->bezierToolView->setEnabled(false);
    tool = ui->mainView->cylinders[idx];
    break;
  case ToolType::Tool_Drum:
    ui->angleSpinBox->setEnabled(false);
    ui->drumRadiusSpinBox->setEnabled(true);
    ui->bezierToolView->setEnabled(false);
    tool = ui->mainView->drums[idx];
    break;
  case ToolType::Tool_Bezier:
    ui->angleSpinBox->setEnabled(false);
    ui->drumRadiusSpinBox->setEnabled(false);
    ui->bezierToolView->setEnabled(true);
    tool = ui->mainView->bezierTools[idx];
    break;
  default:
    ui->angleSpinBox->setEnabled(false);
    ui->drumRadiusSpinBox->setEnabled(false);
    ui->bezierToolView->setEnabled(false);
    break;
  }

  ui->mainView->envelopes[idx]->setTool(tool);
  ui->mainView->toolRenderers[idx]->setTool(tool);

  // QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
  // ui->mainView->toolMeshUpdates += depEnvs;
  // ui->mainView->envelopeMeshUpdates += depEnvs;
  // ui->mainView->toolTransfUpdates += depEnvs;
  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->update();
}

// void MainWindow::on_controlPointChanged()
// {
//   int idx = ui->mainView->settings.selectedIdx;
//   if (idx == -1) return;
//   QVector2D p0(ui->p0x->value(), ui->p0y->value());
//   QVector2D p1(ui->p1x->value(), ui->p1y->value());
//   QVector2D p2(ui->p2x->value(), ui->p2y->value());
//   QVector2D p3(ui->p3x->value(), ui->p3y->value());
//
//   BezierTool *bezier_tool = ui->mainView->bezierTools[idx];
//   bezier_tool->setPoints(p0, p1, p2, p3);
//   qDebug() << "Control Points changed to:";
//   qDebug() << "-| P0 " << p0;
//   qDebug() << "-| P1 " << p1;
//   qDebug() << "-| P2 " << p2;
//   qDebug() << "-| P3 " << p3;
//
//   // QSet<int> depEnvs = ui->mainView->envelopes[idx]->getAllDependents();
//   // ui->mainView->toolMeshUpdates += depEnvs;
//   // ui->mainView->envelopeMeshUpdates += depEnvs;
//   // ui->mainView->toolTransfUpdates += depEnvs;
//   ui->mainView->envelopeMeshUpdates += idx;
//   ui->mainView->toolMeshUpdates += idx;
//   ui->mainView->toolTransfUpdates += idx;
//   ui->mainView->update();
// }

void MainWindow::on_bezierToolChanged(CubicBezier2D bezier)
{
  int idx = ui->mainView->settings.selectedIdx;
  ui->mainView->bezierTools[idx]->setBezier(bezier);

  ui->mainView->envelopeMeshUpdates += idx;
  ui->mainView->toolMeshUpdates += idx;
  ui->mainView->toolTransfUpdates += idx;
  ui->mainView->update();
}


/***********************************************************/
/*********************** Render Menu ***********************/
/***********************************************************/

/**
 * @brief MainWindow::on_envelopeCheckBox_toggled Updates the envelope(s) visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_envelopeCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_envelopeCheckBox_toggled";
  ui->mainView->settings.showEnvelope = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_toolCheckBox_toggled Updates the tool(s) visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_toolCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_toolCheckBox_toggled";
  ui->mainView->settings.showTool = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_grazCurveCheckBox_toggled Updates the grazing curves visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_grazCurveCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_grazCurveCheckBox_toggled";
  ui->mainView->settings.showGrazingCurve = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_pathCheckBox_toggled Updates the path visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_pathCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_pathCheckBox_toggled";
  ui->mainView->settings.showPath = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_controlPointsCheckBox_toggled Updates the control points visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_controlPointsCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_controlPointsCheckBox_toggled";
  ui->mainView->settings.showControlPoints = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_orientationCPsCheckBox_toggled Updates the orientation control points visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_orientationCPsCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_orientationCPsCheckBox_toggled";
  ui->mainView->settings.showOrientationControlPoints = checked;
  ui->mainView->update();
}


/**
 * @brief MainWindow::on_toolAxisCheckBox_toggled Updates the tool axis visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_toolAxisCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_toolAxisCheckBox_toggled";
  ui->mainView->settings.showToolAxis = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_normalsCheckBox_toggled Updates the normals visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_normalsCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_normalsCheckBox_toggled";
  ui->mainView->settings.showNormals = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_sphereCheckBox_toggled Updates the spheres visibility.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_sphereCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_sphereCheckBox_toggled";
  ui->mainView->settings.showSpheres = checked;
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_reflecLinesCheckBox_toggled Updates the envelope's shading.
 * @param checked The new value of the checkbox.
 */
void MainWindow::on_reflecLinesCheckBox_toggled(bool checked)
{
  qDebug() << ":: on_reflecLinesCheckBox_toggled";
  ui->fracReflSpinBox->setEnabled(checked);
  ui->freqReflSpinBox->setEnabled(checked);

  ui->mainView->settings.reflectionLines = checked;
  for (int i = 0; i < ui->mainView->envelopes.size(); i++)
  {
    if (!ui->mainView->indicesUsed[i]) continue;
    ui->mainView->envelopes[i]->updateRenderSettings(ui->mainView->settings);
    ui->mainView->envelopeMeshUpdates += i;
  }
  ui->mainView->update();
}

void MainWindow::on_freqReflSpinBox_valueChanged(int value)
{
  qDebug() << ":: on_freqReflSpinBox_valueChanged";
  ui->mainView->settings.reflFreq = value;
  for (int i = 0; i < ui->mainView->envelopes.size(); i++)
  {
    if (!ui->mainView->indicesUsed[i]) continue;
    ui->mainView->envelopes[i]->updateRenderSettings(ui->mainView->settings);
    ui->mainView->envelopeMeshUpdates += i;
  }
  ui->mainView->update();
}

void MainWindow::on_fracReflSpinBox_valueChanged(double value)
{
  qDebug() << ":: on_fracReflSpinBox_valueChanged";
  ui->mainView->settings.percentBlack = value;
  for (int i = 0; i < ui->mainView->envelopes.size(); i++)
  {
    if (!ui->mainView->indicesUsed[i]) continue;
    ui->mainView->envelopes[i]->updateRenderSettings(ui->mainView->settings);
    ui->mainView->envelopeMeshUpdates += i;
  }
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_axisSectorsSpinBox_valueChanged Updates the number of sectors for the construction of the cylinder.
 * @param value The new number of sectors.
 */
void MainWindow::on_axisSectorsSpinBox_valueChanged(int value)
{
  qDebug() << ":: on_axisSectorsSpinBox_valueChanged";
  qDebug() << "TODO change to dynamic";
  ui->aSlider->setMaximum(value);
  ui->mainView->settings.aSectors = value;

  for (int i = 0; i < ui->mainView->envelopes.size(); i++)
  {
    if (!ui->mainView->indicesUsed[i]) continue;
    ui->mainView->envelopes[i]->setSectorsA(value);
    ui->mainView->envelopes[i]->update();

    ui->mainView->cylinders[i]->setSectors(value);
    ui->mainView->cylinders[i]->update();

    ui->mainView->drums[i]->setSectors(value);
    ui->mainView->drums[i]->update();

    ui->mainView->bezierTools[i]->setSectors(value);
    ui->mainView->bezierTools[i]->update();
  }

  ui->mainView->updateBuffers();
  ui->mainView->updateToolTransf();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_timeSectorsSpinBox_valueChanged Updates the number of sectors for the construction of the path.
 * @param value The new number of sectors.
 */
void MainWindow::on_timeSectorsSpinBox_valueChanged(int value)
{
  qDebug() << ":: on_timeSectorsSpinBox_valueChanged";
  ui->TimeSlider->setMaximum(value);
  ui->mainView->settings.tSectors = value;

  for (int i = 0; i < ui->mainView->envelopes.size(); i++)
  {
    if (!ui->mainView->indicesUsed[i]) continue;
    ui->mainView->envelopes[i]->setSectorsT(value);
    ui->mainView->envelopes[i]->update();
    BezierPath& path = ui->mainView->envelopes[i]->getToolMovement().getPath();
    path.setSectors(value);
  }

  ui->mainView->updateBuffers();
  ui->mainView->updateToolTransf();
  ui->mainView->update();
}


/***********************************************************/
/******************** General Side Menu ********************/
/***********************************************************/

/**
 * @brief MainWindow::on_TimeSlider_sliderMoved Updates the time value.
 * @param value The new time value.
 */
void MainWindow::on_TimeSlider_sliderMoved(int value)
{
  qDebug() << ":: on_TimeSlider_sliderMoved";
  CylinderMovement& move = ui->mainView->envelopes[0]->getToolMovement();
  BezierPath& path = move.getPath();
  ui->mainView->settings.timeIdx = value;

  ui->mainView->updateBuffers();
  ui->mainView->updateToolTransf();
  ui->mainView->updateAllUniforms = true;
  ui->mainView->update();
}


/**
 * @brief MainWindow::on_aSlider_sliderMoved Updates the time value for the tool axis.
 * @param value The new a value.
 */
void MainWindow::on_aSlider_sliderMoved(int value)
{
  qDebug() << ":: on_aSlider_sliderMoved";
  ui->mainView->settings.aIdx = value;
  ui->mainView->updateBuffers();
  ui->mainView->update();
}

/**
 * @brief MainWindow::on_ResetRotationButton_clicked Resets the rotation.
 */
void MainWindow::on_ResetRotationButton_clicked()
{
  qDebug() << ":: on_ResetRotationButton_clicked";
  ui->mainView->setRotation(0, 0, 0);
  ui->mainView->update();
}

/*
/**
 * @brief MainWindow::on_RotationDialX_sliderMoved Updates the number of degrees
 * of rotation in the x direction.
 * @param value Unused.

void MainWindow::on_RotationDialX_sliderMoved(int value)
{
  qDebug() << ":: on_RotationDialX_sliderMoved";
  ui->mainView->setRotation(value, ui->RotationDialY->value(),
                            ui->RotationDialZ->value());
}

/**
 * @brief MainWindow::on_RotationDialY_sliderMoved Updates the number of degrees
 * of rotation in the y direction.
 * @param value Unused.

void MainWindow::on_RotationDialY_sliderMoved(int value)
{
  qDebug() << ":: on_RotationDialY_sliderMoved";
  ui->mainView->setRotation(ui->RotationDialX->value(), value,
                            ui->RotationDialZ->value());
}

/**
 * @brief MainWindow::on_RotationDialZ_sliderMoved Updates the number of degrees
 * of rotation in the z direction.
 * @param value Unused.

void MainWindow::on_RotationDialZ_sliderMoved(int value)
{
  qDebug() << ":: on_RotationDialZ_sliderMoved";
  ui->mainView->setRotation(ui->RotationDialX->value(),
                            ui->RotationDialY->value(), value);
  ui->mainView->update();
}
*/

/**
 * @brief MainWindow::on_ResetScaleButton_clicked Resets the scale factor.
 */
void MainWindow::on_ResetScaleButton_clicked()
{
  qDebug() << ":: on_ResetScaleButton_clicked";
  ui->mainView->setScale(1);
  ui->mainView->update();
}
/*
/**
 * @brief MainWindow::on_ScaleSlider_sliderMoved Updates the scale value.
 * @param value The new scale value.

void MainWindow::on_ScaleSlider_sliderMoved(int value)
{
  qDebug() << ":: on_ScaleSlider_sliderMoved";
  ui->mainView->setScale(value / 100.0f);
  ui->mainView->update();
}
*/

/**
 * @brief MainWindow::renderToFile Used to render the frame buffer to the file.
 * DO NOT REMOVE OR MODIFY!
 */
void MainWindow::renderToFile()
{
  QImage image = ui->mainView->grabFramebuffer();
  image.save("framebuffer_contents.png", "PNG",
             100); // Save using the best quality
}
