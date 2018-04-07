/*
 * The MIT License
 *
 * Copyright (c) 2011 Paul Solt, PaulSolt@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#import "ImageHelper.h"
#import <CoreGraphics/CGImage.h>
#import <CoreGraphics/CGBitmapContext.h>
#import "FaceDetect.h"
#import "LandmarkDetect.h"

#define Mask8(x) ( (x) & 0xFF )
#define R(x) ( Mask8(x) )
#define G(x) ( Mask8(x >> 8 ) )
#define B(x) ( Mask8(x >> 16) )

@implementation ImageHelper

+ (unsigned char *) convertUIImagetoBitmapRGB8:(UIImage *) image {
    CGImageRef c = image.CGImage;
    
    size_t bitsPerPixel = CGImageGetBitsPerPixel(c);
    size_t bitsPerComponent = CGImageGetBitsPerComponent(c);
    size_t width = CGImageGetWidth(c);
    size_t height = CGImageGetHeight(c);
    
//    NSAssert(bitsPerPixel == 32 && bitsPerComponent == 8 && a == kCGImageAlphaNoneSkipLast, @"unsupported image type supplied");
    
//    CGContextRef targetImage = CGBitmapContextCreate(NULL, width, height, 8, 1 * CGImageGetWidth(c), CGColorSpaceCreateDeviceGray(), kCGImageAlphaNone);
    
//    UInt32 *sourceData = (UInt32*)[((__bridge_transfer NSData*) CGDataProviderCopyData(CGImageGetDataProvider(c))) bytes];
    
    UInt32 *sourceData = (UInt32 *) calloc(height * width, sizeof(UInt32));
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(sourceData, width, height, bitsPerComponent, (bitsPerPixel / 8) * width, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    
    // 4.
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), c);
    
    // 5. Cleanup
    CGColorSpaceRelease(colorSpace);
    CGContextRelease(context);
    
    size_t bytesPerRow = width * 3;
    unsigned char *newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * bytesPerRow * height);
    
    uint offset;
    
    UInt32 * currentPixel = sourceData;
    
    for (uint y = 0; y < height; y++)
    {
        for (uint x = 0; x < width; x++)
        {
            offset = y * width + x;
            
            UInt32 color = *currentPixel;

            newBitmap[offset * 3 + 0] = B(color);
            newBitmap[offset * 3 + 1] = G(color);
            newBitmap[offset * 3 + 2] = R(color);
            
            currentPixel++;
        }
    }
    
    return newBitmap;
}

+ (unsigned char *) convertUIImageToBitmapRGBA8:(UIImage *) image {
	
	CGImageRef imageRef = image.CGImage;
	
	// Create a bitmap context to draw the uiimage into
	CGContextRef context = [self newBitmapRGBA8ContextFromImage:imageRef];
	
	if(!context) {
		return NULL;
	}
	
	size_t width = CGImageGetWidth(imageRef);
	size_t height = CGImageGetHeight(imageRef);
	
	CGRect rect = CGRectMake(0, 0, width, height);
	
	// Draw image into the context to get the raw image data
	CGContextDrawImage(context, rect, imageRef);
	
	// Get a pointer to the data	
	unsigned char *bitmapData = (unsigned char *)CGBitmapContextGetData(context);
	
	// Copy the data and release the memory (return memory allocated with new)
    size_t bytesPerRow = CGBitmapContextGetBytesPerRow(context);
	size_t bufferLength = bytesPerRow * height;
	
	unsigned char *newBitmap = NULL;
	
	if(bitmapData) {
		newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * bytesPerRow * height);
		
		if(newBitmap) {	// Copy the data
			for(int i = 0; i < bufferLength; ++i) {
				newBitmap[i] = bitmapData[i];
			}
		}
		
		free(bitmapData);
		
	} else {
		NSLog(@"Error getting bitmap pixel data\n");
	}
	
	CGContextRelease(context);
	
	return newBitmap;	
}

+ (CGContextRef) newBitmapRGBA8ContextFromImage:(CGImageRef) image {
	CGContextRef context = NULL;
	CGColorSpaceRef colorSpace;
	uint32_t *bitmapData;
	
	size_t bitsPerPixel = 32;
	size_t bitsPerComponent = 8;
	size_t bytesPerPixel = bitsPerPixel / bitsPerComponent;
	
	size_t width = CGImageGetWidth(image);
	size_t height = CGImageGetHeight(image);
	
	size_t bytesPerRow = width * bytesPerPixel;
	size_t bufferLength = bytesPerRow * height;
	
	colorSpace = CGColorSpaceCreateDeviceRGB();
	
	if(!colorSpace) {
		NSLog(@"Error allocating color space RGB\n");
		return NULL;
	}
	
	// Allocate memory for image data
	bitmapData = (uint32_t *)malloc(bufferLength);
	
	if(!bitmapData) {
		NSLog(@"Error allocating memory for bitmap\n");
		CGColorSpaceRelease(colorSpace);
		return NULL;
	}
	
	//Create bitmap context
	context = CGBitmapContextCreate(bitmapData, 
									width, 
									height, 
									bitsPerComponent, 
									bytesPerRow, 
									colorSpace,
//                                    kCGImageAlphaNone);
                                    kCGImageAlphaPremultipliedLast);	// RGBA
	
	if(!context) {
		free(bitmapData);
		NSLog(@"Bitmap context not created");
	}
	
	CGColorSpaceRelease(colorSpace);
	
	return context;	
}

+ (UIImage *) convertBitmapRGBA8ToUIImage:(unsigned char *) buffer 
								withWidth:(int) width
							   withHeight:(int) height {
	
	
	size_t bufferLength = width * height * 4;
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, buffer, bufferLength, NULL);
	size_t bitsPerComponent = 8;
	size_t bitsPerPixel = 32;
	size_t bytesPerRow = 4 * width;
	
	CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
	if(colorSpaceRef == NULL) {
		NSLog(@"Error allocating color space");
		CGDataProviderRelease(provider);
		return nil;
	}
	CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;
	CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
	
	CGImageRef iref = CGImageCreate(width, 
									height, 
									bitsPerComponent, 
									bitsPerPixel, 
									bytesPerRow, 
									colorSpaceRef, 
									bitmapInfo, 
									provider,	// data provider
									NULL,		// decode
									YES,			// should interpolate
									renderingIntent);
		
	uint32_t* pixels = (uint32_t*)malloc(bufferLength);
	
	if(pixels == NULL) {
		NSLog(@"Error: Memory not allocated for bitmap");
		CGDataProviderRelease(provider);
		CGColorSpaceRelease(colorSpaceRef);
		CGImageRelease(iref);		
		return nil;
	}
	
	CGContextRef context = CGBitmapContextCreate(pixels, 
												 width, 
												 height, 
												 bitsPerComponent, 
												 bytesPerRow, 
												 colorSpaceRef,
                                                 bitmapInfo);
	
	if(context == NULL) {
		NSLog(@"Error context not created");
		free(pixels);
	}
	
	UIImage *image = nil;
	if(context) {
		
		CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, width, height), iref);
		
		CGImageRef imageRef = CGBitmapContextCreateImage(context);
		
		// Support both iPad 3.2 and iPhone 4 Retina displays with the correct scale
		if([UIImage respondsToSelector:@selector(imageWithCGImage:scale:orientation:)]) {
			float scale = [[UIScreen mainScreen] scale];
			image = [UIImage imageWithCGImage:imageRef scale:scale orientation:UIImageOrientationUp];
		} else {
			image = [UIImage imageWithCGImage:imageRef];
		}
		
		CGImageRelease(imageRef);	
		CGContextRelease(context);	
	}
	
	CGColorSpaceRelease(colorSpaceRef);
	CGImageRelease(iref);
	CGDataProviderRelease(provider);
	
	if(pixels) {
		free(pixels);
	}	
	return image;
}

+ (UIImage *) convertBitmapRGB8ToUIImage:(unsigned char *) buffer
                                withWidth:(int) width
                               withHeight:(int) height {
    // buffer size is width * 3 * height;
    // BGR bitmap --> RGBA bitmap
    
    size_t bytesPerRow = width * 4;
    unsigned char *newBitmap = (unsigned char *)malloc(sizeof(unsigned char) * bytesPerRow * height);
    uint offset;
    
    for (uint y = 0; y < height; y++)
    {
        for (uint x = 0; x < width; x++)
        {
            offset = y * width + x;
            
            newBitmap[offset * 4 + 2] = buffer[offset * 3 + 0];
            newBitmap[offset * 4 + 1] = buffer[offset * 3 + 1];
            newBitmap[offset * 4 + 0] = buffer[offset * 3 + 2];
            newBitmap[offset * 4 + 3] = 0x79;
        }
    }
    
    /////////////////////////////////////////
    
    size_t bufferLength = width * height * 4;
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, newBitmap, bufferLength, NULL);
    size_t bitsPerComponent = 8;
    size_t bitsPerPixel = 32;
//    size_t bytesPerRow = 4 * width;
    
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    if(colorSpaceRef == NULL) {
        NSLog(@"Error allocating color space");
        CGDataProviderRelease(provider);
        return nil;
    }
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    
    CGImageRef iref = CGImageCreate(width,
                                    height,
                                    bitsPerComponent,
                                    bitsPerPixel,
                                    bytesPerRow,
                                    colorSpaceRef,
                                    bitmapInfo,
                                    provider,	// data provider
                                    NULL,		// decode
                                    YES,			// should interpolate
                                    renderingIntent);
    
    uint32_t* pixels = (uint32_t*)malloc(bufferLength);
    
    if(pixels == NULL) {
        NSLog(@"Error: Memory not allocated for bitmap");
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpaceRef);
        CGImageRelease(iref);
        return nil;
    }
    
    CGContextRef context = CGBitmapContextCreate(pixels,
                                                 width,
                                                 height,
                                                 bitsPerComponent,
                                                 bytesPerRow,
                                                 colorSpaceRef,
                                                 bitmapInfo);
    
    if(context == NULL) {
        NSLog(@"Error context not created");
        free(pixels);
    }
    
    UIImage *image = nil;
    if(context) {
        
        CGContextDrawImage(context, CGRectMake(0.0f, 0.0f, width, height), iref);
        
        CGImageRef imageRef = CGBitmapContextCreateImage(context);
        
        // Support both iPad 3.2 and iPhone 4 Retina displays with the correct scale
        if([UIImage respondsToSelector:@selector(imageWithCGImage:scale:orientation:)]) {
            float scale = [[UIScreen mainScreen] scale];
            image = [UIImage imageWithCGImage:imageRef scale:scale orientation:UIImageOrientationUp];
        } else {
            image = [UIImage imageWithCGImage:imageRef];
        }
        
        CGImageRelease(imageRef);	
        CGContextRelease(context);	
    }
    
    CGColorSpaceRelease(colorSpaceRef);
    CGImageRelease(iref);
    CGDataProviderRelease(provider);
    
    if(pixels) {
        free(pixels);
    }	
    return image;
}

+ (UIImage *)toImage: (cv::Mat) mat{
    int bitsPerComponent = 8; // 1
    int width = mat.cols;
    int height = mat.rows;
    int bytesPerPixel = 4;
    int bytesPerRow = width * bytesPerPixel;
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); // 2
    
    int bitmapInfo = kCGBitmapByteOrder32Big;
    bitmapInfo |= kCGImageAlphaPremultipliedLast & kCGBitmapAlphaInfoMask;
    
    CGContextRef imageContext = CGBitmapContextCreateWithData(mat.data, width, height, bitsPerComponent, bytesPerRow, colorSpace, bitmapInfo, nil, nil);
    
    CGImageRef cgImage = CGBitmapContextCreateImage(imageContext);
    if (cgImage == nil) {
        return nil;
    }
    
    UIImage *image = [[UIImage alloc] initWithCGImage:cgImage];
    return image;
}

+(UIImage *)UIImageFromCVMat:(cv::Mat)cvMat
{
    NSData *data = [NSData dataWithBytes:cvMat.data length:cvMat.elemSize()*cvMat.total()];
    CGColorSpaceRef colorSpace;
    
    if (cvMat.elemSize() == 1) {
        colorSpace = CGColorSpaceCreateDeviceGray();
    } else {
        colorSpace = CGColorSpaceCreateDeviceRGB();
    }
    
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
    
    // Creating CGImage from cv::Mat
    CGImageRef imageRef = CGImageCreate(cvMat.cols,                                 //width
                                        cvMat.rows,                                 //height
                                        8,                                          //bits per component
                                        8 * cvMat.elemSize(),                       //bits per pixel
                                        cvMat.step[0],                            //bytesPerRow
                                        colorSpace,                                 //colorspace
                                        kCGImageAlphaNone|kCGBitmapByteOrderDefault,// bitmap info
                                        provider,                                   //CGDataProviderRef
                                        NULL,                                       //decode
                                        false,                                      //should interpolate
                                        kCGRenderingIntentDefault                   //intent
                                        );
    
    // Getting UIImage from CGImage
    UIImage *finalImage = [UIImage imageWithCGImage:imageRef];
    CGImageRelease(imageRef);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    return finalImage;
}

+ (void)initializeFaceDetect: (NSString *)resourcePath {
    NSString *dataPath = [resourcePath stringByAppendingPathComponent:@""];
    const char *pszDataPath = [dataPath UTF8String];
    
    InitFaceDetector((char*)pszDataPath);
    InitRCPRModel((char*)pszDataPath);
}

@end
