//
//  ImageViewController.swift
//  Souvenirs
//
//  Created by Administrator on 4/6/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit
import SVProgressHUD

class ImageViewController: UIViewController, UIGestureRecognizerDelegate {
    
    var imgMain: UIImage?
    
    /// opencv image object
    var imgObjFace: ImageObject!
    
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
    let DRAW_DONE = 3
    
    var drawMode: Int = 0 {
        didSet {
            lblNotice.text = "Extract human shape if needed"
            butPenBackground.isSelected = false
            butPenForeground.isSelected = false
            
            if drawMode == DRAW_BACKGROUND {
                butPenBackground.isSelected = true
                lblNotice.text = "Draw background area with finger"
            }
            else if drawMode == DRAW_FOREGROUND {
                butPenForeground.isSelected = true
                lblNotice.text = "Draw foreground area with finger"
            }
            else if drawMode == DRAW_DONE {
                lblNotice.text = "Extract done! You can use this image or improve extraction with drawing again"
            }
        }
    }
    
    // draw parameters
    var lastPoint: CGPoint!
    
    var extractDone = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        drawMode = DRAW_NONE
        
        // right buttons
        self.navigationItem.rightBarButtonItem = butItemDone

        // init main image
        self.imgViewMain.image = imgMain
        
        // touch event
        let panGesture = UIPanGestureRecognizer(target: self, action: #selector(pan))
        panGesture.delegate = self
        imgViewDraw.addGestureRecognizer(panGesture)
        
        // init face image
        DispatchQueue.global(qos: .background).async {
            let bundle = Bundle(for: type(of: self))
            ImageObject.initializeFaceDetect(bundle.bundlePath)
            
            self.imgObjFace = ImageObject.init(image: self.imgMain)
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

    @IBAction func pan(_ gesture: UIPanGestureRecognizer) {
        
        if drawMode == DRAW_NONE || drawMode == DRAW_DONE {
            return
        }
        
        let position = gesture.location(in: self.imgViewDraw)
        print("\(gesture.state): \(position.x), \(position.y)")
        
        if gesture.state == .changed {
            
            // draw lines
            let renderer = UIGraphicsImageRenderer(size: self.imgViewDraw.frame.size)
            let img = renderer.image { ctx in
                self.imgViewDraw.drawHierarchy(in: self.imgViewDraw.bounds, afterScreenUpdates: true)
                ctx.cgContext.setStrokeColor(drawMode == DRAW_BACKGROUND ? UIColor.blue.cgColor : UIColor.red.cgColor)
                ctx.cgContext.setLineWidth(3)
                
                ctx.cgContext.move(to: lastPoint)
                ctx.cgContext.addLine(to: position)
                ctx.cgContext.drawPath(using: .stroke)
            }
            
            self.imgViewDraw.image = img
        }
        
        if gesture.state == .ended {
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
        
        lastPoint = position
    }

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */
    
    @IBAction func onButExtractFace(_ sender: Any) {
        SVProgressHUD.show(withStatus: "Extracting human shape...")
        
        // push points
        
        // process extracting face
        DispatchQueue.global(qos: .background).async {
            let imgResult = self.imgObjFace.process()
            
            DispatchQueue.main.async {
                self.imgViewMain.image = imgResult;
                
                SVProgressHUD.dismiss()
                self.enableDrawButtons(enable: true)
                
                if self.drawMode > self.DRAW_NONE {
                    self.extractDone = true
                }
                else {
                    self.drawMode = self.DRAW_BACKGROUND
                }
            }
        }
    }
    
    @IBAction func onButDone(_ sender: Any) {
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
        
        imgViewDraw.image = nil
    }
    
}
