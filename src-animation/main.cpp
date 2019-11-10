#include "ofMain.h"
#include "ofApp.h"

#include <string>
#include <vector>
#include "Settings.h"

//========================================================================
//int main( ){
//    ofSetupOpenGL(1024,768,OF_WINDOW);            // <-------- setup the GL context
//
//    // this kicks off the running of my app
//    // can be OF_WINDOW or OF_FULLSCREEN
//    // pass in width and height too:
//    ofRunApp(new ofApp());
//
//}
int main(int argc, char * argv[]){
    bool fullscreen = false;
    
    vector<string> arguments = vector<string>(argv, argv + argc);
    for(int i = 0; i < arguments.size(); ++i){
        if(arguments.at(i) == "-f"){
            fullscreen = true;
            break;
        }
    }
    
    //    Settings::instance()->setFullscreen(true);
    Settings::instance()->setFullscreen(fullscreen);
    
    ofSetupOpenGL(800, 450, OF_WINDOW);
    ofRunApp(new ofApp());
}
