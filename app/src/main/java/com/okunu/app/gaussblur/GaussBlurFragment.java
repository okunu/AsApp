package com.okunu.app.gaussblur;

import android.app.Fragment;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;

import com.okunu.app.R;

/**
 * Created by oukun.ok on 2017/10/18.
 */

public class GaussBlurFragment extends Fragment implements View.OnClickListener{

    ImageView mBlurImg;

    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.gauss_blur_layout, container, false);
        View button = view.findViewById(R.id.btn);
        button.setOnClickListener(this);
        mBlurImg = (ImageView) view.findViewById(R.id.blur_img);
        return view;
    }

    @Override
    public void onClick(View view) {
        if (view.getId() == R.id.btn){
            Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.mipmap.beauty);
            Log.i("okunu","click");
            BlurJni.blurBitmap(bitmap, 50);
            mBlurImg.setImageBitmap(bitmap);
        }
    }
}
