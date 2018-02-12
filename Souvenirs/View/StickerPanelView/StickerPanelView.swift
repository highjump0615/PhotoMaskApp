//
//  StickerPanelView.swift
//  Souvenirs
//
//  Created by Administrator on 2/4/18.
//  Copyright © 2018 Administrator. All rights reserved.
//

import UIKit

class StickerPanelView: UIView, UICollectionViewDelegate, UICollectionViewDataSource, UICollectionViewDelegateFlowLayout {
    
    @IBOutlet weak var scrollViewCategory: UIScrollView!
    @IBOutlet weak var collectionView: UICollectionView!
    
    var categories = [StickerCategory]()
    var currentCategoryIndex = 0

    /*
    // Only override draw() if you perform custom drawing.
    // An empty implementation adversely affects performance during animation.
    override func draw(_ rect: CGRect) {
        // Drawing code
    }
    */
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        
        //
        // Initialize Sticker
        //
        self.categories.append(StickerCategory(icon: "AFRO0.png", name: "AFRO", count: 4))
        self.categories.append(StickerCategory(icon: "BEARD1.png", name: "BEARD", count: 10))
        self.categories.append(StickerCategory(icon: "Glass0.png", name: "Glass", count: 18))
        self.categories.append(StickerCategory(icon: "Mustasch0.png", name: "Mustasch", count: 8))
        self.categories.append(StickerCategory(icon: "Swag0.png", name: "Swag", count: 16))
    }
    
    
    /// Add category button
    ///
    /// - Parameters:
    ///   - icon: <#icon description#>
    ///   - index: <#index description#>
    func addCategory(icon: String, index: Int) {
        let fWidth = 40.0
        let fPadding: CGFloat = 7.0;
        
        let edgeInset = UIEdgeInsetsMake(fPadding, fPadding, fPadding, fPadding)
        
        let butCategory = UIButton(frame: CGRect(x: Double(index) * fWidth, y: 0, width: fWidth, height: fWidth))
        butCategory.contentEdgeInsets = edgeInset
        butCategory.imageView?.contentMode = .scaleAspectFit
        butCategory.setImage(UIImage(named: icon), for: .normal)
        butCategory.addBorder(toSide: .Right, withColor: UIColor.lightGray.cgColor, andThickness: 1)
        butCategory.tag = getTagValue(index: index)
        butCategory.addTarget(self, action: #selector(self.onButCategory), for: .touchUpInside)
        butCategory.setBackgroundImage(UIImage.init(color: UIColor(displayP3Red: 0.9, green: 0.9, blue: 0.9, alpha: 1)), for: .selected)
        
        self.scrollViewCategory.addSubview(butCategory)
    }
    
    @IBAction func onButCategory(_ sender: AnyObject?) {
        let butCategory = sender as! UIButton
        let nIndex = butCategory.tag - 1000
        
        if self.currentCategoryIndex == nIndex {
            return
        }
        
        updateStickerList(index: nIndex)
    }

    func getTagValue(index: Int) -> Int {
        return 1000 + index
    }
    
    /// Update sticker collection view
    ///
    /// - Parameter index: <#index description#>
    func updateStickerList(index: Int) {
        // deselect current button
        var butCurCategory = self.scrollViewCategory.viewWithTag(getTagValue(index: self.currentCategoryIndex)) as! UIButton
        butCurCategory.isSelected = false
        
        // select new button
        butCurCategory = self.scrollViewCategory.viewWithTag(getTagValue(index: index)) as! UIButton
        butCurCategory.isSelected = true
        
        self.currentCategoryIndex = index;
        
        self.collectionView.reloadData()
    }
    
    override func awakeFromNib() {
        //
        // Initialize Category
        //
        for i in 0 ..< self.categories.count {
            let category = self.categories[i]
            addCategory(icon: category.iconName, index: i)
        }
        
        // Init collection view
        self.collectionView.delegate = self
        self.collectionView.dataSource = self
        
        updateStickerList(index: 0)
    }
    
    override func layoutSubviews() {
        super.layoutSubviews()
        
        self.scrollViewCategory.addBorder(toSide: .Top, withColor: UIColor.lightGray.cgColor, andThickness: 1.0)
    }
    
    // MARK: - CollectionView DataSoruce & Delegate
    
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return self.categories[self.currentCategoryIndex].count
    }
    
    func numberOfSections(in collectionView: UICollectionView) -> Int {
        return 1
    }
    
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "StickerCellID", for: indexPath) as! StickerCollectionViewCell
        let strImgName = self.categories[self.currentCategoryIndex].name + String(indexPath.row) + ".png"
        
        cell.imgView.image = UIImage(named: strImgName)
        cell.layoutIfNeeded()
        
        return cell
    }
    
    func collectionView(_ collectionView: UICollectionView, layout collectionViewLayout: UICollectionViewLayout, sizeForItemAt indexPath: IndexPath) -> CGSize {
        let width = 72
        
        return CGSize(width: width, height: width)
    }
    
    func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let mainVC = self.parentViewController as! MainViewController
        let strImgName = self.categories[self.currentCategoryIndex].name + String(indexPath.row) + ".png"
        let imgSticker = UIImage(named: strImgName)
        
        mainVC.addSticker(stickerImage: imgSticker!)
    }
}
