package masashi.com.opengltest;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;

public class GLSurf extends GLSurfaceView {
  private final GLRenderer mRenderer;

  public GLSurf(Context context) {
    super(context);

    mRenderer = new GLRenderer(context);
    this.setEGLContextClientVersion(2);  // Enable OpenGL 2.0

    setRenderer(mRenderer);
  }

  public void changeImage(Bitmap bitmap) {
    mRenderer.changeImage(bitmap);
  }

}
