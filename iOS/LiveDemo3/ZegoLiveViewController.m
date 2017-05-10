//
//  ZegoLiveViewController.m
//  LiveDemo3
//
//  Created by Strong on 16/6/28.
//  Copyright © 2016年 ZEGO. All rights reserved.
//

#import "ZegoLiveViewController.h"
#import "ZegoAnchorOptionViewController.h"
#import "ZegoLogTableViewController.h"
#import "ZegoSettings.h"
#import "ZegoInstrument.h"

#import <TencentOpenAPI/QQApiInterface.h>
#import <TencentOpenAPI/QQApiInterfaceObject.h>

@interface ZegoLiveViewController () <UIAlertViewDelegate, ZegoLiveApiAudioRecordDelegate>

@property (assign) UIInterfaceOrientation currentOrientation;

//混流时的数据源
@property (nonatomic, strong) NSData *auxData;
@property (nonatomic, assign) void *pPos;

//处理父view及子view弹框
@property (nonatomic, strong) NSMutableArray *requestAlertList;
@property (nonatomic, strong) NSMutableArray *requestAlertContextList;

@property (nonatomic, assign) CGSize videoSize;
@property (nonatomic, strong) NSTimer *usageTimer;
@property (nonatomic, copy) NSString *usageFilePath;

@property (strong) NSMutableData *recordedAudio;

@property (nonatomic, strong) NSTimer *captureTimer;
@property (nonatomic, strong) NSMutableDictionary *playTimerDictionary;

@property (nonatomic, assign) NSUInteger subViewSpace;
@property (nonatomic, assign) NSUInteger subViewWidth;
@property (nonatomic, assign) NSUInteger subViewHeight;
@property (nonatomic, assign) NSUInteger subViewPerRow;

@end

@implementation ZegoLiveViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    
    self.useFrontCamera = YES;
    self.enableTorch = NO;
    self.beautifyFeature = [ZegoSettings sharedInstance].beautifyFeature;
    self.filter = ZEGO_FILTER_NONE;
    self.enableMicrophone = YES;
    self.enablePreview = YES;
    self.viewMode = ZegoVideoViewModeScaleAspectFill;
    self.enableCamera = YES;
    self.enableLoopback = NO;
    
    _maxStreamCount = [ZegoLiveRoomApi getMaxPlayChannelCount];

    self.subViewSpace = 10;
    if (self.maxStreamCount <= 3)
    {
        self.subViewWidth = 140;
        self.subViewHeight = 210;
        self.subViewPerRow = 2;
    }
    else
    {
        self.subViewWidth = 90;
        self.subViewHeight = 135;
        self.subViewPerRow = 3;
    }
    
    self.enableMicrophone = YES;
    self.enablePreview = YES;
    self.viewMode = ZegoVideoViewModeScaleAspectFill;
    self.enableCamera = YES;
    self.enableSpeaker = YES;
    self.enableAux = NO;
    if ([ZegoDemoHelper recordTime])
        self.enablePreviewMirror = NO;
    else
        self.enablePreviewMirror = YES;
    
    self.enableCaptureMirror = NO;
    
    self.logArray = [NSMutableArray array];
    self.staticsArray = [NSMutableArray array];
    
    if ([self isDeviceiOS7])
        self.requestAlertContextList = [NSMutableArray array];
    else
        self.requestAlertList = [NSMutableArray array];
    
    self.videoSize = [ZegoSettings sharedInstance].currentConfig.videoEncodeResolution;
    
    // 监听电话事件
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(audioSessionWasInterrupted:) name:AVAudioSessionInterruptionNotification object:nil];
    
    // 监听耳机插拔
    [ZegoDemoHelper checkHeadSet];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(handleAudioRouteChanged:) name:AVAudioSessionRouteChangeNotification object:nil];
    
    self.usageTimer = [NSTimer scheduledTimerWithTimeInterval:10 target:self selector:@selector(onUsage) userInfo:nil repeats:YES];
    
    self.usageFilePath = [self getLogFileName];
    
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        [[NSFileManager defaultManager] removeItemAtPath:self.usageFilePath error:nil];
    });
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:self.usageFilePath])
    {
        [[NSFileManager defaultManager] createFileAtPath:self.usageFilePath contents:nil attributes:nil];
    }
    
    NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:self.usageFilePath];
    [fileHandle seekToEndOfFile];
    NSString *content = [NSString stringWithFormat:@"%@\n", NSStringFromClass([self class])];
    [fileHandle writeData:[content dataUsingEncoding:NSUTF8StringEncoding]];
    [fileHandle closeFile];
    
    self.playTimerDictionary = [NSMutableDictionary dictionaryWithCapacity:self.maxStreamCount];
    
    //彩蛋
    UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(onShowStaticsViewController:)];
    [self.view addGestureRecognizer:longPressGesture];
}

