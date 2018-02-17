//
//  LandingViewController.swift
//  Souvenirs
//
//  Created by Administrator on 2/2/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit

class LandingViewController: UIViewController {

    @IBOutlet weak var mLblTitle: UILabel!
    
    @IBOutlet weak var butStart: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        self.mLblTitle.adjustsFontSizeToFitWidth = true
        
        // check acvitation status
        let deviceId = UIDevice.current.identifierForVendor?.uuidString
        
        let userInfo = User.ref.child(User.TableName).child(deviceId!).queryLimited(toFirst: 1)
        userInfo.observe(.value) { snapshot in
            if snapshot.childrenCount > 0 {
                self.butStart.isEnabled = true
            }
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        // Hide navigation bar
        self.navigationController?.setNavigationBarHidden(true, animated: animated)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        
        // Hide navigation bar
        self.navigationController?.setNavigationBarHidden(false, animated: animated)
    }

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
