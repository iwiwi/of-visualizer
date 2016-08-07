#include "ofMain.h"
#include "ofApp.h"
#include "common.h"

//========================================================================
int main(int argc, char **argv){
  I a("100");
  R b(100, 200);
  cout << a << b << endl;

  {
    ifstream ifs(argc > 1 ? argv[1] : "data/14.txt");
    assert(ifs);
    SetInput(string((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>()));
  }
  if (argc > 2) {
    solver_command = argv[2];
  }

  {
    char dir[1024];
    getcwd(dir, 1024);
    output_directory = dir;
  }

  ofSetupOpenGL(1024, 768, OF_WINDOW);			// <-------- setup the GL context

  // this kicks off the running of my app
  // can be OF_WINDOW or OF_FULLSCREEN
  // pass in width and height too:
  ofRunApp(new ofApp());
}
