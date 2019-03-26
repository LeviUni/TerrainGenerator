#include "mainview.h"
#include "math.h"
#include "noisegrid.h"

#include <QDateTime>

#include <vector>
#include <chrono>
#include <ctime>

#define PI 3.14159265359f

#define TEXTURE_LOCATION_DIFFUSE     0
#define TEXTURE_LOCATION_NORMAL      1
#define TEXTURE_LOCATION_SPECULAR    2
#define TEXTURE_LOCATION_DUDV        0
#define TEXTURE_LOCATION_REFLECTION  1
#define TEXTURE_LOCATION_REFRACTION  2
#define TEXTURE_LOCATION_DEPTHMAP    3

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
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
MainView::~MainView() {
    debugLogger->stopLogging();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    // start the timer
    timer.start(static_cast<int>(1000.0f / 60.0f));

    debugLogger = new QOpenGLDebugLogger();
    connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ),
             this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

    if ( debugLogger->initialize() ) {
        qDebug() << ":: Logging initialized";
        debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
        debugLogger->enableMessages();
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable clipping
    glEnable(GL_CLIP_DISTANCE0);

    // Set the color of the screen on clear (new frame)
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();
    createFramebuffers();
    createModels();

    // initialize the matrices for rotation and scale
    setRotation(354, 0);
    setScale(30);
    t = 0;
}

void MainView::createShaderProgram() {
    // Create shader programs
    terrainShaderProgram = ShaderProgramPtr(new ShaderProgram(":/shaders/vertshader_terrain.glsl", ":/shaders/fragshader_terrain.glsl"));
    waterShaderProgram = ShaderProgramPtr(new ShaderProgram(":/shaders/vertshader_water.glsl", ":/shaders/fragshader_water.glsl"));

    // upload runtime-constant uniforms
    terrainShaderProgram->setUniform("diffuseTexture", std::vector<GLint>{
            TEXTURE_LOCATION_DIFFUSE, TEXTURE_LOCATION_DIFFUSE + 3, TEXTURE_LOCATION_DIFFUSE + 6 });
    terrainShaderProgram->setUniform("normalTexture", std::vector<GLint>{
            TEXTURE_LOCATION_NORMAL, TEXTURE_LOCATION_NORMAL + 3, TEXTURE_LOCATION_NORMAL + 6 });
    terrainShaderProgram->setUniform("specularTexture", std::vector<GLint>{
            TEXTURE_LOCATION_SPECULAR, TEXTURE_LOCATION_SPECULAR + 3, TEXTURE_LOCATION_SPECULAR + 6 });

    waterShaderProgram->setUniform("dudvMap", TEXTURE_LOCATION_DUDV);
    waterShaderProgram->setUniform("reflectionTexture", TEXTURE_LOCATION_REFLECTION);
    waterShaderProgram->setUniform("refractionTexture", TEXTURE_LOCATION_REFRACTION);
    waterShaderProgram->setUniform("depthMap", TEXTURE_LOCATION_DEPTHMAP);
}

void MainView::createModels() {

    // grass material
    grassMaterial = MaterialPtr(new Material(0.1f, 0.9f, 1.0f, 3));
    grassMaterial->addTexture(TEXTURE_LOCATION_DIFFUSE,  ":/textures/grass_diff.png");
    grassMaterial->addTexture(TEXTURE_LOCATION_NORMAL,   ":/textures/grass_norm.png");
    grassMaterial->addTexture(TEXTURE_LOCATION_SPECULAR, ":/textures/grass_spec.png");

    // rock material
    rockMaterial = MaterialPtr(new Material(0.1f, 0.9f, 1.0f, 12));
    rockMaterial->addTexture(TEXTURE_LOCATION_DIFFUSE,  ":/textures/rock_diff.png");
    rockMaterial->addTexture(TEXTURE_LOCATION_NORMAL,   ":/textures/rock_norm.png");
    rockMaterial->addTexture(TEXTURE_LOCATION_SPECULAR, ":/textures/rock_spec.png");

    // sand material
    sandMaterial = MaterialPtr(new Material(0.1f, 0.9f, 1.0f, 8));
    sandMaterial->addTexture(TEXTURE_LOCATION_DIFFUSE,  ":/textures/sand_diff.png");
    sandMaterial->addTexture(TEXTURE_LOCATION_NORMAL,   ":/textures/sand_norm.png");
    sandMaterial->addTexture(TEXTURE_LOCATION_SPECULAR, ":/textures/sand_spec.png");

    regenerateTerrain();

    // water model
    NoiseGrid waterGrid(1);
    ModelDataPtr waterModel = waterGrid.createModelData();

    // water material
    MaterialPtr waterMaterial(new Material(0.1f, 0.9f, 0.58f, 91));
    waterMaterial->addTexture(TEXTURE_LOCATION_DUDV, ":/textures/water_dudv.png");
    waterMaterial->addTexture(TEXTURE_LOCATION_REFLECTION, reflectionBuffer->getTextures()[0]->id());
    waterMaterial->addTexture(TEXTURE_LOCATION_REFRACTION, refractionBuffer->getTextures()[0]->id());
    waterMaterial->addTexture(TEXTURE_LOCATION_DEPTHMAP, refractionBuffer->getTextures()[1]->id());
    waterMaterial->setCustomShader(waterShaderProgram);

    // creating the water object
    ObjectPtr water(new Object(waterModel, waterMaterial));
    water->setScale({1000, 1, 1000});
    objects["water"] = water;

    // update the water height
    waterHeight = -2.0f;
    water->setTranslation({0, waterHeight, 0});
    terrainShaderProgram->setUniform("waterHeight", waterHeight);
}

