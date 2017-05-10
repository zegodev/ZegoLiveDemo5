package com.zego.livedemo5;


import android.text.TextUtils;
import android.widget.Toast;

import com.zego.livedemo5.utils.PreferenceUtil;
import com.zego.livedemo5.utils.SystemUtil;
import com.zego.livedemo5.videocapture.VideoCaptureFactoryDemo;
import com.zego.livedemo5.videofilter.VideoFilterFactoryDemo;
import com.zego.zegoliveroom.ZegoLiveRoom;
import com.zego.zegoliveroom.constants.ZegoAvConfig;

/**
 * des: zego api管理器.
 */
public class ZegoApiManager {

    private static ZegoApiManager sInstance = null;

    private ZegoLiveRoom mZegoLiveRoom = null;

    private ZegoAvConfig mZegoAvConfig;

    /**
     * 外部渲染开关.
     */
    private boolean mUseExternalRender = false;

    /**
     *  测试环境开关.
     */
    private boolean mUseTestEvn = false;

    /**
     * 外部采集开关.
     */
    private boolean mUseVideoCapture = false;

    /**
     * 外部滤镜开关.
     */
    private boolean mUseVideoFilter = false;

    private boolean mUseHardwareEncode = false;

    private boolean mUseHardwareDecode = false;

    private boolean mUseRateControl = false;

    private boolean mEnableAudioPrep = false;

    private long mAppID = 1;

    private ZegoApiManager() {
        mZegoLiveRoom = new ZegoLiveRoom();
    }

    public static ZegoApiManager getInstance() {
        if (sInstance == null) {
            synchronized (ZegoApiManager.class) {
                if (sInstance == null) {
                    sInstance = new ZegoApiManager();
                }
            }
        }
        return sInstance;
    }

    /**
     * 高级功能.
     */
    private void openAndvancedFunctions(){

        // 开启测试环境
        if(mUseTestEvn){
            ZegoLiveRoom.setTestEnv(true);
        }

        // 外部渲染
        if(mUseExternalRender){
            // 开启外部渲染
            ZegoLiveRoom.enableExternalRender(true);
        }

        // 外部采集
        if(mUseVideoCapture){
            // 外部采集
            VideoCaptureFactoryDemo factoryDemo = new VideoCaptureFactoryDemo();
            factoryDemo.setContext(ZegoApplication.sApplicationContext);
            ZegoLiveRoom.setVideoCaptureFactory(factoryDemo);
        }

        // 外部滤镜
        if(mUseVideoFilter){
            // 外部滤镜
            VideoFilterFactoryDemo videoFilterFactoryDemo = new VideoFilterFactoryDemo();
            ZegoLiveRoom.setVideoFilterFactory(videoFilterFactoryDemo);
        }

        ZegoLiveRoom.enableAudioPrep(mEnableAudioPrep);
    }

    private void initUserInfo(){
        // 初始化用户信息
        String userID = PreferenceUtil.getInstance().getUserID();
        String userName = PreferenceUtil.getInstance().getUserName();

        if (TextUtils.isEmpty(userID) || TextUtils.isEmpty(userName)) {
            long ms = System.currentTimeMillis();
            userID = ms + "";
            userName = "Android_" + SystemUtil.getOsInfo() + "-" + ms;

            // 保存用户信息
            PreferenceUtil.getInstance().setUserID(userID);
            PreferenceUtil.getInstance().setUserName(userName);
        }
        // 必须设置用户信息
        ZegoLiveRoom.setUser(userID, userName);

    }


    private void init(long appID, byte[] signKey){

        initUserInfo();

        // 开发者根据需求定制
        openAndvancedFunctions();

        mAppID = appID;


        // 初始化sdk
        boolean ret = mZegoLiveRoom.initSDK(appID, signKey, ZegoApplication.sApplicationContext);
        if(!ret){
            // sdk初始化失败
            Toast.makeText(ZegoApplication.sApplicationContext, "Zego SDK初始化失败!", Toast.LENGTH_LONG).show();
        }
        // 初始化设置级别为"High"
        mZegoAvConfig = new ZegoAvConfig(ZegoAvConfig.Level.High);
        mZegoLiveRoom.setAVConfig(mZegoAvConfig);


        // 开发者根据需求定制
        // 硬件编码
        setUseHardwareEncode(mUseHardwareEncode);
        // 硬件解码
        setUseHardwareDecode(mUseHardwareDecode);
        // 码率控制
        setUseRateControl(mUseRateControl);
    }

