package com.scarecrow0.github.papersresum;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

import OpenCvCanny.OpenCVCannyLib;
import OpenCvCanny.OpenCVCannyLibInteract;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button default_test = findViewById(R.id.btn_defaut_img_test);
        Button open_camera = findViewById(R.id.btn_camera_img_take);

        default_test.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent in = new Intent(MainActivity.this,ShowResultDemoActivity.class);
                startActivity(in);
            }
        });


        open_camera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent in = new Intent(MainActivity.this,CameraActivity.class);
                startActivity(in);
            }
        });

    }
}
