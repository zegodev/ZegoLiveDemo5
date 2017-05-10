//
//  ZegoSetTableViewController.m
//  LiveDemo3
//
//  Created by Strong on 16/6/22.
//  Copyright © 2016年 ZEGO. All rights reserved.
//

#import "ZegoSetTableViewController.h"
#import "ZegoAVKitManager.h"
#import "ZegoSettings.h"
#import "ZegoTestPullViewController.h"
#import "ZegoTestPushViewController.h"

@interface ZegoSetTableViewController () <UITextFieldDelegate>

@property (weak, nonatomic) IBOutlet UILabel *version;

@property (weak, nonatomic) IBOutlet UITextField *userID;
@property (weak, nonatomic) IBOutlet UITextField *userName;

@property (weak, nonatomic) IBOutlet UIPickerView *presetPicker;
@property (weak, nonatomic) IBOutlet UILabel *videoResolution;
@property (weak, nonatomic) IBOutlet UILabel *videoFrameRate;
@property (weak, nonatomic) IBOutlet UILabel *videoBitRate;
@property (weak, nonatomic) IBOutlet UISlider *videoResolutionSlider;
@property (weak, nonatomic) IBOutlet UISlider *videoFrameRateSlider;
@property (weak, nonatomic) IBOutlet UISlider *videoBitRateSlider;

@property (nonatomic, strong) UITapGestureRecognizer *tapGesture;

@end

@implementation ZegoSetTableViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    self.videoResolutionSlider.maximumValue = 5;
    
    self.tableView.contentInset = UIEdgeInsetsMake(0, 0, 44, 0);
    
    // 彩蛋
    UILongPressGestureRecognizer *longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(testPullAndPushStream:)];
    [self.tableView addGestureRecognizer:longPressGesture];
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    [self loadVideoSettings];
    [self loadAccountSettings];
}

- (void)viewWillDisappear:(BOOL)animated {
    if (self.userID.text.length != 0 && self.userName.text.length != 0)
    {
        [ZegoSettings sharedInstance].userID = self.userID.text;
        [ZegoSettings sharedInstance].userName = self.userName.text;
    }
    
    [super viewWillDisappear:animated];
}

#pragma mark - Event response

- (void)onTapTableView:(UIGestureRecognizer *)gesture
{
    [self.view endEditing:YES];
}

- (IBAction)sliderDidChange:(id)sender {
    [self.presetPicker selectRow:[ZegoSettings sharedInstance].presetVideoQualityList.count - 1 inComponent:0 animated:YES];
    
    ZegoAVConfig *config = [ZegoSettings sharedInstance].currentConfig;
    
    if (sender == self.videoResolutionSlider) {
        
        int v = (int)self.videoResolutionSlider.value;
        CGSize resolution = CGSizeMake(360, 640);
        switch (v)
        {
            case 0:
                resolution = CGSizeMake(240, 320);
                break;
            case 1:
                resolution = CGSizeMake(288, 352);
                break;
            case 2:
                resolution = CGSizeMake(360, 640);
                break;
            case 3:
                resolution = CGSizeMake(480, 640);
                break;
            case 4:
                resolution = CGSizeMake(720, 1280);
                break;
            case 5:
                resolution = CGSizeMake(1080, 1920);
                break;
                
            default:
                break;
        }
        config.videoEncodeResolution = resolution;
        config.videoCaptureResolution = resolution;
        
    } else if (sender == self.videoFrameRateSlider) {
        int v = (int)self.videoFrameRateSlider.value;
        config.fps = v;
    } else if (sender == self.videoBitRateSlider) {
        int v = (int)self.videoBitRateSlider.value;
        config.bitrate = v;
    }
    
    [ZegoSettings sharedInstance].currentConfig = config;
    [self updateViedoSettingUI];
}

#pragma mark -- Test egg

