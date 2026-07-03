#include <math.h>
#include "mainview.h"
#include "vertex.h"

#include <QDateTime>
#include <QOpenGLVersionFunctionsFactory>
/**
 * @brief MainView::MainView Constructs a new main view.
 *
 * @param parent Parent widget.
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent)
{
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView()
{
    //qDebug() << "MainView destructor";
    indicesUsed.clear();
    indicesUsed.squeeze();
    for (auto i : toolRenderers){
        delete i;
    }
    toolRenderers.clear();
    toolRenderers.squeeze();
    for (auto i : drums){
        delete i;
    }
    drums.clear();
    drums.squeeze();
    for (auto i : cylinders){
        delete i;
    }
    cylinders.clear();
    cylinders.squeeze();
    for (auto i : bezierTools){
        delete i;
    }
    bezierTools.clear();
    bezierTools.squeeze();
    for (auto i : moveRenderers){
        delete i;
    }
    moveRenderers.clear();
    moveRenderers.squeeze();
    for (auto i : envelopes){
        delete i;
    }
    envelopes.clear();
    envelopes.squeeze();
    for (auto i : envelopeRenderers){
        delete i;
    }
    envelopeRenderers.clear();
    envelopeRenderers.squeeze();
    for (const QVector<ControlPoint*> &cps : envelopeControlPoints){
        for (ControlPoint *cp : cps) {
            delete cp;
        }
    }
    envelopeControlPoints.clear();
    envelopeControlPoints.squeeze();

    for (const QVector<ControlPoint*> &cps : envelopeOrientationCPs){
        for (ControlPoint *cp : cps) {
            delete cp;
        }
    }
    envelopeOrientationCPs.clear();
    envelopeOrientationCPs.squeeze();

    makeCurrent();
}

/**
 * @brief MainView::addNewEnvelope Creates a new envelope. Return nullptr if no new envelope can be created.
 * @return
 */
Envelope* MainView::addNewEnvelope() {
    int idx = envelopes.indexOf(nullptr);
    if (idx == -1) return nullptr;

    //qDebug() << "addNewEnvelope setup tools";
    // Tools
    Cylinder *cyl = new Cylinder();
    cyl->initTool();
    cylinders[idx] = cyl;
    Drum *drum = new Drum();
    drum->initTool();
    drums[idx] = drum;
    BezierTool *bezierTool = new BezierTool();
    bezierTool->initTool();
    bezierTools[idx] = bezierTool;

    //qDebug() << "addNewEnvelope setup path";

    // Path and envelope
    QVector<ControlPoint*> controlPoints = {
        new ControlPoint(QVector3D(0,0,0), 0.05, 20),
        new ControlPoint(QVector3D(1.0/3,0,0), 0.05, 20),
        new ControlPoint(QVector3D(2.0/3,0,0), 0.05, 20),
        new ControlPoint(QVector3D(1.0,0,0), 0.05, 20)
    };
    envelopeControlPoints[idx] = controlPoints;
    BezierPath path(controlPoints);

    QVector<ControlPoint*> orientationCPs = {
        new ControlPoint(QVector3D(0,2,0), 0.05, 20),
        new ControlPoint(QVector3D(1.0/3,2,0), 0.05, 20),
        new ControlPoint(QVector3D(2.0/3,2,0), 0.05, 20),
        new ControlPoint(QVector3D(1.0,2,0), 0.05, 20)
    };
    envelopeOrientationCPs[idx] = orientationCPs;
    BezierPath orientationPath(orientationCPs);

    Tool *tool;
    switch (defaultTool)
    {
    case ToolType::Tool_Cylinder: tool = cyl;          break;
    case ToolType::Tool_Drum:     tool = drum;         break;
    case ToolType::Tool_Bezier:   tool = bezierTool;   break;
    default:                      tool = nullptr;      break;
    }

    //qDebug() << "addNewEnvelope setup envelope";
    Envelope *env = new Envelope(idx, tool, path);
    env->getToolMovement().setOrientationPath(orientationPath);
    env->initEnvelope();
    envelopes[idx] = env;

    //qDebug() << "addNewEnvelope setup tool renderer";

    // Set related renderers
    toolRenderers[idx]->setTool(tool);
    toolRenderers[idx]->setModelTransf(modelTransf);
    toolRenderers[idx]->setProjTransf(projTransf);

    //qDebug() << "addNewEnvelope envelope renderer";

    envelopeRenderers[idx]->setEnvelope(env);
    envelopeRenderers[idx]->setModelTransf(modelTransf);
    envelopeRenderers[idx]->setProjTransf(projTransf);

    //qDebug() << "addNewEnvelope setup movement renderer";

    moveRenderers[idx]->setMovement(&env->getToolMovement());
    moveRenderers[idx]->setModelTransf(modelTransf);
    moveRenderers[idx]->setProjTransf(projTransf);

    //qDebug() << "addNewEnvelope setup control point renderer";

    controlPointsRenderers[idx]->setControlPoints(env->getToolMovement().getPath().getControlPoints());
    controlPointsRenderers[idx]->setModelTransf(modelTransf);
    controlPointsRenderers[idx]->setProjTransf(projTransf);

    //qDebug() << "addNewEnvelope setup orientation control point renderer";

    orientationCPsRenderers[idx]->setOrientationControlPoints(envelopeOrientationCPs[idx]);
    orientationCPsRenderers[idx]->setControlPoints(envelopeControlPoints[idx]);
    orientationCPsRenderers[idx]->setModelTransf(modelTransf);
    orientationCPsRenderers[idx]->setProjTransf(projTransf);

    env->updateRenderSettings(settings);

    // Activate
    indicesUsed[idx] = true;
    return env;
}

