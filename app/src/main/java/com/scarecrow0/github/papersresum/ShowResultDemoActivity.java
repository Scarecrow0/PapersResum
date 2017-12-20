package com.scarecrow0.github.papersresum;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import OpenCvCanny.OpenCVCannyLibInteract;

/**
 * Created by root on 17-12-18.
 *
 */

public class ShowResultDemoActivity extends AppCompatActivity {


    public static final int FROM_DEFAULT = 5465;
    public static final int FROM_CAMERA = 877;

    private OpenCVCannyLibInteract openCVCannyLib;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.show_result);
        openCVCannyLib = new OpenCVCannyLibInteract(new OpenCVCannyLibInteract.CallbackInterface() {
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

        Intent in = getIntent();
        int flag = in.getIntExtra("call_from", 0);
        Bitmap bitmap;
        if (flag != FROM_CAMERA)
            bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.sample4);
        else {
            bitmap = BitmapFactory.decodeFile(this.getExternalFilesDir(null) + "/pic.jpg");
        }

        ImageView iv = findViewById(R.id.orgin_img);
        iv.setImageBitmap(bitmap);
        openCVCannyLib.startMatchimg(bitmap);

    }

    @Override
    public void onStop() {
        openCVCannyLib.exit();
        super.onStop();
    }
}
