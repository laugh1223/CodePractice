#include "glwidget.h"
#include "vertexData.h"
#include <QImage>
#include <math.h>

static float g_ratio = 0.5;
#define PI 3.1415926
glWidget::glWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    setFocusPolicy(Qt::StrongFocus);
//    m_timer = new QTimer(this);
//    //启动定时器
//    connect(m_timer,&QTimer::timeout,this,[this](){
//        update();
//    });
//    m_timer->start(100);
//    m_time.start();

    cameraPos = QVector3D(0.0f, 0.0f, 3.0f);//摄像机的位置
    cameraFront =  QVector3D(0.0f, 0.0f, -1.0f);//摄像机看向的中心
    cameraUp = QVector3D(0.0f, 1.0f, 0.0f);//世界坐标的y轴
}

glWidget::~glWidget()
{
    makeCurrent();
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
    doneCurrent();
}

void glWidget::draw(bool bDraw)
{
    m_bdraw = bDraw;
    update();
}

void glWidget::setWireframe(bool bwireframe)
{
    makeCurrent();
    if(bwireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    update();
    doneCurrent();
}

void glWidget::initializeGL()
{
    //绑定opengl接口
    initializeOpenGLFunctions();
    initShaderProgram();
    initTexture();
    initData();
}

void glWidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);Q_UNUSED(h);
}

void glWidget::paintGL()
{
    glClearColor(0.2f, 0.5f, 0.5f, 1.0f);    //设置颜色
    glEnable(GL_DEPTH_TEST);//打开深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    if(m_bdraw){

        //使用着色器
        m_shaderProgram.bind();
        m_shaderProgram.setUniformValue("texture0", 0);
        m_shaderProgram.setUniformValue("texture1", 1);

//        QTime timeNow = QTime::currentTime();
//        GLfloat ratio = sin(timeNow.msec());
        m_shaderProgram.setUniformValue("ratio", g_ratio);

        {
            //旋转、缩放、平移矩阵的使用
            //            QMatrix4x4 matrix;
            //            matrix.scale(0.5f, 0.5f);
            //            matrix.translate(1.0f, 1.0f, 0.0f);
            //            matrix.rotate(QTime::currentTime().msec(), 0.0f, 0.0f, 1.0f);
//            QMatrix4x4 model;//模型矩阵
//            model.rotate(QTime::currentTime().msec(), 1.0f, 0.3f, 0.5f);
//            m_shaderProgram.setUniformValue("model", model);

            QMatrix4x4 view;//观察矩阵 即摄像机
//            float time = m_time.elapsed() / 1000;
//            float camX = sin(time) * 10.0f;
//            float camZ = cos(time) * 10.0f;
//            view.translate(0.0f, 0.0f, -6.0f);
//            view.lookAt(QVector3D(camX, 0.0f, camZ), cameraTarget, up);
            view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
            m_shaderProgram.setUniformValue("view", view);

            QMatrix4x4 projection;//投影矩阵
            projection.perspective(fov, (float)width()/height(), 0.1f, 100.0f);
            m_shaderProgram.setUniformValue("projection", projection);
        }

        m_texture01->bind(0);
        m_texture02->bind(1);
        //绑定vao
        glBindVertexArray(m_vao);
        //绑定ebo
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        //开始绘制
        foreach(auto item ,cubePositions){
            QMatrix4x4 model;//模型矩阵
            model.translate(item);
//            model.rotate(QTime::currentTime().msec(), 1.0f, 0.3f, 0.5f);
            m_shaderProgram.setUniformValue("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//    }
}

void glWidget::initData()
{
    //创建vbo，绑定vbo，将数据放到vbo的缓存中
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //创建vao，绑定vao
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    //设置数据属性
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);//解除vbo的绑定，必须数据属性设置完成后才能解除vbo的绑定
    glBindVertexArray(0);//解除vao绑定

    //创建ebo,将数据放到ebo的缓存中
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);//解除ebo的绑定；重新使用的时候要重新绑定
}

