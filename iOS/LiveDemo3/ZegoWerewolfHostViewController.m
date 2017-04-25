//
//  ZegoWerewolfHostViewController.m
//  LiveDemo3
//
//  Created by Strong on 2017/3/29.
//  Copyright © 2017年 ZEGO. All rights reserved.
//

#import "ZegoWerewolfHostViewController.h"
#import "ZegoAVKitManager.h"
#import "ZegoSettings.h"


@interface ZegoWerewolfHostViewController () <ZegoRoomDelegate, ZegoLivePublisherDelegate, ZegoLivePlayerDelegate, ZegoIMDelegate>

@property (nonatomic, weak) IBOutlet UIButton *inTurnSpeakButton;
@property (nonatomic, weak) IBOutlet UIButton *endInTurnSpeakButton;
@property (nonatomic, weak) IBOutlet UILabel *tipsLabel;

@property (nonatomic, copy) NSString *roomID;

@property (nonatomic, strong) NSMutableArray<ZegoWerewolUserInfo *> *userList;

//主播控制座位号
@property (nonatomic, assign) NSUInteger currentIndex;
@property (nonatomic, strong) NSMutableArray<ZegoWerewolUserInfo *> *inturnSpekingList;

@property (nonatomic, assign) NSUInteger selfCharacter;

//说话时间计时
@property (nonatomic, strong) NSTimer *speakingTimer;
//主播时间计时
@property (nonatomic, strong) NSTimer *anchorTimer;
//当前说话模式
@property (nonatomic, assign) ZegoSpeakingMode speakingMode;

//当前说话座位号
@property (nonatomic, assign) NSUInteger currentSpeakingIndex;
@property (nonatomic, copy) NSString *currentSpeakingUserId;

@property (nonatomic, strong) NSTimer *sendStopSpeakingTimer;
@property (nonatomic, assign) BOOL isPublishing;
@property (nonatomic, assign) BOOL isSpeaking;

@end

@implementation ZegoWerewolfHostViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.

    _userList = [[NSMutableArray alloc] initWithCapacity:self.maxStreamCount];
    self.speakingMode = kFreeSpeakingMode;
    self.roomID = [ZegoDemoHelper getMyRoomID:WerewolfInTurnRoom];
    
    self.speakButton.enabled = NO;
    [self.speakButton setTitle:kStartSpeakingTitle forState:UIControlStateNormal];
    [self.speakButton setExclusiveTouch:YES];
    
    [self checkAudioAuthorizationWithResult:^(BOOL granted) {
        if (granted == NO)
            self.speakButton.enabled = NO;
    }];
    
    [self checkVideoAuthorizationWithResult:^(BOOL granted) {
        if (granted == NO)
            self.speakButton.enabled = NO;
    }];
    
    self.characterLabel.hidden = YES;
    
    self.endInTurnSpeakButton.enabled = NO;
    self.inTurnSpeakButton.enabled = NO;
    
    [self setupLiveKit];
    [self loginChatRoom];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark Action
- (IBAction)onCloseView:(id)sender
{
    if (self.speakingTimer)
    {
        [self onSpeakingTimer];
    }
    
    if (self.sendStopSpeakingTimer)
    {
        [self stopTimer:self.sendStopSpeakingTimer];
        [self onStopSpeakingTimer];
    }
    
    [self stopCurrentMode];
    [self.userList removeAllObjects];
    
    [[ZegoDemoHelper api] logoutRoom];
    
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)onStartTalking:(id)sender
{
    if (self.speakButton.enabled == NO)
        return;
    
    if (!self.isSpeaking)
    {
        //开始说活
        [self.speakButton setTitle:NSLocalizedString(@"结束说话", nil) forState:UIControlStateNormal];
        [self startTalking];
    }
    else
    {
        [self stopTalking];
    }
}

- (IBAction)onInturnButton:(id)sender
{
    self.inturnSpekingList = [NSMutableArray arrayWithArray:self.userList];
    
    self.speakButton.enabled = NO;
    
    [self stopTimer:self.sendStopSpeakingTimer];
    [self stopCurrentMode];
    
    NSDictionary *characters = [self randomCharacter];
    [self broadInTurnSpeaking:characters];
}

- (BOOL)isRandomNumberExist:(NSArray *)countArray number:(int)number
{
    for (NSNumber *character in countArray)
    {
        if ([character intValue] == number)
            return YES;
    }
    
    return NO;
}

