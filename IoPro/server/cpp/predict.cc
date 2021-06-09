#include <cPMML.h>
#include <iostream>
#include <map>
using namespace std;

const std::string model_filepath("./pmmlDecisionTree.pmml");
const cpmml::Model model(model_filepath);

int predictIo(string size, string read, string write, string randread,
              string randwrite) {
  int ret = 0;
  unordered_map<string, string> params = {
      {"x1", size},  {"x2", randread},      {"x3", randwrite},    {"x4", read},
      {"x5", write}, {"x6", to_string(90)}, {"x7", to_string(1)},
  };
  ret = stoi(model.predict(params));
  cout << ret << endl;
  return ret;
}

int main(int argc, char *argv[]) {
  // for (int i = 0; i < argc; i++) {
  // cout << "argument[" << i << "] is: " << argv[i] << endl;
  //}
  predictIo(argv[1], argv[2], argv[3], argv[4], argv[5]);
}
