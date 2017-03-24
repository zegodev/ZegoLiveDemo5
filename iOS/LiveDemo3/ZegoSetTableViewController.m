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
}

- (void)onTapTableView:(UIGestureRecognizer *)gesture
{
    [self.view endEditing:YES];
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

#pragma mark -- UIPickerViewDelegate, UIPickerViewDataSource
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    return [ZegoSettings sharedInstance].presetVideoQualityList.count;
}

- (void)pickerView:(UIPickerView *)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{
    if (row >= [ZegoSettings sharedInstance].presetVideoQualityList.count) {
        return ;
    }
    
    NSLog(@"%s: %@", __func__, [ZegoSettings sharedInstance].presetVideoQualityList[row]);
    
    [[ZegoSettings sharedInstance] selectPresetQuality:row];
    
    [self updateViedoSettingUI];
}

//返回当前行的内容,此处是将数组中数值添加到滚动的那个显示栏上
-(NSString*)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component
{
    if (row >= [ZegoSettings sharedInstance].presetVideoQualityList.count) {
        return @"ERROR";
    }
    
    return [[ZegoSettings sharedInstance].presetVideoQualityList objectAtIndex:row];
}


- (void)loadAccountSettings {
    self.userID.text = [ZegoSettings sharedInstance].userID;
    self.userName.text = [ZegoSettings sharedInstance].userName;
}

- (void)loadVideoSettings {
    self.version.text = [ZegoLiveRoomApi version];
    [self.presetPicker selectRow:[ZegoSettings sharedInstance].presetIndex inComponent:0 animated:YES];
    [self updateViedoSettingUI];
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

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    if (indexPath.section == 0 && indexPath.row == 1)
    {
        [ZegoLiveRoomApi uploadLog];
        [self showUploadAlertView];
    }
}

- (void)showUploadAlertView
{
    NSString *message = [NSString stringWithFormat:NSLocalizedString(@"Log Uploaded", nil)];
//    if ([[[UIDevice currentDevice] systemVersion] floatValue] < 8.0)
    {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
        [alertView show];
    }
    
    //ios9 bug， 下面这段代码会crash
//    else
//    {
//        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:@"" message:message preferredStyle:UIAlertControllerStyleAlert];
//        
//        UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction * _Nonnull action) {
//        }];
//        
//        [alertController addAction:cancelAction];
//        
//        [self presentViewController:alertController animated:YES completion:nil];
//    }
}

- (BOOL)tableView:(UITableView *)tableView shouldHighlightRowAtIndexPath:(NSIndexPath *)indexPath {
    if (indexPath.section == 3 || indexPath.section == 4)
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
