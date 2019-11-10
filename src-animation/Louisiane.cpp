#include "Louisiane.h"

void Louisiane::setup(){
	// Give our source a decent name
    name = "Louisiane";
   
	// Allocate our FBO source, decide how big it should be
    float pc = 0.95;
    allocate(291*pc, 1080*pc);
    
    //L video:
    ofSetVerticalSync(true);
    frameByframe = false;
    myMovie.load("HERMA-Slow-Long.mp4");
    myMovie.setLoopState(OF_LOOP_NORMAL);
    myMovie.play();

}
//--------------------------------------------------------------------
void Louisiane::setName(string _name){
    name = _name;
}
//--------------------------------------------------------------------
void Louisiane::update(){
    myMovie.update();
}
//--------------------------------------------------------------------
// No need to take care of fbo.begin() and fbo.end() here.
// All within draw() is being rendered into fbo;
void Louisiane::draw(){
    ofClear(0); //clear the buffer
    
    //draw the videos
    myMovie.draw(0,0,fbo->getWidth(),fbo->getHeight());
}
//--------------------------------------------------------------------
void Louisiane::setSpeed(float sp){
     myMovie.setSpeed(sp);
}