- (NSDictionary *)randomCharacter
{
    NSUInteger count = self.inturnSpekingList.count;
    NSMutableDictionary *characters = [NSMutableDictionary dictionary];
    NSMutableArray *countArray = [NSMutableArray array];
    
    for (ZegoWerewolUserInfo *userInfo in self.inturnSpekingList)
    {
        int x = arc4random() % count + 1;
        while ([self isRandomNumberExist:countArray number:x])
        {
            x = arc4random() % count + 1;
        }
        
        [countArray addObject:@(x)];
        [characters setObject:@(x) forKey:userInfo.userId];
    }
    
    return characters;
}

- (IBAction)onEndInturnButton:(id)sender
{
    //每个人都可以推流
    [self.inturnSpekingList removeAllObjects];
    self.inturnSpekingList = nil;
    self.characterLabel.hidden = YES;
    self.speakButton.enabled = NO;
    
    [self stopTimer:self.sendStopSpeakingTimer];
    [self stopCurrentMode];
    
    if (self.speakingTimer)
    {
        [self.speakingTimer invalidate];
        self.speakingTimer = nil;
    }
    
    if (self.anchorTimer)
    {
        [self.anchorTimer invalidate];
        self.anchorTimer = nil;
    }
    
    self.currentSpeakingIndex = 0;
    [self broadFreeSpeaking];
}

#pragma mark speakButton
- (void)stopTimer:(NSTimer *)timer
{
    if (timer)
    {
        [timer invalidate];
        timer = nil;
    }
}

- (void)startTalking
{
    self.tipsLabel.text = NSLocalizedString(@"正在系统同步...", nil);
    
    [self stopTimer:self.sendStopSpeakingTimer];
    
    [[ZegoDemoHelper api] enableMic:NO];
    [[ZegoDemoHelper api] enableCamera:NO];
    [self doPublish];
}

- (void)stopTalking
{
    [[ZegoDemoHelper api] enableMic:NO];
    [[ZegoDemoHelper api] enableCamera:NO];
    [[ZegoDemoHelper api] setPreviewView:nil];
    
    ZegoWerewolUserInfo *userInfo = [self getSelfUserInfo];
    if (userInfo == nil)
        return;
    
    if (userInfo.streamId == nil)
    {
        //推流失败
    }
    else if (self.isPublishing)
    {
        //正在推流
        [self stopPublish];
        
        [self updateSpeakingButton:YES];
        self.tipsLabel.text = NSLocalizedString(@"推流中止", nil);
    }
    else
    {
        //推流成功，停止推流
        self.tipsLabel.text = NSLocalizedString(@"正在系统同步...", nil);
    
        self.speakButton.enabled = NO;
        self.sendStopSpeakingTimer = [NSTimer scheduledTimerWithTimeInterval:kPostSpeakingInterval target:self selector:@selector(onStopSpeakingTimer) userInfo:nil repeats:NO];
    }
}

- (void)updateSpeakingButton:(BOOL)enable
{
    self.speakButton.enabled = enable;
    [self.speakButton setTitle:kStartSpeakingTitle forState:UIControlStateNormal];
}

- (void)onStopSpeakingTimer
{
    if (self.speakingMode == kInTurnSpeakingMode)
    {
        [self onSpeakingTimer];
    }
    else if (self.speakingMode == kFreeSpeakingMode)
    {
        [self stopPublish];
        [self updateSpeakingButton:YES];
    }
    
    [self stopTimer:self.sendStopSpeakingTimer];
    
    if (self.speakingMode == kFreeSpeakingMode)
        self.tipsLabel.text = NSLocalizedString(@"当前模式:自由说话", nil);
    else if (self.speakingMode == kInTurnSpeakingMode)
        self.tipsLabel.text = NSLocalizedString(@"当前模式:轮流说话", nil);
}


#pragma mark Help function

- (NSArray<ZegoUser *> *)getCurrentMemberList
{
    NSMutableArray *memberList = [NSMutableArray arrayWithCapacity:self.userList.count];
    for (ZegoWerewolUserInfo *info in self.userList)
    {
        if ([info.userId isEqualToString:[ZegoSettings sharedInstance].userID])
            continue;
        
        ZegoUser *user = [ZegoUser new];
        user.userId = info.userId;
        user.userName = info.userName;
        
        [memberList addObject:user];
    }
    
    return memberList;
}

- (void)removeInTurnSpeakingUser:(NSString *)userId
{
    if (self.speakingMode == kFreeSpeakingMode)
        return;
    
    ZegoWerewolUserInfo *userInfo = nil;
    for (ZegoWerewolUserInfo *info in self.inturnSpekingList)
    {
        if ([userId isEqualToString:info.userId])
        {
            userInfo = info;
            break;
        }
    }
    
    if (userInfo != nil)
    {
        if (userInfo.index == self.currentSpeakingIndex && self.currentSpeakingIndex != 0)
        {
            [self resetPlayViewAndStop:userId];
            [self arrangeNextSpeaker];
        }
        
        [self.inturnSpekingList removeObject:userInfo];
    }
}

