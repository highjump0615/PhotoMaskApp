Photo Mask iOS App
======

> iOS Photo App with template image and stickers

## Overview

### 1. Main Features
- Photo masking with template images  
- Adding Stickers to image
- Save image to disk
 
### 2. Techniques (Swift 4.0)
#### 2.1 UI Implementation
- Variable font size on different screen  
Different font size based on size classes  
- Stack View  
\* Activate page  
- Padding on UITextField in Activate page  
- Material clickable background  
\* Adding view on text field programatically  

#### 2.2 Function Implementation
- Dealing with multiple gestures for background image and sticker & touch-ignored view  
Implementing UIGestureRecognizerDelegate method to filter touched views  
```swift  
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
```
- Implemented Activation logic with Firebase  
UUID is the key for each phone, added some more user info on that key  

#### 2.2.3 Third-Party Libraries
- [CHTStickerView](https://github.com/chiahsien/CHTStickerView)  
Adding stickers on main image
- [SVProgressHUD](https://github.com/SVProgressHUD/SVProgressHUD)  

## Need to Improve
- Extracting human shape from photo and applying to template