/**
 * @brief MainView::deleteEnvelope. Deletes an envelope. The pointer will point to nothing.
 * @param env
 */
void MainView::deleteEnvelope(Envelope *env) {
    //qDebug() << "TODO";
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions.
 */
void MainView::initializeGL()
{
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this,
            SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize())
    {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion{reinterpret_cast<const char *>(glGetString(GL_VERSION))};
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    // glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // grab the opengl context
    gl =
        QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_4_1_Core>(
            this->context());

    // Set the color to be used by glClear.
    // This is the background color.
    glClearColor(0.37f, 0.42f, 0.45f, 1.0f);

    // Set model translation
    modelTranslation.translate(-2, 0, -6);

    // Set the initial model transformation to
    // just the translation
    modelTransf = modelTranslation;

    /***********************************************************/
    /************************ IMPORTANT ************************/
    /***********************************************************/
    // Initialize the VAOs and VBOs of a pool of renderers.
    // This pool is used due to an unknown bug that on Windows systems assigns
    // used VAO memory addresses when initializing them outside of this function.
    // It is likely an issue with a stray openGL command messing something up,
    // but due to time constraints and the scope of the code that needs investigating
    // it is not yet solved. The pool, though imposing restrictions on the amount of envelopes,
    // was an acceptable compromise at the time.
    // Changing this will also require reworking the addNewEnvelope and removeEnvelope functions.
    indicesUsed.fill(false, settings.NUM_ENVELOPES);
    toolRenderers.reserve(settings.NUM_ENVELOPES);
    envelopeRenderers.reserve(settings.NUM_ENVELOPES);
    moveRenderers.reserve(settings.NUM_ENVELOPES);
    controlPointsRenderers.reserve(settings.NUM_ENVELOPES);
    envelopeControlPoints.reserve(settings.NUM_ENVELOPES);
    orientationCPsRenderers.reserve(settings.NUM_ENVELOPES);
    envelopeOrientationCPs.reserve(settings.NUM_ENVELOPES);
    envelopes.fill(nullptr, settings.NUM_ENVELOPES);
    cylinders.fill(nullptr, settings.NUM_ENVELOPES);
    drums.fill(nullptr, settings.NUM_ENVELOPES);
    bezierTools.fill(nullptr, settings.NUM_ENVELOPES);
    for (size_t i = 0; i < settings.NUM_ENVELOPES; i++) {
        ToolRenderer *toolRend = new ToolRenderer();
        toolRend->init(gl, &settings);
        toolRenderers.append(toolRend);

        EnvelopeRenderer *envRend = new EnvelopeRenderer();
        envRend->init(gl, &settings);
        envelopeRenderers.append(envRend);

        MoveRenderer *moveRend = new MoveRenderer();
        moveRend->init(gl, &settings);
        moveRenderers.append(moveRend);

        // Control points renderer
        ControlPointsRenderer *cpRend = new ControlPointsRenderer();
        cpRend->init(gl, &settings);
        controlPointsRenderers.append(cpRend);
        envelopeControlPoints.append(QVector<ControlPoint*>());

        // Orientation control points renderer
        OrientationCPsRenderer *orientationCPsRend = new OrientationCPsRenderer();
        orientationCPsRend->init(gl, &settings);
        orientationCPsRenderers.append(orientationCPsRend);
        envelopeOrientationCPs.append(QVector<ControlPoint*>());
    }

    // Trigger buffer update
    updateBuffers();
    updateToolTransf();
    updateAllUniforms = true;
}


/**
 * @brief MainView::updateBuffers gets the vertex array of the cylinder and updates the buffer
 * TODO: extend to update buffer of other cylinders and enveloping surfaces.
 */
void MainView::updateBuffers(){
    //qDebug() << "main update buffers";

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        toolRenderers[i]->updateBuffers();
        envelopeRenderers[i]->updateBuffers();
        moveRenderers[i]->updateBuffers();
        controlPointsRenderers[i]->updateBuffers();
        orientationCPsRenderers[i]->updateBuffers();
    }
}