void glWidget::initShaderProgram()
{
    //编译链接着色器代码
    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaderProgram/test.vs");
    m_shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaderProgram/test.fs");
    m_shaderProgram.link();
}

void glWidget::initTexture()
{
    //启用纹理的透明度
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //加载纹理
    m_texture01 = new QOpenGLTexture(QImage(":/textures/awesomeface.png").mirrored());

    {
//    Repeat         = 0x2901, // GL_REPEAT
//    MirroredRepeat = 0x8370, // GL_MIRRORED_REPEAT
//    ClampToEdge    = 0x812F, // GL_CLAMP_TO_EDGE
//    ClampToBorder  = 0x812D  // GL_CLAMP_TO_BORDER
//    m_texture->setWrapMode(QOpenGLTexture::ClampToBorder); //设置纹理的环绕方式

//    DirectionS = 0x2802, // GL_TEXTURE_WRAP_S 即x轴
//    DirectionT = 0x2803, // GL_TEXTURE_WRAP_T 即y轴
//    DirectionR = 0x8072  // GL_TEXTURE_WRAP_R 即z轴
//    m_texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToBorder);//设置不同轴的纹理的环绕方式
//    m_texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::Repeat);//设置不同轴的纹理的环绕方式

//        Nearest                 = 0x2600,   // GL_NEAREST
//        Linear                  = 0x2601,   // GL_LINEAR
//        NearestMipMapNearest    = 0x2700,   // GL_NEAREST_MIPMAP_NEAREST
//        NearestMipMapLinear     = 0x2702,   // GL_NEAREST_MIPMAP_LINEAR
//        LinearMipMapNearest     = 0x2701,   // GL_LINEAR_MIPMAP_NEAREST
//        LinearMipMapLinear      = 0x2703    // GL_LINEAR_MIPMAP_LINEAR
    }

    m_texture01->setMinMagFilters(QOpenGLTexture::Nearest, QOpenGLTexture::Nearest);//设置纹理过滤方式
    m_texture01->setBorderColor(0.5f,0.5f,0.5f,1.0f);

    m_texture02 = new QOpenGLTexture(QImage(":/textures/container.jpg").mirrored());
}

void glWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_Up:
        if(g_ratio >= 1) g_ratio = 1;
        else g_ratio += 0.2f;
        break;
    case Qt::Key_Down:
        if(g_ratio <= 0) g_ratio = 0;
        else g_ratio -= 0.2f;
        break;
    case Qt::Key_W:cameraPos += cameraSpeed * cameraFront;break;
    case Qt::Key_S:cameraPos -= cameraSpeed * cameraFront;break;
    case Qt::Key_A:cameraPos -= QVector3D::crossProduct(cameraFront, cameraUp).normalized() * cameraSpeed;break;
    case Qt::Key_D:cameraPos += QVector3D::crossProduct(cameraFront, cameraUp).normalized() * cameraSpeed;break;
    default:break;
    }
    update();//重新绘制画面
    //    qDebug() << ">>>> " << g_ratio;
}

void glWidget::mouseMoveEvent(QMouseEvent *event)
{
    //设置相机的俯仰角和偏航角
    float yaw = -90.0f;
    float pitch = 0.0f;
    QPoint lastPos(width()/2, height()/2);
    auto currentPos = event->pos();
    auto deltaPos = currentPos - lastPos;
//    lastPos = currentPos;
    float sensitivity = 0.1f;
    deltaPos *= sensitivity;
    yaw += deltaPos.x();
    pitch -= deltaPos.y();
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;
    cameraFront.setX(cos(yaw * PI /180)*cos(pitch*PI/180));
    cameraFront.setY(sin(pitch*PI/180));
    cameraFront.setZ(sin(yaw*PI/180)*cos(pitch*PI/180));
    cameraFront.normalize();
    update();
}

void glWidget::wheelEvent(QWheelEvent *event)
{
    if(fov >= 1.0f && fov <= 75.0f)
        fov -= event->angleDelta().y()/120;
    if(fov <= 1.0f) fov = 1.0f;
    if(fov >= 75.0f) fov = 75.0f;
    update();
}
