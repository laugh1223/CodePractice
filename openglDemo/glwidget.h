#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QKeyEvent>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>

class glWidget : public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit glWidget(QWidget *parent = nullptr);
    ~glWidget();
    void draw(bool bDraw);
    void setWireframe(bool bwireframe);

signals:

protected slots:


private:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
    void initData(); //初始化vao、vbo、ebo等
    void initShaderProgram();//编译链接着色器
    void initTexture();//初始化纹理
    void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    bool m_bdraw = false;
    unsigned int m_vao, m_vbo, m_ebo;
    QOpenGLShaderProgram m_shaderProgram;
    QOpenGLTexture *m_texture01, *m_texture02;
    QTimer *m_timer;
    QTime m_time;
    QVector3D cameraPos, cameraFront, cameraUp;
    float cameraSpeed = 0.5f;
    float fov = 45.0f;

};

#endif // GLWIDGET_H
