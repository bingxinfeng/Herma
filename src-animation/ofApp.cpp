#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    //OSC-------------------
    // listen on the given port
    cout << "listening for osc messages on port " << PORT << "\n";
    receiver.setup(PORT);
    
    //PiMapper--------------
    ofBackground(0);
    // Enable or disable audio for video sources globally
    // Set this to false to save resources on the Raspberry Pi
    ofx::piMapper::VideoSource::enableAudio = true;
    ofx::piMapper::VideoSource::useHDMIForAudio = false;
    
    // Add our CustomSource to list of fbo sources of the piMapper
    // FBO sources should be added before piMapper.setup() so the
    // piMapper is able to load the source if it is assigned to
    // a surface in XML settings.
    louisiane = new Louisiane();
    piMapper.registerFboSource(louisiane);
    
    piMapper.setup();
    
    // The info layer is hidden by default, press <i> to toggle
    // piMapper.showInfo();
    
    ofSetFullscreen(Settings::instance()->getFullscreen());
    ofSetEscapeQuitsApp(false);
    
    hide = false;

}

//--------------------------------------------------------------
void ofApp::update(){
    
    //OSC-------------------
    if (messageBuffer.size()>maxBufferSize) messageBuffer.pop_back();
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // check for sp message
        if(m.getAddress() == "/speed"){
            speed = m.getArgAsFloat(0);
        }
        
        if (m.getAddress() == "/test") {
            string msg_string;
            myString = m.getArgAsString(0);
            myTime = m.getArgAsFloat(1);
            msg_string = m.getAddress() + " " + myString + " " + ofToString(myTime);// + " " + msg_string;
            messageBuffer.push_front(msg_string);
        }
    }
    
    //PiMapper--------------
    piMapper.update();
    //now I need to set its speed here:
    if (ofGetFrameNum() % 50 == 0) {
        currSpeed = speed;
        louisiane->setSpeed(currSpeed);
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //PiMapper--------------
    piMapper.draw();
    
    //OSC-------------------
    if(hide == false){
        string buf;
        buf = "listening for osc messages on port" + ofToString(PORT);
        ofDrawBitmapString(buf, 10, 20);
        
        // draw mouse state
        buf = "speed: " + ofToString(speed);//speed,4 means 0.0000
        ofDrawBitmapString(buf, 430, 20);
        
        // read the buffer
        for(int i = 0; i < messageBuffer.size(); i++){
            ofDrawBitmapString(messageBuffer[i], 10, 40 + 15 * i);
        }
    }
}
//--------------------------------------------------------------

void ofApp::keyPressed(int key){
    piMapper.keyPressed(key);
    
    //Press key m to hide the text info
    switch (key) {
        case 'm':
            hide = ! hide;
            break;
    }
}

void ofApp::keyReleased(int key){
    piMapper.keyReleased(key);
}

void ofApp::mousePressed(int x, int y, int button){
    piMapper.mousePressed(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button){
    piMapper.mouseReleased(x, y, button);
}

void ofApp::mouseDragged(int x, int y, int button){
    piMapper.mouseDragged(x, y, button);
}

