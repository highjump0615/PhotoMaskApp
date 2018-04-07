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
    
    InitImage(mainBitmap, mainSize.width, mainSize.height);
    
    return self;
}

- (void) pushPints: (NSArray *)points {
    
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
