#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector3D>

#include "vertex.h"
#include "tools/cylinder.h"
#include "tools/bezierTool.h"
#include "movement/cylindermovement.h"
#include "movement/BezierPath.h"
#include "envelope.h"
#include "settings.h"
#include "renderers/controlpointsrenderer.h"
#include "renderers/orientationcpsrenderer.h"
#include "renderers/toolrenderer.h"
#include "renderers/enveloperenderer.h"
#include "renderers/moverenderer.h"


/**
 * @brief The MainView class is resonsible for the actual content of the main
 * window.
 */
class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
    friend class MainWindow;

    // Administration
    QVector<bool> indicesUsed;
    // NOTE: We do not delete any of pointers in the arrays below to avoid creating and deleting objectsv all the time.
    // If the pool needs to increase, we do so, and keep track in the indicesUsed array for which objects are in use.
    // There are likely optimizations possible to shrink memory usage when possible, but not for now.

    QSet<int> envelopeMeshUpdates;
    QSet<int> toolMeshUpdates;
    QSet<int> toolTransfUpdates;
    bool updateAllUniforms;

    // Tool rendering
    ToolType defaultTool = ToolType::Tool_Cylinder;

    QVector<ToolRenderer*> toolRenderers;
    QVector<Drum*> drums;
    QVector<Cylinder*> cylinders;
    QVector<BezierTool*> bezierTools;

    // Path rendering
    QVector<MoveRenderer*> moveRenderers;

    // Envelope rendering
    QVector<Envelope*> envelopes;
    QVector<EnvelopeRenderer*> envelopeRenderers;

    // Control points rendering
    QVector<ControlPointsRenderer*> controlPointsRenderers;
    QVector<QVector<ControlPoint*>> envelopeControlPoints;

    // Orientation control points rendering
    QVector<QVector<ControlPoint*>> envelopeOrientationCPs;
    QVector<OrientationCPsRenderer*> orientationCPsRenderers;

    // Transformation matrices for the model
    float scalingFactor = 1.0f;
    QMatrix4x4 modelScaling;
    QMatrix4x4 modelRotation;
    QMatrix4x4 modelTranslation;
    QMatrix4x4 modelTransf;

    // Transformation matrix for the normal
    QMatrix3x3 normalTransf;

    // View matrix
    QVector3D cameraPosition;
    QQuaternion cameraRotation;
    QMatrix4x4 viewTransf;

    // Transformation matrix for the projection
    QMatrix4x4 projTransf;

    // Mouse input for rotating and panning
    bool isSingleDragging = false;
    bool isDoubleDragging = false;
    QVector2D lastMousePos;

    // Mouse input for control points
    int selectedControlPoint = -1;
    int selectedOrientationControlPoint = -1;
    int selectedEnvelope = -1;
    bool controlPointPressed = false;
    bool orientationControlPointPressed = false;
    bool addNewBezierCurve = false;
    // Orientation direction at each control point of the needed envelope;
    QVector<QVector3D> directioncps;

    int viewportWidth = 1;
    int viewportHeight = 1;



public:
    MainView(QWidget *parent = nullptr);
    ~MainView() override;

    //performance measuring
    bool m_pendingTimerMeasurement = false;
    QElapsedTimer m_timer;

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setScale(float scale);
    void updateToolTransf(); //TODO remove need for this, and use the indicesUsed array to set toolTransfUpdates
    void updateBuffers();

    Envelope *addNewEnvelope();
    void deleteEnvelope(Envelope *env);

protected:
    void initializeGL() override;
    void updateUniforms();
    void updateMotionAfterInteraction();
    void resizeGL(int newWidth, int newHeight) override;
    void paintGL() override;
    QVector2D toScreenCoordinates(const QVector3D &worldPos);
    QVector3D toWorldCoordinates(const QVector2D &screenPos, float ndcZ);

    QList<int> getTopoSortEnvelopes();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void wheelEvent(QWheelEvent *ev) override;

private slots:
    void onMessageLogged(QOpenGLDebugMessage Message);

private:
    QOpenGLDebugLogger debugLogger;
    QTimer timer; // timer used for animation

    QOpenGLFunctions_4_1_Core *gl;
    Settings settings;
};

#endif // MAINVIEW_H