- (NSString *)getLogFileName
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    if (paths.count == 0)
    {
        NSLog(@"Error! Cannot Get Caches Dir.");
        return nil;
    }
    
    NSString *cachesDir = [paths objectAtIndex:0];
    NSString *logName = [cachesDir stringByAppendingPathComponent:@"usage.log"];
    
    return logName;
}

- (NSString *)getCurrentLogTime
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"[yyyy-dd-MM HH:mm:ss]";
    
    return [formatter stringFromDate:[NSDate date]];
}

- (void)onUsage
{
    float cpu = [[ZegoInstrument shareInstance] getCPUUsage];
    float memory = [[ZegoInstrument shareInstance] getMemoryUsage];
    float battery = [[ZegoInstrument shareInstance] getBatteryLevel];

    NSString *usage = [NSString stringWithFormat:@"cpu %.3f, memory %.3f, battery %.2f\n", cpu, memory, battery];
    NSString *content = [NSString stringWithFormat:@"%@ %@", [self getCurrentLogTime], usage];

    NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:self.usageFilePath];
    [fileHandle seekToEndOfFile];
    [fileHandle writeData:[content dataUsingEncoding:NSUTF8StringEncoding]];
    [fileHandle closeFile];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self setIdelTimerDisable:YES];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self setIdelTimerDisable:NO];
    [super viewWillDisappear:animated];
    
    [self.usageTimer invalidate];
    self.usageTimer = nil;
    
    if (self.isBeingDismissed)
    {
        [[NSNotificationCenter defaultCenter] removeObserver:self];
    }
}

- (void)audioSessionWasInterrupted:(NSNotification *)notification
{
    NSLog(@"%s: %@", __func__, notification);
    if (AVAudioSessionInterruptionTypeBegan == [notification.userInfo[AVAudioSessionInterruptionTypeKey] intValue])
    {
        // 暂停音频设备
        [[ZegoDemoHelper api] pauseModule:ZEGOAPI_MODULE_AUDIO];
    }
    else if(AVAudioSessionInterruptionTypeEnded == [notification.userInfo[AVAudioSessionInterruptionTypeKey] intValue])
    {
        // 恢复音频设备
        [[ZegoDemoHelper api] resumeModule:ZEGOAPI_MODULE_AUDIO];
    }
}

- (void)handleAudioRouteChanged:(NSNotification *)notification
{
    NSInteger reason = [[notification.userInfo objectForKey:AVAudioSessionRouteChangeReasonKey] integerValue];
    
    if (reason == AVAudioSessionRouteChangeReasonNewDeviceAvailable ||
        reason == AVAudioSessionRouteChangeReasonOldDeviceUnavailable)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            [ZegoDemoHelper checkHeadSet];
            if (![ZegoDemoHelper useHeadSet] && self.enableLoopback)
            {
                self.enableLoopback = NO;
            }
            
            [[NSNotificationCenter defaultCenter] postNotificationName:kHeadSetStateChangeNotification object:self];
        });
    }
}

#pragma mark ZegoAnchorOptionDelegate
- (void)onUseFrontCamera:(BOOL)use
{
    self.useFrontCamera = use;
}

- (void)onEnableMicrophone:(BOOL)enabled
{
    self.enableMicrophone = enabled;
}

- (void)onEnableTorch:(BOOL)enable
{
    self.enableTorch = enable;
}

- (void)onSelectedBeautify:(NSInteger)row
{
    int feature = 0;
    switch (row) {
        case 1:
            feature = ZEGO_BEAUTIFY_POLISH;
            break;
        case 2:
            feature = ZEGO_BEAUTIFY_WHITEN;
            break;
        case 3:
            feature = ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_WHITEN;
            break;
        case 4:
            feature = ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_SKINWHITEN;
        default:
            break;
    }
    [ZegoSettings sharedInstance].beautifyFeature = feature;
    [[ZegoDemoHelper api] enableBeautifying:feature];
    self.beautifyFeature = feature;
}

- (void)onSelectedFilter:(NSInteger)row
{
    self.filter = row;
}

- (void)onEnableCamera:(BOOL)enabled
{
    self.enableCamera = enabled;
}

- (void)onEanbleSpeaker:(BOOL)enable
{
    self.enableSpeaker = enable;
}

- (void)onEnableAux:(BOOL)enabled
{
    self.enableAux = enabled;
}

- (void)onEnablePreviewMirror:(BOOL)enabled
{
    self.enablePreviewMirror = enabled;
}

- (void)onEnableCaptureMirror:(BOOL)enable
{
    self.enableCaptureMirror = enable;
}

- (void)onEnableLoopback:(BOOL)enable
{
    self.enableLoopback = enable;
}

- (BOOL)onGetUseFrontCamera
{
    return self.useFrontCamera;
}

- (BOOL)onGetEnableMicrophone
{
    return self.enableMicrophone;
}

