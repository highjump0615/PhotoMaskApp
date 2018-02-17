//
//  User.swift
//  Souvenirs
//
//  Created by Administrator on 2/17/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import Foundation
import FirebaseDatabase

class User {
    // table name
    static let TableName = "activation"
    
    // field names
    static let KeyName = "name"
    static let KeyEmail = "email"
    static let KeyStoreName = "storeName"
    static let KeyPhone = "phone"
    static let KeyDeviceId = "deviceId"
    
    static var ref: DatabaseReference {
        get {
            return Database.database().reference()
        }
    }
}
