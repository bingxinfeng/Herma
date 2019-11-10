#pragma once

#include "ofMain.h"

#include "ofxOsc.h"
// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20

#include "Settings.h"
#include "ofxPiMapper.h"
#include "Louisiane.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
    
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    
    //OSC for receiving message-------------------
    ofxOscReceiver receiver;
    float myTime;
    string myString;
    deque<string> messageBuffer;
    int maxBufferSize=20;
    
    float speed;
    
    //PiMapper for projection mapping-------------------
    ofxPiMapper piMapper;
    
    // By using a custom source that is derived from FboSource
    // you will be able to see the source listed in sources editor
    Louisiane * louisiane;
    float currSpeed;
    
    bool hide;
		
};