- (BOOL)onGetEnableTorch
{
    return self.enableTorch;
}

- (NSInteger)onGetSelectedBeautify
{
    if (self.beautifyFeature == ZEGO_BEAUTIFY_POLISH)
        return 1;
    else if (self.beautifyFeature == ZEGO_BEAUTIFY_WHITEN)
        return 2;
    else if (self.beautifyFeature == (ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_WHITEN))
        return 3;
    else if (self.beautifyFeature == (ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_SKINWHITEN))
        return 4;
    else
        return 0;
}

- (NSInteger)onGetSelectedFilter
{
    return self.filter;
}

- (BOOL)onGetEnableCamera
{
    return self.enableCamera;
}

- (BOOL)onGetEnableAux
{
    return self.enableAux;
}

- (BOOL)onGetEnablePreviewMirror
{
    return self.enablePreviewMirror;
}

- (BOOL)onGetEnableCaptureMirror
{
    return self.enableCaptureMirror;
}

- (BOOL)onGetEnableLoopback
{
    return self.enableLoopback;
}

#pragma mark setter
- (void)setBeautifyFeature:(ZegoBeautifyFeature)beautifyFeature
{
    if (_beautifyFeature == beautifyFeature)
        return;
    
    _beautifyFeature = beautifyFeature;
    [[ZegoDemoHelper api] enableBeautifying:beautifyFeature];
}

- (void)setFilter:(ZegoFilter)filter
{
    if (_filter == filter)
        return;
    
    _filter = filter;
    [[ZegoDemoHelper api] setFilter:filter];
}

- (void)setUseFrontCamera:(BOOL)useFrontCamera
{
    if (_useFrontCamera == useFrontCamera)
        return;
    
    _useFrontCamera = useFrontCamera;
    [[ZegoDemoHelper api] setFrontCam:useFrontCamera];
}

- (void)setEnableMicrophone:(BOOL)enableMicrophone
{
    if (_enableMicrophone == enableMicrophone)
        return;
    
    _enableMicrophone = enableMicrophone;
    [[ZegoDemoHelper api] enableMic:enableMicrophone];
}

- (void)setEnableTorch:(BOOL)enableTorch
{
    if (_enableTorch == enableTorch)
        return;
    
    _enableTorch = enableTorch;
    [[ZegoDemoHelper api] enableTorch:enableTorch];
}

- (void)setEnableCamera:(BOOL)enableCamera
{
    if (_enableCamera == enableCamera)
        return;
    
    _enableCamera = enableCamera;
    [[ZegoDemoHelper api] enableCamera:enableCamera];
}

- (void)setEnableSpeaker:(BOOL)enableSpeaker
{
    if (_enableSpeaker == enableSpeaker)
        return;
    
    _enableSpeaker = enableSpeaker;
    [[ZegoDemoHelper api] enableSpeaker:enableSpeaker];
}

- (void)setEnableAux:(BOOL)enableAux
{
    if (_enableAux == enableAux)
        return;
    
    _enableAux = enableAux;
    [[ZegoDemoHelper api] enableAux:enableAux];
    
    if (enableAux == NO)
    {
        self.pPos = NULL;
        self.auxData = nil;
    }
}

- (void)setEnablePreviewMirror:(BOOL)enablePreviewMirror
{
    if (_enablePreviewMirror == enablePreviewMirror)
        return;
    
    _enablePreviewMirror = enablePreviewMirror;
    [[ZegoDemoHelper api] enablePreviewMirror:enablePreviewMirror];
}

- (void)setEnableCaptureMirror:(BOOL)enableCaptureMirror
{
    if (_enableCaptureMirror == enableCaptureMirror)
        return;
    
    _enableCaptureMirror = enableCaptureMirror;
    [[ZegoDemoHelper api] enableCaptureMirror:enableCaptureMirror];
}

- (void)setEnableLoopback:(BOOL)enableLoopback
{
    if (_enableLoopback == enableLoopback)
        return;
    
    _enableLoopback = enableLoopback;
    [[ZegoDemoHelper api] enableLoopback:enableLoopback];
    if (enableLoopback)
        [[ZegoDemoHelper api] setLoopbackVolume:50];
}

