//
//  StickerCategory.swift
//  Souvenirs
//
//  Created by Administrator on 2/4/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

class StickerCategory {
    
    var iconName: String
    var name: String
    var count: Int

    /*
    // Only override draw() if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func draw(_ rect: CGRect) {
        // Drawing code
    }
    */
    
    init (icon: String, name: String, count: Int) {
        self.iconName = icon
        self.name = name
        self.count = count
    }

}
