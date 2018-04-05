//
//  ActivationViewController.swift
//  Souvenirs
//
//  Created by Administrator on 2/17/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit
import SVProgressHUD

class ActivationViewController: UIViewController, UITextFieldDelegate {

    @IBOutlet weak var editName: UITextField!
    @IBOutlet weak var editEmail: UITextField!
    @IBOutlet weak var editStoreName: UITextField!
    @IBOutlet weak var editPhone: UITextField!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        addPaddingView(view: self.editName)
        addPaddingView(view: self.editEmail)
        addPaddingView(view: self.editStoreName)
        addPaddingView(view: self.editPhone)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    /// Add padding to text field
    ///
    /// - Parameter view: <#view description#>
    func addPaddingView(view: UITextField) {
        let paddingView: UIView = UIView.init(frame: CGRect(x: 0, y: 0, width: 5, height: 20))
        
        view.leftView = paddingView
        view.leftViewMode = .always
    }
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */
    
    @IBAction func onButActivate(_ sender: Any) {
        
        // check if they are empty
        if (self.editName.text!.isEmpty) {
            showAlert(title: "Input your name", description: "Name cannot be empty")
            return
        }
        if (self.editEmail.text!.isEmpty) {
            showAlert(title: "Input your email", description: "Email cannot be empty")
            return
        }
        if (self.editStoreName.text!.isEmpty) {
            showAlert(title: "Input your store name", description: "Store name cannot be empty")
            return
        }
        if (self.editPhone.text!.isEmpty) {
            showAlert(title: "Input your phone number", description: "Phone number cannot be empty")
            return
        }
        
        let deviceId = UIDevice.current.identifierForVendor?.uuidString
        if (deviceId ?? "").isEmpty {
            showAlert(title: "Activation failed", description: "Cannot get device UUID")
            return
        }

        // activation init
        let user = [
            User.KeyName: self.editName.text,
            User.KeyEmail: self.editEmail.text,
            User.KeyStoreName: self.editStoreName.text,
            User.KeyPhone: self.editPhone.text,
        ]
        
        let button = sender as! UIButton
        
        SVProgressHUD.show(withStatus: "Activating...")
        button.isEnabled = false
        
        User.ref.child(User.TableName + "/" + deviceId!).setValue(user, withCompletionBlock: { (error, ref) in
            SVProgressHUD.dismiss()
            
            // fail
            if error != nil {
                button.isEnabled = true
            }
            // sucess
            else {
                // save activate flag
                let defaluts = UserDefaults.standard
                defaluts.set(true, forKey: Common.KEY_ACTIVATION)
                
                self.performSegue(withIdentifier: "Activate2Select", sender: nil)
            }
        })
    }
    
    func showAlert(title: String, description: String?) {
        let alertController = UIAlertController(title: title,
                                                message: description,
                                                preferredStyle: .alert)
        let defaultAction = UIAlertAction(title: "OK", style: .default, handler: nil)
        alertController.addAction(defaultAction)
        
        self.present(alertController, animated: true, completion: nil)
    }
    
    // MARK: - UITextFieldDelegate
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        if textField == self.editName {
            self.editEmail.becomeFirstResponder()
        }
        else if textField == self.editEmail {
            self.editStoreName.becomeFirstResponder()
        }
        else if textField == self.editStoreName {
            self.editPhone.becomeFirstResponder()
        }
        else if textField == self.editPhone {
            textField.resignFirstResponder()
        }
        
        return true
    }
}
