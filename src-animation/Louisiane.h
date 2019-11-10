#pragma once

#include "ofMain.h"
#include "FboSource.h"


class Louisiane : public ofx::piMapper::FboSource {
	public:
        void setup();
		void update();
		void draw();
        void setName(string _name);
    
    void exit();
    void drawPointCloud();
    void keyPressed(int key);
    void windowResized(int w, int h);
    
    
    // L:
    float parameter;
    ofVideoPlayer myMovie;
    bool frameByframe;
    int frameNum;
    
    void setSpeed(float sp);

};