- (void)setAnchorConfig:(UIView *)publishView
{
    [[ZegoDemoHelper api] setAppOrientation:[UIApplication sharedApplication].statusBarOrientation];
 
    ZegoAVConfig *config = [ZegoSettings sharedInstance].currentConfig;
    
    if (UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation)
        && config.videoEncodeResolution.height > config.videoEncodeResolution.width)
    {
        // * adjust width/height for landscape
        config.videoEncodeResolution = CGSizeMake(config.videoEncodeResolution.height, config.videoEncodeResolution.width);
        config.videoCaptureResolution = config.videoEncodeResolution;
    }
    
    self.videoSize = config.videoEncodeResolution;
    
    int ret = [[ZegoDemoHelper api] setAVConfig:config];
    assert(ret);
    
    bool b = [[ZegoDemoHelper api] setFrontCam:self.useFrontCamera];
    assert(b);
    
    b = [[ZegoDemoHelper api] enableMic:self.enableMicrophone];
    assert(b);
    
    b = [[ZegoDemoHelper api] enableBeautifying:self.beautifyFeature];
    assert(b);
    
    b = [[ZegoDemoHelper api] setFilter:self.filter];
    assert(b);
    
    [self enablePreview:self.enablePreview LocalView:publishView];
    [[ZegoDemoHelper api] setPreviewViewMode:self.viewMode];
    
    if ([ZegoDemoHelper recordTime])
    {
        [[ZegoDemoHelper api] enablePreviewMirror:false];
        self.enablePreviewMirror = NO;
    }
}

- (void)enablePreview:(BOOL)enable LocalView:(UIView *)view
{
    if (enable && view)
    {
        [[ZegoDemoHelper api] setPreviewView:view];
        [[ZegoDemoHelper api] startPreview];
    }
    else
    {
        [[ZegoDemoHelper api] setPreviewView:nil];
        [[ZegoDemoHelper api] stopPreview];
    }
}

- (void)addFirstPlayViewConstraints:(UIView *)firstView containerView:(UIView *)containerView
{
    [containerView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"V:|[firstView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(firstView)]];
    [containerView addConstraints:[NSLayoutConstraint constraintsWithVisualFormat:@"H:|[firstView]|" options:0 metrics:nil views:NSDictionaryOfVariableBindings(firstView)]];
}

- (UIView *)getFirstViewInContainer:(UIView *)containerView
{
    for (UIView *subview in containerView.subviews)
    {
        if (CGRectGetWidth(subview.frame) == CGRectGetWidth(containerView.frame))
            return subview;
    }
    
    return nil;
}

- (void)addPlayViewConstraints:(UIView *)view containerView:(UIView *)containerView viewIndex:(NSUInteger)viewIndex
{
    if (viewIndex == 0)
        [self addFirstPlayViewConstraints:view containerView:containerView];
    else
    {
        NSUInteger xIndex = (viewIndex - 1) % self.subViewPerRow;
        NSUInteger yIndex = (viewIndex - 1) / self.subViewPerRow;
        
        CGFloat xToLeftConstraints = xIndex * (self.subViewSpace + self.subViewWidth) + self.subViewSpace;
        CGFloat yTobottomConstraints = yIndex * (self.subViewSpace + self.subViewHeight) + self.subViewSpace;
        
        NSLayoutConstraint *widthConstraints = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeWidth relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeWidth multiplier:1.0 constant:self.subViewWidth];
        NSLayoutConstraint *heightConstraints = [NSLayoutConstraint constraintWithItem:view attribute:NSLayoutAttributeHeight relatedBy:NSLayoutRelationEqual toItem:nil attribute:NSLayoutAttributeHeight multiplier:1.0 constant:self.subViewHeight];
        NSLayoutConstraint *leftConstraints = [NSLayoutConstraint constraintWithItem:containerView attribute:NSLayoutAttributeTrailing relatedBy:NSLayoutRelationEqual toItem:view attribute:NSLayoutAttributeTrailing multiplier:1.0 constant:xToLeftConstraints];
        NSLayoutConstraint *bottomConstraints = [NSLayoutConstraint constraintWithItem:containerView attribute:NSLayoutAttributeBottom relatedBy:NSLayoutRelationEqual toItem:view attribute:NSLayoutAttributeBottom multiplier:1.0 constant:yTobottomConstraints];
        
        [containerView addConstraints:@[widthConstraints, heightConstraints, leftConstraints, bottomConstraints]];
    }
}

- (NSUInteger)getViewIndex:(UIView *)view containerView:(UIView *)containerView
{
    if (CGRectGetWidth(view.frame) == CGRectGetWidth(containerView.frame) &&
        CGRectGetHeight(view.frame) == CGRectGetHeight(containerView.frame))
        return 0;
    else
    {
        CGFloat deltaHeight = CGRectGetHeight(containerView.frame) - CGRectGetMaxY(view.frame) - self.subViewSpace;
        CGFloat deltaWidth = CGRectGetWidth(containerView.frame) - CGRectGetMaxX(view.frame) - self.subViewSpace;
        
        NSUInteger xIndex = deltaWidth / (self.subViewSpace + self.subViewWidth);
        NSUInteger yIndex = deltaHeight / (self.subViewSpace + self.subViewHeight);
        
        return yIndex * self.subViewPerRow + xIndex + 1;
    }
}

