Photo Mask iOS App
======

> iOS Photo App with template image and stickers

## Overview

### 1. Main Features
- Photo masking with template images  
- Adding Stickers to image
- Extract human face from any photo or image
- Save image to disk
 
### 2. Techniques  
Main language: Swift 4.0
#### 2.1 UI Implementation
- Variable font size on different screen  
Different font size based on size classes  
- Stack View
  - Activate page
- Padding on UITextField in Activate page  
- Material clickable background
  - Adding view on text field programatically  

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
- Hand-drawing for marking face area  
  - Drawing as image using Image Context with touched points  
``UIGraphicsBeginImageContext()``  
``UIGraphicsGetImageFromCurrentImageContext()``  
``UIGraphicsEndImageContext()``  
- Save/Load data to UserDefaults  
  - Activation flag for determining status when opens app  
- Face extrating module  
``./Lib/imagemat``  
  - Using [SSE2NEON.h](https://github.com/jratcliff63367/sse2neon) for ARM Architecture (iOS devices, not simulator)  

#### 2.2.3 Third-Party Libraries
- [CHTStickerView](https://github.com/chiahsien/CHTStickerView)  
Adding stickers on main image
- [SVProgressHUD](https://github.com/SVProgressHUD/SVProgressHUD)  
- [IHKeyboardAvoiding](https://github.com/IdleHandsApps/IHKeyboardAvoiding) v4.2  
  - Activation page  
- [KMPlaceholderTextView](https://github.com/MoZhouqi/KMPlaceholderTextView) v1.3.0  
- [ChromaColorPicker](https://github.com/joncardasis/ChromaColorPicker) v1.5  
  - Selecting colors for Text Sticker  
- [KMPlaceholderTextView](https://github.com/MoZhouqi/KMPlaceholderTextView) v1.3.0  
- [Google Firebase](https://github.com/firebase/firebase-ios-sdk) v4.9.0  
  - Firebase Database  
- [OpenCV](https://opencv.org/releases.html) v3.4.1  
  - Extracting face

## Need to Improve
- Improve face extraction speed, etc