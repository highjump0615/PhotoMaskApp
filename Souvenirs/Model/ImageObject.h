//
//  ImageObject.h
//  Souvenirs
//
//  Created by Administrator on 4/7/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface ImageObject : NSObject {
    unsigned char* mainBitmap;
    unsigned char* maskBitmap;
    CGSize mainSize;
}

@property (nonatomic) BOOL detectedFace;

- (id)initWithImage: (UIImage *)img;
- (UIImage *) process;
- (void) pushPoints: (NSArray *)points isBackground:(BOOL) bBg;

+ (void)initializeFaceDetect: (NSString *)resourcePath;

@end
