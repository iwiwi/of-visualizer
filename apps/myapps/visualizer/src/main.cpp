#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(int argc, char **argv){
  {
    ifstream ifs(argc > 1 ? argv[1] : "data/sample_input.txt");
    assert(ifs);
    set_input(string((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>()));
  }
  if (argc > 2) {
    ifstream ifs(argv[2]);
    assert(ifs);
    set_solution(string((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>()));
  }


  ofSetupOpenGL(1024,/*768*/ 30 * 20,OF_WINDOW);			// <-------- setup the GL context

  // this kicks off the running of my app
  // can be OF_WINDOW or OF_FULLSCREEN
  // pass in width and height too:
  ofRunApp(new ofApp());
}
