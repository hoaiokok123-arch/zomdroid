#import "ZomdroidGameViewController.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include <stdlib.h>
#include <string.h>

#include "zomdroid_ios_bridge.h"

enum {
    ZOMDROID_TOUCH_DOWN = 0,
    ZOMDROID_TOUCH_MOVE = 1,
    ZOMDROID_TOUCH_UP = 2
};

static NSString* zm_string_or_nil(id value) {
    return [value isKindOfClass:NSString.class] ? (NSString*)value : nil;
}

static NSArray<NSString*>* zm_array_or_empty(id value) {
    if (![value isKindOfClass:NSArray.class]) {
        return @[];
    }
    NSMutableArray<NSString*>* out = [NSMutableArray array];
    for (id item in (NSArray*)value) {
        if ([item isKindOfClass:NSString.class]) {
            [out addObject:(NSString*)item];
        }
    }
    return out;
}

static const char** zm_make_cstr_array(NSArray<NSString*>* values, int* out_count) {
    if (!values || values.count == 0) {
        *out_count = 0;
        return NULL;
    }

    int count = (int)values.count;
    const char** out = (const char**)calloc((size_t)count, sizeof(*out));
    if (!out) {
        *out_count = 0;
        return NULL;
    }
    for (int i = 0; i < count; i++) {
        out[i] = strdup(values[(NSUInteger)i].UTF8String);
        if (!out[i]) {
            for (int j = 0; j < i; j++) {
                free((void*)out[j]);
            }
            free((void*)out);
            *out_count = 0;
            return NULL;
        }
    }

    *out_count = count;
    return out;
}

static void zm_free_cstr_array(const char** values, int count) {
    if (!values) return;
    for (int i = 0; i < count; i++) {
        free((void*)values[i]);
    }
    free((void*)values);
}

@class ZomdroidGameViewController;

@interface ZomdroidSurfaceView : UIView
@property(nonatomic, weak) ZomdroidGameViewController* owner;
@end

@interface ZomdroidGameViewController ()
@property(nonatomic, strong) ZomdroidSurfaceView* surfaceView;
@property(nonatomic, strong) UILabel* statusLabel;
@property(nonatomic, strong) UIButton* startButton;
@property(nonatomic, strong) UIButton* stopButton;
@property(nonatomic, assign) BOOL started;
@property(nonatomic, assign) CGFloat inputScale;
- (void)handleTouchPhase:(int)phase point:(CGPoint)point;
@end

@implementation ZomdroidSurfaceView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (!self) return nil;
    self.multipleTouchEnabled = YES;
    self.backgroundColor = UIColor.blackColor;
    return self;
}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    (void)event;
    UITouch* touch = touches.anyObject;
    if (!touch) return;
    [self.owner handleTouchPhase:ZOMDROID_TOUCH_DOWN point:[touch locationInView:self]];
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    (void)event;
    UITouch* touch = touches.anyObject;
    if (!touch) return;
    [self.owner handleTouchPhase:ZOMDROID_TOUCH_MOVE point:[touch locationInView:self]];
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    (void)event;
    UITouch* touch = touches.anyObject;
    if (!touch) return;
    [self.owner handleTouchPhase:ZOMDROID_TOUCH_UP point:[touch locationInView:self]];
}

- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    (void)event;
    UITouch* touch = touches.anyObject;
    if (!touch) return;
    [self.owner handleTouchPhase:ZOMDROID_TOUCH_UP point:[touch locationInView:self]];
}

@end

