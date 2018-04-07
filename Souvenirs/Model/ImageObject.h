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

- (id)initWithImage: (UIImage *)img;
- (UIImage *) process;

+ (void)initializeFaceDetect: (NSString *)resourcePath;

@end
