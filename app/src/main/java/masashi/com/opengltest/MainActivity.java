package masashi.com.opengltest;

import android.graphics.Bitmap;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;


public class MainActivity extends ActionBarActivity implements View.OnClickListener {

  private GLSurf glSurfaceView;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);

    glSurfaceView = new GLSurf(this);
    LinearLayout container = (LinearLayout)this.findViewById(R.id.conainer);
    container.addView(glSurfaceView);

    Button testBtn = (Button)this.findViewById(R.id.testBtn);
    testBtn.setOnClickListener(this);

  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    return false;
  }

  @Override
  public boolean onOptionsItemSelected(MenuItem item) {
    return false;
  }

  @Override
  public void onClick(View v) {
    Bitmap bitmap = MyUtil.getBitmapFromAsset(this, "b.png");
    glSurfaceView.changeImage(bitmap);
  }

  @Override
  public void onPause() {
    super.onPause();
    glSurfaceView.onPause();
  }

  @Override
  public void onResume() {
    super.onResume();
    glSurfaceView.onResume();
  }
}