- (void)removeOldUser:(NSString *)userId broadcast:(BOOL)broadcast
{
    ZegoWerewolUserInfo *userInfo = [self getUserInfoByUserId:userId];
    if (userInfo != nil)
    {
        [self stopPlay:userInfo];
        
        [self removeInTurnSpeakingUser:userId];
        if (broadcast)
            [self broadUserLeave:userId];
        
        [self updateContainerConstraintsForRemove:userInfo.videoView];
        
        [self.userList removeObject:userInfo];
    }
    
    if (self.userList.count < 2)
    {
        if (self.inTurnSpeakButton.enabled)
        {
            [self onEndInturnButton:nil];
        }
        
        self.inTurnSpeakButton.enabled = NO;
        self.endInTurnSpeakButton.enabled = NO;
        
        //只有一个人时，切换为自由模式
        [self stopCurrentMode];
        self.speakingMode = kFreeSpeakingMode;
        [self updateSpeakingButton:YES];
        self.tipsLabel.text = NSLocalizedString(@"所有玩家都退出了", nil);
    }
}

- (void)setupLiveKit
{
    [[ZegoDemoHelper api] setRoomDelegate:self];
    [[ZegoDemoHelper api] setPlayerDelegate:self];
    [[ZegoDemoHelper api] setPublisherDelegate:self];
    [[ZegoDemoHelper api] setIMDelegate:self];
}

- (void)loginChatRoom
{
    self.tipsLabel.text = NSLocalizedString(@"开始登录房间", nil);
    
    [[ZegoDemoHelper api] setRoomConfig:NO userStateUpdate:YES];
    [[ZegoDemoHelper api] loginRoom:self.roomID roomName:self.liveTitle role:ZEGO_ANCHOR withCompletionBlock:^(int errorCode, NSArray<ZegoStream *> *streamList) {
        if (errorCode == 0)
        {
            self.tipsLabel.text = NSLocalizedString(@"登录房间成功", nil);
            
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"登录房间成功. roomId %@", nil), self.roomID];
            [self addLogString:logString];
            
            ZegoWerewolUserInfo *userInfo = [ZegoWerewolUserInfo new];
            userInfo.userId = [ZegoSettings sharedInstance].userID;
            userInfo.userName = [ZegoSettings sharedInstance].userName;
            userInfo.index = 1;
            
            self.currentIndex = 1;
            
            [self createPublishView:userInfo];
            [self.userList addObject:userInfo];
            
            self.speakButton.enabled = YES;
            self.tipsLabel.text = NSLocalizedString(@"当前模式:自由说话", nil);
        }
        else
        {
            self.tipsLabel.text = NSLocalizedString(@"登录房间失败", nil);
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"登录房间失败. error: %d", nil), errorCode];
            [self addLogString:logString];
        }
    }];
    
    [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"开始登录房间", nil)]];
}

- (void)createPublishView:(ZegoWerewolUserInfo *)userInfo
{
    [self createPlayView:userInfo];
    
    [self setAnchorConfig];
}

- (void)createPlayView:(ZegoWerewolUserInfo *)userInfo
{
    userInfo.videoView = [[UIView alloc] init];
    userInfo.videoView.translatesAutoresizingMaskIntoConstraints = NO;
    userInfo.videoView.backgroundColor = [UIColor colorWithWhite:0.667 alpha:0.5];
    [self.playViewContainer addSubview:userInfo.videoView];
    
    [self addNumber:userInfo.index toView:userInfo.videoView];
    [self addText:userInfo.userName toView:userInfo.videoView];
    
    [self.playViewContainer sendSubviewToBack:userInfo.videoView];
    
    [self setContainerConstraints:userInfo.videoView viewIndex:self.playViewContainer.subviews.count];
}

- (void)setAnchorConfig
{
    ZegoAVConfig *config = [[ZegoAVConfig alloc] init];
    config.videoEncodeResolution = self.videoSize;
    config.videoCaptureResolution = config.videoEncodeResolution;
    config.fps= 15;
    config.bitrate = self.bitrate;
    
    [[ZegoDemoHelper api] setAVConfig:config];
    [[ZegoDemoHelper api] enableBeautifying:ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_WHITEN];
    [[ZegoDemoHelper api] setPreviewViewMode:ZegoVideoViewModeScaleAspectFill];
    
    if ([ZegoDemoHelper recordTime])
    {
        [[ZegoDemoHelper api] enablePreviewMirror:false];
    }
}