- (void)updateContainerConstraintsForTap:(UIView *)tapView containerView:(UIView *)containerView
{
    UIView *bigView = [self getFirstViewInContainer:containerView];
    if (bigView == tapView || tapView == nil)
        return;
    
    NSUInteger tapIndex = [self getViewIndex:tapView containerView:containerView];
    [containerView removeConstraints:containerView.constraints];
    [containerView exchangeSubviewAtIndex:0 withSubviewAtIndex:tapIndex];
    
    for (int i = 0; i < containerView.subviews.count; i++)
    {
        UIView *view = containerView.subviews[i];
        [self addPlayViewConstraints:view containerView:containerView viewIndex:i];
    }
    
    [UIView animateWithDuration:0.1 animations:^{
        [self.view layoutIfNeeded];
    }];
    
}

- (void)updateContainerConstraintsForRemove:(UIView *)removeView containerView:(UIView *)containerView
{
    if (removeView == nil)
        return;
    
    NSUInteger removeIndex = [self getViewIndex:removeView containerView:containerView];
    [containerView removeConstraints:containerView.constraints];
    
    for (UIView *view in containerView.subviews)
    {
        NSUInteger viewIndex = [self getViewIndex:view containerView:containerView];
        if (viewIndex == 0 && removeIndex != 0)
            [self addFirstPlayViewConstraints:view containerView:containerView];
        else if (viewIndex > removeIndex)
            [self addPlayViewConstraints:view containerView:containerView viewIndex:viewIndex - 1];
        else if (viewIndex < removeIndex)
            [self addPlayViewConstraints:view containerView:containerView viewIndex:viewIndex];
    }
    
    [removeView removeFromSuperview];
    [UIView animateWithDuration:0.1 animations:^{
        [self.view layoutIfNeeded];
    }];
}

- (BOOL)setContainerConstraints:(UIView *)view containerView:(UIView *)containerView viewCount:(NSUInteger)viewCount
{
    [self addPlayViewConstraints:view containerView:containerView viewIndex:viewCount];
    
    [UIView animateWithDuration:0.1 animations:^{
        [self.view layoutIfNeeded];
    }];
    
    return YES;
}

- (BOOL)isDeviceiOS7
{
    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0)
        return YES;
    
    return NO;
}

- (void)showPublishOption
{
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    ZegoAnchorOptionViewController *optionController = (ZegoAnchorOptionViewController *)[storyboard instantiateViewControllerWithIdentifier:@"anchorOptionID"];
    
    optionController.delegate = self;
    
    self.definesPresentationContext = YES;
    if (![self isDeviceiOS7])
        optionController.modalPresentationStyle = UIModalPresentationOverCurrentContext;
    else
        optionController.modalPresentationStyle = UIModalPresentationCurrentContext;
    
    optionController.view.backgroundColor = [UIColor clearColor];
    [self presentViewController:optionController animated:YES completion:nil];
}

- (void)showLogViewController
{
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    UINavigationController *navigationController = (UINavigationController *)[storyboard instantiateViewControllerWithIdentifier:@"logNavigationID"];
    
    ZegoLogTableViewController *logViewController = (ZegoLogTableViewController *)[navigationController.viewControllers firstObject];
    logViewController.logArray = self.logArray;
    
    [self presentViewController:navigationController animated:YES completion:nil];
}

- (void)onShowStaticsViewController:(UIGestureRecognizer *)gesture
{
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    UINavigationController *navigationController = (UINavigationController *)[storyboard instantiateViewControllerWithIdentifier:@"logNavigationID"];
    
    ZegoLogTableViewController *logViewController = (ZegoLogTableViewController *)[navigationController.viewControllers firstObject];
    logViewController.logArray = self.staticsArray;
    
    [self presentViewController:navigationController animated:YES completion:nil];
}

- (void)sendRequestPublishRespond:(BOOL)agreed seq:(int)seq requestPublisher:(ZegoUser *)requestUser
{
    [[ZegoDemoHelper api] respondJoinLiveReq:seq result:(agreed == false)];
}

- (BOOL)shouldShowPublishAlert
{
    return YES;
}

- (void)continueOtherRequest
{
    if ([self isDeviceiOS7])
    {
        if (self.requestAlertContextList.count == 0)
            return;
        
        if ([self shouldShowPublishAlert])
        {
            NSDictionary *dict = [self.requestAlertContextList firstObject];
            UIAlertView *alertView = dict[@"AlertView"];
            [alertView show];
        }
        else
        {
            for (NSDictionary *dict in self.requestAlertContextList)
                [self sendRequestPublishRespond:NO seq:[dict[@"Magic"] intValue] requestPublisher:dict[@"User"]];
            
            [self.requestAlertContextList removeAllObjects];
        }
    }
    else
    {
        if (self.requestAlertList.count == 0)
            return;
        
        if ([self shouldShowPublishAlert])
        {
            NSDictionary *dict = [self.requestAlertList firstObject];
            UIAlertController *alertController = dict[@"AlertController"];
            [self presentViewController:alertController animated:YES completion:nil];
        }
        else
        {
            for (NSDictionary *dict in self.requestAlertList)
            {
                [self sendRequestPublishRespond:NO seq:[dict[@"Magic"] intValue] requestPublisher:dict[@"User"]];
            }
            
            [self.requestAlertList removeAllObjects];
        }
    }
}