void MainView::updateUniforms() {
    //qDebug() << "main update uniforms";

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        toolRenderers[i]->updateUniforms();
        moveRenderers[i]->updateUniforms();
        envelopeRenderers[i]->updateUniforms();
        controlPointsRenderers[i]->updateUniforms();
        orientationCPsRenderers[i]->updateUniforms();
    }
}

/**
 * @brief MainView::updateMotionAfterInteraction Updates the selcted envelope 
 * after every mouse interaction with the control or orientation points.
 * 
 */
void MainView::updateMotionAfterInteraction(){
    QVector<ControlPoint*>& cps = envelopeControlPoints[selectedEnvelope];
    QVector<ControlPoint*>& orientationcps = envelopeOrientationCPs[selectedEnvelope];
    if(orientationControlPointPressed) {
        // First orientation control point can change the height of the tool
        ControlPoint* controlPoint = cps[selectedOrientationControlPoint];
        float s1 = (orientationcps[0]->getPosition().x() - cps[0]->getPosition().x())*(orientationcps[0]->getPosition().x() - cps[0]->getPosition().x());
        float s2 = (orientationcps[0]->getPosition().y() - cps[0]->getPosition().y())*(orientationcps[0]->getPosition().y() - cps[0]->getPosition().y());
        float s3 = (orientationcps[0]->getPosition().z() - cps[0]->getPosition().z())*(orientationcps[0]->getPosition().z() - cps[0]->getPosition().z());
        float height = std::sqrt(s1 + s2 + s3);
        cylinders[selectedEnvelope]->setHeight(height);
        drums[selectedEnvelope]->setHeight(height);
        bezierTools[selectedEnvelope]->setHeight(height);
    }
    if(addNewBezierCurve) {
        // Add the new control points and orientation control points properly
        envelopes[selectedEnvelope]->getToolMovement().addNewBezierCurve(envelopes[selectedEnvelope]->getTool()->getHeight());
        envelopeControlPoints[selectedEnvelope] = envelopes[selectedEnvelope]->getToolMovement().getPath().getControlPoints();
        envelopeOrientationCPs[selectedEnvelope] = envelopes[selectedEnvelope]->getToolMovement().getOrientationPath().getControlPoints();
        controlPointsRenderers[selectedEnvelope]->setControlPoints(envelopeControlPoints[selectedEnvelope]);
        orientationCPsRenderers[selectedEnvelope]->setControlPoints(envelopeControlPoints[selectedEnvelope]);
        orientationCPsRenderers[selectedEnvelope]->setOrientationControlPoints(envelopeOrientationCPs[selectedEnvelope]);
        controlPointsRenderers[selectedEnvelope]->updateBuffers();
        orientationCPsRenderers[selectedEnvelope]->updateBuffers();
        cps = envelopeControlPoints[selectedEnvelope];
        orientationcps = envelopeOrientationCPs[selectedEnvelope];
    }
    // maintain the distance and direction between a control point and its orientation control point
    for(int i = 1; i<cps.size(); i++) {
        float height = envelopes[selectedEnvelope]->getTool()->getHeight();
        orientationcps[i]->setPosition(cps[i]->getPosition() + envelopes[selectedEnvelope]->getToolMovement().getAxisAtCp(i) * height);
    }
    orientationCPsRenderers[selectedEnvelope]->updateBuffers();
}

/**
 * @brief MainView::paintGL Actual function used for drawing to the screen.
 *
 */
