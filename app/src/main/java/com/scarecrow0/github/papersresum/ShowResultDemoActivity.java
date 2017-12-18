package com.scarecrow0.github.papersresum;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import OpenCvCanny.OpenCVCannyLibInteract;

/**
 * Created by root on 17-12-18.
 */

public class ShowResultDemoActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.show_result);
        OpenCVCannyLibInteract openCVCannyLib = new OpenCVCannyLibInteract(new OpenCVCannyLibInteract.CallbackInterface() {
            @Override
            public void onEdgeDetectComplete(Bitmap img1) {
                ImageView imageView = findViewById(R.id.edge_detect);
                imageView.setImageBitmap(img1);
            }

            @Override
            public void onLineSegmentExtractionComplete(Bitmap img1) {
                ImageView imageView = findViewById(R.id.segment_detect);
                imageView.setImageBitmap(img1);
            }
        });
        Bitmap bitmap = BitmapFactory.decodeResource(getResources(),R.drawable.sample3);
        ImageView iv = findViewById(R.id.orgin_img);
        iv.setImageBitmap(bitmap);
        openCVCannyLib.startMatchimg(bitmap);

    }
}
