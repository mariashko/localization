#include "search.h"
#include "trapezoid_map.h"
using namespace std;

trapezoid count_bounds(vector <segment> &v, double gap) {
  double max_x = 0, max_y = 0, min_x = 1400, min_y = 1400;

  for (int i = 0; i < (int)v.size(); ++i) {
    max_x = max(max_x, max(v[i].start.x, v[i].end.x));
    min_x = min(min_x, min(v[i].start.x, v[i].end.x));
    max_y = max(max_y, max(v[i].start.y, v[i].end.y));
    min_y = min(min_y, min(v[i].start.y, v[i].end.y));
  }

  if (v.size() == 0) {
    max_x = 1348;
    max_y = 748;
    min_x = 1;
    min_y = 1;
  } else {
    min_x -= gap;
    min_y -= gap;
    max_x += gap;
    max_y += gap;
  }

  segment* top = new segment(min_x, max_y, max_x, max_y);
  segment* bottom = new segment(min_x, min_y, max_x, min_y);
  return trapezoid(top, bottom, bottom->start, top->end);
}

// FIX
void set_link(shared_ptr<node> old, shared_ptr<node> mid, trapezoid* t) {
  auto old_prev = old.get()->get_previous();
  for (int i = 0; i < old_prev.size(); ++i) {
    node* curr = old_prev[i].get()->get_left().get();
    auto r = dynamic_cast<trapezoid_node*>(curr);
    
    if (curr != NULL && r->get_trapezoid() == t) {
      old_prev[i].get()->set_left(mid);
    } else {
      old_prev[i].get()->set_right(mid);
    }
    mid.get()->set_previous(old_prev[i]);
  }
}

void set_children(shared_ptr<node> mid, shared_ptr<node> up, shared_ptr<node> ddown) {
  mid.get()->set_left(up);
  up.get()->set_previous(mid);
  mid.get()->set_right(down);
  down.get()->set_previous(mid);
}

void trapezoid_map::single_trap(trapezoid* t, segment* s) {
  auto up = shared_ptr<node>(new trapezoid_node(t->top, s, s->start, s->end));
  auto down = shared_ptr<node>(new trapezoid_node(s, t->bottom, s->end, s->end));
  auto mid = shared_ptr<node>(new segment_node(s));
  auto left = shared_ptr<node>(new trapezoid_node(t->top, t->bottom, t->left, s->start));
  auto mid1 = shared_ptr<node>(new point_node(s->start));
  auto right = shared_ptr<node>(new trapezoid_node(t->top, t->bottom, s->end, t->right));
  auto mid2 = shared_ptr<node>(new point_node(s->end));

  set_children(mid, up, down);
  auto old = link[t];
  
  if (s->start == t->left && s->end == t->right) {
        //set previous
    set_link(old, mid, t);
    return;
  }
  set_link(old, mid1, t);
  if (s->start == t->left) {
    //set previous
    set_children(mid2, mid, right);
  } else if (s->end == t->right) {
    //set previous
    set_children(mid1, left, mid);
  } else {
    //set previous
    set_children(mid1, left, mid2);
    set_children(mid2, mid, right);
  }
}



void trapezoid_map::add_segment(segment& s){
    trapezoid* fst = find(s.start, &s);
    trapezoid* curr = fst;

    if (fst->right < s.end || fst->right == s.end) {
      single_trap(fst, &s);
      return;
    } 

    auto up = shared_ptr<node>(new trapezoid_node(fst->top, &s, s.start, fst->right));
    auto down = shared_ptr<node>(new trapezoid_node(&s, fst->bottom, s.start, fst->right));
    auto mid = shared_ptr<node>(new segment_node(&s));
    auto left = shared_ptr<node>(new trapezoid_node(fst->top, fst->bottom, fst->left, s.start));
    auto mid1 = shared_ptr<node>(new point_node(s.start));

    //set previous
    set_children(mid, up, down);

    auto old = link[fst];
    if (s.start == fst->left) {
      set_link(old, mid, fst);
    } else {
      set_link(old, mid1, fst);
        //set previous
      set_children(mid1, left, mid);
    }

    if (left_turn(s.start, s.end, fst->right) > 0)
      curr = fst->right_down;
    else
      curr = fst->right_up;

    while (!(curr->right < s.end || curr->right == s.end)) {
      shared_ptr<node> curr_up;
      shared_ptr<node> curr_down;
      mid = shared_ptr<node>(new segment_node(&s));

      if (left_turn(s.start, s.end, curr->left) > 0) {
        curr_up = shared_ptr<node>(new trapezoid_node(curr->top, &s, curr->left, curr->right));
        curr_down = down;
        //set previous
      } else {
        curr_up = up;
        curr_down = shared_ptr<node>(new trapezoid_node(&s, curr->bottom, curr->left, curr->right));
        //set previous
      }

      //set previous
      set_children(mid, up, down);
      old = link[curr];
      set_link(old, mid, curr);

      if (left_turn(s.start, s.end, curr->right) > 0)
        curr = curr->right_down;
      else
        curr = curr->right_up;
      up = curr_up;
      down = curr_down;
    }


    up = shared_ptr<node>(new trapezoid_node(curr->top, &s, s.start, curr->right));
    down = shared_ptr<node>(new trapezoid_node(&s, curr->bottom, s.start, curr->right));
    mid = shared_ptr<node>(new segment_node(&s));
    auto right = shared_ptr<node>(new trapezoid_node(curr->top, curr->bottom, s.end, curr->right));
    auto mid2 = shared_ptr<node>(new point_node(s.end));

    //set previous
    set_children(mid, up, down);
    old = link[curr];
    
    if (s.end == curr->right) {
      set_link(old, mid, curr);
    } else {
      set_link(old, mid2, curr);
      //set previous
      set_children(mid2, mid, right);
    }
}


trapezoid_map::trapezoid_map(int ac, char** av, vector<segment> v, double g) {
  segments = move(v);
  bound = count_bounds(segments, gap);
  argc = ac;
  argv = av;
  gap = g;

  trapezoid_node* ptr = new trapezoid_node(bound.top, bound.bottom, bound.left, bound.right);
  shared_ptr<node> t = shared_ptr<node>(ptr);
  link[ptr->get_trapezoid()] = t;
  root = t;
  for (int i = 0; i < segments.size(); ++i) {
    add_segment(segments[i]);
  }
}

trapezoid_map::~trapezoid_map() {
  free(bound.top);
  free(bound.bottom);
}

segment* trapezoid_map::find(point p){
  std::shared_ptr<node> curr(root);
  int res;
  while ((res = curr.get()->get_direction(p)) != 0) {
    if (res < 0)
      curr = curr.get()->get_left();
    else
      curr = curr.get()->get_right();
  }
  return curr.get()->get_segment();
}

trapezoid* trapezoid_map::find(point p, segment* s){
  std::shared_ptr<node> curr(root);
  int res;
  while ((res = curr.get()->get_direction(p, s)) != 0) {
    if (res < 0)
      curr = curr.get()->get_left();
    else
      curr = curr.get()->get_right();
  }
  
  auto r = dynamic_cast<trapezoid_node*>(curr.get());
  return r->get_trapezoid();
}