- (ZegoWerewolUserInfo *)getSelfUserInfo
{
    for (ZegoWerewolUserInfo *userInfo in self.userList)
    {
        if ([userInfo.userId isEqualToString:[ZegoSettings sharedInstance].userID])
        {
            return userInfo;
        }
    }
    return nil;
}

- (ZegoWerewolUserInfo *)getUserInfoByUserId:(NSString *)userId
{
    for (ZegoWerewolUserInfo *userInfo in self.userList)
    {
        if ([userInfo.userId isEqualToString:userId])
            return userInfo;
    }
    
    return nil;
}

- (void)doPublish
{
    ZegoWerewolUserInfo *userInfo = [self getSelfUserInfo];
    if (userInfo == nil)
        return;
    
    userInfo.streamId = [ZegoDemoHelper getPublishStreamID];
    
    [[ZegoDemoHelper api] setWaterMarkImagePath:nil];
    [[ZegoDemoHelper api] setPreviewWaterMarkRect:CGRectMake(-1, -1, -1, -1)];
    [[ZegoDemoHelper api] setPublishWaterMarkRect:CGRectMake(-1, -1, -1, -1)];
    
    [[ZegoDemoHelper api] startPreview];
    [[ZegoDemoHelper api] setPreviewView:nil];
    
    bool b = [[ZegoDemoHelper api] startPublishing:userInfo.streamId title:self.liveTitle flag:self.isUrtralServer ? ZEGO_JOIN_PUBLISH : ZEGO_SINGLE_ANCHOR];
    if (b)
    {
        self.isPublishing = YES;
        self.isSpeaking = YES;
        [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"开始直播，流ID:%@", nil), userInfo.streamId]];
    }
}

- (void)stopPublish
{
    [[ZegoDemoHelper api] setPreviewView:nil];
    [[ZegoDemoHelper api] stopPreview];
    [[ZegoDemoHelper api] stopPublishing];
    
    self.isSpeaking = NO;
}

#pragma mark InTurnSpeaking
- (void)updatePlayView:(NSString *)userId
{
    for (ZegoWerewolUserInfo *userInfo in self.userList)
    {
        if ([userInfo.userId isEqualToString:userId])
            userInfo.videoView.backgroundColor = [UIColor colorWithWhite:0.33 alpha:0.5];
        else
            userInfo.videoView.backgroundColor = [UIColor colorWithWhite:0.667 alpha:0.5];
    }
}

- (void)resetPlayViewAndStop:(NSString *)userId
{
    ZegoWerewolUserInfo *userInfo = [self getUserInfoByUserId:userId];
    if (userInfo == nil)
        return;
    
    if ([userId isEqualToString:[ZegoSettings sharedInstance].userID])
    {
        [self stopPublish];
        userInfo.streamId = nil;
    }
    else
    {
        [self stopPlay:userInfo];
        userInfo.streamId = nil;
    }
    
    userInfo.videoView.backgroundColor = [UIColor colorWithWhite:0.667 alpha:0.5];
}

- (void)onAnchorSpeaking
{
    [self updateSpeakingButton:YES];
    
    self.speakingTimer = [NSTimer scheduledTimerWithTimeInterval:kSpeakingTimerInterval target:self selector:@selector(onSpeakingTimer) userInfo:nil repeats:NO];
    
    ZegoWerewolUserInfo *userInfo = [self getSelfUserInfo];
    if (userInfo == nil)
        return;
    
    [self updatePlayView:userInfo.userId];
    [self broadAllowSpeaking:userInfo.userId];
}

- (void)onSpeakingTimer
{
    [self stopTimer:self.speakingTimer];
    [self stopTimer:self.sendStopSpeakingTimer];
    
    [self resetPlayViewAndStop:[ZegoSettings sharedInstance].userID];
    [self reportStopSpeaking];
    
    [self updateSpeakingButton:NO];
}

- (void)onAnchorTimer
{
    if (self.anchorTimer)
    {
        [self.anchorTimer invalidate];
        self.anchorTimer = nil;
    }
    
    [self reportNoRespondStopSpeaking:self.currentSpeakingUserId];
    self.currentSpeakingUserId = nil;
    
    [self arrangeNextSpeaker];
}

#pragma mark message
- (void)broadAllowSpeaking:(NSString *)userId
{
    if (userId.length == 0)
        return;
    
    NSDictionary *infoDict = @{kSpeakingCommandKey: @(kAllowSpeaking), kSpeakingUserIdKey: userId};
    
    NSString *content = [self encodeDictionaryToJSON:infoDict];
    if (content == nil)
        return;
    
    [[ZegoDemoHelper api] sendCustomCommand:[self getCurrentMemberList] content:content completion:^(int errorCode, NSString *roomID) {
        
        [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"下一个说话: %@", nil), userId]];
        
        if (self.anchorTimer)
        {
            [self.anchorTimer invalidate];
            self.anchorTimer = nil;
        }
        
        self.anchorTimer = [NSTimer scheduledTimerWithTimeInterval:kAnchorTimerInterval target:self selector:@selector(onAnchorTimer) userInfo:nil repeats:NO];
    }];
}