void MainView::paintGL()
{
    // Clear the screen before rendering
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (updateAllUniforms) {
        // Reset the scale matrix
        modelScaling.setToIdentity();
        // Make the new scale matrix
        modelScaling.scale(scalingFactor);

        modelTransf = modelTranslation * modelScaling * modelRotation;
        normalTransf = modelTransf.normalMatrix();

        for (int i = 0; i < indicesUsed.size(); i++) {
            if (!indicesUsed[i]) continue;
            if (!envelopes[i]->isActive()) continue;
            toolRenderers[i]->setModelTransf(modelTransf);
            toolRenderers[i]->setNormalTransf(normalTransf);
            envelopeRenderers[i]->setModelTransf(modelTransf);
            envelopeRenderers[i]->setNormalTransf(normalTransf);
            moveRenderers[i]->setModelTransf(modelTransf);
            moveRenderers[i]->setNormalTransf(normalTransf);
            controlPointsRenderers[i]->setModelTransf(modelTransf);
            controlPointsRenderers[i]->setNormalTransf(normalTransf);
            orientationCPsRenderers[i]->setModelTransf(modelTransf);
            orientationCPsRenderers[i]->setNormalTransf(normalTransf);
        }
        updateToolTransf();
        updateUniforms();
        updateAllUniforms = false;
    }

    // topological sort of the envelopes so we update in the correct order
    QList topoSort = getTopoSortEnvelopes();

    while (!topoSort.isEmpty())
    {

        int idx = topoSort.takeFirst();

        if (envelopeMeshUpdates.contains(idx))
        {
            envelopes[idx]->update();
            envelopeRenderers[idx]->updateBuffers();
            moveRenderers[idx]->updateBuffers();
            controlPointsRenderers[idx]->updateBuffers();
            orientationCPsRenderers[idx]->updateBuffers();

            envelopeMeshUpdates.unite(envelopes[idx]->getAllDependents());

        }

        if (toolMeshUpdates.contains(idx))
        {
            cylinders[idx]->update();
            drums[idx]->update();
            bezierTools[idx]->update();
            toolRenderers[idx]->updateBuffers();

            toolMeshUpdates.unite(envelopes[idx]->getAllDependents());
        }

        if (toolTransfUpdates.contains(idx))
        {
            toolRenderers[idx]->setToolTransf(envelopes[idx]->getToolTransformAt(settings.t()));
            toolRenderers[idx]->updateUniforms();

            toolTransfUpdates.unite(envelopes[idx]->getAllDependents());

        }


    }
    envelopeMeshUpdates.clear();
    toolMeshUpdates.clear();
    toolTransfUpdates.clear();

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        if (!envelopes[i]->isActive()) continue;
        toolRenderers[i]->paintGL();
        moveRenderers[i]->paintGL();
        envelopeRenderers[i]->paintGL();
        if (!envelopes[i]->isPositContinuous()) {
            controlPointsRenderers[i]->paintGL();
            orientationCPsRenderers[i]->paintGL();
        }
    }

    // for measuring performance
    if (m_pendingTimerMeasurement) {
        m_pendingTimerMeasurement = false;
        qDebug() << "wall time to frame:" << m_timer.elapsed() << "ms";
    }

}

/**
 * @brief MainView::resizeGL Called upon resizing of the screen.
 *
 * @param newWidth The new width of the screen in pixels.
 * @param newHeight The new height of the screen in pixels.
 */
void MainView::resizeGL(int newWidth, int newHeight)
{
    //qDebug() << "MainView::resizeGL";
    // Get the aspect ratio of the new screen size
    float aspectRatio = newWidth / ((float)newHeight);

    viewportWidth = newWidth;
    viewportHeight = newHeight;

    // Set the viewport to the new size
    projTransf.setToIdentity();
    projTransf.perspective(60.0f, aspectRatio, 0.2f, 20.0f);

    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        toolRenderers[i]->setProjTransf(projTransf);
        envelopeRenderers[i]->setProjTransf(projTransf);
        moveRenderers[i]->setProjTransf(projTransf);
        controlPointsRenderers[i]->setProjTransf(projTransf);
        orientationCPsRenderers[i]->setProjTransf(projTransf);
    }
    updateAllUniforms = true;
}

/**
 * @brief MainView::setRotation Changes the rotation of the displayed objects.
 * @param rotateX Number of degrees to rotate around the x axis.
 * @param rotateY Number of degrees to rotate around the y axis.
 * @param rotateZ Number of degrees to rotate around the z axis.
 */
