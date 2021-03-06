#include <iostream>
#include <cstdio>
#include "TMap_Widget.h"
#include "trapezoid_map.h"
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
using namespace std;

const int WIDTH = 1350;
const int HEIGHT = 750;

void segment::draw(double scale, point begin) {
  double x1 = (start.x - begin.x + 1.0) * scale;
  double y1 = HEIGHT - (start.y - begin.y + 1.0) * scale;
  double x2 = (end.x - begin.x + 1.0) * scale;
  double y2 = HEIGHT - (end.y - begin.y + 1.0) * scale;
  fl_line(x1, y1, x2, y2);
}

void trapezoid::draw(double scale, point begin) {
  top->draw(scale, begin);
  bottom->draw(scale, begin);
  
  int left_high_end = HEIGHT - 1;
  int left_low_end = 1;
  int right_high_end = HEIGHT - 1;
  int right_low_end = 1;
  
  if (left.x >= top->start.x && left.x <= top->end.x) {
    left_high_end = left.projection(top);
  }
  if (left.x >= bottom->start.x && left.x <= bottom->end.x) {
    left_low_end = left.projection(bottom);
  }
  if (right.x >= top->start.x && right.x <= top->end.x) {
    right_high_end = right.projection(top);
  }
  if (right.x >= bottom->start.x && right.x <= bottom->end.x) {
    right_low_end = right.projection(bottom);
  }

  segment(left.x, left_high_end, left.x, left_low_end).draw(scale, begin);
  segment(right.x, right_high_end, right.x, right_low_end).draw(scale, begin);
}

TMap_Widget::TMap_Widget(int x, int y, int w, int h, const char *label = 0) : Fl_Widget(x, y, w, h, label) {}

void TMap_Widget::draw() {
  fl_draw_box(FL_FLAT_BOX, 0, 0, WIDTH, HEIGHT, FL_WHITE);		
  fl_color(FL_BLACK);
  for (int i = 0; i < (int)segments.size(); ++i) {
    segments[i].draw(scale, bounds.left);
  }
  
  bounds.draw(scale, bounds.left);
}

int TMap_Widget::handle(int event) {
  switch(event) {
  case FL_PUSH:
    fl_color(FL_BLACK);
    if (prev != 0) 
      prev->draw(scale, bounds.left);

    double x1 = (Fl::event_x()) / scale + map->bound.bottom->start.x - 1.0;
    double y1 = ((HEIGHT - Fl::event_y()) / scale) + map->bound.bottom->start.y - 1.0;

    segment* s = map->find(point(x1, y1));
    prev = s;
    fl_color(FL_RED);
    s->draw(scale, bounds.left);
    return 1;
  }
  return 0;
}

double calc_scale(trapezoid t) {
  double lx = t.right.x - t.left.x + 1.0;
  double ly = t.right.y - t.left.y + 1.0;
  double allx = WIDTH - 1.0;
  double ally = HEIGHT - 1.0;
  if (lx * ally < ly * allx) {
    return ally / ly;
  } else {
    return allx / lx;
  }
    
}

int trapezoid_map::visualize(int argc, char** argv) {
  Fl_Window *window = new Fl_Window(WIDTH, HEIGHT);
  TMap_Widget *tmap = new TMap_Widget(0, 0, WIDTH, HEIGHT);

  tmap->segments = move(segments);
  tmap->bounds = bound;
  tmap->scale = calc_scale(tmap->bounds);
  tmap->map = this;

  window->end();
  window->show(argc, argv);
  int res = Fl::run();
  delete(tmap);
  delete(window);
  return res;
}