- (void)reportStopSpeaking
{
    ZegoWerewolUserInfo *userInfo = [self getSelfUserInfo];
    if (userInfo == nil)
        return;
    
    NSDictionary *dict = @{kSpeakingCommandKey : @(kStopSpeaking), kSpeakingUserIdKey: [ZegoSettings sharedInstance].userID};
    NSString *content = [self encodeDictionaryToJSON:dict];
    if (content == nil)
        return;
    
    [[ZegoDemoHelper api] sendCustomCommand:[self getCurrentMemberList] content:content completion:^(int errorCode, NSString *roomID) {
        [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"结束说话", nil)]];
        
        //主播说完了，下一个人
        [self arrangeNextSpeaker];
        
    }];
}

- (void)reportNoRespondStopSpeaking:(NSString *)userId
{
    if (userId.length == 0)
        return;
    
    NSDictionary *dict = @{kSpeakingCommandKey : @(kStopSpeaking), kSpeakingUserIdKey: userId};
    NSString *content = [self encodeDictionaryToJSON:dict];
    if (content == nil)
        return;
    
    [[ZegoDemoHelper api] sendCustomCommand:[self getCurrentMemberList] content:content completion:^(int errorCode, NSString *roomID) {
        [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"结束%@说话", nil), userId]];
    }];
}

- (void)broadFreeSpeaking
{
    NSDictionary *dict = @{kSpeakingCommandKey : @(kFreeSpeaking)};
    NSString *content = [self encodeDictionaryToJSON:dict];
    if (content == nil)
        return;
    
    [[ZegoDemoHelper api] sendCustomCommand:[self getCurrentMemberList] content:content completion:^(int errorCode, NSString *roomID) {
        
        [self addLogString:[NSString stringWithFormat:@"Free Speaking"]];
        
        [self updateSpeakingButton:YES];
        self.inTurnSpeakButton.enabled = YES;
        self.endInTurnSpeakButton.enabled = NO;
        
        self.speakingMode = kFreeSpeakingMode;
        self.tipsLabel.text = NSLocalizedString(@"当前模式:自由说话", nil);
    }];
}

- (void)broadInTurnSpeaking:(NSDictionary<NSString *, NSNumber*>*)characters
{
    if (characters == nil)
        return;
    
    NSDictionary *dict = @{kSpeakingCommandKey : @(kInTurnSpeaking), kUserCharacterKey: characters};
    NSString *content = [self encodeDictionaryToJSON:dict];
    if (content == nil)
        return;
    
    [[ZegoDemoHelper api] sendCustomCommand:[self getCurrentMemberList] content:content completion:^(int errorCode, NSString *roomID) {
        [self addLogString:[NSString stringWithFormat:@"InTurn Speaking"]];
        
        self.characterLabel.hidden = NO;
        NSUInteger selfCharacter = [[characters objectForKey:[ZegoSettings sharedInstance].userID] unsignedIntegerValue];
        self.characterLabel.text = [NSString stringWithFormat:NSLocalizedString(@"角色:%lu", nil), selfCharacter];
        
        self.currentSpeakingIndex = 0;
        self.inTurnSpeakButton.enabled = NO;
        self.endInTurnSpeakButton.enabled = YES;
        
        self.tipsLabel.text = NSLocalizedString(@"当前模式:轮流说话", nil);
        self.speakingMode = kInTurnSpeakingMode;

        [self arrangeNextSpeaker];
    }];
}

- (void)broadUserLeave:(NSString *)userId
{
    if (userId.length == 0)
        return;
    
    NSDictionary *dict = @{kSpeakingCommandKey : @(kUserLeaveRoom), kUserIdKey: userId};
    NSString *content = [self encodeDictionaryToJSON:dict];
    if (content == nil)
        return;
    
    [[ZegoDemoHelper api] sendCustomCommand:[self getCurrentMemberList] content:content completion:^(int errorCode, NSString *roomID) {
        [self addLogString:[NSString stringWithFormat:@"user Leave Room"]];
    }];
}

