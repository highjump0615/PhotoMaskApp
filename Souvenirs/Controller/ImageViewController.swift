//
//  ImageViewController.swift
//  Souvenirs
//
//  Created by Administrator on 4/6/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit
import SVProgressHUD

protocol HumanImageDelegate {
    func setImageExtracted(img: UIImage)
}

class ImageViewController: UIViewController, UIGestureRecognizerDelegate {
    
    var imgMain: UIImage?
    var delegate: HumanImageDelegate?
    
    /// opencv image object
    var imgObjFace: ImageObject!
    
    @IBOutlet weak var butExtractFace: UIButton!
    @IBOutlet weak var imgViewDraw: UIImageView!
    @IBOutlet weak var imgViewMain: UIImageView!
    @IBOutlet weak var butItemDone: UIBarButtonItem!
    
    @IBOutlet weak var butPenForeground: UIButton!
    @IBOutlet weak var butPenBackground: UIButton!
    @IBOutlet weak var butPenRedraw: UIButton!
    
    @IBOutlet weak var lblNotice: UILabel!
    
    let DRAW_NONE = 0
    let DRAW_FOREGROUND = 1
    let DRAW_BACKGROUND = 2
    
    var pointsFg : [CGPoint] = []
    var pointsBg : [CGPoint] = []
    
    var drawMode: Int = 0 {
        didSet {
            self.lblNotice.text = "Extract face area if needed"
            self.butPenBackground.isSelected = false
            self.butPenForeground.isSelected = false
            
            if self.drawMode == self.DRAW_BACKGROUND {
                self.butPenBackground.isSelected = true
                self.lblNotice.text = "Draw background area with finger"
            }
            else if self.drawMode == self.DRAW_FOREGROUND {
                self.butPenForeground.isSelected = true
                self.lblNotice.text = "Draw foreground area with finger"
            }
            else {
                if (self.extractDone) {
                    self.lblNotice.text = "Extract done! You can use this image or improve extraction with drawing again"
                }
            }
        }
    }
    
    // draw parameters
    var lastPoint: CGPoint! = CGPoint(x: 0, y: 0)
    
    var extractDone = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // right buttons
        self.navigationItem.rightBarButtonItem = butItemDone

        // init main image
        self.imgViewMain.image = imgMain
        
        // init face image
        SVProgressHUD.show(withStatus: "Initializing image data...")
        DispatchQueue.global(qos: .background).async {
            let bundle = Bundle(for: type(of: self))
            ImageObject.initializeFaceDetect(bundle.bundlePath)
            
            // compres image
            self.imgObjFace = ImageObject.init(image: self.imgMain?.resized(withPercentage: 0.3))
            
            // not detected face
            DispatchQueue.main.async {
                if !self.imgObjFace.detectedFace {
                    self.lblNotice.text = "No face detected"
                    self.butExtractFace.isEnabled = false
                }
            
                SVProgressHUD.dismiss()
            }
        }
        
        enableDrawButtons(enable: false)
        
        // progress mask, disabling user action when progress
        SVProgressHUD.setDefaultMaskType(.gradient)
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func enableDrawButtons(enable: Bool) {
        butPenForeground.isEnabled = enable
        butPenBackground.isEnabled = enable
        butPenRedraw.isEnabled = enable
    }


    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        let touch = touches.first!
        let currentPoint = touch.location(in: self.imgViewDraw)
        
        lastPoint = currentPoint
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        let touch = touches.first!
        let currentPoint = touch.location(in: self.imgViewDraw)

        if drawMode == DRAW_NONE {
            return
        }
        
        let imgSize = self.imgViewMain.image?.size
        let ptAdd = CGPoint(x: (CGFloat)(currentPoint.x / self.imgViewDraw.frame.size.width * imgSize!.width),
                            y: (CGFloat)(currentPoint.y / self.imgViewDraw.frame.size.height * imgSize!.height))
        if drawMode == DRAW_FOREGROUND {
            pointsFg.append(ptAdd)
        }
        else if drawMode == DRAW_BACKGROUND {
            pointsBg.append(ptAdd)
        }
        
        UIGraphicsBeginImageContext(self.imgViewDraw.frame.size)
        
        let ctx = UIGraphicsGetCurrentContext()
        self.imgViewDraw.image?.draw(in: self.imgViewDraw.bounds)
        
        ctx?.move(to: lastPoint)
        ctx?.addLine(to: currentPoint)
        ctx?.setLineWidth(3.0)
        ctx?.setLineCap(.round)
        ctx?.setStrokeColor(drawMode == DRAW_BACKGROUND ? UIColor.blue.cgColor : UIColor.red.cgColor)
        ctx?.setBlendMode(.normal)
        ctx?.strokePath()
        
        self.imgViewDraw.image = UIGraphicsGetImageFromCurrentImageContext()
        
        UIGraphicsEndImageContext()
        
        lastPoint = currentPoint
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        if drawMode == DRAW_NONE {
            return
        }
        
        if extractDone {
            drawMode = DRAW_NONE
        }
        else {
            // change mode
            if drawMode == DRAW_BACKGROUND {
                drawMode = DRAW_FOREGROUND
            }
            else if drawMode == DRAW_FOREGROUND {
                drawMode = DRAW_NONE
            }
        }
    }
    
    @IBAction func onButExtractFace(_ sender: Any) {
        SVProgressHUD.show(withStatus: "Extracting face area...")
        
        // push points
        imgObjFace.pushPoints(pointsFg, isBackground: false)
        imgObjFace.pushPoints(pointsBg, isBackground: true)
        
        // process extracting face
        DispatchQueue.global(qos: .background).async {
            let imgResult = self.imgObjFace.process()
            
            DispatchQueue.main.async {
                self.imgViewMain.image = imgResult;
                
                // resize draw image area based on cropped face size
                let frameImgView = self.imgViewMain.frameForImageInImageViewAspectFit()
                self.imgViewDraw.frame = frameImgView.offsetBy(dx: self.imgViewMain.frame.origin.x,
                                                               dy: self.imgViewMain.frame.origin.y)
                
                SVProgressHUD.dismiss()
                self.enableDrawButtons(enable: true)
                
                if self.drawMode > self.DRAW_NONE {
                    self.extractDone = true
                }
                else {
                    self.drawMode = self.DRAW_BACKGROUND
                }
                
                self.clearDrawImage()
            }
        }
    }
    
    
    /// pass image to parent view
    ///
    /// - Parameter sender: <#sender description#>
    @IBAction func onButDone(_ sender: Any) {
        delegate?.setImageExtracted(img: imgViewMain.image!)
        
        // back to previous
        self.navigationController?.popViewController(animated: true)
    }
    
    @IBAction func onButDrawForeground(_ sender: Any) {
        let button = sender as! UIButton
        
        if button == butPenForeground {
            drawMode = DRAW_FOREGROUND
        }
        else if button == butPenBackground {
            drawMode = DRAW_BACKGROUND
        }
    }
    
    @IBAction func onButRedraw(_ sender: Any) {
        drawMode = DRAW_NONE
        
        clearDrawImage()
    }

    /// clear drawn lines
    func clearDrawImage() {
        imgViewDraw.image = nil

        // clear points
        self.pointsFg.removeAll()
        self.pointsBg.removeAll()
    }
    
}