- (void)testPullAndPushStream:(UIGestureRecognizer *)gesture {
    if (gesture.state == UIGestureRecognizerStateEnded) {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"指定ID拉/推流", nil) message:nil preferredStyle:UIAlertControllerStyleActionSheet];
        
        UIAlertAction *testPullStream  = [UIAlertAction actionWithTitle:NSLocalizedString(@"指定ID拉流", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            [self testPullStream];
        }];
        
        UIAlertAction *testPushStream  = [UIAlertAction actionWithTitle:NSLocalizedString(@"指定ID推流", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
            [self testPushStream];
        }];
        
        UIAlertAction *cancel = [UIAlertAction actionWithTitle:NSLocalizedString(@"取消", nil) style:UIAlertActionStyleCancel handler:nil];
        
        [alertController addAction:testPullStream];
        [alertController addAction:testPushStream];
        [alertController addAction:cancel];
        
        [self presentViewController:alertController animated:YES completion:nil];
    }
}

- (void)testPullStream {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"指定ID拉流",nil)message:nil preferredStyle:UIAlertControllerStyleAlert];
    
    [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = NSLocalizedString(@"输入Room ID，不可为空",nil);
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
        textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
    }];
    
    [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = NSLocalizedString(@"输入Stream ID，不可为空",nil);
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
        textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
    }];
    
    UIAlertAction *confirm = [UIAlertAction actionWithTitle:NSLocalizedString(@"确定",nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {

        UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
        ZegoTestPullViewController *testViewController = (ZegoTestPullViewController *)[storyboard instantiateViewControllerWithIdentifier:@"testPullID"];
        testViewController.roomID = alertController.textFields[0].text;
        testViewController.streamID = alertController.textFields[1].text;
        
        [self presentViewController:testViewController animated:YES completion:nil];
    }];
    
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:NSLocalizedString(@"取消",nil) style:UIAlertActionStyleCancel handler:nil];
    
    [alertController addAction:confirm];
    [alertController addAction:cancel];
    
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)testPushStream {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"请选择推流模式",nil)message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    
    UIAlertAction *singleAnchorMode  = [UIAlertAction actionWithTitle:NSLocalizedString(@"无连麦模式", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [self pushStreamWithFlag:ZEGOAPI_SINGLE_ANCHOR];
    }];
    
    UIAlertAction *multiAnchorMode  = [UIAlertAction actionWithTitle:NSLocalizedString(@"连麦模式", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [self pushStreamWithFlag:ZEGOAPI_JOIN_PUBLISH];
    }];
    
    UIAlertAction *mixStreamMode  = [UIAlertAction actionWithTitle:NSLocalizedString(@"混流模式", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [self pushStreamWithFlag:ZEGOAPI_MIX_STREAM];
    }];
    
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:NSLocalizedString(@"取消", nil) style:UIAlertActionStyleCancel handler:nil];
    
    [alertController addAction:singleAnchorMode];
    [alertController addAction:multiAnchorMode];
    [alertController addAction:mixStreamMode];
    [alertController addAction:cancel];
    
    [self presentViewController:alertController animated:YES completion:nil];
}

- (void)pushStreamWithFlag:(ZegoAPIPublishFlag)flag {
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:NSLocalizedString(@"指定ID推流",nil)message:nil preferredStyle:UIAlertControllerStyleAlert];
    
    [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = NSLocalizedString(@"输入直播标题",nil);
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
        textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
    }];
    
    [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = NSLocalizedString(@"输入Room ID，不可为空",nil);
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
        textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
    }];
    
    [alertController addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = NSLocalizedString(@"输入Stream ID，不可为空",nil);
        textField.clearButtonMode = UITextFieldViewModeWhileEditing;
        textField.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
    }];
    
    UIAlertAction *confirm = [UIAlertAction actionWithTitle:NSLocalizedString(@"确定",nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
        ZegoTestPushViewController *testViewController = (ZegoTestPushViewController *)[storyboard instantiateViewControllerWithIdentifier:@"testPushID"];
        testViewController.liveTitle = [alertController.textFields[0].text isEqualToString:@""] ? @"指定ID推流测试" : alertController.textFields[0].text;
        testViewController.roomID = alertController.textFields[1].text;
        testViewController.streamID = alertController.textFields[2].text;
        testViewController.flag = flag;
        
        [self presentViewController:testViewController animated:YES completion:nil];
        
    }];
    
    UIAlertAction *cancel = [UIAlertAction actionWithTitle:NSLocalizedString(@"取消",nil) style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
    }];
    
    [alertController addAction:confirm];
    [alertController addAction:cancel];
    
    [self presentViewController:alertController animated:YES completion:nil];
}

