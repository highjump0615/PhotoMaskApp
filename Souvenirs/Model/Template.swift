//
//  Template.swift
//  Souvenirs
//
//  Created by Administrator on 2/2/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

class Template {
    
    var name: String
    var imgPathBackground: String?
    var imgPathThumb: String?
    
    init(name: String) {
        self.name = name
    }
    
    func thumbnail() -> String {
        return self.imgPathThumb == nil ? self.imgPathBackground! : self.imgPathThumb!
    }
}
