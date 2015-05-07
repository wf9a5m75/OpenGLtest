#include "glsurface.h"

extern void nativeOnSurfaceCreated(JNIEnv *env, jobject thiz, jintArray pixels, jint width, jint height) {
  LOGI("----------- nativeOnSurfaceCreated ---------------");
  printGLString("Version", GL_VERSION);
  printGLString("Vendor", GL_VENDOR);
  printGLString("RENDERER", GL_RENDERER);
  printGLString("Extensions", GL_EXTENSIONS);

  // Create a program
  program = createProgram(VERTEX_SHADER, FRAGMENT_SHADER);
  if (!program) {
    LOGE("--> [error] Could not create program.");
    return;
  }
  glUseProgram(program);
  checkGlError("glUseProgram");

  // Obtain the variable for shader
  position = glGetAttribLocation(program, "position");
  checkGlError("glGetAttribLocation position");
  glEnableVertexAttribArray(position);

  texcoord = glGetAttribLocation(program, "texcoord");
  checkGlError("glGetAttribLocation texcoord");
  glEnableVertexAttribArray(texcoord);

  textures[0] = glGetUniformLocation(program, "texture");
  checkGlError("glGetUniformLocation texture");

  // Create a texture
  unsigned int* _pixels = (unsigned int*) env->GetPrimitiveArrayCritical(pixels, 0);

  // Convert ARGB -> RGBA
  const int size = width * height;
  for (int i = 0; i < size; i++) {
    unsigned int px = _pixels[i];
    _pixels[i] = (
      (px & 0xFF000000) |         //Alpha
      ((px << 16) & 0x00FF0000) | // Red
      (px & 0x0000FF00) |         // Green
      ((px >> 16) & 0x000000FF)   // Blue
    );
  }

  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Create a texture object, then set the image
  glGenTextures(1, textures);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);

  // Specify the methods of scaling
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Scaling down
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Scaling up

  env->ReleasePrimitiveArrayCritical(pixels, _pixels, JNI_ABORT);

  glBindTexture(GL_TEXTURE_2D, 0);
}

extern void nativeChangeTexture(JNIEnv *env, jobject thiz, jintArray pixels, jint width, jint height) {
  LOGE("----------- nativeChangeTexture ---------------");

  //glEnable(GL_TEXTURE_2D);
  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // simple alpha blending

  // Create a texture
  unsigned int* _pixels = (unsigned int*) env->GetPrimitiveArrayCritical(pixels, 0);

  // Convert ARGB -> RGBA
  const int size = width * height;
  for (int i = 0; i < size; i++) {
    unsigned int px = _pixels[i];
    _pixels[i] = (
      (px & 0xFF000000) |         //Alpha
      ((px << 16) & 0x00FF0000) | // Red
      (px & 0x0000FF00) |         // Green
      ((px >> 16) & 0x000000FF)   // Blue
    );
  }

  //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  LOGI("textures[0] = %d ", textures[0]);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
    checkGlError("glBindTexture");
  //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 512, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
  //checkGlError("glTexSubImage2D");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _pixels);
  checkGlError("glTexImage2D");

  glBindTexture ( GL_TEXTURE_2D, 0 );
    checkGlError("glBindTexture");

  // Specify the methods of scaling
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Scaling down
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Scaling up

  env->ReleasePrimitiveArrayCritical(pixels, _pixels, JNI_ABORT);

  glFlush();
}


extern void nativeOnSurfaceChanged(JNIEnv *env, jobject thiz, jint width, jint height) {
  LOGI("----------- nativeOnSurfaceChanged ---------------");
  LOGD("width = %d, height = %d", width, height);

  glViewport(0, 0, width, height);
  checkGlError("glViewport");
}

extern void nativeOnDrawFrame(JNIEnv *env, jobject thiz) {
  //LOGI("----------- nativeOnDrawFrame --------------- textures[0] = %d ", textures[0]);

  // Fill white color to background
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Specify blending methods
  //glEnable(GL_TEXTURE_2D);
 // glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // simple alpha blending

  // Specify texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  //glUniform1i(textures[0], 0);
  glVertexAttribPointer(texcoord, 2, GL_FLOAT, false, 0, TEXCOORDS);
  glVertexAttribPointer(position, 3, GL_FLOAT, false, 0, VERTEXS);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  glBindTexture(GL_TEXTURE_2D, 0);
  //glDisable(GL_BLEND);
  //glDisable(GL_TEXTURE_2D);
}

static void printGLString(const char *name, GLenum s) {
  const char *v = (const char *) glGetString(s);
  LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char *op) {
  for (GLuint error = glGetError(); error; error = glGetError()) {
    LOGE("--> [Error] after %s() glError (0x%x)\n", op, error);
  }
}

GLuint loadShader(GLenum shaderType, const char *shaderCode) {
  GLuint shader = glCreateShader(shaderType);
  if (shader == 0) {
    return 0;
  }

  glShaderSource(shader, 1, &shaderCode, NULL);
  glCompileShader(shader);
  GLint compiled = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if (infoLen) {
      char *buf = (char*)malloc(infoLen);
      if (buf) {
        glGetShaderInfoLog(program, infoLen, NULL, buf);
        LOGE("--> [error]Could not compile shader:\n%s\n", buf);
        free(buf);
      }
    }
    glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

// Create a program from shader language
GLuint createProgram(const char *vertexCode, const char *fragmentCode) {
  // Compile the vertexCode
  GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexCode);
  if (!vertexShader) {
    LOGE("--> [error] vertex missed!");
    return 0;
  }

  // Compile the fragmentCode
  GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, fragmentCode);
  if (!pixelShader) {
    LOGE("--> [error] fragment missed!");
    return 0;
  }

  // Create a program, then bind vertexShader and pixelShader to it.
  GLuint program = glCreateProgram();
  if (program) {
    // Attach the vertexShader
    glAttachShader(program, vertexShader);
    checkGlError("glAttachShader");

    // Attach the pixelShader
    glAttachShader(program, pixelShader);
    checkGlError("glAttachShader");

    glLinkProgram(program);
    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
      GLint bufLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
      if (bufLength) {
        char *buf = (char*)malloc(bufLength);
        if (buf) {
          glGetProgramInfoLog(program, bufLength, NULL, buf);
          LOGE("--> [error]Could not link program:\n%s\n", buf);
          free(buf);
        }
      }

      LOGE("--> [error]program missed!");
      glDeleteProgram(program);
      program = 0;
    }

  }
  return program;
}

extern "C" {

  JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    // Register export functions to JAVA
    JNIEnv* env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
      return JNI_ERR;
    } else {
      jclass clazz = env->FindClass("masashi/com/opengltest/GLRenderer");
      if (clazz) {
        int method_table_size = sizeof(method_table) / sizeof(method_table[0]);
        jint ret = env->RegisterNatives(clazz, method_table, method_table_size);
        env->DeleteLocalRef(clazz);
        return ret == 0 ? JNI_VERSION_1_6 : JNI_ERR;
      } else {
        return JNI_ERR;
      }
    }
  }

}