#pragma mark - Private method

- (void)loadAccountSettings {
    self.userID.text = [ZegoSettings sharedInstance].userID;
    self.userName.text = [ZegoSettings sharedInstance].userName;
}

- (void)loadVideoSettings {
    self.version.text = [ZegoLiveRoomApi version];
    [self.presetPicker selectRow:[ZegoSettings sharedInstance].presetIndex inComponent:0 animated:YES];
    [self updateViedoSettingUI];
}

- (void)updateViedoSettingUI {
    ZegoAVConfig *config = [[ZegoSettings sharedInstance] currentConfig];
    
    CGSize r = [ZegoSettings sharedInstance].currentResolution;
    self.videoResolution.text = [NSString stringWithFormat:@"%d X %d", (int)r.width, (int)r.height];
    switch ((int)r.height) {
        case 320:
            self.videoResolutionSlider.value = 0;
            break;
        case 352:
            self.videoResolutionSlider.value = 1;
            break;
        case 640:
            if (r.width == 360) {
                self.videoResolutionSlider.value = 2;
            } else {
                self.videoResolutionSlider.value = 3;
            }
            break;
        case 1280:
            self.videoResolutionSlider.value = 4;
            break;
        case 1920:
            self.videoResolutionSlider.value = 5;
            break;
        default:
            break;
    }
    
    self.videoFrameRateSlider.value = config.fps;
    self.videoFrameRate.text = [NSString stringWithFormat:@"%d", config.fps];
    
    self.videoBitRateSlider.value = config.bitrate;
    self.videoBitRate.text = [NSString stringWithFormat:@"%d", config.bitrate];
}

- (void)showUploadAlertView
{
    NSString *message = [NSString stringWithFormat:NSLocalizedString(@"Log Uploaded", nil)];
    //    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0)
    {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alertView show];
    }
}

#pragma mark - UIPickerViewDelegate, UIPickerViewDataSource

- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    if (pickerView == self.presetPicker)
        return [ZegoSettings sharedInstance].presetVideoQualityList.count;
    else
        return 0;
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    if (pickerView == self.presetPicker)
    {
        if (row >= [ZegoSettings sharedInstance].presetVideoQualityList.count) {
            return ;
        }
        
        NSLog(@"%s: %@", __func__, [ZegoSettings sharedInstance].presetVideoQualityList[row]);
        
        [[ZegoSettings sharedInstance] selectPresetQuality:row];
        
        [self updateViedoSettingUI];
    }
    
    return;
}

//返回当前行的内容,此处是将数组中数值添加到滚动的那个显示栏上
-(NSString*)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    if (pickerView == self.presetPicker)
    {
        if (row >= [ZegoSettings sharedInstance].presetVideoQualityList.count) {
            return @"ERROR";
        }
    
        return [[ZegoSettings sharedInstance].presetVideoQualityList objectAtIndex:row];
    }
    return nil;
}

#pragma mark - UITableView delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    if (indexPath.section == 0 && indexPath.row == 1)
    {
        [ZegoLiveRoomApi uploadLog];
        [self showUploadAlertView];
    }
}


- (BOOL)tableView:(UITableView *)tableView shouldHighlightRowAtIndexPath:(NSIndexPath *)indexPath {
    if (indexPath.section == 3 || indexPath.section == 5)
        return YES;
    
    if (indexPath.section == 0 && indexPath.row == 1)
        return YES;
    return NO;
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    if (!self.userID.isEditing && !self.userName.isEditing)
        [self.view endEditing:YES];
}

#pragma mark - UITextField delegate

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    if (textField.text.length != 0)
    {
        [textField resignFirstResponder];
        return YES;
    }
    
    return NO;
}

- (void)textFieldDidBeginEditing:(UITextField *)textField
{
    if (self.tapGesture == nil)
        self.tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(onTapTableView:)];
    
    [self.tableView addGestureRecognizer:self.tapGesture];
}

- (void)textFieldDidEndEditing:(UITextField *)textField
{
    if (self.tapGesture)
    {
        [self.tableView removeGestureRecognizer:self.tapGesture];
        self.tapGesture = nil;
    }
}

@end