void MainView::createFramebuffers() {
    // create the reflection buffer
    reflectionBuffer = FramebufferPtr(new Framebuffer(width, height));
    reflectionBuffer->addTexture(GL_RGB8, GL_RGB, GL_FLOAT);
    reflectionBuffer->addRenderbuffer(GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
    reflectionBuffer->create();

    // create the refraction buffer
    refractionBuffer = FramebufferPtr(new Framebuffer(width, height));
    refractionBuffer->addTexture(GL_RGB8, GL_RGB, GL_FLOAT);
    refractionBuffer->addTexture(GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_ATTACHMENT);
    refractionBuffer->create();
}

void MainView::updateProjectionMatrix() {
    float fov = 60;

    // Change the projection matrix
    projMatrix.setToIdentity();
    projMatrix.perspective(fov, float(width) / float(height), nearPlane, farPlane);

    // Upload the projection matrix to the current shader
    terrainShaderProgram->setUniform("projMatrix", projMatrix);
    waterShaderProgram->setUniform("projMatrix", projMatrix);
    waterShaderProgram->setUniform("near", nearPlane);
    waterShaderProgram->setUniform("far", farPlane);

    update();
}

void MainView::updateViewMatrix() {
    // calculate the actual view scale;
    float minscale = 0.002f;
    float maxscale = 0.05f;
    float a = (maxscale - minscale) / 1.99f;
    float b = minscale - (maxscale - minscale) / 199.0f;
    viewScale = a * scale + b;

    // Change the view matrix
    viewMatrix.setToIdentity();
    viewMatrix.translate(0, 0, -1);
    viewMatrix.scale(viewScale);
    viewMatrix.rotate(rotation.x(), { -1, 0, 0 });
    viewMatrix.rotate(rotation.y(), {  0, 1, 0 });

    // Change the camera position
    QVector4D pos(0, 0, 0, 1);
    pos = viewMatrix.inverted() * pos;
    cameraPosition = QVector3D(pos.x(), pos.y(), pos.z());

    // Change the reflected view matrix
    reflectedViewMatrix.setToIdentity();
    reflectedViewMatrix.translate(0, 0, -1);
    reflectedViewMatrix.scale(viewScale);
    reflectedViewMatrix.rotate(rotation.x(), { 1, 0, 0 });
    reflectedViewMatrix.rotate(rotation.y(), { 0, 1, 0 });
    reflectedViewMatrix.translate(0, -2 * waterHeight, 0);

    // Change the reflected camera position
    reflectedCameraPosition = QVector3D(pos.x(), -pos.y() + 2 * waterHeight, pos.z());

    update();
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // First: perform the animation
    animate();

    // if the terrain should be regenerated, do that here
    if (shouldRegenerate) {
        regenerateTerrain();
        shouldRegenerate = false;
    }

    // set the light
    float lightPeriod = 15.0f;
    float cosl = cosf(t / lightPeriod);
    float sinl = sinf(t / lightPeriod);
    QVector3D lightPosition(10000.0f * cosl, 10000.0f * sinl, -10000.0f);
    QVector3D lightColor(1.0f, 1.0f - 0.5f * powf(cosl, 20), 1.0f - powf(cosl, 20));

    // change the 'sky' color
    QVector4D skyColor(0.2f, 0.5f, 0.7f, 1.0f);
    skyColor /= 1 + expf(-20 * sinl);

    glClearColor(skyColor.x(), skyColor.y(), skyColor.z(), skyColor.w());

    // set frame-constant uniforms
    terrainShaderProgram->setUniform("lightPosition", lightPosition);
    terrainShaderProgram->setUniform("lightColor", lightColor);

    waterShaderProgram->setUniform("lightPosition", lightPosition);
    waterShaderProgram->setUniform("lightColor", lightColor);
    waterShaderProgram->setUniform("time", t);
    waterShaderProgram->setUniform("scale", viewScale);

    //----------------------------------//
    //  FIRST PASS: reflection texture  //
    //----------------------------------//

    // bind the framebuffer
    reflectionBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set uniforms
    terrainShaderProgram->setUniform("clipSign", 1);
    terrainShaderProgram->setUniform("cameraPosition", reflectedCameraPosition);
    terrainShaderProgram->setUniform("viewMatrix", reflectedViewMatrix);

    // draw objects to the texture
    for (const auto& p : objects) {
        // don't render the water
        if (p.first == "water") {
            continue;
        }

        // render the objects
        paintObject(p.second);
    }


    //-------------------------------------//
    //  SECOND PASS: 'refraction' texture  //
    //-------------------------------------//

    // bind the framebuffer
    refractionBuffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set uniforms
    terrainShaderProgram->setUniform("clipSign", -1);
    terrainShaderProgram->setUniform("cameraPosition", cameraPosition);
    terrainShaderProgram->setUniform("viewMatrix", viewMatrix);

    // draw objects to the texture
    for (const auto& p : objects) {
        // don't render the water
        if (p.first == "water") {
            continue;
        }

        // render the objects
        paintObject(p.second);
    }

    //----------------------------------//
    //  THIRD PASS: screen framebuffer  //
    //----------------------------------//

    // bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set uniforms
    terrainShaderProgram->setUniform("clipSign", 0);
    terrainShaderProgram->setUniform("cameraPosition", cameraPosition);
    terrainShaderProgram->setUniform("viewMatrix", viewMatrix);
    waterShaderProgram->setUniform("cameraPosition", cameraPosition);
    waterShaderProgram->setUniform("viewMatrix", viewMatrix);

    // draw all objects
    for (const auto& p : objects) {
        // render the objects
        paintObject(p.second);
    }
}

void MainView::paintObject(const ObjectPtr &object) {
    // get the correct shader
    ShaderProgramPtr shader = terrainShaderProgram;

    if (!object->getMaterials().empty()) {
        ShaderProgramPtr materialShader = object->getMaterials()[0]->getCustomShader();

        if (materialShader != nullptr) {
            shader = materialShader;
        }
    }

    shader->bind();

    // update the (normal) model matrix
    QMatrix4x4 modelMatrix = object->getModelMatrix();
    shader->setUniform("modelMatrix", modelMatrix);
    shader->setUniform("normalModelMatrix", modelMatrix.normalMatrix());

    // update the material
    std::vector<QVector4D> materials;
    GLuint startSlot = 0;
    for (const auto& material : object->getMaterials()) {
        material->bindTextures(startSlot);
        materials.push_back(material->getMaterialVector());
        startSlot += 3;
    }
    shader->setUniform("material", materials);

    // draw the model
    object->getModel()->draw();
}

/**
 * @brief MainView::animate
 *
 * The function used to animate the objects
 */
void MainView::animate() {
    t += 1.0f / 60.0f;
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;

    reflectionBuffer = reflectionBuffer->getResizedCopy(newWidth, newHeight);
    refractionBuffer = refractionBuffer->getResizedCopy(newWidth, newHeight);

    // resizing framebuffers changes their textures, so we have to update the textures in
    // the material of the water object too.
    MaterialPtr waterMaterial = objects["water"]->getMaterials()[0];
    waterMaterial->addTexture(TEXTURE_LOCATION_REFLECTION, reflectionBuffer->getTextures()[0]->id());
    waterMaterial->addTexture(TEXTURE_LOCATION_REFRACTION, refractionBuffer->getTextures()[0]->id());
    waterMaterial->addTexture(TEXTURE_LOCATION_DEPTHMAP, refractionBuffer->getTextures()[1]->id());

    updateProjectionMatrix();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY)
{
    rotation = QVector2D(rotateX, rotateY);

    updateViewMatrix();
}

void MainView::setScale(int s)
{
    scale = s / 100.0f;

    updateViewMatrix();
}

void MainView::regenerate() {
    shouldRegenerate = true;
}

void MainView::regenerateTerrain() {
    // terrain model
    NoiseGrid terrainGrid(9);
    terrainGrid.addSpike(-2, 1, 3);
    terrainGrid.addOctaves(5, 1.0f, 4);
    ModelDataPtr terrainModel = terrainGrid.createModelData();

    // creating the terrain object
    ObjectPtr terrain(new Object(terrainModel, { grassMaterial, rockMaterial, sandMaterial }));
    terrain->setScale({1, 8.5f, 1});
    terrain->setTranslation({0, -1.0f, 0});
    objects["terrain"] = terrain;
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
