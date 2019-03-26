#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "object.h"
#include "shaderprogram.h"
#include "framebuffer.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLDebugLogger>
#include <QTimer>
#include <QVector3D>
#include <memory>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger *debugLogger;
    QTimer timer; // timer used for animation

    ShaderProgramPtr terrainShaderProgram;
    ShaderProgramPtr waterShaderProgram;
    ShaderProgramPtr screenQuadShader;

    MaterialPtr grassMaterial;
    MaterialPtr rockMaterial;
    MaterialPtr sandMaterial;

public:
    MainView(QWidget *parent = nullptr);
    ~MainView();

    // Functions for widget input events
    void setRotation(int rotateX, int rotateY);
    void setScale(int scale);
    void regenerate();

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();
    void animate();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();
    void createModels();
    void createFramebuffers();
    void updateProjectionMatrix();
    void updateViewMatrix();
    void paintObject(const ObjectPtr& object);
    void regenerateTerrain();

    std::map<std::string, ObjectPtr> objects;

    GLsizei width, height;
    QVector2D rotation;
    float scale;
    float viewScale;

    QMatrix4x4 projMatrix;
    QMatrix4x4 viewMatrix;
    QVector3D cameraPosition;

    QMatrix4x4 reflectedViewMatrix;
    QVector3D reflectedCameraPosition;

    float t;
    float waterHeight;
    bool shouldRegenerate;

    FramebufferPtr reflectionBuffer;
    FramebufferPtr refractionBuffer;

    static constexpr GLfloat nearPlane = 0.1f;
    static constexpr GLfloat farPlane = 100.0f;

};

#endif // MAINVIEW_H
