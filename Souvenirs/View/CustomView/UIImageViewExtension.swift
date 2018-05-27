//
//  UIImageViewExtension.swift
//  Souvenirs
//
//  Created by Administrator on 2/4/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit

public extension UIImage {
    public convenience init?(color: UIColor, size: CGSize = CGSize(width: 1, height: 1)) {
        let rect = CGRect(origin: .zero, size: size)
        UIGraphicsBeginImageContextWithOptions(rect.size, false, 0.0)
        color.setFill()
        UIRectFill(rect)
        let image = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        guard let cgImage = image?.cgImage else { return nil }
        self.init(cgImage: cgImage)
    }
    
    func crop( rect: CGRect) -> UIImage {
        var rect = rect
        rect.origin.x*=self.scale
        rect.origin.y*=self.scale
        rect.size.width*=self.scale
        rect.size.height*=self.scale
        
        let imageRef = self.cgImage!.cropping(to: rect)
        let image = UIImage(cgImage: imageRef!, scale: self.scale, orientation: self.imageOrientation)
        return image
    }
    
    func normalizedImage() -> UIImage? {
        if self.imageOrientation == .up {
            return self
        }
        
        UIGraphicsBeginImageContextWithOptions(self.size, false, self.scale)
        self.draw(in: CGRect(origin: CGPoint(x: 0, y: 0), size: self.size))
        let imgNorm = UIGraphicsGetImageFromCurrentImageContext()
        UIGraphicsEndImageContext()
        
        return imgNorm
    }
    
    func resized(withPercentage percentage: CGFloat) -> UIImage? {
        let canvasSize = CGSize(width: size.width * percentage, height: size.height * percentage)
        UIGraphicsBeginImageContextWithOptions(canvasSize, false, scale)
        defer { UIGraphicsEndImageContext() }
        draw(in: CGRect(origin: .zero, size: canvasSize))
        return UIGraphicsGetImageFromCurrentImageContext()
    }
    
    func resized(toWidth width: CGFloat) -> UIImage? {
        let canvasSize = CGSize(width: width, height: CGFloat(ceil(width/size.width * size.height)))
        UIGraphicsBeginImageContextWithOptions(canvasSize, false, scale)
        defer { UIGraphicsEndImageContext() }
        draw(in: CGRect(origin: .zero, size: canvasSize))
        return UIGraphicsGetImageFromCurrentImageContext()
    }
}

extension UIImageView {
    func frameForImageInImageViewAspectFit() -> CGRect
    {
        if  let img = self.image {
            let imageRatio = img.size.width / img.size.height;
            
            let viewRatio = self.frame.size.width / self.frame.size.height;
            
            if(imageRatio < viewRatio)
            {
                let scale = self.frame.size.height / img.size.height;
                
                let width = floor(scale * img.size.width);
                
                let topLeftX = floor((self.frame.size.width - width) * 0.5);
                
                return CGRect(x: topLeftX, y: 0, width: width, height: self.frame.size.height);
            }
            else
            {
                let scale = self.frame.size.width / img.size.width;
                
                let height = floor(scale * img.size.height);
                
                let topLeftY = floor((self.frame.size.height - height) * 0.5);
                
                return CGRect(x: 0, y: topLeftY, width: self.frame.size.width, height: height);
            }
        }
        
        return CGRect.zero
    }
}

