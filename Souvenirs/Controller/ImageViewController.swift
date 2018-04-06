//
//  ImageViewController.swift
//  Souvenirs
//
//  Created by Administrator on 4/6/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit

class ImageViewController: UIViewController, UIGestureRecognizerDelegate {
    
    var imgMain: UIImage?
    
    @IBOutlet weak var imgViewDraw: UIImageView!
    @IBOutlet weak var imgViewMain: UIImageView!
    @IBOutlet weak var butItemDone: UIBarButtonItem!
    
    @IBOutlet weak var butPenForeground: UIButton!
    @IBOutlet weak var butPenBackground: UIButton!
    
    let DRAW_NONE = 0
    let DRAW_FOREGROUND = 1
    let DRAW_BACKGROUND = 2
    var drawMode: Int = 0
    
    // draw parameters
    var lastPoint: CGPoint!
    
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
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    @IBAction func pan(_ gesture: UIPanGestureRecognizer) {
        
        if (drawMode == DRAW_NONE) {
            return
        }
        
        let position = gesture.location(in: self.imgViewDraw)
        print("\(gesture.state): \(position.x), \(position.y)")
        
        if gesture.state == .changed {
            
            // draw lines
            let renderer = UIGraphicsImageRenderer(size: self.imgViewDraw.frame.size)
            let img = renderer.image { ctx in
                self.imgViewDraw.drawHierarchy(in: self.imgViewDraw.bounds, afterScreenUpdates: true)
                ctx.cgContext.setStrokeColor(UIColor.red.cgColor)
                ctx.cgContext.setLineWidth(3)
                
                ctx.cgContext.move(to: lastPoint)
                ctx.cgContext.addLine(to: position)
                ctx.cgContext.drawPath(using: .stroke)
            }
            
            self.imgViewDraw.image = img
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
    }
    
    @IBAction func onButDrawForeground(_ sender: Any) {
    }
    
    @IBAction func onButRedraw(_ sender: Any) {
    }
    
}