- (NSUInteger)getWaitingRequestListIndex:(int)seq
{
    if ([self isDeviceiOS7])
    {
        for (NSDictionary *dict in self.requestAlertContextList)
        {
            if ([dict[@"Magic"] intValue] == seq)
                return [self.requestAlertContextList indexOfObject:dict];
        }
        
        return NSNotFound;
    }
    else
    {
        for (NSDictionary *dict in self.requestAlertList)
        {
            if ([dict[@"Magic"] intValue] == seq)
                return [self.requestAlertList indexOfObject:dict];
        }
    }
    
    return NSNotFound;
}

- (void)requestPublishAlert:(ZegoUser *)requestUser seq:(int)seq
{
    NSString *message = [NSString stringWithFormat:NSLocalizedString(@"%@ 请求直播，是否允许", nil), requestUser.userName];
    if ([self isDeviceiOS7])
    {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"" message:message delegate:self cancelButtonTitle:NSLocalizedString(@"拒绝", nil) otherButtonTitles:NSLocalizedString(@"允许", nil), nil];
        NSDictionary *contextDictionary = @{@"Magic": @(seq), @"User": requestUser, @"AlertView": alertView};
        if (self.requestAlertContextList.count == 0)
            [alertView show];
        [self.requestAlertContextList addObject:contextDictionary];
    }
    else
    {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"" message:message preferredStyle:UIAlertControllerStyleAlert];
        
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:NSLocalizedString(@"拒绝", nil) style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            [self sendRequestPublishRespond:NO seq:seq requestPublisher:requestUser];
            
            NSUInteger index = [self getWaitingRequestListIndex:seq];
            if (index != NSNotFound)
                [self.requestAlertList removeObjectAtIndex:index];
            [self continueOtherRequest];
        }];
        
        UIAlertAction *okAction = [UIAlertAction actionWithTitle:NSLocalizedString(@"允许", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            [self sendRequestPublishRespond:YES seq:seq requestPublisher:requestUser];
            
            NSUInteger index = [self getWaitingRequestListIndex:seq];
            if (index != NSNotFound)
                [self.requestAlertList removeObjectAtIndex:index];
            [self continueOtherRequest];
        }];
        
        [alertController addAction:cancelAction];
        [alertController addAction:okAction];
        
        NSDictionary *contextDictionary = @{@"Magic": @(seq), @"User": requestUser, @"AlertController": alertController};
        [self.requestAlertList addObject:contextDictionary];
        
        if (![self.presentedViewController isKindOfClass:[UIAlertController class]])
        {
            [self.presentedViewController dismissViewControllerAnimated:YES completion:nil];
            
            [self presentViewController:alertController animated:YES completion:nil];
        }
    }
}

- (void)requestPublishResultAlert:(NSString *)fromUserName
{
    if (self.presentedViewController)
        [self.presentedViewController dismissViewControllerAnimated:YES completion:nil];
    
    NSString *message = [NSString stringWithFormat:NSLocalizedString(@"%@ 不允许连麦", nil), fromUserName];
    if ([self isDeviceiOS7])
    {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alertView show];
    }
    else
    {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"" message:message preferredStyle:UIAlertControllerStyleAlert];
        
        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
            
            [self continueOtherRequest];
        }];
        
        [alertController addAction:cancelAction];
        
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

- (void)onReceiveJoinLive:(NSString *)userId userName:(NSString *)userName seq:(int)seq
{
    ZegoUser *requestUser = [ZegoUser new];
    requestUser.userId = userId;
    requestUser.userName = userName;
    
    [self requestPublishAlert:requestUser seq:seq];
}

- (void)setIdelTimerDisable:(BOOL)disable
{
    [[UIApplication sharedApplication] setIdleTimerDisabled:disable];
}

- (NSString *)getCurrentTime
{
    NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
    formatter.dateFormat = @"[HH-mm-ss:SSS]";
    return [formatter stringFromDate:[NSDate date]];
}

- (void)addLogString:(NSString *)logString
{
    if (logString.length != 0)
    {
        NSString *totalString = [NSString stringWithFormat:@"%@: %@", [self getCurrentTime], logString];
        [self.logArray insertObject:totalString atIndex:0];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:@"logUpdateNotification" object:self userInfo:nil];
    }
}

- (void)addStaticsInfo:(BOOL)publish stream:(NSString *)streamID fps:(double)fps kbs:(double)kbs
{
    if (streamID.length == 0)
        return;
    
    NSString *totalString = [NSString stringWithFormat:@"%@: %@ fps %.3f, kbs %.3f", publish ? @"PUBS": @"PULL", streamID, fps, kbs];
    [self.staticsArray insertObject:totalString atIndex:0];
    
    [[NSNotificationCenter defaultCenter] postNotificationName:@"logUpdateNotification" object:self userInfo:nil];
}