- (void)answerRoomInfoAggreed:(NSString *)userId userName:(NSString *)userName
{
    self.currentIndex += 1;
    
    NSMutableArray *userContent = [NSMutableArray arrayWithCapacity:self.userList.count];
    for (ZegoWerewolUserInfo *info in self.userList)
    {
        [userContent addObject:[self serializeWolfUserInfo:info]];
    }
    
    if (userContent.count == 0)
        return;
    
    NSDictionary *dict = @{kSpeakingCommandKey: @(kAnswerRoomInfo), kUserIndexKey: @(self.currentIndex), kSpeakingModeKey: @(self.speakingMode), kCurrentUserListKey: userContent, kServerModeIndexKey : @(self.isUrtralServer)};
    NSString *content = [self encodeDictionaryToJSON:dict];
    if (content == nil)
        return;
    
    ZegoUser *user = [ZegoUser new];
    user.userId = userId;
    user.userName = userName;
    
    [[ZegoDemoHelper api] sendCustomCommand:@[user] content:content completion:^(int errorCode, NSString *roomID) {
        [self addLogString:[NSString stringWithFormat:@"Answer %@ about Room Info", userName]];
        
        ZegoWerewolUserInfo *userInfo = [ZegoWerewolUserInfo new];
        userInfo.userId = userId;
        userInfo.userName = userName;
        userInfo.index = self.currentIndex;
        
        [self createPlayView:userInfo];
        
        //通知其他人新用户加入
        NSDictionary *notifyDict = @{kSpeakingCommandKey: @(kNewUserInRoom), kNewUserKey: [self serializeWolfUserInfo:userInfo]};
        NSString *notifyContent = [self encodeDictionaryToJSON:notifyDict];
        if (notifyContent == nil)
            return;
        
        [[ZegoDemoHelper api] sendCustomCommand:[self getCurrentMemberList] content:notifyContent completion:^(int errorCode, NSString *roomID) {
            [self addLogString:[NSString stringWithFormat:@"notify others about %@", userName]];
        }];
        
        [self.userList addObject:userInfo];
        
        if (self.speakingMode == kFreeSpeakingMode)
        {
            if (self.userList.count >= 2 && self.inTurnSpeakButton.enabled == NO)
                self.inTurnSpeakButton.enabled = YES;
            
            self.tipsLabel.text = NSLocalizedString(@"当前模式:自由说话", nil);
        }
    }];
}

- (void)answerRoomInfo:(NSString *)userId userName:(NSString *)userName
{
    [self removeOldUser:userId broadcast:YES];
    
    return [self answerRoomInfoAggreed:userId userName:userName];
}

#pragma mark Stream
- (void)playInSmallView:(ZegoWerewolUserInfo *)userInfo
{
    if (userInfo.streamId.length != 0)
    {
        [[ZegoDemoHelper api] startPlayingStream:userInfo.streamId inView:userInfo.videoView];
        [[ZegoDemoHelper api] setViewMode:ZegoVideoViewModeScaleAspectFill ofStream:userInfo.streamId];
    }
}

- (void)playInBigView:(ZegoWerewolUserInfo *)userInfo
{
    if (userInfo.streamId.length != 0)
    {
        [[ZegoDemoHelper api] startPlayingStream:userInfo.streamId inView:self.bigPlayView];
        [[ZegoDemoHelper api] setViewMode:ZegoVideoViewModeScaleAspectFill ofStream:userInfo.streamId];
    }
}

- (void)stopPlayInSmallView:(ZegoWerewolUserInfo *)userInfo
{
    [self stopPlay:userInfo];
}

- (void)stopPlayInBigView:(ZegoWerewolUserInfo *)userInfo
{
    [self stopPlay:userInfo];
}

- (void)stopPlay:(ZegoWerewolUserInfo *)userInfo
{
    if (userInfo.streamId.length != 0)
    {
        [[ZegoDemoHelper api] updatePlayView:nil ofStream:userInfo.streamId];
        [[ZegoDemoHelper api] stopPlayingStream:userInfo.streamId];
    }
}

- (void)stopCurrentMode
{
    for (ZegoWerewolUserInfo *userInfo in self.userList)
    {
        if ([userInfo.userId isEqualToString:[ZegoSettings sharedInstance].userID])
            [self stopPublish];
        else
            [self stopPlay:userInfo];
        
        userInfo.streamId = nil;
        userInfo.videoView.backgroundColor = [UIColor colorWithWhite:0.667 alpha:0.5];
    }
    
    self.currentSpeakingIndex = 0;
}

