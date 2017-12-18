package OpenCvCanny;

import android.graphics.Bitmap;

/**
 * Created by root on 17-11-1.
 * 从jni callback传回数据后 从工作线程传回主线程
 * 需要对数据进行一个打包
 * 这是一个简单的打包类（结构体）
 */

public class CallbackResult {
    Bitmap img1, img2;
    double matchRate;

    public CallbackResult(Bitmap img1) {
        this.img1 = img1;
    }

}