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
        self.templates.append(template)
        
        template = Template(name: "Dank Weed")
        template.imgPathBackground = "dankweed_bg.png"
        self.templates.append(template)
        
        template = Template(name: "Winner Best Stoner 1")
        template.imgPathBackground = "winnerbest1_bg.png"
        self.templates.append(template)
        
        template = Template(name: "Winner Best Stoner 2")
        template.imgPathBackground = "winnerbest2_bg.png"
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
        cell.mImgViewBg.image = UIImage(named: self.templates[indexPath.row].imgPathBackground!)
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
