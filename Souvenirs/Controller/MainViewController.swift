//
//  MainViewController.swift
//  Souvenirs
//
//  Created by Administrator on 2/3/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit

class MainViewController: UIViewController, UIGestureRecognizerDelegate, UIImagePickerControllerDelegate, UINavigationControllerDelegate {
    
    var template: Template?
    var picker : UIImagePickerController?
    
    @IBOutlet weak var imgViewTemp: UIImageView!
    @IBOutlet weak var constraintToolbarOffset: NSLayoutConstraint!
    
    @IBOutlet weak var butSticker: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        self.imgViewTemp.image = UIImage(named: (self.template?.imgPathBackground)!)
        
        self.picker = UIImagePickerController()
        self.picker?.delegate = self
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */
    
    @IBAction func onButSticker(_ sender: UIButton) {
        self.butSticker.isSelected = !self.butSticker.isSelected
        
        if (butSticker.isSelected) {
            constraintToolbarOffset.constant = -232
        }
        else {
            constraintToolbarOffset.constant = -48
        }
    }
    
    @IBAction func onButCamera(_ sender: Any) {
    }
    
    @IBAction func onButGallery(_ sender: Any) {
        self.picker?.sourceType = UIImagePickerControllerSourceType.photoLibrary
        self.present(self.picker!, animated: true, completion: nil)
    }
    
    // MARK: - Image Picker Process
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [String : Any]) {
        let chosenImage = info[UIImagePickerControllerOriginalImage] as! UIImage
//        self.firstImageView.image  = sFunc_imageFixOrientation(img: chosenImage)
//        self.firstImageView.contentMode = .scaleAspectFill
        dismiss(animated: true, completion: nil)
    }
    
}
