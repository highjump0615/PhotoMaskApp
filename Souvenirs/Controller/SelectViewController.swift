//
//  SelectViewController.swift
//  Souvenirs
//
//  Created by Administrator on 2/2/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit

class SelectViewController: UIViewController, UICollectionViewDelegate, UICollectionViewDataSource, UICollectionViewDelegateFlowLayout {
    
    var templates = [Template]()

    override func viewDidLoad() {
        super.viewDidLoad()

        //
        // Initialize templates
        //
        var template = Template(name: "Buddha Meme")
        template.imgPathBackground = "budmeme_bg.png"
        template.rectContent = CGRect(x: 672 / 1440.0,
                                      y: 312 / 1440.0,
                                      width: 160 / 1440.0,
                                      height: 182 / 1440.0)
        self.templates.append(template)
        
        template = Template(name: "Dank Weed")
        template.imgPathBackground = "dankweed_bg.png"
        template.rectContent = CGRect(x: 531 / 1440.0,
                                      y: 486 / 1440.0,
                                      width: 210 / 1440.0,
                                      height: 191 / 1440.0)
        self.templates.append(template)
        
        template = Template(name: "Winner Best Stoner 1")
        template.imgPathBackground = "winnerbest1_bg.png"
        template.rectContent = CGRect(x: 295 / 1111.0,
                                      y: 299 / 1111.0,
                                      width: 515 / 1111.0,
                                      height: 521 / 1111.0)
        self.templates.append(template)
        
        template = Template(name: "Winner Best Stoner 2")
        template.imgPathBackground = "winnerbest2_bg.png"
        template.rectContent = CGRect(x: 295 / 1111.0,
                                      y: 299 / 1111.0,
                                      width: 515 / 1111.0,
                                      height: 521 / 1111.0)
        self.templates.append(template)
        
        template = Template(name: "Los Angeles 1")
        template.imgPathBackground = "losag_bg.png"
        template.imgPathThumb = "losag_thumb.png"
        template.rectContent = CGRect(x: 271 / 1200.0,
                                      y: 346 / 1200.0,
                                      width: 665 / 1200.0,
                                      height: 665 / 1200.0)
        self.templates.append(template)
        
        template = Template(name: "Los Angeles 2")
        template.imgPathBackground = "losfa_bg.png"
        template.imgPathThumb = "losfa_thumb.png"
        template.rectContent = CGRect(x: 271 / 1200.0,
                                      y: 346 / 1200.0,
                                      width: 665 / 1200.0,
                                      height: 665 / 1200.0)
        self.templates.append(template)
        
        template = Template(name: "Smoking")
        template.imgPathBackground = "wanted_bg"
        template.imgPathThumb = "wanted_thumb"
        template.rectContent = CGRect(x: 256 / 1200.0,
                                      y: 292 / 1200.0,
                                      width: 682 / 1200.0,
                                      height: 682 / 1200.0)
        self.templates.append(template)
        
        template = Template(name: "Venis Rasta")
        template.imgPathBackground = "vrasta_high_front"
        template.imgPathTempBg = "vrasta_high_bg.jpg"
        template.imgPathThumb = "vrasta_high_thumb.png"
        template.rectContent = CGRect(x: 95 / 300.0,
                                      y: 67 / 250.0,
                                      width: 131 / 300.0,
                                      height: 131 / 250.0)
        self.templates.append(template)
        
        template = Template(name: "Chalice 1")
        template.imgPathBackground = "chalice1_bg"
        template.rectContent = CGRect(x: 500 / 3000.0,
                                      y: 576 / 3000.0,
                                      width: 2040 / 3000.0,
                                      height: 1784 / 3000.0)
        self.templates.append(template)
        
        template = Template(name: "Chalice 2")
        template.imgPathBackground = "chalice2_bg"
        template.rectContent = CGRect(x: 636 / 3000.0,
                                      y: 592 / 3000.0,
                                      width: 1944 / 3000.0,
                                      height: 1960 / 3000.0)
        self.templates.append(template)
        
        template = Template(name: "Chalice 3")
        template.imgPathBackground = "chalice3_bg"
        template.rectContent = CGRect(x: 376 / 3000.0,
                                      y: 430 / 3000.0,
                                      width: 2250 / 3000.0,
                                      height: 2140 / 3000.0)
        self.templates.append(template)
        
        template = Template(name: "Chalice 4")
        template.imgPathBackground = "chalice4_bg"
        template.rectContent = CGRect(x: 536 / 3000.0,
                                      y: 532 / 3000.0,
                                      width: 2040 / 3000.0,
                                      height: 1992 / 3000.0)
        self.templates.append(template)
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

    
    // MARK: - CollectionView DataSoruce & Delegate
    
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return self.templates.count
    }
    
    func numberOfSections(in collectionView: UICollectionView) -> Int {
        return 1
    }
    
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "TemplateCellID", for: indexPath) as! TemplateCollectionViewCell
        cell.mImgViewBg.image = UIImage(named: self.templates[indexPath.row].thumbnail())
        cell.layoutIfNeeded()
        
        return cell
    }
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        let width = (self.view.frame.size.width - 20) / 3 - 20
        
        return CGSize(width: width, height: width)
    }

    func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let mainVC = self.storyboard!.instantiateViewController(withIdentifier: "MainView") as! MainViewController
        mainVC.template = self.templates[indexPath.row]

        self.navigationController?.pushViewController(mainVC, animated: true)
    }
}
