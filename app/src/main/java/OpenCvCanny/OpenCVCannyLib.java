package OpenCvCanny;

import android.graphics.Bitmap;
import android.os.Handler;


/**
 * Created by root on 17-10-31.
 * 直接封装jni部分
 * 负责从interface接受调用 构造 启动工作线程
 */

public class OpenCVCannyLib {

    static {
        System.loadLibrary("MyCannyLib"); // 加载编译好的.so动态库
    }


    //      jni 相关函数定义
    /**
     * 声明native方法，调用OpenCV的边缘检测
     *
     * @param img1buf 图像1
     * @param w1   图像1宽
     * @param h1   图像1高
     * @param callback 回调类
     */

    public static native void cannyLauncher
    (int[] img1buf, int w1, int h1, JNIProcessInteractCallback callback);

    //      处理内部事务的线程
    private ProcessHandlerThread processingThread;

    //  负责从外部接受调用的方法
    public void startMatching(Handler callback, final Bitmap img1) {
        processingThread = new ProcessHandlerThread(callback);
        processingThread.setLooperPreparedListenner(new ProcessHandlerThread.LooperPreparedListenner() {
            @Override
            public void onLooperPrepared() {
                processingThread.startCannyProcess(img1);
                // 将匹配图片传入内部线程 发起匹配过程
            }
        });
        processingThread.start();
        processingThread.getLooper();
        //启动内部线程 准备进行canny

    }

    public static int[] bitmap2IntaArray(Bitmap bitmap) {
        int width = bitmap.getWidth();
        int height = bitmap.getHeight();
        int[] pix = new int[width * height];
        bitmap.getPixels(pix, 0, width, 0, 0, width, height);
        return pix;
    }

    public static Bitmap intArray2Bitmap(int[] pix, int width, int height) {
        Bitmap resultBitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        resultBitmap.setPixels(pix, 0, width, 0, 0, width, height);
        return resultBitmap;
    }
}
