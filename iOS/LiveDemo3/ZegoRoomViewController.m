//
//  ZegoRoomViewController.m
//  LiveDemo3
//
//  Created by Strong on 16/6/22.
//  Copyright © 2016年 ZEGO. All rights reserved.
//

#import "ZegoRoomViewController.h"
#import "ZegoRoomInfo.h"
#import "ZegoAVKitManager.h"
#import "ZegoAudienceViewController.h"
#import "ZegoMoreAudienceViewController.h"
#import "ZegoMixStreamAudienceViewController.h"
#import "ZegoRenderAudienceViewController.h"
#import <Foundation/NSURLSession.h>
#import "ZegoSettings.h"

@implementation ZegoRoomTableViewCell

@end

@interface ZegoRoomViewController () <UITableViewDataSource, UITableViewDelegate>
@property (weak, nonatomic) IBOutlet UITableView *liveView;

@property (nonatomic, strong) NSMutableArray<ZegoRoomInfo *>* roomList;

@property (nonatomic, strong) UIRefreshControl *refreshControl;

@end

@implementation ZegoRoomViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    _roomList = [NSMutableArray array];
    
    _refreshControl = [[UIRefreshControl alloc] init];
    [self.refreshControl addTarget:self action:@selector(handleRefresh:) forControlEvents:UIControlEventValueChanged];
    [self.liveView insertSubview:self.refreshControl atIndex:0];
    
    [self getLiveRoom];
    
    self.liveView.tableFooterView = [[UIView alloc] init];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onApplicationActive:) name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(onRoomInstanceClear:) name:@"RoomInstanceClear" object:nil];
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

- (void)refreshRoomList
{
    if ([self.refreshControl isRefreshing])
        return;
    
    [self.roomList removeAllObjects];
    [self getLiveRoom];
}

- (void)handleRefresh:(UIRefreshControl *)refreshControl
{
    [self.roomList removeAllObjects];
    [self getLiveRoom];
}

- (void)onApplicationActive:(NSNotification *)notification
{
    [self handleRefresh:self.refreshControl];
}

- (void)onRoomInstanceClear:(NSNotification *)notification
{
    [self getLiveRoom];
}

- (void)getLiveRoom
{
    [self.refreshControl beginRefreshing];
    
    NSString *baseUrl =@"https://test2-liveroom-api.zego.im";
    if(![ZegoDemoHelper usingTestEnv])
        baseUrl = @"https://liveroom1-api.zego.im";
    
    NSURL *URL = [NSURL URLWithString:[NSString stringWithFormat:@"%@/demo/roomlist?appid=%u", baseUrl, [ZegoDemoHelper appID]]];
    NSURLRequest *request = [NSURLRequest requestWithURL:URL];
    
    NSLog(@"URL %@", URL.absoluteString);
    
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
    configuration.timeoutIntervalForRequest = 10;
    
    NSURLSession *session = [NSURLSession sessionWithConfiguration:configuration];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:request completionHandler:^(NSData * _Nullable data, NSURLResponse * _Nullable response, NSError * _Nullable error) {
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
            if ([self.refreshControl isRefreshing])
                [self.refreshControl endRefreshing];
            
            if ([self.delegate respondsToSelector:@selector(onRefreshRoomListFinished)])
                [self.delegate onRefreshRoomListFinished];
            
            if (error)
            {
                NSLog(@"get live room error: %@", error);
                return;
            }
            
            if ([response isKindOfClass:[NSHTTPURLResponse class]])
            {
                NSError *jsonError;
                NSDictionary *jsonResponse = [NSJSONSerialization JSONObjectWithData:data options:0 error:&jsonError];
                if (jsonError)
                {
                    NSLog(@"parsing json error");
                    return;
                }
                else
                {
                    NSLog(@"%@", jsonResponse);
                    NSUInteger code = [jsonResponse[@"code"] integerValue];
                    if (code != 0)
                        return;
                    
                    NSArray *roomList = jsonResponse[@"data"][@"room_list"];
                    for (int idx = 0; idx < roomList.count; idx++)
                    {
                        ZegoRoomInfo *info = [ZegoRoomInfo new];
                        NSDictionary *infoDict = roomList[idx];
                        info.roomID = infoDict[@"room_id"];
                        if (info.roomID.length == 0)
                            continue;
                        info.anchorID = infoDict[@"anchor_id_name"];
                        info.anchorName = infoDict[@"anchor_nick_name"];
                        
                        [self.roomList addObject:info];
                    }
                    
                    [self.liveView reloadData];
                }
            }
        });
    }];
    
    [task resume];
}

#pragma mark UITableViewDataSource & UITableViewDelegate
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return self.roomList.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    ZegoRoomTableViewCell *cell = (ZegoRoomTableViewCell *)[tableView dequeueReusableCellWithIdentifier:@"roomListID" forIndexPath:indexPath];
    
    if (indexPath.row >= self.roomList.count)
        return cell;
    
    ZegoRoomInfo *info = self.roomList[indexPath.row];
    
    if (info.anchorName.length == 0)
        cell.publishTitleLabel.text = NSLocalizedString(@"Empty Title", nil);
    else
        cell.publishTitleLabel.text = info.anchorName;
    
//    if (info.livesCount > 1)
//    {
//        cell.livesCountLabel.text = [NSString stringWithFormat:NSLocalizedString(@"%lu人正在连麦", nil), (unsigned long)info.livesCount];
//        cell.livesCountLabel.hidden = NO;
//    }
//    else
    {
        cell.livesCountLabel.text = @"";
        cell.livesCountLabel.hidden = YES;
    }
    
    
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    if (indexPath.row >= self.roomList.count)
        return;
    
    ZegoRoomInfo *info = [self.roomList objectAtIndex:indexPath.row];
    
    UIViewController *controller = [[ZegoSettings sharedInstance] getViewControllerFromRoomID:info.roomID];
    if (controller)
        [self presentViewController:controller animated:YES completion:nil];
    
}

@end
