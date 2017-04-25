//
//  ZegoAVKitManager.m
//  LiveDemo
//
//  Copyright © 2015年 Zego. All rights reserved.
//

#include "ZegoAVKitManager.h"

static ZegoAVKitManager *avkitManager;

@interface ZegoAVKitManager () <ZegoRoomDelegate, ZegoLivePublisherDelegate>

@property (nonatomic, strong, readonly) ZegoLiveRoomApi *zegoLiveApi;

@property (nonatomic, copy) NSString *liveTitle;
@property (nonatomic, assign) BOOL requiredHardwareAccelerate;
@property (nonatomic, assign) BOOL testEnvironment;

@property (nonatomic, copy) NSString *userID;
@property (nonatomic, copy) NSString *userName;
@property (nonatomic, copy) NSString *liveChannel;
@property (nonatomic, copy) NSString *streamID;

@property (nonatomic, assign) CGSize videoSize;

@end

@implementation ZegoAVKitManager

+ (instancetype)sharedInstance
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        avkitManager = [[ZegoAVKitManager alloc] init];
    });
    
    return avkitManager;
}

- (instancetype)init
{
    self = [super init];
    if (self)
    {
        _requiredHardwareAccelerate = YES;
        _testEnvironment = NO;
        
        NSUserDefaults *sharedDefaults = [[NSUserDefaults alloc] initWithSuiteName:@"group.liveDemo5"];
        self.userID = [sharedDefaults stringForKey:@"userid"];
        self.userName = [sharedDefaults stringForKey:@"username"];
        if (self.userID.length == 0)
        {
            srand((unsigned)time(0));
            _userID = [NSString stringWithFormat:@"%u", (unsigned)rand()];
            [sharedDefaults setObject:_userID forKey:@"userid"];
            
#if TARGET_OS_SIMULATOR
            _userName = [NSString stringWithFormat:@"simulator-%@", _userID];
#else
            _userName = [NSString stringWithFormat:@"iphone-%@", _userID];
#endif
            [sharedDefaults setObject:_userName forKey:@"username"];
            
            NSLog(@"assign userID");
        }
        
        NSLog(@"userID %@", self.userID);
        NSLog(@"userName %@", self.userName);
        
        [self initZegoLiveApi];
    }
    
    return self;
}

- (void)initZegoLiveApi
{
    if (self.zegoLiveApi != nil)
        return;
    
    [ZegoLiveRoomApi setUseTestEnv:self.testEnvironment];
    [ZegoLiveRoomApi prepareReplayLiveCapture];
    
    NSData *appSign = [self getZegoAppSign];
    
    [ZegoLiveRoomApi setUserID:self.userID userName:self.userName];
    _zegoLiveApi = [[ZegoLiveRoomApi alloc] initWithAppID:1 appSignature:appSign];
    
    [ZegoLiveRoomApi requireHardwareDecoder:self.requiredHardwareAccelerate];
    [ZegoLiveRoomApi requireHardwareEncoder:self.requiredHardwareAccelerate];
    
//    [self.zegoLiveApi uploadLog];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidReceiveMemoryWarningNotification object:nil queue:[NSOperationQueue mainQueue] usingBlock:^(NSNotification * _Nonnull note) {
        NSLog(@"receive Memory warning");
    }];
}

- (NSData *)getZegoAppSign
{
    Byte signkey[] = {0x91,0x93,0xcc,0x66,0x2a,0x1c,0xe,0xc1,0x35,0xec,0x71,0xfb,0x7,0x19,0x4b,0x38,0x15,0xf1,0x43,0xf5,0x7c,0xd2,0xb5,0x9a,0xe3,0xdd,0xdb,0xe0,0xf1,0x74,0x36,0xd};
    
    return [NSData dataWithBytes:signkey length:32];
}

- (void)handleVideoInputSampleBuffer:(CMSampleBufferRef)sampleBuffer
{
    [self.zegoLiveApi handleVideoInputSampleBuffer:sampleBuffer];
}

- (void)handleAudioInputSampleBuffer:(CMSampleBufferRef)sampleBuffer withType:(RPSampleBufferType)sampleBufferType
{
    [self.zegoLiveApi handleAudioInputSampleBuffer:sampleBuffer withType:sampleBufferType];
}

- (void)startLiveWithTitle:(NSString *)liveTitle videoSize:(CGSize)videoSize
{
    if (liveTitle.length == 0)
        self.liveTitle = [NSString stringWithFormat:@"Hello-%@", self.userName];
    else
        self.liveTitle = liveTitle;
    
    self.videoSize = videoSize;
    NSLog(@"videoSize %@", NSStringFromCGSize(videoSize));
    
    [self loginChatRoom];
}

- (void)stopLive
{
    [self.zegoLiveApi stopPublishing];
    [self.zegoLiveApi logoutRoom];
}

- (void)loginChatRoom
{
    NSString *roomID = [NSString stringWithFormat:@"#d-%@", self.userID];
    [self.zegoLiveApi loginRoom:roomID role:ZEGO_ANCHOR withCompletionBlock:^(int errorCode, NSArray<ZegoStream *> *streamList) {
        if (errorCode != 0)
            NSLog(@"login room error %d", errorCode);
        else
        {
            ZegoAVConfig *config = [ZegoAVConfig new];
            config.videoEncodeResolution = CGSizeMake(640, 368);
            config.fps = 25;
            config.bitrate = 800000;
            [self.zegoLiveApi setAVConfig:config];
            
            unsigned long currentTime = (unsigned long)[[NSDate date] timeIntervalSince1970];
            self.streamID = [NSString stringWithFormat:@"s-%@-%lu", self.userID, currentTime];
            
            [self.zegoLiveApi startPublishing:self.streamID title:self.liveTitle flag:ZEGOAPI_SINGLE_ANCHOR];
        }
    }];
    
    NSLog(@"login Room %@", self.userName);
}


#pragma mark ZegoLivePublisherDelegate
- (void)onPublishStateUpdate:(int)stateCode streamID:(NSString *)streamID streamInfo:(NSDictionary *)info
{
    if (stateCode == 0)
        NSLog(@"publish success");
    else
        NSLog(@"publish failed %d", stateCode);
}


@end
