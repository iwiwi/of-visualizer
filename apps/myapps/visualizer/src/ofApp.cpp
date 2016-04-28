#include "ofApp.h"
#include <boost/variant.hpp>

//
// External events, which are mainly assumed as user operations via javascript.
//
namespace ext_event {
struct set_solution {
  string solution;
};

struct set_input {
  string input;
};

struct set_input_and_solution {
  string input;
  string solution;
};

using info = boost::variant<
  set_solution,
  set_input,
  set_input_and_solution
  >;

queue<info> queue;
mutex queue_mutex;

void push(info i) {
  lock_guard<mutex> l(queue_mutex);
  queue.push(i);
}

void push_set_solution(const string &txt) {
  ext_event::queue.push(ext_event::set_solution{txt});
}

void push_set_input(const string &txt) {
  ext_event::queue.push(ext_event::set_input{txt});
}

void push_set_input_and_solution(const string &input, const string &solution) {
  ext_event::queue.push(ext_event::set_input_and_solution{input, solution});
}
}  // namespace ext_event

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("setSolution", &ext_event::push_set_solution);
  emscripten::function("setInput", &ext_event::push_set_input);
  emscripten::function("setInputAndSolution", &ext_event::push_set_input_and_solution);
}
#endif

//
// Official output checker
//
namespace chokudai {
int output_checker(string in_str, string out_str) {
  const int N = 30;

  try{
    // ifstream inp(argv[1]);
    // ifstream ans(argv[3]);
    // ifstream out(argv[2]);
    istringstream inp(in_str);
    istringstream out(out_str);
    int num[N][N] = {};

    int sum = 0;
    for (int i = 0; i < N; i++)
    {
      for (int j = 0; j < N; j++)
      {
        inp >> num[i][j];
        sum += num[i][j];
      }
    }

    int prey = -2;
    int prex = -2;
    int prenum = -1;

    int cnt = 0;
    for (int i = 0; i < sum; i++)
    {
      int y, x;
      out >> y >> x;
      y--; x--;
      if (y < 0 || x < 0 || y >= N || x >= N) continue;

      int d = abs(prey - y) + abs(prex - x);
      if (d != 1 || prenum != num[y][x] + 1) cnt++;

      prey = y;
      prex = x;
      prenum = num[y][x];
      num[y][x]--;
    }

    for (int i = 0; i < N; i++)
    {
      for (int j = 0; j < N; j++)
      {
        if (num[i][j] != 0) return -1;
      }
    }

    int point = 100000 - cnt;

    // printf("IMOJUDGE<<<%d>>>\n", point);
    //cout << "ok" << endl;
    return point;
  }
  catch(char* str){
	cerr << "error: " << str << endl;
    return -1;
  }
}
}  // namespace chokudai

//
// Global variables
//
namespace {
const int N = 30;
vector<vector<int>> A;
string A_str;
vector<pair<int, int>> sol;
vector<pair<int, int>> path;

int scale = 20;
int step;
}  // namespace

void set_input(const string &txt) {
  A.assign(N, vector<int>(N));
  istringstream ss(txt);
  for (int y = 0; y < N; ++y) {
    for (int x = 0; x < N; ++x) {
      if (!(ss >> A[y][x])) {
        cerr << "Error: input too short" << endl;
        return;
      }
    }
  }
  A_str = txt;
  // TODO: check EOF
}

void set_solution(const string &txt) {
  {
    int score = chokudai::output_checker(A_str, txt);
    printf("Score: %d\n", score);
    if (score < 0) return;  // Invalid solution
  }
  {
    istringstream ss(txt);
    sol.clear();
    for (int x, y; ss >> y >> x; ) {
      sol.emplace_back(x - 1, y - 1);
    }
  }
}

void move_step(int target) {
  while (step < target && step < (int)sol.size()) {
    --A[sol[step].second][sol[step].first];
    ++step;
  }
  while (step > target) {
    --step;
    ++A[sol[step].second][sol[step].first];
  }
}