    /**
     * 此方法是通过 appId 模拟获取与之对应的 signKey，强烈建议 signKey 不要存储在本地，而是加密存储在云端，通过网络接口获取
     * @param appId
     * @return
     */
    private byte[] requestSignKey(long appId) {
        if (isRtmpProduct(appId)) {
            return new byte[] {(byte) 0x91, (byte) 0x93, (byte) 0xcc, (byte) 0x66, (byte) 0x2a, (byte) 0x1c, (byte) 0x0e, (byte) 0xc1,
                    (byte) 0x35, (byte) 0xec, (byte) 0x71, (byte) 0xfb, (byte) 0x07, (byte) 0x19, (byte) 0x4b, (byte) 0x38,
                    (byte) 0x41, (byte) 0xd4, (byte) 0xad, (byte) 0x83, (byte) 0x78, (byte) 0xf2, (byte) 0x59, (byte) 0x90,
                    (byte) 0xe0, (byte) 0xa4, (byte) 0x0c, (byte) 0x7f, (byte) 0xf4, (byte) 0x28, (byte) 0x41, (byte) 0xf7
            };
        } else if (isUdpProduct(appId)) {
            return new byte[] {(byte)0x1e, (byte)0xc3, (byte)0xf8, (byte)0x5c, (byte)0xb2, (byte)0xf2, (byte)0x13, (byte)0x70,
                    (byte)0x26, (byte)0x4e, (byte)0xb3, (byte)0x71, (byte)0xc8, (byte)0xc6, (byte)0x5c, (byte)0xa3,
                    (byte)0x7f, (byte)0xa3, (byte)0x3b, (byte)0x9d, (byte)0xef, (byte)0xef, (byte)0x2a, (byte)0x85,
                    (byte)0xe0, (byte)0xc8, (byte)0x99, (byte)0xae, (byte)0x82, (byte)0xc0, (byte)0xf6, (byte)0xf8};
        } else if (isInternationalProduct(appId)) {
            return new byte[] {(byte)0x5d, (byte)0xe6, (byte)0x83, (byte)0xac, (byte)0xa4, (byte)0xe5, (byte)0xad, (byte)0x43,
                    (byte)0xe5, (byte)0xea, (byte)0xe3, (byte)0x70, (byte)0x6b, (byte)0xe0, (byte)0x77, (byte)0xa4,
                    (byte)0x18, (byte)0x79, (byte)0x38, (byte)0x31, (byte)0x2e, (byte)0xcc, (byte)0x17, (byte)0x19,
                    (byte)0x32, (byte)0xd2, (byte)0xfe, (byte)0x22, (byte)0x5b, (byte)0x6b, (byte)0x2b, (byte)0x2f};
        }

        throw new IllegalArgumentException(String.format("appId: <%d> is illegal, must both 1 or 1739272706", appId));
    }

    /**
     * 初始化sdk.
     */
    public void initSDK(){
        // 即构分配的key与id
        long appID = BuildConfig.APP_ID;
        byte[] signKey = requestSignKey(appID);

        init(appID, signKey);
    }

    public void reInitSDK(long appID, byte[] signKey) {
        init(appID, signKey);
    }

    public void releaseSDK() {
        // 清空高级设置
        ZegoLiveRoom.setTestEnv(false);
        ZegoLiveRoom.enableExternalRender(false);
        ZegoLiveRoom.setVideoCaptureFactory(null);
        ZegoLiveRoom.setVideoFilterFactory(null);

        mZegoLiveRoom.unInitSDK();
    }

    public ZegoLiveRoom getZegoLiveRoom() {
        return mZegoLiveRoom;
    }

    public void setZegoConfig(ZegoAvConfig config) {
        mZegoAvConfig = config;
        mZegoLiveRoom.setAVConfig(config);
    }


    public ZegoAvConfig getZegoAvConfig(){
        return  mZegoAvConfig;
    }


    public void setUseTestEvn(boolean useTestEvn) {
        mUseTestEvn = useTestEvn;
    }

    public boolean isUseExternalRender(){
        return mUseExternalRender;
    }

    public void setUseExternalRender(boolean useExternalRender){
        mUseExternalRender = useExternalRender;
    }

    public void setUseVideoCapture(boolean useVideoCapture) {
        mUseVideoCapture = useVideoCapture;
    }

    public void setUseVideoFilter(boolean useVideoFilter) {
        mUseVideoFilter = useVideoFilter;
    }

    public boolean isUseVideoCapture() {
        return mUseVideoCapture;
    }

    public boolean isUseVideoFilter() {
        return mUseVideoFilter;
    }

    public void setUseHardwareEncode(boolean useHardwareEncode) {
        if(useHardwareEncode){
            // 开硬编时, 关闭码率控制
            if(mUseRateControl){
                mUseRateControl = false;
                mZegoLiveRoom.enableRateControl(false);
            }
        }
        mUseHardwareEncode = useHardwareEncode;
        ZegoLiveRoom.requireHardwareEncoder(useHardwareEncode);
    }

    public void setUseHardwareDecode(boolean useHardwareDecode) {
        mUseHardwareDecode = useHardwareDecode;
        ZegoLiveRoom.requireHardwareDecoder(useHardwareDecode);
    }

    public void setUseRateControl(boolean useRateControl) {
        if(useRateControl){
            // 开码率控制时, 关硬编
            if(mUseHardwareEncode){
                mUseHardwareEncode = false;
                ZegoLiveRoom.requireHardwareEncoder(false);
            }
        }
        mUseRateControl = useRateControl;
        mZegoLiveRoom.enableRateControl(useRateControl);
    }

    public void setEnableAudioPrep(boolean enableAudioPrep) {
        mEnableAudioPrep = enableAudioPrep;
    }

    public long getAppID() {
        return mAppID;
    }

    public boolean isInternationalProduct(long appId) {
        return appId == 3322882036L;
    }

    public boolean isUdpProduct(long appId) {
        return appId == 1739272706L;
    }

    public boolean isRtmpProduct(long appId) {
        return appId == 1L;
    }
}
