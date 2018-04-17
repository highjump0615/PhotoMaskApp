//
//  MainViewController.swift
//  Souvenirs
//
//  Created by Administrator on 2/3/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit
import MessageUI

class MainViewController: UIViewController,
                        UIGestureRecognizerDelegate,
                        UIImagePickerControllerDelegate,
                        UINavigationControllerDelegate,
                        MFMailComposeViewControllerDelegate,
                        CHTStickerViewDelegate,
                        HumanImageDelegate {
    
    var template: Template?
    var picker : UIImagePickerController?
    
    var pinchGesture: UIPinchGestureRecognizer?
    var rotationGesture: UIRotationGestureRecognizer?
    var panGesture: UIPanGestureRecognizer?
    
    var rtImgWorkArea: CGRect?
    var firstImageView: DraggableImageView?
    
    var viewTopMask: UIView?
    var viewBottomMask: UIView?
    
    @IBOutlet weak var viewWork: UIView!
    @IBOutlet weak var imgViewTemp: UIImageView!
    @IBOutlet weak var constraintToolbarOffset: NSLayoutConstraint!
    @IBOutlet var contentView: UIView!
    @IBOutlet weak var panelView: UIView!
    
    @IBOutlet weak var butSticker: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // fill template image
        self.imgViewTemp.image = UIImage(named: (self.template?.imgPathBackground)!)
        
        self.picker = UIImagePickerController()
        self.picker?.delegate = self
        
        // Initialize sticker panel
        showStickerPanel(show: false)
        
        // top & bottom mask view
        viewTopMask = UIView(frame: CGRect(x: 0, y: 0, width: 0, height: 0))
        viewTopMask?.backgroundColor = Common.colorTheme
        viewBottomMask = UIView(frame: CGRect(x: 0, y: 0, width: 0, height: 0))
        viewBottomMask?.backgroundColor = Common.colorTheme
        
        self.viewWork.addSubview(viewTopMask!)
        self.viewWork.addSubview(viewBottomMask!)
        
        //
        // Initialize for gesture
        //
        self.pinchGesture = UIPinchGestureRecognizer(target: self, action: #selector(pinch))
        self.rotationGesture = UIRotationGestureRecognizer(target: self, action: #selector(rotate))
        self.panGesture = UIPanGestureRecognizer(target: self, action: #selector(pan))
        let tapGesture = UITapGestureRecognizer(target: self, action: #selector(tap))
        
        self.pinchGesture?.delegate = self
        self.rotationGesture?.delegate = self
        self.panGesture?.delegate = self
        tapGesture.delegate = self
        
        contentView.addGestureRecognizer(self.pinchGesture!)
        contentView.addGestureRecognizer(self.rotationGesture!)
        contentView.addGestureRecognizer(self.panGesture!)
        contentView.addGestureRecognizer(tapGesture)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        
        setupViews()
    }
    
    func initTemplateImage() {
        rtImgWorkArea = self.imgViewTemp.frameForImageInImageViewAspectFit()
        
        // resize top & bottom
        viewTopMask?.frame = CGRect(x: 0, y: 0, width: imgViewTemp.frame.width, height: rtImgWorkArea!.origin.y)
        viewBottomMask?.frame = CGRect(x: 0,
                                       y: rtImgWorkArea!.origin.y + rtImgWorkArea!.height,
                                       width: rtImgWorkArea!.width,
                                       height: imgViewTemp.frame.height - rtImgWorkArea!.origin.y + rtImgWorkArea!.height)
    }
    
    /// add sticker to view
    func addSticker(stickerImage: UIImage) {
        hideStickerEditFrame()
        
        let screenSize = UIScreen.main.bounds.size
        let imgWidth = screenSize.width / 3
        let imgHeight = imgWidth * (stickerImage.size.height / stickerImage.size.width)
        let stickerImageView: UIImageView = UIImageView(frame: CGRect(x: 0, y: 0, width: imgWidth, height: imgHeight))
        stickerImageView.image = stickerImage
        
        let stickerView: CHTStickerView = CHTStickerView(contentView: stickerImageView)
        stickerView.center = CGPoint(x: screenSize.width / 2, y: screenSize.height / 2)
        stickerView.delegate = self
        stickerView.outlineBorderColor = UIColor.clear
        stickerView.setImage(UIImage(named: "StickerClose"), for: .close)
        stickerView.setImage(UIImage(named: "StickerRotate"), for: .rotate)
        stickerView.setImage(UIImage(named: "StickerFlip"), for: .flip)
        stickerView.setHandlerSize(30)
        stickerView.showEditingHandlers = true
        self.viewWork.insertSubview(stickerView, at: 2)
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
        if UIImagePickerController .isSourceTypeAvailable(UIImagePickerControllerSourceType.camera) {
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
    
    @IBAction func onButSave(_ sender: Any) {
        // hide edit frmaes before save
        hideStickerEditFrame()
        
        let image = self.viewWork.capture()
        let imageMain = image.crop(rect: self.imgViewTemp.frame)
        let imageSave = imageMain.crop(rect: rtImgWorkArea!)
        
        //
        // save to pdf
        //
        
        // make 300dpi image
        let my300dpiImage = UIImage(cgImage: imageSave.cgImage!, scale: 300.0 / 72.0, orientation: imageSave.imageOrientation)
        
        // setup file name
        let documentDirectories = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)
        let documentDirectory = documentDirectories[0]
        let pdfFileNameUrl = NSURL(fileURLWithPath: documentDirectory).appendingPathComponent("mypdf.pdf")
        let pdfFileName = pdfFileNameUrl?.path
        
        // write to pdf
        UIGraphicsBeginPDFContextToFile(pdfFileName!, CGRect.zero, nil)
        
        let nWidth = 72 * 4 // 4 inches
        let nHeight = 72 * 5 // 4 inches
        
        UIGraphicsBeginPDFPageWithInfo(CGRect(x: 0, y: 0, width: nWidth, height: nHeight), nil);
        
        // image height & offset
        let fImgWidth = CGFloat(nWidth)
        let fImgHeight = my300dpiImage.size.height / (my300dpiImage.size.width / fImgWidth)
        let fOffset = (CGFloat(nHeight) - fImgHeight) / 2
        my300dpiImage.draw(in: CGRect(x: 0, y: fOffset, width: fImgWidth, height: fImgHeight))
        
        UIGraphicsEndPDFContext()
        
        print("pdf file was made: \(pdfFileName)")
        
        // send email
        if MFMailComposeViewController.canSendMail() {
            let mail = MFMailComposeViewController()
            mail.mailComposeDelegate = self
            mail.setSubject("Photo from High Res Souvenirs")
            mail.setMessageBody("<p>Photo edited by High Res Souvenirs App</p>", isHTML: true)
            
            if let fileData = NSData(contentsOfFile: pdfFileName!) {
                mail.addAttachmentData(fileData as Data,
                                       mimeType: "application/pdf",
                                       fileName: "Souvenirs.pdf")
            }
                
            present(mail, animated: true)
        }
        else {
            // show failure alert
            let ac = UIAlertController(title: "Cannot send email", message: "You may set up email accounts in your device", preferredStyle: .alert)
            ac.addAction(UIAlertAction(title: "OK", style: .default))
            present(ac, animated: true)
        }
        
//        UIImageWriteToSavedPhotosAlbum(my300dpiImage, self, #selector(image(_:didFinishSavingWithError:contextInfo:)), nil)
    }
    
    // MARK: Image
    func setupViews() {
        
        initTemplateImage()
        
        if self.firstImageView == nil {
            self.firstImageView = {
                let iv = DraggableImageView()
                iv.isUserInteractionEnabled = true
                iv.contentMode = .scaleAspectFit
                return iv
            }()
            
            self.viewWork.insertSubview(self.firstImageView!, at: 0)
        }
        
        print("\(self.viewWork.frame.minX), \(self.viewWork.frame.minY), \(self.viewWork.frame.maxX), \(self.viewWork.frame.maxY)")
        
        let originX = template!.rectContent!.origin.x * self.viewWork.frame.size.width
        let originY = rtImgWorkArea!.origin.y + template!.rectContent!.origin.y * rtImgWorkArea!.height
        let width = template!.rectContent!.width * self.viewWork.frame.size.width
        let height = template!.rectContent!.height * rtImgWorkArea!.height
        
        self.firstImageView!.frame = CGRect(x: originX,
                                            y: originY,
                                            width: width,
                                            height: height)
    }
    
    // MARK: - Image Picker Process
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [String : Any]) {
        let chosenImage = info[UIImagePickerControllerOriginalImage] as! UIImage
        
        // go to image process page
        let imageVC = self.storyboard!.instantiateViewController(withIdentifier: "ImageView") as! ImageViewController
        imageVC.imgMain = chosenImage.normalizedImage()
        imageVC.delegate = self
        self.navigationController?.pushViewController(imageVC, animated: true)

        dismiss(animated: true, completion: nil)
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
    
    @IBAction func tap(_ gesture: UITapGestureRecognizer) {
        hideStickerEditFrame()
    }
    
    @IBAction func pan(_ gesture: UIPanGestureRecognizer) {
        if !isGestureAvailable(gestureRecognizer: gesture) {
            return
        }
        
        if gesture.state == .began || gesture.state == .changed {
            
            let translation = gesture.translation(in: self.view)
            let translatedCenter = CGPoint(x: CGFloat(firstImageView!.center.x + translation.x), y: firstImageView!.center.y + translation.y)

            firstImageView!.center = translatedCenter
            
            gesture.setTranslation(CGPoint.zero, in: firstImageView)
        }
        
        makeImageTransparentGesture(state: gesture.state)
    }
    
    @IBAction func rotate(_ gesture: UIRotationGestureRecognizer) {
        if !isGestureAvailable(gestureRecognizer: gesture) {
            return
        }
        
        firstImageView!.transform = firstImageView!.transform.rotated(by: gesture.rotation)
        gesture.rotation = 0
        
        makeImageTransparentGesture(state: gesture.state)
    }
    
    @IBAction func pinch(_ gesture: UIPinchGestureRecognizer) {
        if !isGestureAvailable(gestureRecognizer: gesture) {
            return
        }
        
        firstImageView!.transform = firstImageView!.transform.scaledBy(x: gesture.scale, y: gesture.scale)
        gesture.scale = 1
        
        makeImageTransparentGesture(state: gesture.state)
    }
    
    /// check gesture availability for main image
    ///
    /// - Parameter gestureRecognizer: <#gestureRecognizer description#>
    /// - Returns: <#return value description#>
    func isGestureAvailable(gestureRecognizer: UIGestureRecognizer) -> Bool {
        // if image is not loaded, return
        if firstImageView!.image == nil {
            return false
        }

        return true
    }
    
    // MARK: - UIGestureRecognizerDelegate
    
    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldRecognizeSimultaneouslyWith otherGestureRecognizer: UIGestureRecognizer) -> Bool {
        return true
    }
    
    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldReceive touch: UITouch) -> Bool {
        // touched sticker view, cancel this gesture
        if touch.view is CHTStickerView {
            hideStickerEditFrame()
            
            // show edit frame
            let stickerView = touch.view as! CHTStickerView
            stickerView.showEditingHandlers = true
            
            return false
        }
        // touched unnecessary view, cancel this gesture
        if touch.view!.isDescendant(of: self.panelView) {
            return false
        }

        // allow this gesture
        return true
    }
    
    // MARK: - Public Methods
    
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
        
        hideStickerEditFrame()
    }
    
    func makeImageTransparent(transparent: Bool) {
        if (transparent) {
            self.imgViewTemp.alpha = 0.5
            viewTopMask!.alpha = 0.5
            viewBottomMask!.alpha = 0.5
        }
        else {
            self.imgViewTemp.alpha = 1
            viewTopMask!.alpha = 1
            viewBottomMask!.alpha = 1
        }
    }
    
    /// Remove edit frame for stickers
    func hideStickerEditFrame() {
        for view in self.viewWork.subviews {
            if (view is CHTStickerView) {
                let stickerView = view as! CHTStickerView
                stickerView.showEditingHandlers = false
            }
        }
    }
    
    @objc func image(_ image: UIImage, didFinishSavingWithError error: Error?, contextInfo: UnsafeRawPointer) {
        if let error = error {
            // we got back an error!
            let ac = UIAlertController(title: "Save error", message: error.localizedDescription, preferredStyle: .alert)
            ac.addAction(UIAlertAction(title: "OK", style: .default))
            present(ac, animated: true)
        } else {
            let ac = UIAlertController(title: "Saved!", message: "Your new image has been saved to your photos.", preferredStyle: .alert)
            ac.addAction(UIAlertAction(title: "OK", style: .default))
            present(ac, animated: true)
        }
    }

    // MARK: - HumanImageDelegate
    func setImageExtracted(img: UIImage) {
        self.firstImageView!.image  = sFunc_imageFixOrientation(img: img)
        self.firstImageView!.contentMode = .scaleAspectFill

        // Make template semi-transparent
//        makeImageTransparent(transparent: true)
    }
    
    // MARK: - MFMailComposeViewControllerDelegate
    func mailComposeController(_ controller: MFMailComposeViewController, didFinishWith result: MFMailComposeResult, error: Error?) {
        controller.dismiss(animated: true)
        
        // show failure alert
        let ac = UIAlertController(title: "Email Sent!", message: "Photo PDF file is sent to your email.", preferredStyle: .alert)
        ac.addAction(UIAlertAction(title: "OK", style: .default))
        present(ac, animated: true)
    }
}
