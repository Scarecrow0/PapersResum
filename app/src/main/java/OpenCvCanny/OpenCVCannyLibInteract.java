package OpenCvCanny;

import android.graphics.Bitmap;
import android.os.Handler;
import android.os.Message;

import java.lang.ref.WeakReference;

/**
 * Created by root on 17-10-31.
 * 直接与使用者交互，使用者通过这里发起canny过程，
 * 设定回调函数，当canny流程到达特点节点时
 * 在主线程进行什么样的动作 （更新imageview textview ）
 */

public class OpenCVCannyLibInteract {
    private Handler interactHandler;
    private static final OpenCVCannyLib openCVCanny = new OpenCVCannyLib();

    public static final int ON_EDGE_DETECT_COMPLETE = 1,
            ON_LINE_SEGMENT_EXTRACTION_COMPLETE = 5,
            ON_LINE_MATCHING_COMPLETE = 85;


    //接受从工作线程传来的消息和数据
    // 进行用户设定回调函数的回调
    public OpenCVCannyLibInteract(final CallbackInterface callbackInterface) {
        interactHandler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                CallbackResult callbackResult = (CallbackResult) msg.obj;
                switch (msg.what) {
                    case ON_EDGE_DETECT_COMPLETE:
                        callbackInterface
                                .onEdgeDetectComplete(callbackResult.img1);
                        break;
                    case ON_LINE_SEGMENT_EXTRACTION_COMPLETE:
                        callbackInterface
                                .onLineSegmentExtractionComplete(callbackResult.img1);
                        break;
                    default:
                        break;
                }
            }
        };
    }

    public void startMatchimg(Bitmap img1) {
        // 这里传入图片进行调用
        openCVCanny.startMatching(interactHandler, img1);
    }


    public interface CallbackInterface {
        void onEdgeDetectComplete(Bitmap img1);

        //当边缘提取完毕回调传回图像
        void onLineSegmentExtractionComplete(Bitmap img1);

    }


    public void exit() {
        openCVCanny.stopWorkThread();
        interactHandler.removeCallbacksAndMessages(null);
        interactHandler = null;
    }

}

