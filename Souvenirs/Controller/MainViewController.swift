//
//  MainViewController.swift
//  Souvenirs
//
//  Created by Administrator on 2/3/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit
import MessageUI
import IHKeyboardAvoiding
import ChromaColorPicker

class MainViewController: UIViewController,
                        UIGestureRecognizerDelegate,
                        UIImagePickerControllerDelegate,
                        UINavigationControllerDelegate,
                        CHTStickerViewDelegate,
                        HumanImageDelegate,
                        TextStickerViewDelegate,
                        UITextViewDelegate {
    
    var template: Template?
    var picker : UIImagePickerController?
    
    var pinchGesture: UIPinchGestureRecognizer?
    var rotationGesture: UIRotationGestureRecognizer?
    var panGesture: UIPanGestureRecognizer?
    
    var rtImgWorkArea: CGRect?
    var firstImageView: DraggableImageView?
    
    var viewTopMask: UIView?
    var viewBottomMask: UIView?
    
    var viewTextSticker: TextStickerView?
    
    var currentStickerView: CHTStickerView?
    
    // right bar items
    @IBOutlet weak var butItemSave: UIBarButtonItem!
    
    // text sticker params
    var stickerTextSize = 20
    var stickerTextColor = UIColor.red
    
    @IBOutlet weak var viewWork: UIView!
    @IBOutlet weak var imgViewTempBg: UIImageView!
    @IBOutlet weak var imgViewTemp: UIImageView!
    @IBOutlet weak var constraintToolbarOffset: NSLayoutConstraint!
    @IBOutlet var contentView: UIView!
    @IBOutlet weak var panelView: UIView!
    
    @IBOutlet weak var stickerPanelView: StickerPanelView!
    @IBOutlet weak var textStickerPanelView: UIView!
    
    @IBOutlet weak var butFlip: UIButton!
    @IBOutlet weak var butSticker: UIButton!
    @IBOutlet weak var butTextSticker: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // right buttons
        self.navigationItem.rightBarButtonItem = butItemSave
        
        // fill template image
        self.imgViewTemp.image = UIImage(named: (self.template?.imgPathBackground)!)
        if (self.template?.imgPathTempBg != nil) {
            self.imgViewTempBg.image = UIImage(named: (self.template?.imgPathTempBg)!)
        }
        
        self.picker = UIImagePickerController()
        self.picker?.delegate = self
        
        // Initialize sticker panel
        showStickerPanel()
        
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
        
        KeyboardAvoiding.avoidingView = self.panelView
        
        // text sticker view
        viewTextSticker?.removeFromSuperview()
        viewTextSticker = TextStickerView.loadFromNib() as? TextStickerView
        viewTextSticker?.initView(delegate: self)
        viewTextSticker?.setTextDelegate(delgate: self)
        clearTextStickerView()
        
        self.textStickerPanelView.addSubview(viewTextSticker!)
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
        
        currentStickerView = stickerView
        
        self.viewWork.insertSubview(stickerView, at: 3)
    }
    
    func addTextSticker(text: String) {
        hideStickerEditFrame()
        
        let textFont = UIFont(name: "Helvetica Bold", size: CGFloat(stickerTextSize))!
        
        let textFontAttributes = [
            NSAttributedStringKey.font: textFont,
            NSAttributedStringKey.foregroundColor: stickerTextColor,
            ] as [NSAttributedStringKey : Any]
        let size = (text as NSString).size(withAttributes: textFontAttributes)

        let stickerLabel: UILabel = UILabel()
        stickerLabel.font = textFont
        stickerLabel.textColor = stickerTextColor
        stickerLabel.text = text
        stickerLabel.sizeToFit()
        stickerLabel.frame.size = CGSize(width: max(stickerLabel.frame.width, 44),
                                         height: max(stickerLabel.frame.height, 30))
        
        let screenSize = UIScreen.main.bounds.size
        let stickerView: CHTStickerView = CHTStickerView(contentView: stickerLabel)
        
        stickerView.center = CGPoint(x: screenSize.width / 2, y: screenSize.height / 2)
        stickerView.delegate = self
        stickerView.outlineBorderColor = UIColor.clear
        stickerView.setImage(UIImage(named: "StickerClose"), for: .close)
        stickerView.setImage(UIImage(named: "StickerRotate"), for: .rotate)
        stickerView.setHandlerSize(30)
        stickerView.showEditingHandlers = true
        
        stickerView.enableFlip = false
        
        self.viewWork.insertSubview(stickerView, at: 3)
        
        print("new text sticker size: \(size.width), \(size.height)")
        currentStickerView = stickerView
    }
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */
    
    func clearButtonSelection() {
        self.butSticker.isSelected = false;
        
    }

    
    /// Sticker  button click
    ///
    /// - Parameter sender: <#sender description#>
    @IBAction func onButSticker(_ sender: UIButton) {
        self.butTextSticker.isSelected = false;
        self.butSticker.isSelected = !self.butSticker.isSelected
        
        showStickerPanel()
    }
    
    /// Flip button click
    ///
    /// - Parameter sender: <#sender description#>
    @IBAction func onButFlip(_ sender: UIButton) {
        self.butFlip.isSelected = !self.butFlip.isSelected
        
        firstImageView?.transform = CGAffineTransform(scaleX: self.butFlip.isSelected ? -1 : 1, y: 1)
    }
    
    /// Text Sticker button click
    ///
    /// - Parameter sender: <#sender description#>
    @IBAction func onButTextSticker(_ sender: Any) {
        self.butSticker.isSelected = false;
        self.butTextSticker.isSelected = !self.butTextSticker.isSelected
        
        showStickerPanel()
    }
    
    /// Show/Hide sticker panel
    ///
    /// - Parameter show: 
    func showStickerPanel() {
        if (self.butSticker.isSelected || self.butTextSticker.isSelected) {
            // panel shoud be shown
            self.constraintToolbarOffset.constant = -232
        }
        else {
            self.constraintToolbarOffset.constant = -48
        }
        
        // sticker view
        if (self.butSticker.isSelected) {
            self.stickerPanelView.isHidden = false
            self.textStickerPanelView.isHidden = true
        }
        // text sticker view
        if (self.butTextSticker.isSelected) {
            self.stickerPanelView.isHidden = true
            self.textStickerPanelView.isHidden = false
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
    
    func clearTextStickerView() {
        viewTextSticker?.initTextSizeColor(text: "", size: 20, color: UIColor.red)
    }
    
    @IBAction func onButSave(_ sender: Any) {
        // hide edit frmaes before save
        hideStickerEditFrame()
        
        //
        // extract image
        //
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
        
        
        //
        // show actionsheet for save or share
        //
        let alertController = UIAlertController(title: "Save image", message: "You can save photo as image or Pdf", preferredStyle: .actionSheet)
        
        if let popoverController = alertController.popoverPresentationController {
            popoverController.sourceView = self.view
            popoverController.sourceRect = CGRect(x: self.view.bounds.midX, y: self.view.bounds.midY, width: 0, height: 0)
            popoverController.permittedArrowDirections = []
        }
        
        let butImgSave = UIAlertAction(title: "Save into Photos", style: .default, handler: { (action) -> Void in
            UIImageWriteToSavedPhotosAlbum(my300dpiImage, self, #selector(self.image(_:didFinishSavingWithError:contextInfo:)), nil)
        })
        
        let butImgShare = UIAlertAction(title: "Share with PDF", style: .destructive, handler: { (action) -> Void in
            if let fileData = NSData(contentsOfFile: pdfFileName!) {
                let activityViewController = UIActivityViewController(activityItems: [fileData], applicationActivities: nil)
                
                if let popoverController = activityViewController.popoverPresentationController {
                    popoverController.sourceView = self.view
                    popoverController.sourceRect = CGRect(x: self.view.bounds.midX, y: self.view.bounds.midY, width: 0, height: 0)
                    popoverController.permittedArrowDirections = []
                }

                activityViewController.setValue("Photo from High Res Souvenirs" , forKey: "subject") ;
                activityViewController.excludedActivityTypes = [
                    .postToTwitter,
                    .postToWeibo,
                    .message,
                    .markupAsPDF,
                    .copyToPasteboard,
                    .assignToContact,
                    .saveToCameraRoll,
                    .addToReadingList,
                    .postToFlickr,
                    .postToVimeo,
                    .postToTencentWeibo,
                    .openInIBooks
                ]
                
                self.present(activityViewController, animated: true, completion: {})
            }
        })
        
        let butImgCancel = UIAlertAction(title: "Cancel", style: .cancel, handler: { (action) -> Void in
            print("Cancel button tapped")
        })
        
        alertController.addAction(butImgSave)
        alertController.addAction(butImgShare)
        alertController.addAction(butImgCancel)
        
        self.navigationController!.present(alertController, animated: true, completion: nil)
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
            
            self.viewWork.insertSubview(self.firstImageView!, at: 1)
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
        
        viewTextSticker?.initView(delegate: self)
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
    
    // MARK: - Gestures
    
    @IBAction func tap(_ gesture: UITapGestureRecognizer) {
        hideStickerEditFrame()
        clearTextStickerView()
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
    
    //
    // MARK: - UIGestureRecognizerDelegate
    //
    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldRecognizeSimultaneouslyWith otherGestureRecognizer: UIGestureRecognizer) -> Bool {
        return true
    }
    
    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldReceive touch: UITouch) -> Bool {
        // touched sticker view, cancel this gesture
        if touch.view is CHTStickerView {
            hideStickerEditFrame()
            clearTextStickerView()
            
            // show edit frame
            let stickerView = touch.view as! CHTStickerView
            stickerView.showEditingHandlers = true
            
            currentStickerView = stickerView
            
            // init edit view for text sticker
            if stickerView.contentView is UILabel {
                let viewLabel = stickerView.contentView as! UILabel
                viewTextSticker?.initTextSizeColor(text: viewLabel.text!,
                                                   size: Int(viewLabel.font.pointSize),
                                                   color: viewLabel.textColor)
            }
            
            return false
        }
        
        // close the keyboard
        self.view.endEditing(true)
        
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
        
        currentStickerView = nil
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

    //
    // MARK: - HumanImageDelegate
    //
    func setImageExtracted(img: UIImage) {
        self.firstImageView!.image  = img
        self.firstImageView!.contentMode = .scaleAspectFill

        // Make template semi-transparent
//        makeImageTransparent(transparent: true)
    }
    
    func getCurrentStickerLabel() -> UILabel? {
        if currentStickerView == nil {
            return nil
        }
        
        return currentStickerView!.contentView as? UILabel
    }
    
    //
    // MARK: - TextStickerViewDelegate
    //
    func updatedTextColor(color: UIColor) {
        let stickerLabel = getCurrentStickerLabel()
        stickerLabel?.textColor = color
        
        stickerTextColor = color
    }
    
    func updatedTextSize(size: Int) {
        let stickerLabel = getCurrentStickerLabel()
        stickerLabel?.font = UIFont(name: "Helvetica Bold", size: CGFloat(size))
        
        stickerTextSize = size
    }
    
    //
    // MARK: - UITextViewDelegate
    //
    func textView(_ textView: UITextView, shouldChangeTextIn range: NSRange, replacementText text: String) -> Bool {
        if (text == "\n") {
            textView.resignFirstResponder()
    
            if getCurrentStickerLabel() == nil {
                addTextSticker(text: textView.text)
            }
            else {
                // update text
                let stickerLabel = getCurrentStickerLabel()
                stickerLabel?.text = textView.text
            }
        }
        return true
    }
    
}