#pragma mark UIAlertViewDelegate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    int seq = 0;
    ZegoUser *requestUser = nil;
    
    NSUInteger index = NSNotFound;
    for (NSDictionary *dict in self.requestAlertContextList)
    {
        if (dict[@"AlertView"] == alertView)
        {
            seq = [dict[@"magic"] intValue];
            requestUser = dict[@"User"];
            
            break;
        }
    }
    
    if (seq == 0 || requestUser == nil)
        return;
    
    if (buttonIndex == 0)
    {
        //cancel
        [self sendRequestPublishRespond:NO seq:seq requestPublisher:requestUser];
    }
    else if (buttonIndex == 1)
    {
        [self sendRequestPublishRespond:YES seq:seq requestPublisher:requestUser];
    }
    
    [self.requestAlertList removeObjectAtIndex:index];
    [self continueOtherRequest];
    
}

- (void)updateQuality:(int)quality view:(UIView *)playerView
{
    if (playerView == nil)
        return;
    
    CALayer *qualityLayer = nil;
    CATextLayer *textLayer = nil;
    
    for (CALayer *layer in playerView.layer.sublayers)
    {
        if ([layer.name isEqualToString:@"quality"])
            qualityLayer = layer;
        
        if ([layer.name isEqualToString:@"indicate"])
            textLayer = (CATextLayer *)layer;
    }
    
    int originQuality = 0;
    if (qualityLayer != nil)
    {
        if (CGColorEqualToColor(qualityLayer.backgroundColor, [UIColor greenColor].CGColor))
            originQuality = 0;
        else if (CGColorEqualToColor(qualityLayer.backgroundColor, [UIColor yellowColor].CGColor))
            originQuality = 1;
        else if (CGColorEqualToColor(qualityLayer.backgroundColor, [UIColor redColor].CGColor))
            originQuality = 2;
        else
            originQuality = 3;
        
        if (quality == originQuality)
            return;
    }
    
    UIFont *textFont = [UIFont systemFontOfSize:10];
    
    if (qualityLayer == nil)
    {
        qualityLayer = [CALayer layer];
        qualityLayer.name = @"quality";
        [playerView.layer addSublayer:qualityLayer];
        qualityLayer.frame = CGRectMake(12, 22, 10, 10);
        qualityLayer.contentsScale = [UIScreen mainScreen].scale;
        qualityLayer.cornerRadius = 5.0f;
    }
    
    if (textLayer == nil)
    {
        textLayer = [CATextLayer layer];
        textLayer.name = @"indicate";
        [playerView.layer addSublayer:textLayer];
        textLayer.backgroundColor = [UIColor clearColor].CGColor;
        textLayer.font = (__bridge CFTypeRef)textFont.fontName;
        textLayer.foregroundColor = [UIColor blackColor].CGColor;
        textLayer.fontSize = textFont.pointSize;
        textLayer.contentsScale = [UIScreen mainScreen].scale;
    }
    
    UIColor *qualityColor = nil;
    NSString *text = nil;
    if (quality == 0)
    {
        qualityColor = [UIColor greenColor];
        text = [NSString stringWithFormat:@"%@%@", NSLocalizedString(@"当前质量:", nil), NSLocalizedString(@"优", nil)];
    }
    else if (quality == 1)
    {
        qualityColor = [UIColor yellowColor];
        text = [NSString stringWithFormat:@"%@%@", NSLocalizedString(@"当前质量:", nil), NSLocalizedString(@"良", nil)];
    }
    else if (quality == 2)
    {
        qualityColor = [UIColor redColor];
        text = [NSString stringWithFormat:@"%@%@", NSLocalizedString(@"当前质量:", nil), NSLocalizedString(@"中", nil)];
    }
    else
    {
        qualityColor = [UIColor grayColor];
        text = [NSString stringWithFormat:@"%@%@", NSLocalizedString(@"当前质量:", nil), NSLocalizedString(@"差", nil)];
    }
    
    qualityLayer.backgroundColor = qualityColor.CGColor;
    CGSize textSize = [text sizeWithAttributes:@{NSFontAttributeName: textFont}];
    CGRect textFrame = CGRectMake(CGRectGetMaxX(qualityLayer.frame) + 3, CGRectGetMinY(qualityLayer.frame) + (CGRectGetHeight(qualityLayer.frame) - ceilf(textSize.height))/2, ceilf(textSize.width), ceilf(textSize.height));
    textLayer.frame = textFrame;
    textLayer.string = text;
}