void ofApp::setup(){
  if (A.empty()) {
    ifstream ifs("data/sample_input.txt");
    assert(ifs);
    set_input(string((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>()));
  }

  ofSetFrameRate(30);
  ofBackground(255, 255, 255);

  ofTrueTypeFont::setGlobalDpi(72);
  font.load("verdana.ttf", 12, true, true);

  gui.setup();
  gui.add(step_slider.setup("step", 0, 0, sol.size()));
  gui.add(speed_slider.setup("speed", 2.0, -1, 3));
  gui.add(history_slider.setup("history", 100, 1, 1000));
  gui.setPosition(N * scale, 0);

  ofSetWindowShape(N * scale + gui.getWidth(), N * scale);

  step = 0;
}

struct ext_event_handler : boost::static_visitor<void> {
  ext_event_handler(ofApp &app) : app_(app) {}

  void operator()(ext_event::set_solution &t) const {
    move_step(0);
    set_solution(t.solution);
    app_.step_slider.setMax(sol.size());
    app_.step_slider = 0;
  }

  void operator()(ext_event::set_input &t) const {
    move_step(0);
    set_input(t.input);
    set_solution("");
    app_.step_slider.setMax(sol.size());
    app_.step_slider = 0;
  }

  void operator()(ext_event::set_input_and_solution &t) const {
    move_step(0);
    set_input(t.input);
    set_solution(t.solution);
    app_.step_slider.setMax(sol.size());
    app_.step_slider = 0;
  }

  template<typename T>
  void operator()(T &t) const {
    assert(false);
  }

  ofApp &app_;
};

void ofApp::update(){
  // External events
  {
    lock_guard<mutex> l(ext_event::queue_mutex);

    while (!ext_event::queue.empty()) {
      ext_event::info i = ext_event::queue.front();
      ext_event::queue.pop();
      boost::apply_visitor(ext_event_handler(*this), i);
    }
  }

  // Proceed
  if ((int)step_slider < (int)sol.size()) {
    step_slider = min((float)sol.size(), step_slider + pow(10, speed_slider));
  }
  move_step((int)step_slider);
}

void ofApp::draw(){
  ofSetColor(0, 0, 0);

  ofSetLineWidth(1);
  for (int y = 0; y <= N; ++y) ofDrawLine(0, y * scale, N * scale, y * scale);
  for (int x = 0; x <= N; ++x) ofDrawLine(x * scale, 0, x * scale, N * scale);

  for (int y = 0; y < N; ++y) {
    for (int x = 0; x < N; ++x) {
      ofSetColor(ofColor::fromHsb(int((100 - A[y][x]) * 1.7), 255, 255));
      ofDrawRectangle(x * scale, y * scale, scale, scale);
      ofSetColor(0, 0, 0);
      font.drawString(to_string(A[y][x]), x * scale, (y + 1) * scale);
    }
  }

  for (int i = 0; i < history_slider; ++i) {
    int s = step - 1 - i;
    if (s < 0) break;
    int x = sol[s].first, y = sol[s].second;

    ofSetColor(0, 0, 0, 255 * (history_slider - i) / history_slider);
    ofDrawCircle((x + 0.5) * scale, (y + 0.5) * scale, 3);

    if (s > 1) {
      int tx = sol[s - 1].first, ty = sol[s - 1].second;
      if (abs(tx - x) + abs(ty - y) == 1 && A[ty][tx] == A[y][x] + 1) {
        ofVec2f p1((x + 0.5) * scale, (y + 0.5) * scale);
        ofVec2f p2((tx + 0.5) * scale, (ty + 0.5) * scale);
        ofVec2f pm = (p1 + p2) / 2.0;
        ofVec2f v = p1 - p2;

        ofSetLineWidth(max(1, scale / 10));
        ofDrawLine(p1, p2);
        ofDrawLine(pm, pm + v.getRotated(135) * 0.3);
        ofDrawLine(pm, pm + v.getRotated(-135) * 0.3);
      }
    }
  }

  gui.draw();
}

void ofApp::windowResized(int w, int h){
  scale = min((int)((w - gui.getWidth()) / N), h / N);
  gui.setPosition(N * scale, 0);
  font.load("verdana.ttf", scale / 2, true, true);
}

void ofApp::keyPressed(int key){}
void ofApp::keyReleased(int key){}
void ofApp::mouseMoved(int x, int y){}
void ofApp::mouseDragged(int x, int y, int button){}
void ofApp::mousePressed(int x, int y, int button){}
void ofApp::mouseReleased(int x, int y, int button){}
void ofApp::mouseEntered(int x, int y){}
void ofApp::mouseExited(int x, int y){}
void ofApp::gotMessage(ofMessage msg){}
void ofApp::dragEvent(ofDragInfo dragInfo){}
