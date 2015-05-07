package masashi.com.opengltest;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GLRenderer implements GLSurfaceView.Renderer {
  private Context context;

  static {
    System.loadLibrary("glsurface");
  }
  public static native void nativeOnSurfaceCreated(int[] pixels, int width, int height);
  public static native void nativeOnSurfaceChanged(int width, int height);
  public static native void nativeOnDrawFrame();
  public static native void nativeChangeTexture(int[] pixels, int width, int height);


  public GLRenderer(Context context) {
    this.context = context;
  }

  @Override
  public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
    Log.e("GLRenderer", "-----> onSurfaceCreated");

    Bitmap bitmap = MyUtil.getBitmapFromAsset(context, "a.png");

    int width = bitmap.getWidth();
    int height = bitmap.getHeight();

    int[] pixels = new int[width * height];
    bitmap.getPixels(pixels, 0, width, 0, 0, width, height);
    bitmap.recycle();

    nativeOnSurfaceCreated(pixels, width, height);

    System.gc();
  }

  @Override
  public void onSurfaceChanged(GL10 gl10, int width, int height) {
    nativeOnSurfaceChanged(width, height);
  }

  @Override
  public void onDrawFrame(GL10 gl10) {
    nativeOnDrawFrame();
  }

  public void changeImage(Bitmap bitmap) {

    int width = bitmap.getWidth();
    int height = bitmap.getHeight();

    int[] pixels = new int[width * height];
    bitmap.getPixels(pixels, 0, width, 0, 0, width, height);
    bitmap.recycle();

    nativeChangeTexture(pixels, 256, 512);

    System.gc();
  }
}
