#include <iostream>
#include <utility>
#include "trapezoid_map.h"
using namespace std;

int main(int argc, char** argv) {
  vector <segment> v;
  double a, b, c, d;
  while (cin >> a >> b >> c >> d) {
    v.emplace_back(a, b, c, d);
  }

  trapezoid_map map(argc, argv, move(v), 30.0);
  return map.visualize();
}
