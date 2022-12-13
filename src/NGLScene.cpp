#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/NGLStream.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Util.h>
#include <ngl/Random.h>
#include <QGuiApplication>
#include <QMouseEvent>

constexpr size_t TextureWidth=1024;
constexpr size_t TextureHeight=720;

NGLScene::NGLScene()
{
  setTitle("Render to Screen Triangle");
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(int _w, int _h)
{

  m_win.width = static_cast<int>(_w * devicePixelRatio());
  m_win.height = static_cast<int>(_h * devicePixelRatio());
}
constexpr auto ScreenTri = "ScreenTri";

void NGLScene::initializeGL()
{
  ngl::NGLInit::initialize();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  ngl::ShaderLib::loadShader(ScreenTri,"shaders/ScreenTriVertex.glsl","shaders/ScreenTriFragment.glsl");
  // Need a vertex array to call draw arrays
  // this will have no buffers
  glGenVertexArrays(1,&m_vao);
  // Now generate a texture
  glGenTextures(1, &m_textureID);
  // Generate our buffer for the texture data
  m_buffer= std::make_unique<uint32_t []>(TextureWidth*TextureHeight);
  clearBuffer();
  updateTextureBuffer();
  // create random generator for image size
  auto width=std::uniform_int_distribution<>(0,TextureWidth-1);
  ngl::Random::addIntGenerator("width",width);
  auto height=std::uniform_int_distribution<>(0,TextureHeight-1);
  ngl::Random::addIntGenerator("height",height);

  auto colour=std::uniform_int_distribution<>(0,255);
  ngl::Random::addIntGenerator("colour",colour);
  startTimer(10);
  
}

uint32_t NGLScene::encodePixel(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a) const noexcept
{
  return (_a << 24) + (_b << 16) + (_g << 8) + _r;
}
void NGLScene::clearBuffer()
{
  // clear screen to white  
  for(size_t i=0; i<TextureWidth*TextureHeight; ++i)
  {
    m_buffer[i]=encodePixel(255,255,255);
  }
}

void NGLScene::setPixel(int _x, int _y,unsigned char _r, unsigned char _g, unsigned char _b) noexcept
{
  // if(_x>TextureWidth || _y >TextureHeight )
  //   return;
  // easier as we are only using square at  present
  size_t offset=(_y*TextureWidth)+_x;
  auto pixel=encodePixel(_r,_g,_b);
  m_buffer[offset]=pixel;
}

void NGLScene::drawLine(int _x0, int _y0, int _x1, int _y1,unsigned char _r, unsigned char _g, unsigned char _b) noexcept
{
 auto dx = std::abs(_x1 - _x0);
 auto dy = std::abs(_y1 - _y0);
 auto sx = (_x0<_x1) ? 1 : -1;
 auto sy = (_y0<_y1) ? 1 : -1;
 auto err = dx - dy;
 while(true)
 {
    setPixel(_x0, _y0, _r, _g, _b);
    if ((_x0 == _x1) && (_y0 == _y1))
        break;
    auto e2 = 2 * err;
    if (e2 > -dy)
    {
        err -= dy;
        _x0 += sx;
    }
    if (e2 < dx)
    {
        err += dx;
        _y0 += sy;
    }
  }    
}

void NGLScene::updateTextureBuffer()
{
  glBindTexture(GL_TEXTURE_2D, m_textureID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureWidth, TextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_buffer.get());
  glGenerateMipmap(GL_TEXTURE_2D); //  Allocate the mipmaps
}

void NGLScene::paintGL()
{
  glViewport(0, 0, m_win.width, m_win.height);
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // grab an instance of the shader manager
  ngl::ShaderLib::use(ScreenTri);
  // Draw screen Tri with bound Texture
  glBindVertexArray(m_vao);
  glBindTexture(GL_TEXTURE_2D,m_textureID);
  glDrawArrays(GL_TRIANGLES,0,3);
  glBindVertexArray(0);

}

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // that method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quit
  case Qt::Key_Escape:
    QGuiApplication::exit(EXIT_SUCCESS);
    break;
// turn on wirframe rendering
  case Qt::Key_W:
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  // turn off wire frame
  case Qt::Key_S:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case Qt::Key_P :
    m_drawmode=DrawMode::Pixels;
  break;
  case Qt::Key_L :
    m_drawmode=DrawMode::Lines;
    break;
  case Qt::Key_Space :
    clearBuffer();
  break;
  // show full screen
  case Qt::Key_F:
    showFullScreen();
    break;
  // show windowed
  case Qt::Key_N:
    showNormal();
    break;
  case Qt::Key_A :
    m_animate^=true;
    break;
  default:
    break;
  }
  update();
}

void NGLScene::randomPixels()
{
  auto r=ngl::Random::getIntFromGeneratorName("colour");
  auto g=ngl::Random::getIntFromGeneratorName("colour");
  auto b=ngl::Random::getIntFromGeneratorName("colour");

  for(int i=0; i<1000; ++i)
  {
    auto x=ngl::Random::getIntFromGeneratorName("width");
    auto y=ngl::Random::getIntFromGeneratorName("height");
    setPixel(x,y,r,g,b);
  }
    updateTextureBuffer();
}

void NGLScene::randomLines()
{
  auto loopEnd=ngl::Random::getIntFromGeneratorName("image_offset")/10;
  
  clearBuffer();
  for(int i=0; i<1000; ++i)
  {
  auto r=ngl::Random::getIntFromGeneratorName("colour");
  auto g=ngl::Random::getIntFromGeneratorName("colour");
  auto b=ngl::Random::getIntFromGeneratorName("colour");  auto x0=ngl::Random::getIntFromGeneratorName("width");
    auto y0=ngl::Random::getIntFromGeneratorName("height");
    auto x1=ngl::Random::getIntFromGeneratorName("width");
    auto y1=ngl::Random::getIntFromGeneratorName("height");
    drawLine(x0,y0,x1,y1,r,g,b);
  }
    updateTextureBuffer();
}




void NGLScene::timerEvent(QTimerEvent *)
{
  if(!m_animate) return;
  if(m_drawmode==DrawMode::Pixels)
    randomPixels();
  else 
    randomLines();
update();
}