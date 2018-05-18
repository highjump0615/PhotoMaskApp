//
//  TextStickerView.swift
//  Souvenirs
//
//  Created by Administrator on 5/18/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit
import ChromaColorPicker

class TextStickerView: UIView {
    
    @IBOutlet weak var mViewColorPicker: UIView!
    
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
    
    func initView(delegate: ChromaColorPickerDelegate) {
        // remove all subviews
        for view in mViewColorPicker.subviews {
            view.removeFromSuperview()
        }
        
        let neatColorPicker = ChromaColorPicker(frame: CGRect(x: 0, y: 0,
                                                              width: mViewColorPicker.frame.width,
                                                              height: mViewColorPicker.frame.height))
        neatColorPicker.delegate = delegate //ChromaColorPickerDelegate
        mViewColorPicker.addSubview(neatColorPicker)
        
        neatColorPicker.padding = 0
        neatColorPicker.stroke = 3
        
        neatColorPicker.layout()
    }
    
    @IBAction func onButTextSize(_ sender: Any) {
        var pickerData = [[String: String]]()
        
        for i in 15...30 {
            pickerData.append(["value": String(i), "display": String(i)])
        }
        
        PickerDialog().show(title: "Distance units", options: pickerData, selected: "kilometer") {
            (value) -> Void in
            
            print("Unit selected: \(value)")
        }
    }
    
}
