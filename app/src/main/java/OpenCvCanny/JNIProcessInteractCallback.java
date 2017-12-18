package OpenCvCanny;

import android.graphics.Bitmap;
import android.os.Handler;

/**
 * Created by root on 17-11-1.
 * 这个类是要被直接传入cpp部分进行回调的回调对象
 * 负责获取数据将其打包 发给interface的主线程handler
 * 通知interface的handler进行用户设定回调方法的执行
 */

public class JNIProcessInteractCallback {
    private Handler interactCallback;
    //主线程的handler

    public JNIProcessInteractCallback(Handler interactCallback) {
        this.interactCallback = interactCallback;
    }

    public void onEdgeDetectComplete(int[] img1, int w, int h) {
        CallbackResult callbackResult = callbackResultFactory(img1, w, h);
        interactCallback
                .obtainMessage(OpenCVCannyLibInteract.ON_EDGE_DETECT_COMPLETE, callbackResult)
                .sendToTarget();
    }
    //当边缘提取完毕回调 通知主线程 并传回图像

    public void onLineSegmentExtractionComplete(int[] img1, int w, int h) {
        CallbackResult callbackResult = callbackResultFactory(img1, w, h);
        interactCallback
                .obtainMessage(OpenCVCannyLibInteract.ON_LINE_SEGMENT_EXTRACTION_COMPLETE, callbackResult)
                .sendToTarget();

    }
    //当直线提取完毕时 同上



    // 负责将传回的数据打包
    private CallbackResult callbackResultFactory(int[] img1, int w, int h) {
        Bitmap bitmapImg1 = OpenCVCannyLib.intArray2Bitmap(img1, w, h);
        return new CallbackResult(bitmapImg1);
    }
}