#pragma mark dispatch
- (void)arrangeNextSpeaker
{
    if (self.anchorTimer)
    {
        [self.anchorTimer invalidate];
        self.anchorTimer = nil;
    }
    
    [self addLogString:[NSString stringWithFormat:@"current speaking Index: %lu", self.currentSpeakingIndex]];
    
    NSInteger minDelta = INT_MAX;
    NSUInteger index = 0;
    for (ZegoWerewolUserInfo *userInfo in self.inturnSpekingList)
    {
        if (userInfo.index > self.currentSpeakingIndex && userInfo.index - self.currentSpeakingIndex < minDelta)
        {
            minDelta = userInfo.index - self.currentSpeakingIndex;
            index = [self.inturnSpekingList indexOfObject:userInfo];
        }
    }
    
    ZegoWerewolUserInfo *userInfo = [self.inturnSpekingList objectAtIndex:index];
    self.currentSpeakingIndex = userInfo.index;
    self.currentSpeakingUserId = userInfo.userId;
    
    [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"下一个说话: %@", nil), userInfo.userName]];
    
    if ([userInfo.userId isEqualToString:[ZegoSettings sharedInstance].userID])
    {
        [self onAnchorSpeaking];
    }
    else
    {
        [self updatePlayView:userInfo.userId];
        [self broadAllowSpeaking:userInfo.userId];
    }
}

#pragma mark ZegoRoomDelegate
- (void)onDisconnect:(int)errorCode roomID:(NSString *)roomID
{
    NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"连接失败, error: %d", nil), errorCode];
    [self addLogString:logString];
}

- (void)onStreamUpdated:(int)type streams:(NSArray<ZegoStream *> *)streamList roomID:(NSString *)roomID
{
    if (type == ZEGO_STREAM_ADD)
        [self onStreamUpdateAdded:streamList];
    else if (type == ZEGO_STREAM_DELETE)
        [self onStreamUpdateDeleted:streamList];
}

- (void)onStreamUpdateAdded:(NSArray<ZegoStream *> *)streamList
{
    for (ZegoStream *stream in streamList)
    {
        ZegoWerewolUserInfo *userInfo = [self getUserInfoByUserId:stream.userID];
        if (userInfo == nil)
        {
            [[ZegoDemoHelper api] updatePlayView:nil ofStream:stream.streamID];
            [[ZegoDemoHelper api] stopPlayingStream:stream.streamID];
            
            continue;
        }
        
        userInfo.streamId = stream.streamID;
        if (self.speakingMode == kFreeSpeakingMode)
        {
            [self playInSmallView:userInfo];
        }
        else
        {
            [self playInBigView:userInfo];
        }
    }
}

- (void)onStreamUpdateDeleted:(NSArray<ZegoStream *> *)streamList
{
    for (ZegoStream *stream in streamList)
    {
        ZegoWerewolUserInfo *userInfo = [self getUserInfoByUserId:stream.userID];
        if (userInfo == nil)
            continue;
        
        if (![userInfo.streamId isEqualToString:stream.streamID])
        {
            [[ZegoDemoHelper api] updatePlayView:nil ofStream:stream.streamID];
            [[ZegoDemoHelper api] stopPlayingStream:stream.streamID];
        }
        
        if (self.speakingMode == kFreeSpeakingMode)
        {
            [self stopPlayInSmallView:userInfo];
        }
        else
        {
            [self stopPlayInBigView:userInfo];
        }
        userInfo.streamId = nil;
    }
}

- (void)onReceiveCustomCommand:(NSString *)fromUserID userName:(NSString *)fromUserName content:(NSString *)content roomID:(NSString *)roomID
{
    NSDictionary *dict = [self decodeJSONToDictionary:content];
    if (dict == nil)
        return;
    
    NSUInteger command = [dict[kSpeakingCommandKey] integerValue];
    
    if (command == kStopSpeaking)
    {
        NSString *userId = dict[kSpeakingUserIdKey];
        
        if (userId.length == 0 || [[ZegoSettings sharedInstance].userID isEqualToString:userId])
            return;
        
        [self resetPlayViewAndStop:userId];
        
        //安排下一个人说话
        [self arrangeNextSpeaker];
    }
    else if (command == kAskRoomInfo)
    {
        //收到请求房间信息
        [self answerRoomInfo:fromUserID userName:fromUserName];
    }
    else if (command == kUserLeaveRoom)
    {
        NSString *userId = dict[kSpeakingUserIdKey];
        
        if (userId.length == 0 || [[ZegoSettings sharedInstance].userID isEqualToString:userId])
            return;
        
        [self removeOldUser:userId broadcast:NO];
    }
    else
    {
        //主播不可能给自己发此消息
        [self addLogString:[NSString stringWithFormat:@"receive %@ info message %lu", fromUserName, (unsigned long)command]];
    }
    
}

#pragma mark ZegoLivePlayerDelegate
- (void)onPlayStateUpdate:(int)stateCode streamID:(NSString *)streamID
{
    NSLog(@"%s, streamID:%@", __func__, streamID);
    
    if (stateCode == 0)
    {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"播放流成功, 流ID:%@", nil), streamID];
        [self addLogString:logString];
    }
    else
    {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"播放流失败, 流ID:%@,  error:%d", nil), streamID, stateCode];
        [self addLogString:logString];
    }
}