- (void)auxCallback:(void *)pData dataLen:(int *)pDataLen sampleRate:(int *)pSampleRate channelCount:(int *)pChannelCount
{
    if (self.auxData == nil)
    {
        //初始化auxData
        NSURL *auxURL = [[NSBundle mainBundle] URLForResource:@"a.pcm" withExtension:nil];
        if (auxURL)
        {
            self.auxData = [NSData dataWithContentsOfURL:auxURL options:0 error:nil];
            self.pPos = (void *)[self.auxData bytes];
        }
    }
    
    if (self.auxData)
    {
        int nLen = (int)[self.auxData length];
        if (self.pPos == 0)
            self.pPos = (void *)[self.auxData bytes];
        
        const void *pAuxData = [self.auxData bytes];
        if (pAuxData == NULL)
            return;
        
        int nLeftLen = (int)(pAuxData + nLen - self.pPos);
        if (nLeftLen < *pDataLen) {
            self.pPos = (void *)pAuxData;
            *pDataLen = 0;
            return;
        }
        
        if (pSampleRate)
            *pSampleRate = 44100;
        
        if (pChannelCount)
            *pChannelCount = 2;
        
        memcpy(pData, self.pPos, *pDataLen);
        self.pPos = self.pPos + *pDataLen;
    }
}

- (NSString *)encodeStringAddingEscape:(NSString *)urlString
{
    return (NSString *)CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault, (__bridge CFStringRef)urlString, NULL, (CFStringRef)@"!*'();:@&=+$,/?%#[]", kCFStringEncodingUTF8));
}

- (void)shareToQQ:(NSString *)hls rtmp:(NSString *)rtmp bizToken:(NSString *)bizToken bizID:(NSString *)bizID streamID:(NSString *)streamID
{
#if TARGET_OS_SIMULATOR
#else
    
    NSString *encodeHls = [self encodeStringAddingEscape:hls];
    NSString *encodeRtmp = [self encodeStringAddingEscape:rtmp];
    NSString *encodeID = [self encodeStringAddingEscape:bizID];
    NSString *encodeStream = [self encodeStringAddingEscape:streamID];
    
    NSString *urlString = [NSString stringWithFormat:@"http://www.zego.im/share/index2?video=%@&rtmp=%@&id=%@&stream=%@",
                           encodeHls,
                           encodeRtmp,
                           encodeID,
                           encodeStream ];
    
    NSURL *url = [NSURL URLWithString:urlString];
    UIImage *logoImage = [UIImage imageNamed:@"zego"];
    NSData *previewImageData = UIImagePNGRepresentation(logoImage);
    
    NSString *title = @"LiveDemo";
    NSString *description = @"快来围观我的直播";
    
    QQApiURLObject *urlObject = [QQApiURLObject objectWithURL:url title:title description:description previewImageData:previewImageData targetContentType:QQApiURLTargetTypeNews];
    SendMessageToQQReq *req = [SendMessageToQQReq reqWithContent:urlObject];
    QQApiSendResultCode result = [QQApiInterface sendReq:req];
    
    NSLog(@"share To QQ URL: %@, result %d", urlString, result);

#endif
    
}


#pragma mark - ZegoLiveApiAudioRecordDelegate

- (void)onAudioRecord:(NSData *)audioData sampleRate:(int)sampleRate numOfChannels:(int)numOfChannels bitDepth:(int)bitDepth
{
    if (!self.recordedAudio)
    {
        self.recordedAudio = [NSMutableData data];
    }
    
    [self.recordedAudio appendData:audioData];
}

- (void)enableAudioRecord:(BOOL)enable
{
    //
    // * test audio record
    //
    [[ZegoDemoHelper api] enableAudioRecord:enable];
    if (enable)
    {
        [[ZegoDemoHelper api] setAudioRecordDelegate:self];
    }
    else
    {
        [[ZegoDemoHelper api] setAudioRecordDelegate:nil];
        
        if (self.recordedAudio)
        {
            NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
            NSString *cachesDir = [paths objectAtIndex:0];
            NSString *auidoFilePathname = [cachesDir stringByAppendingPathComponent:@"recored_audio"];
            
            [self.recordedAudio writeToFile:auidoFilePathname atomically:YES];
            self.recordedAudio = nil;
        }
    }
}

- (NSString *)encodeDictionaryToJSON:(NSDictionary *)dictionary
{
    if (dictionary == nil)
        return nil;
    
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dictionary options:0 error:nil];
    if (jsonData)
    {
        NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
        return jsonString;
    }
    
    return nil;
}

- (NSDictionary *)decodeJSONToDictionary:(NSString *)json
{
    if (json == nil)
        return nil;
    
    NSData *jsonData = [json dataUsingEncoding:NSUTF8StringEncoding];
    if (jsonData)
    {
        NSDictionary *dictionary = [NSJSONSerialization JSONObjectWithData:jsonData options:0 error:nil];
        return dictionary;
    }
    
    return nil;
}


@end