void MainView::setRotation(int rotateX, int rotateY, int rotateZ)
{
    //qDebug() << "Rotation changed to (" << rotateX << "," << rotateY << ","
    //         << rotateZ << ")";

    // Reset the rotation matrix
    modelRotation.setToIdentity();

    // Make the new rotation matrix
    modelRotation.rotate(rotateX, 1, 0, 0); // rotate around x axis
    modelRotation.rotate(rotateY, 0, 1, 0); // rotate around y axis
    modelRotation.rotate(rotateZ, 0, 0, 1); // rotate around z axis

    // Update the model transformation matrix
    updateAllUniforms = true;
}

/**
 * @brief MainView::setScale Changes the scale of the displayed objects.
 * @param scale The new scale factor. A scale factor of 1.0 should scale the
 * mesh to its original size.
 */
// TODO remove the update call here and move it to wherever in mainwindow this function is called
void MainView::setScale(float scale)
{
    //qDebug() << "Scale changed to " << scale;

    scalingFactor = scale;

    updateAllUniforms = true;
}

/**
 * @brief MainView::updateToolTransf Updates the tools transformation matrices.
 */
void MainView::updateToolTransf(){
    for (int i = 0; i < indicesUsed.size(); i++) {
        if (!indicesUsed[i]) continue;
        if (!envelopes[i]->isActive()) continue;
        toolRenderers[i]->setToolTransf(envelopes[i]->getToolTransformAt(settings.t()));
    }
}

QVector2D MainView::toScreenCoordinates(const QVector3D &Position) {
    QVector4D clipCoordinates = projTransf * modelTransf * QVector4D(Position, 1.0f);
    QVector3D normalizedDeviceCoord = clipCoordinates.toVector3D() / clipCoordinates.w();
    float screenX = normalizedDeviceCoord.x() * 0.5f * viewportWidth + 0.5f * viewportWidth;
    // Invert y coordinates because in the screen coordinates y goes down
    float screenY = viewportHeight - normalizedDeviceCoord.y() * 0.5f * viewportHeight - 0.5f* viewportHeight;
    return QVector2D(screenX, screenY);
}
QVector3D MainView::toWorldCoordinates(const QVector2D &screenPosition, float normalizedDeviceCoordZ) {
    float normalizedDeviceCoordX = (screenPosition.x() / viewportWidth) * 2.0f - 1.0f;
    float normalizedDeviceCoordY = 1.0f - (screenPosition.y() / viewportHeight) * 2.0f;
    QVector3D normalizedDeviceCoord = QVector3D(normalizedDeviceCoordX, normalizedDeviceCoordY, normalizedDeviceCoordZ);
    QVector4D worldCoordinates4D = modelTransf.inverted() * projTransf.inverted() * QVector4D(normalizedDeviceCoord, 1.0f);
    QVector3D worldCoordinates = worldCoordinates4D.toVector3D() / worldCoordinates4D.w();
    return worldCoordinates;
}

QList<int> MainView::getTopoSortEnvelopes() {
    QList<int> independentNodes;
    QList<int> updateQueue;

    //first add all independent nodes to the list.
    for (Envelope* env : envelopes)
    {
        if (env == nullptr) continue;
        if (env->isIndependent())
        {
            independentNodes.append(env->getIndex());
        }

    }

    while (!independentNodes.isEmpty())
    {
        int n = independentNodes.takeFirst();
        updateQueue.append(n);

        for (Envelope* depEnv : envelopes[n]->getDependentEnvelopes())
        {
            bool resolved = true;
            Envelope* a0adj = depEnv->getAdjA0Envelope();
            Envelope* a1adj = depEnv->getAdjA1Envelope();
            if (a0adj != nullptr && !updateQueue.contains(a0adj->getIndex()))
            {
                resolved = false;
            }
            if (a1adj != nullptr && !updateQueue.contains(a1adj->getIndex()))
            {
                resolved = false;
            }
            if (resolved)
            {
                independentNodes.append(depEnv->getIndex());
            }
        }
    }

    return updateQueue;
}

/**
 * @brief MainView::onMessageLogged OpenGL logging function, do not change.
 *
 * @param Message The message to be logged.
 */
void MainView::onMessageLogged(QOpenGLDebugMessage Message)
{
    qDebug() << " → Log:" << Message;
}