- (void)onPlayQualityUpdate:(int)quality stream:(NSString *)streamID videoFPS:(double)fps videoBitrate:(double)kbs
{
    [self addStaticsInfo:NO stream:streamID fps:fps kbs:kbs];
}

- (void)onVideoSizeChangedTo:(CGSize)size ofStream:(NSString *)streamID
{
    NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"第一帧画面, 流ID:%@", nil), streamID];
    [self addLogString:logString];
}

#pragma mark ZegoLivePublishDelegate
- (void)onPublishStateUpdate:(int)stateCode streamID:(NSString *)streamID streamInfo:(NSDictionary *)info
{
    NSLog(@"%s, stream: %@", __func__, streamID);
    
    self.isPublishing = NO;
    if (stateCode == 0)
    {
        NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"发布直播成功,流ID:%@", nil), streamID];
        [self addLogString:logString];
        
        self.tipsLabel.text = NSLocalizedString(@"系统同步成功", nil);
        
        ZegoWerewolUserInfo *userInfo = [self getSelfUserInfo];
        if (userInfo == nil || ![userInfo.streamId isEqualToString:streamID])
            return;
        
        //出预览画面
        [[ZegoDemoHelper api] enableMic:YES];
        [[ZegoDemoHelper api] enableCamera:YES];
        
        if (self.speakingMode == kFreeSpeakingMode)
            [[ZegoDemoHelper api] setPreviewView:userInfo.videoView];
        else
            [[ZegoDemoHelper api] setPreviewView:self.bigPlayView];
    }
    else
    {
        ZegoWerewolUserInfo *userInfo = [self getSelfUserInfo];
        if (userInfo == nil || ![userInfo.streamId isEqualToString:streamID])
            return;
        
        //推流失败，streamId清空
        userInfo.streamId = nil;
        self.tipsLabel.text = NSLocalizedString(@"系统同步失败", nil);
        
        if (stateCode != 1)
        {
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"发布直播失败, 流ID:%@, err:%d", nil), streamID, stateCode];
            [self addLogString:logString];
        }
        else
        {
            NSString *logString = [NSString stringWithFormat:NSLocalizedString(@"发布直播结束, 流ID:%@", nil), streamID];
            [self addLogString:logString];
        }
        
        NSLog(@"%s, stream: %@, err: %u", __func__, streamID, stateCode);
    }
}

- (void)onPublishQualityUpdate:(int)quality stream:(NSString *)streamID videoFPS:(double)fps videoBitrate:(double)kbs
{
    [self addStaticsInfo:YES stream:streamID fps:fps kbs:kbs];
}

#pragma mark IM Delegate

- (BOOL)isUserShouldDelete:(NSArray<ZegoUserState *> *)userList userId:(NSString *)userId
{
    BOOL isExist = NO;
    for (ZegoUserState *state in userList)
    {
        if ([userId isEqualToString:state.userID])
        {
            isExist = YES;
            break;
        }
    }
    
    if (isExist)
        return NO;
    return YES;
}

- (void)onUserUpdate:(NSArray<ZegoUserState *> *)userList updateType:(ZegoUserUpdateType)type
{
    if (type == ZEGO_UPDATE_TOTAL)
    {
        NSMutableArray *deletedArray = [NSMutableArray array];
        for (ZegoWerewolUserInfo *userInfo in self.userList)
        {
            if ([userInfo.userId isEqualToString:[ZegoSettings sharedInstance].userID])
                continue;
            
            if ([self isUserShouldDelete:userList userId:userInfo.userId])
                [deletedArray addObject:userInfo];
        }
        
        for (ZegoWerewolUserInfo *userInfo in deletedArray)
        {
            [self removeOldUser:userInfo.userId broadcast:YES];
        }
        return;
    }
    
    for (ZegoUserState *state in userList)
    {
        if (state.role == ZEGO_ANCHOR && state.updateFlag == ZEGO_USER_DELETE)
        {
            //主播退出，关闭房间
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"" message:@"anchor is logout" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
            [alertView show];
            
            [self onCloseView:nil];
        }
        else if (state.updateFlag == ZEGO_USER_ADD)
        {
            [self addLogString:[NSString stringWithFormat:NSLocalizedString(@"%@进入房间,等待对方应答", nil),state.userName]];
        }
        else if (state.updateFlag == ZEGO_USER_DELETE)
        {
            [self removeOldUser:state.userID broadcast:YES];
        }
        
    }
}

@end
