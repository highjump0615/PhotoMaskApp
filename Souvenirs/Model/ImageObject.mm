//
//  ImageObject.m
//  Souvenirs
//
//  Created by Administrator on 4/7/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

#import "ImageObject.h"
#import "ImageHelper.h"
#import "MainProc.h"
#import <vector>

@implementation ImageObject


/**
 Init image with UIImage
 */
- (id) initWithImage: (UIImage *)img {
    if (mainBitmap != NULL) {
        free(mainBitmap);
    }
    mainSize = img.size;
    mainBitmap = [ImageHelper convertUIImagetoBitmapRGB8:img];
    
    int nRes = InitImage(mainBitmap, mainSize.width, mainSize.height);
    self.detectedFace = nRes >= 0;
    
    return self;
}

- (void) pushPoints: (NSArray *)points isBackground:(BOOL) bBg {
    cv::Point pt;
    vector<cv::Point> aryPoint;
    
    if (points.count == 0) {
        return;
    }
    
    for (int i = 0; i < points.count; i++) {
        CGPoint point = [points[i] CGPointValue];
        
        pt.x = (int)point.x;
        pt.y = (int)point.y;
        aryPoint.push_back(pt);
    }
    
    PushPoints(aryPoint, bBg);
}

- (UIImage *) process {
    Mat resultImage = ApplyImage();
    UIImage *imgResult = [ImageHelper toImage:resultImage];
    
    return imgResult;
}

+ (void)initializeFaceDetect: (NSString *)resourcePath {
    [ImageHelper initializeFaceDetect:resourcePath];
}

@end
