//
//  TextStickerView.swift
//  Souvenirs
//
//  Created by Administrator on 5/18/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit

class TextStickerView: UIView {

    /*
    // Only override draw() if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func draw(_ rect: CGRect) {
        // Drawing code
    }
    */
    
    static func loadFromNib() -> UIView {
        
        //Detecting Xib name by device model
        let nib = (UIDevice.current.model == "iPad") ? "TextStickerView_iPad" : "TextStickerView_iPhone"
        
        //Loading the nib
        let v: UIView = UINib(nibName: nib, bundle: nil).instantiate(withOwner: self, options: nil)[0] as! UIView
        
        return v
    }

}
