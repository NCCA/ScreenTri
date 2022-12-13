#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include <ngl/Text.h>
#include <ngl/Vec3.h>
#include <ngl/Vec4.h>
#include <ngl/Mat4.h>
#include "WindowParams.h"
#include <QOpenGLWindow>

/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file


class NGLScene : public QOpenGLWindow
{
  Q_OBJECT
public:
  
  /// @brief ctor for our NGL drawing class
  /// @param [in] parent the parent window to the class
  
  NGLScene();
  
  /// @brief dtor must close down ngl and release OpenGL resources
  
  ~NGLScene() override;
  
  /// @brief the initialize class is called once when the window is created and we have a valid GL context
  /// use this to setup any default GL stuff
  
  void initializeGL() override;
  
  /// @brief this is called everytime we want to draw the scene
  
  void paintGL() override;
  
  /// @brief this is called everytime we want to draw the scene
  
  void resizeGL(int _w, int _h) override;

private:
  
  /// @brief the windows params such as mouse and rotations etc
  
  WinParams m_win;
  
  
  
  /// @brief Qt Event called when a key is pressed
  /// @param [in] _event the Qt event to query for size etc  
  void keyPressEvent(QKeyEvent *_event) override;
  uint32_t encodePixel(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a=255) const noexcept;
  void clearBuffer();
  void updateTextureBuffer();
  void setPixel(int _x, int y,unsigned char _r, unsigned char _g, unsigned char _b)noexcept;
  void drawLine(int _x0, int y0, int _x1, int y2,unsigned char _r, unsigned char _g, unsigned char _b)noexcept;
  void randomPixels();
  void randomLines();
  enum class DrawMode{Pixels,Lines};
  DrawMode m_drawmode=DrawMode::Pixels;
  GLuint m_vao=0;
  GLuint m_textureID;
  bool m_animate=true;
  std::unique_ptr<uint32_t []> m_buffer;
  void timerEvent(QTimerEvent *) override;

};


#endif
