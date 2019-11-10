#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    // enable depth->video image calibration
    kinect.setRegistration(true);
    
    kinect.init();
    //kinect.init(true); // shows infrared instead of RGB video image
    //kinect.init(false, false); // disable video image (faster fps)
    
    kinect.open();        // opens first available kinect
    //kinect.open(1);    // open a kinect by id, starting with 0 (sorted by serial # lexicographically))
    //kinect.open("A00362A08602047A");    // open a kinect using it's unique serial #
    
    // print the intrinsic IR sensor values
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
    
    colorImg.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    
    // Adjust the distance threshold here:
    nearThreshold = 255;
    farThreshold = 220;
    bThreshWithOpenCV = true;
    
    ofSetFrameRate(60);
    
    // zero the tilt on startup
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    
    // start from the front
    bDrawPointCloud = false;
    
    frameNum = 0;
    avgSpeed = 1.0;
    lastSpeed = 1.0;
    targetSpeed = 1.0;
    
    
    // open an outgoing connection to HOST:PORT
    sender.setup(HOST, PORT);
}

//--------------------------------------------------------------
void ofApp::update(){
    kinect.update();
    
    // there is a new frame and we are connected
    if(kinect.isFrameNew()) {
        
        // load grayscale depth image from the kinect source
        grayImage.setFromPixels(kinect.getDepthPixels());
        
        // we do two thresholds - one for the far plane and one for the near plane
        // we then do a cvAnd to get the pixels which are a union of the two thresholds
        if(bThreshWithOpenCV) {
            grayThreshNear = grayImage;
            grayThreshFar = grayImage;
            grayThreshNear.threshold(nearThreshold, true);
            grayThreshFar.threshold(farThreshold);
            cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
        } else {
            
            // or we do it ourselves - show people how they can work with the pixels
            ofPixels & pix = grayImage.getPixels();
            int numPixels = pix.size();
            for(int i = 0; i < numPixels; i++) {
                if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = 255;
                } else {
                    pix[i] = 0;
                }
            }
        }
        
        // update the cv images
        grayImage.flagImageChanged();
        
        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
        // also, find holes is set to true so we will get interior contours as well....
        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //B&WImage
    float ratioX = ofGetWidth()  / grayImage.getWidth();
    float ratioY = ofGetHeight()  / grayImage.getHeight();
    grayImage.draw(0, 0, ratioX*grayImage.getWidth(), ratioY*grayImage.getHeight());
    
    //colorImage
    //kinect.draw(0, 0, 400, 300);
    
    //display the speed in text
    stringstream reportStream;
    reportStream << "Speed" << endl << currSpCalculate() << endl;
    ofDrawBitmapString(reportStream.str(), 15, 15);
    
    //OSC draw:
    // display instructions and test
    string buf;
    buf = "sending osc messages to" + string(HOST) + ofToString(PORT);
    //ofDrawBitmapString(buf, 10, 20);
    //ofDrawBitmapString("press A to send osc message [/test 1 3.5 hello <time>]", 10, 40);
    
    //sending the speed to OSC
    ofxOscMessage m;
    m.setAddress("/speed");
    m.addFloatArg(currSpeed);
    sender.sendMessage(m, false);
    cout<<currSpeed<<endl;
}

//--------------------------------------------------------------------
float ofApp::currSpCalculate(){
    
    //----- Calculate the ratio between the window and the kinext display range
    float ratioX = ofGetWidth()  / grayImage.getWidth();
    float ratioY = ofGetHeight()  / grayImage.getHeight();
    
    //----- locate the blob center!
    // make sure there is at least one blob, so that we can get the first one
    if(contourFinder.blobs.size() > 0) {
        float totalSpeed = 0.0;
        int numBlobs = 0;
        
        for(int i=0; i<contourFinder.blobs.size(); i++){
            // pull out the centroid, which is an ofPoint / ofVec3f
            ofVec3f centroid = contourFinder.blobs[i].centroid;
            
            ofPushStyle();
            //draw bounding rect:
            ofSetColor(255, 0, 0);
            ofNoFill();
            ofSetRectMode(OF_RECTMODE_CENTER);
            ofDrawRectangle( centroid.x * ratioX+85, centroid.y * ratioY-70, contourFinder.blobs[i].boundingRect.width* ratioX, contourFinder.blobs[i].boundingRect.height* ratioY);
            ofPopStyle();
            
            float boundingRectSz = contourFinder.blobs[i].boundingRect.width* ratioX * contourFinder.blobs[i].boundingRect.height* ratioY;
            float windowSz = ofGetWidth()*ofGetHeight();
            float ratio = boundingRectSz / windowSz;
            
            //calculate video speed:
            if(!frameByframe){
                float speed = (20*ratio) * 1.0f;//original 15
                totalSpeed = totalSpeed + speed;
                numBlobs++;
            }
        }
        
        avgSpeed = totalSpeed / numBlobs;
        if (avgSpeed<1.0){
            avgSpeed=1.0;
        }
        if (avgSpeed>6.0){
            avgSpeed=6.0;
        }
    }
    else {
        //avgSpeed = 0.15;
        avgSpeed = 1.0;
    }
    
    if (frameNum % 60 == 0) {//origianl 60
        lastSpeed = targetSpeed;
        targetSpeed = avgSpeed;
    }
    
    
    if (frameNum % 10 == 0) {//oringinal 10
        currSpeed = getSpeed(lastSpeed, targetSpeed, (frameNum % 60) / 60.0f);
        return currSpeed;
        cout<< currSpeed << endl;
    }
    
    frameNum++;
    
}
//--------------------------------------------------------------------
float ofApp::getSpeed(float last, float target, float t) {
    return ((target - last) * t) + last;
}
//--------------------------------------------------------------
void ofApp::exit() {
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
    switch (key) {
        case ' ':
            bThreshWithOpenCV = !bThreshWithOpenCV;
            break;
            
        case'p':
            bDrawPointCloud = !bDrawPointCloud;
            break;
            
        case 'x':
            //case '.':
            farThreshold ++;
            if (farThreshold > 255) farThreshold = 255;
            break;
            
        case 'z':
            //case ',':
            farThreshold --;
            if (farThreshold < 0) farThreshold = 0;
            break;
            
        case '+':
        case '=':
            nearThreshold ++;
            if (nearThreshold > 255) nearThreshold = 255;
            break;
            
        case '-':
            nearThreshold --;
            if (nearThreshold < 0) nearThreshold = 0;
            break;
            
        case 'w':
            kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
            break;
            
        case 'o':
            kinect.setCameraTiltAngle(angle); // go back to prev tilt
            kinect.open();
            break;
            
        case 'c':
            kinect.setCameraTiltAngle(0); // zero the tilt
            kinect.close();
            break;
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
    }
    
    //OSC testing:
    if(key == 'a' || key == 'A'){
        ofxOscMessage m;
        m.setAddress("/test");
        m.addStringArg("hello");
        m.addFloatArg(ofGetElapsedTimef());
        sender.sendMessage(m, false);
    }
}

