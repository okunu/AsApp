package ok.com.asapp;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{
    ImageView mPic;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        TextView textView = (TextView)findViewById(R.id.tv);
        textView.setText(getStringFromNative());

        mPic = (ImageView)findViewById(R.id.guassImg);
        Button button = (Button)findViewById(R.id.btn);
        button.setOnClickListener(this);
    }

    public native String getStringFromNative();
    public native void gaussBlue(Bitmap bitmap);

    static {
        System.loadLibrary("hello");
    }

    @Override
    public void onClick(View v) {
        if (v.getId() == R.id.btn){
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.mipmap.timg);
            gaussBlue(bitmap);
            mPic.setImageBitmap(bitmap);
        }
    }
}
