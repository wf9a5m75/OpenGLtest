#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define LOG_TAG "glsurface"
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern void nativeOnSurfaceCreated(JNIEnv *env, jobject thiz, jintArray pixels, jint width, jint height);
extern void nativeChangeTexture(JNIEnv *env, jobject thiz, jintArray pixels, jint width, jint height);
extern void nativeOnSurfaceChanged(JNIEnv *env, jobject thiz, jint width, jint height);
extern void nativeOnDrawFrame(JNIEnv *env, jobject thiz);

static GLuint program;
static GLuint position;
static GLuint texcoord;
static GLuint textures[1];

/**
 * 頂点データです。
 */
static const GLfloat VERTEXS[] = {
	-1.0f,  1.0f, 0.0f,	// 左上
	-1.0f, -1.0f, 0.0f,	// 左下
	 1.0f,  1.0f, 0.0f,	// 右上
	 1.0f, -1.0f, 0.0f	// 右下
};

/**
 * テクスチャ (UV マッピング) データです。
 */
static const GLfloat TEXCOORDS[] = {
	0.0f, 0.0f,	// 左上
	0.0f, 1.0f,	// 左下
	1.0f, 0.0f,	// 右上
	1.0f, 1.0f	// 右下
};


//-------------------------------------------
// Shader code: create vertexes for polygon
//-------------------------------------------
static const char VERTEX_SHADER[] =
  "attribute vec4 position;"
  "attribute vec2 texcoord;"
  "varying vec2 texcoordVarying;"
  "void main() {"
      "gl_Position = position;"
      "texcoordVarying = texcoord;"
  "}";


//-------------------------------------------
// Shader code: create fragment
//-------------------------------------------
static const char FRAGMENT_SHADER[] =
  "precision mediump float;"
  "varying vec2 texcoordVarying;"
  "uniform sampler2D texture;"
  "void main() {"
      "gl_FragColor = texture2D(texture, texcoordVarying);"
  "}";

static void printGLString(const char *name, GLenum s);

static void checkGlError(const char *op);

// Load shader
GLuint loadShader(GLenum shaderType, const char *shaderCode);

// Create a program from shader language
GLuint createProgram(const char *vertexCode, const char *fragmentCode);

extern "C" {

  // Register c++ functions to VM through JNI
  // http://kznote.blogspot.com/2013/01/androidnative.html
  //
  // B - byte
  // C - char
  // D - double
  // F - float
  // I - int
  // J - long
  // S - short
  // V - void
  // Z - boolean
  // L(java class name); - Java class name. i.e : Ljava/lang/String;
  // [(class name) - array
  static JNINativeMethod method_table[] = {
    {"nativeOnSurfaceCreated", "([III)V", (void *) nativeOnSurfaceCreated},
    {"nativeChangeTexture", "([III)V", (void *) nativeChangeTexture},
    {"nativeOnSurfaceChanged", "(II)V", (void *) nativeOnSurfaceChanged},
    {"nativeOnDrawFrame", "()V", (void *) nativeOnDrawFrame}
  };

  JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved);
}