@implementation ZomdroidGameViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = UIColor.blackColor;
    self.inputScale = UIScreen.mainScreen.scale;

    self.surfaceView = [[ZomdroidSurfaceView alloc] initWithFrame:CGRectZero];
    self.surfaceView.owner = self;
    [self.view addSubview:self.surfaceView];

    self.statusLabel = [[UILabel alloc] initWithFrame:CGRectZero];
    self.statusLabel.textColor = UIColor.whiteColor;
    self.statusLabel.numberOfLines = 0;
    self.statusLabel.font = [UIFont monospacedSystemFontOfSize:13 weight:UIFontWeightRegular];
    self.statusLabel.text = @"Zomdroid iOS ready.\nPlace config at Documents/zomdroid-launch.json";
    [self.view addSubview:self.statusLabel];

    self.startButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.startButton setTitle:@"Start" forState:UIControlStateNormal];
    self.startButton.backgroundColor = [UIColor colorWithWhite:0.15 alpha:0.8];
    self.startButton.tintColor = UIColor.whiteColor;
    self.startButton.layer.cornerRadius = 8;
    [self.startButton addTarget:self action:@selector(onStartPressed) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.startButton];

    self.stopButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [self.stopButton setTitle:@"Stop" forState:UIControlStateNormal];
    self.stopButton.backgroundColor = [UIColor colorWithWhite:0.15 alpha:0.8];
    self.stopButton.tintColor = UIColor.whiteColor;
    self.stopButton.layer.cornerRadius = 8;
    [self.stopButton addTarget:self action:@selector(onStopPressed) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:self.stopButton];
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];

    CGSize size = self.view.bounds.size;
    self.surfaceView.frame = self.view.bounds;
    self.statusLabel.frame = CGRectMake(16, 38, size.width - 32, 80);
    self.startButton.frame = CGRectMake(16, size.height - 62, 90, 44);
    self.stopButton.frame = CGRectMake(112, size.height - 62, 90, 44);

    if (self.started) {
        zomdroid_ios_set_surface(self.surfaceView);
    }
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (NSDictionary*)loadLaunchConfig {
    NSString* docs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    NSString* path = [docs stringByAppendingPathComponent:@"zomdroid-launch.json"];
    NSData* data = [NSData dataWithContentsOfFile:path];
    if (!data) {
        return @{};
    }
    NSError* err = nil;
    id json = [NSJSONSerialization JSONObjectWithData:data options:0 error:&err];
    if (err || ![json isKindOfClass:NSDictionary.class]) {
        self.statusLabel.text = [NSString stringWithFormat:@"Config parse error: %@", err.localizedDescription ?: @"invalid json"];
        return @{};
    }
    return (NSDictionary*)json;
}

- (void)onStartPressed {
    if (self.started) {
        self.statusLabel.text = @"Runtime already started.";
        return;
    }

    NSDictionary* cfg = [self loadLaunchConfig];
    NSString* docs = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES).firstObject;
    NSString* gameDir = zm_string_or_nil(cfg[@"game_dir"]);
    NSString* libraryDir = zm_string_or_nil(cfg[@"library_dir"]);
    NSString* mainClass = zm_string_or_nil(cfg[@"main_class"]);
    NSString* renderer = zm_string_or_nil(cfg[@"renderer"]);
    NSString* jvmLib = zm_string_or_nil(cfg[@"jvm_library_path"]);
    NSString* linkerLib = zm_string_or_nil(cfg[@"linker_library_path"]);

    if (!gameDir) gameDir = [docs stringByAppendingPathComponent:@"instances/default/game"];
    if (!libraryDir) libraryDir = [docs stringByAppendingPathComponent:@"runtime"];
    if (!mainClass) mainClass = @"zombie/gameStates/MainScreenState";
    if (!renderer) renderer = @"GL4ES";

    NSArray<NSString*>* jvmArgs = zm_array_or_empty(cfg[@"jvm_args"]);
    NSArray<NSString*>* gameArgs = zm_array_or_empty(cfg[@"game_args"]);
    NSArray<NSString*>* envVars = zm_array_or_empty(cfg[@"env"]);

    int jvmArgc = 0;
    int gameArgc = 0;
    int envVarc = 0;
    const char** jvmArgv = zm_make_cstr_array(jvmArgs, &jvmArgc);
    const char** gameArgv = zm_make_cstr_array(gameArgs, &gameArgc);
    const char** envVarv = zm_make_cstr_array(envVars, &envVarc);

    int initRes = zomdroid_ios_initialize_ex(
        gameDir.UTF8String,
        libraryDir.UTF8String,
        mainClass.UTF8String,
        renderer.UTF8String,
        jvmLib.UTF8String,
        linkerLib.UTF8String,
        jvmArgv, jvmArgc,
        gameArgv, gameArgc,
        envVarv, envVarc
    );

    zm_free_cstr_array(jvmArgv, jvmArgc);
    zm_free_cstr_array(gameArgv, gameArgc);
    zm_free_cstr_array(envVarv, envVarc);

    if (initRes != 0) {
        self.statusLabel.text = [NSString stringWithFormat:@"Init failed: %d", initRes];
        return;
    }

    int surfaceRes = zomdroid_ios_set_surface(self.surfaceView);
    if (surfaceRes != 0) {
        self.statusLabel.text = [NSString stringWithFormat:@"Surface setup failed: %d", surfaceRes];
        return;
    }

    int startRes = zomdroid_ios_start();
    if (startRes != 0) {
        self.statusLabel.text = [NSString stringWithFormat:@"Start failed: %d", startRes];
        return;
    }

    self.started = YES;
    self.statusLabel.text = @"Runtime started. Touch input mapped to LMB.";
}

- (void)onStopPressed {
    zomdroid_ios_shutdown();
    self.started = NO;
    self.statusLabel.text = @"Runtime stopped.";
}

- (void)handleTouchPhase:(int)phase point:(CGPoint)point {
    if (!self.started) return;
    double x = (double)point.x * self.inputScale;
    double y = (double)point.y * self.inputScale;
    zomdroid_ios_send_touch(phase, x, y);
}

@end

