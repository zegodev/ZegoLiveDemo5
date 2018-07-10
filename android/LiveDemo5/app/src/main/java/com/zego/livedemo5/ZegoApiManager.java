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
    
    /** Please contact ZEGO support to get the AppID and signKey */
    /** 请开发者联系 ZEGO support 获取 APPID 与 key */
    private byte[] requestSignKey(long appId) {
        if (isRtmpProduct(appId)) {
            return new byte[] {
            };
        } else if (isUdpProduct(appId)) {
            return new byte[] {
            };
        } else if (isInternationalProduct(appId)) {
            return new byte[] {
            };
        }

        throw new IllegalArgumentException(String.format("appId: <%d> is illegal", appId));
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

    /** Please contact ZEGO support to get the AppID and signKey */
    /** 请开发者联系 ZEGO support 获取 APPID 与 key */
    public long getAppID() {
        return mAppID;
    }

    public boolean isInternationalProduct(long appId) {
        return appId == 100L;
    }

    public boolean isUdpProduct(long appId) {
        return appId == 10L;
    }

    public boolean isRtmpProduct(long appId) {
        return appId == 0L;
    }
}
