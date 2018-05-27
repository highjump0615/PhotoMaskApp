//
//  TextStickerView.swift
//  Souvenirs
//
//  Created by Administrator on 5/18/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit
import ChromaColorPicker
import KMPlaceholderTextView

protocol TextStickerViewDelegate {
    func updatedTextColor(color: UIColor)
    func updatedTextSize(size: Int)
}

public protocol ChromaColorPickerDelegate {
    /* Called when the user taps the add button in the center */
    func colorPickerDidChooseColor(_ colorPicker: ChromaColorPicker, color: UIColor)
}

class TextStickerView: UIView {
    
    @IBOutlet weak var mViewColorPicker: UIView!
    @IBOutlet weak var mButTextSize: UIButton!
    @IBOutlet weak var mText: KMPlaceholderTextView!
    
    var delegate: TextStickerViewDelegate?
    
    var currentFontSize = 20
    
    /*
    // Only override draw() if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func draw(_ rect: CGRect) {
        // Drawing code
    }
    */
    
    static func loadFromNib() -> UIView {
        
        //Detecting Xib name by device model
//        let nib = (UIDevice.current.model == "iPad") ? "TextStickerView_iPad" : "TextStickerView_iPhone"
        let nib = "TextStickerView_iPhone"
        
        //Loading the nib
        let v: UIView = UINib(nibName: nib, bundle: nil).instantiate(withOwner: self, options: nil)[0] as! UIView
        
        return v
    }
    
    func initView(delegate: TextStickerViewDelegate) {
        // remove all subviews
        var neatColorPicker: ChromaColorPicker? = nil
        
        if !mViewColorPicker.subviews.isEmpty {
            neatColorPicker = mViewColorPicker.subviews[0] as? ChromaColorPicker
        }
        
        if neatColorPicker == nil {
            neatColorPicker = ChromaColorPicker()
            
            self.delegate = delegate
            mViewColorPicker.addSubview(neatColorPicker!)
            
            neatColorPicker?.padding = 0
            neatColorPicker?.stroke = 3
            
            neatColorPicker?.addTarget(self, action: #selector(textColorChanged), for: .valueChanged)
        }
        
        neatColorPicker?.frame = CGRect(x: 0, y: 0,
                                       width: mViewColorPicker.frame.width,
                                       height: mViewColorPicker.frame.height)
                
        neatColorPicker?.layout()
    }
    
    @IBAction func textColorChanged(sender: Any) {
        let colorPicker = sender as! ChromaColorPicker
        self.delegate?.updatedTextColor(color: colorPicker.currentColor)
    }
    
    func setTextDelegate(delgate: UITextViewDelegate) {
        mText.delegate = delgate
    }
    
    @IBAction func onButTextSize(_ sender: Any) {
        var pickerData = [[String: String]]()
        
        for i in 20...80 {
            pickerData.append(["value": String(i), "display": String(i)])
        }
        
        PickerDialog().show(title: "Font size", options: pickerData, selected: String(currentFontSize)) {
            (value) -> Void in
            
            // update button
            self.updateTextSizeBut(size: Int(value)!)

            self.delegate?.updatedTextSize(size: Int(value)!)
        }
    }
    
    /// Update text size button
    func updateTextSizeBut(size: Int) {
        currentFontSize = size
        self.mButTextSize.setTitle("Text Size: \(size)", for: .normal)
    }
    
    /// initialize text, size, color
    ///
    /// - Parameters:
    ///   - text: <#text description#>
    ///   - size: <#size description#>
    ///   - color: <#color description#>
    func initTextSizeColor(text:String, size: Int, color: UIColor) {
        mText.text = text
        updateTextSizeBut(size: size)

        let colorPicker = mViewColorPicker.subviews[0] as! ChromaColorPicker
        colorPicker.adjustToColor(color)
    }
}
