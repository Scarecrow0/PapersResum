package com.scarecrow0.github.papersresum;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;

/**
 * Created by root on 17-12-20.
 */

public class PhotoComfirmActivity extends AppCompatActivity implements View.OnClickListener {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.photo_comfirm_activity);
        Button button = findViewById(R.id.photo_confirm);
        button.setOnClickListener(this);
        button = findViewById(R.id.photo_cancel);
        button.setOnClickListener(this);

        String path_str = this.getExternalFilesDir(null) + "/pic.jpg";
        Bitmap bitmap = BitmapFactory.decodeFile(path_str);
        ImageView imageView = findViewById(R.id.photo_display);
        imageView.setImageBitmap(bitmap);

    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.photo_cancel:
                finish();
                break;
            case R.id.photo_confirm:
                Intent intent = new Intent(PhotoComfirmActivity.this, ShowResultDemoActivity.class);
                intent.putExtra("call_from", ShowResultDemoActivity.FROM_CAMERA);
                startActivity(intent);
                finish();
                break;

            default:
                break;
        }
    }
}
