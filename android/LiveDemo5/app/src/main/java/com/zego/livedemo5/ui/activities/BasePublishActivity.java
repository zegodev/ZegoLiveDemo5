package com.zego.livedemo5.ui.activities;

import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.text.TextUtils;
import android.view.Surface;
import android.view.View;

import com.google.gson.Gson;
import com.zego.livedemo5.R;
import com.zego.livedemo5.ZegoApiManager;
import com.zego.livedemo5.constants.Constants;
import com.zego.livedemo5.constants.IntentExtra;
import com.zego.livedemo5.ui.widgets.ViewLive;
import com.zego.livedemo5.utils.PreferenceUtil;
import com.zego.livedemo5.utils.ZegoRoomUtil;
import com.zego.zegoliveroom.entity.ZegoStreamInfo;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Copyright © 2017 Zego. All rights reserved.
 */

public abstract class BasePublishActivity extends BaseLiveActivity {

    @Override
    protected void initExtraData(Bundle savedInstanceState) {
        if (savedInstanceState == null) {
            Intent intent = getIntent();
            mPublishTitle = intent.getStringExtra(IntentExtra.PUBLISH_TITLE);
            mEnableFrontCam = intent.getBooleanExtra(IntentExtra.ENABLE_FRONT_CAM, false);
            mEnableTorch = intent.getBooleanExtra(IntentExtra.ENABLE_TORCH, false);
            mSelectedBeauty = intent.getIntExtra(IntentExtra.SELECTED_BEAUTY, 0);
            mSelectedFilter = intent.getIntExtra(IntentExtra.SELECTED_FILTER, 0);
            mAppOrientation = intent.getIntExtra(IntentExtra.APP_ORIENTATION, Surface.ROTATION_0);
        }

        super.initExtraData(savedInstanceState);
    }

    @Override
    protected void initViews(Bundle savedInstanceState) {
        super.initViews(savedInstanceState);

        // 提前预览, 提升用户体验
        ViewLive freeViewLive = getFreeViewLive();
        if (freeViewLive != null) {
            // 根据推流方向, 设置publish界面的横、竖朝向
            if (mAppOrientation == Surface.ROTATION_90 || mAppOrientation == Surface.ROTATION_270) {
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            } else {
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
            }

            mZegoLiveRoom.setPreviewView(freeViewLive);
            mZegoLiveRoom.startPreview();
        }

        // 提示用户, 当前正处于外部滤镜
        if (ZegoApiManager.getInstance().isUseVideoCapture()) {
            mTvTag.setVisibility(View.VISIBLE);
            mTvTag.setText(getString(R.string.external_capture));
        } else if (ZegoApiManager.getInstance().isUseVideoFilter()) {
            mTvTag.setVisibility(View.VISIBLE);
            mTvTag.setText(getString(R.string.external_filter));
        }
    }

    /**
     * 主播登录房间成功.
     */
    protected void handleAnchorLoginRoomSuccess(ZegoStreamInfo[] zegoStreamInfos) {
        mPublishTitle = PreferenceUtil.getInstance().getUserName() + " is coming";
        mPublishStreamID = ZegoRoomUtil.getPublishStreamID();

        // 开始推流
        startPublish();

        // 打印log
        recordLog(MY_SELF + ": onLoginRoom success(" + mRoomID + "), streamCounts:" + zegoStreamInfos.length);
    }

    /**
     * 主播登录房间失败.
     */
    protected void handleAnchorLoginRoomFail(int errorCode) {
        // 打印log
        recordLog(MY_SELF + ": onLoginRoom fail(" + mRoomID + "), errorCode:" + errorCode);
    }

    /**
     * 响应连麦请求.
     */
    protected void handleJoinLiveRequest(final int seq, String fromUserID, String fromUserName, String roomID) {
        // 有人请求连麦
        recordLog(getString(R.string.someone_is_requesting_to_broadcast, fromUserName));
        mDialogHandleRequestPublish = new AlertDialog.Builder(BasePublishActivity.this).setTitle(getString(R.string.hint))
                .setMessage(getString(R.string.someone_is_requesting_to_broadcast_allow, fromUserName))
                .setPositiveButton(getString(R.string.Allow), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // 同意连麦请求
                        mZegoLiveRoom.respondJoinLiveReq(seq, 0);
                        dialog.dismiss();
                    }
                }).setNegativeButton(getString(R.string.Deny), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        // 拒绝连麦请求
                        mZegoLiveRoom.respondJoinLiveReq(seq, -1);
                        dialog.dismiss();
                    }
                }).create();

        mDialogHandleRequestPublish.show();
    }


    /**
     * 推流成功.
     */
    protected void handlePublishSucc(String streamID, HashMap<String, Object> info) {

        ViewLive viewLivePublish = getViewLiveByStreamID(streamID);
        List<String> listUrls = getShareUrlList(info);
        if (viewLivePublish != null && listUrls.size() > 0) {
            // 设置分享连接
            viewLivePublish.setListShareUrls(listUrls);

            // 将流地址发送给房间观众
            Map<String, String> mapUrls = new HashMap<>();
            mapUrls.put(Constants.FIRST_ANCHOR, String.valueOf(true));
            mapUrls.put(Constants.KEY_HLS, listUrls.get(0));
            mapUrls.put(Constants.KEY_RTMP, listUrls.get(1));
            Gson gson = new Gson();
            String json = gson.toJson(mapUrls);
            mZegoLiveRoom.updateStreamExtraInfo(json);
        }

        super.handlePublishSucc(streamID);
    }

    /**
     * 推流成功, 混流环境.
     */
    protected void handlePublishSuccMix(String streamID, HashMap<String, Object> info) {
        super.handlePublishSucc(streamID);
    }

}
