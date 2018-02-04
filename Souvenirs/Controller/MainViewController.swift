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
    
    var firstImageView: DraggableImageView = {
        let iv = DraggableImageView()
        iv.backgroundColor = .white
        iv.isUserInteractionEnabled = true
        return iv
    }()
    
    @IBOutlet weak var imgViewTemp: UIImageView!
    @IBOutlet weak var constraintToolbarOffset: NSLayoutConstraint!
    @IBOutlet var contentView: UIView!
    
    @IBOutlet weak var butSticker: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        self.imgViewTemp.image = UIImage(named: (self.template?.imgPathBackground)!)
        
        self.picker = UIImagePickerController()
        self.picker?.delegate = self
        
        setupViews()
        
        // Initialize sticker panel
        showStickerPanel(show: false)
        
        //
        // Initialize for gesture
        //
        let pinchGesture = UIPinchGestureRecognizer(target: self, action: #selector(pinch))
        let rotationGesture = UIRotationGestureRecognizer(target: self, action: #selector(rotate))
        let panGesture = UIPanGestureRecognizer(target: self, action: #selector(pan))
        
        pinchGesture.delegate = self
        rotationGesture.delegate = self
        panGesture.delegate = self
        
        contentView.addGestureRecognizer(pinchGesture)
        contentView.addGestureRecognizer(rotationGesture)
        contentView.addGestureRecognizer(panGesture)
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
        
        showStickerPanel(show: butSticker.isSelected)
    }
    
    
    /// Show/Hide sticker panel
    ///
    /// - Parameter show: 
    func showStickerPanel(show: Bool) {
        if (show) {
            self.constraintToolbarOffset.constant = -232
        }
        else {
            self.constraintToolbarOffset.constant = -48
        }
    }
    
    @IBAction func onButCamera(_ sender: Any) {
        if(UIImagePickerController .isSourceTypeAvailable(UIImagePickerControllerSourceType.camera)) {
            picker?.sourceType = UIImagePickerControllerSourceType.camera
            self.present(picker!, animated: true, completion: nil)
        } else {
            let alert  = UIAlertController(title: "Warning", message: "Camera is not available!", preferredStyle: .alert)
            alert.addAction(UIAlertAction(title: "OK", style: .default, handler: nil))
            self.present(alert, animated: true, completion: nil)
        }
    }
    
    @IBAction func onButGallery(_ sender: Any) {
        self.picker?.sourceType = UIImagePickerControllerSourceType.photoLibrary
        self.present(self.picker!, animated: true, completion: nil)
    }
    
    // MARK: Image
    func setupViews() {
        self.contentView.insertSubview(firstImageView, at: 0)
        
        let firstImageWidth = self.view.frame.size.width / 2

        firstImageView.frame = CGRect(x: view.frame.midX - firstImageWidth / 2, y: view.frame.midY - firstImageWidth / 2, width: firstImageWidth, height: firstImageWidth)
    }
    
    // MARK: - Image Picker Process
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [String : Any]) {
        let chosenImage = info[UIImagePickerControllerOriginalImage] as! UIImage
        self.firstImageView.image  = sFunc_imageFixOrientation(img: chosenImage)
        self.firstImageView.contentMode = .scaleAspectFill
        dismiss(animated: true, completion: nil)
        
        // Make template semi-transparent
        makeImageTransparent(transparent: true)
    }
    
    func sFunc_imageFixOrientation(img:UIImage) -> UIImage {
        
        // No-op if the orientation is already correct
        if (img.imageOrientation == UIImageOrientation.up) {
            return img;
        }
        // We need to calculate the proper transformation to make the image upright.
        // We do it in 2 steps: Rotate if Left/Right/Down, and then flip if Mirrored.
        var transform:CGAffineTransform = CGAffineTransform.identity
        
        if (img.imageOrientation == UIImageOrientation.down
            || img.imageOrientation == UIImageOrientation.downMirrored) {
            
            transform = transform.translatedBy(x: img.size.width, y: img.size.height)
            transform = transform.rotated(by: CGFloat.pi)
        }
        
        if (img.imageOrientation == UIImageOrientation.left
            || img.imageOrientation == UIImageOrientation.leftMirrored) {
            
            transform = transform.translatedBy(x: img.size.width, y: 0)
            transform = transform.rotated(by: CGFloat.pi/2)
        }
        
        if (img.imageOrientation == UIImageOrientation.right
            || img.imageOrientation == UIImageOrientation.rightMirrored) {
            
            transform = transform.translatedBy(x: 0, y: img.size.height);
            transform = transform.rotated(by: -CGFloat.pi/2);
        }
        
        if (img.imageOrientation == UIImageOrientation.upMirrored
            || img.imageOrientation == UIImageOrientation.downMirrored) {
            
            transform = transform.translatedBy(x: img.size.width, y: 0)
            transform = transform.scaledBy(x: -1, y: 1)
        }
        
        if (img.imageOrientation == UIImageOrientation.leftMirrored
            || img.imageOrientation == UIImageOrientation.rightMirrored) {
            
            transform = transform.translatedBy(x: img.size.height, y: 0);
            transform = transform.scaledBy(x: -1, y: 1);
        }
        
        
        // Now we draw the underlying CGImage into a new context, applying the transform
        // calculated above.
        let ctx:CGContext = CGContext(data: nil, width: Int(img.size.width), height: Int(img.size.height),
                                      bitsPerComponent: img.cgImage!.bitsPerComponent, bytesPerRow: 0,
                                      space: img.cgImage!.colorSpace!,
                                      bitmapInfo: img.cgImage!.bitmapInfo.rawValue)!
        
        ctx.concatenate(transform)
        
        
        if (img.imageOrientation == UIImageOrientation.left
            || img.imageOrientation == UIImageOrientation.leftMirrored
            || img.imageOrientation == UIImageOrientation.right
            || img.imageOrientation == UIImageOrientation.rightMirrored
            ) {
            
            
            ctx.draw(img.cgImage!, in: CGRect(x:0,y:0,width:img.size.height,height:img.size.width))
            
        } else {
            ctx.draw(img.cgImage!, in: CGRect(x:0,y:0,width:img.size.width,height:img.size.height))
        }
        
        
        // And now we just create a new UIImage from the drawing context
        let cgimg:CGImage = ctx.makeImage()!
        let imgEnd:UIImage = UIImage(cgImage: cgimg)
        
        return imgEnd
    }
    
    // MARK: - Gestures
    
    @IBAction func pan(_ gesture: UIPanGestureRecognizer) {
        if firstImageView.image == nil {
            return
        }
        
        if (gesture.state == .began || gesture.state == .changed) {
            
            let translation = gesture.translation(in: self.view)
            let translatedCenter = CGPoint(x: CGFloat(firstImageView.center.x + translation.x), y: firstImageView.center.y + translation.y)

            firstImageView.center = translatedCenter
            
            gesture.setTranslation(CGPoint.zero, in: firstImageView)
        }
        
        makeImageTransparentGesture(state: gesture.state)
    }
    
    @IBAction func rotate(_ gesture: UIRotationGestureRecognizer) {
        firstImageView.transform = firstImageView.transform.rotated(by: gesture.rotation)
        gesture.rotation = 0
        
        makeImageTransparentGesture(state: gesture.state)
    }
    
    @IBAction func pinch(_ gesture: UIPinchGestureRecognizer) {
        if firstImageView.image == nil {
            return
        }
        
        firstImageView.transform = firstImageView.transform.scaledBy(x: gesture.scale, y: gesture.scale)
        gesture.scale = 1
        
        makeImageTransparentGesture(state: gesture.state)
    }
    
    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldRecognizeSimultaneouslyWith otherGestureRecognizer: UIGestureRecognizer) -> Bool {
        return true
    }
    
    
    /// Make template image view transparent
    ///
    /// - Parameter transparent:
    func makeImageTransparentGesture(state: UIGestureRecognizerState) {
        if state == .began {
            makeImageTransparent(transparent: true)
        }
        else if state == .ended {
            makeImageTransparent(transparent: false)
        }
    }
    
    func makeImageTransparent(transparent: Bool) {
        if (transparent) {
            self.imgViewTemp.alpha = 0.5
        }
        else {
            self.imgViewTemp.alpha = 1
        }
    }
    
}
