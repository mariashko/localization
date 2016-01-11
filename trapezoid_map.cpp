#include "search.h"
#include <iostream>
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

  
  //cout << min_x << " " << max_x << " " << min_y << " " << max_y << endl;
  segment* top = new segment(min_x, max_y, max_x, max_y);
  segment* bottom = new segment(min_x, min_y, max_x, min_y);
  return trapezoid(top, bottom, bottom->start, top->end);
}

void trapezoid_map::set_link(shared_ptr<node> old, shared_ptr<node> mid, trapezoid* t) {
  if (old.get() == root.get()) {
    root = mid;
  }
  
  auto old_prev = old.get()->get_previous();
  for (int i = 0; i < old_prev.size(); ++i) {
    node* curr = old_prev[i].get()->get_left().get();
    auto r = dynamic_cast<trapezoid_node*>(curr);
    
    if (r != NULL && r->get_trapezoid() == t) {
      old_prev[i].get()->set_left(mid);
    } else {
      old_prev[i].get()->set_right(mid);
    }
    mid.get()->set_previous(old_prev[i]);
  }
}

void set_children(shared_ptr<node> mid, shared_ptr<node> up, shared_ptr<node> down) {
  mid.get()->set_left(up);
  up.get()->set_previous(mid);
  mid.get()->set_right(down);
  down.get()->set_previous(mid);
}

void set_right_up(trapezoid* curr1, trapezoid* curr2) {
  if (curr1 != 0) {
      curr1->right_up = curr2;
      curr2->left_up = curr1;
    }
}
void set_right_down(trapezoid* curr1, trapezoid* curr2) {
  if (curr1 != 0) {
      curr1->right_down = curr2;
      curr2->left_down = curr1;
    }
}
void set_left_down(trapezoid* curr1, trapezoid* curr2) {
  if (curr1 != 0) {
      curr1->left_down = curr2;
      curr2->right_down = curr1;
    }
}
void set_left_up(trapezoid* curr1, trapezoid* curr2) {
  if (curr1 != 0) {
      curr1->left_up = curr2;
      curr2->right_up = curr1;
    }
}

shared_ptr<node> trapezoid_map::create_trapezoid_node(segment* a, segment* b, point c, point d) {
  trapezoid_node* ptr = new trapezoid_node(a, b, c, d);
  shared_ptr<node> t = shared_ptr<node>(ptr);
  link[ptr->get_trapezoid()] = t;
  return t;
}

void trapezoid_map::single_trap(trapezoid* t, segment* s) {
  auto up1 = new trapezoid_node(t->top, s, s->start, s->end);
  auto up = shared_ptr<node>(up1);

  auto down1 = new trapezoid_node(s, t->bottom, s->start, s->end);
  auto down = shared_ptr<node>(down1);

  auto mid = shared_ptr<node>(new segment_node(s));

  auto left1 = new trapezoid_node(t->top, t->bottom, t->left, s->start);
  auto left = shared_ptr<node>(left1);

  auto mid1 = shared_ptr<node>(new point_node(s->start));

  auto right1 = new trapezoid_node(t->top, t->bottom, s->end, t->right);
  auto right = shared_ptr<node>(right1);

  auto mid2 = shared_ptr<node>(new point_node(s->end));


  set_children(mid, up, down);
  link[up1->get_trapezoid()] = up;
  link[down1->get_trapezoid()] = down;

  auto old = link[t];
  
  if (s->start == t->left && s->end == t->right) {
    //cout << "1\n";
    set_right_up(t->left_up, up1->get_trapezoid());
    set_right_down(t->left_down, down1->get_trapezoid());

    set_left_up(t->right_up, up1->get_trapezoid());
    set_left_down(t->right_down, down1->get_trapezoid());

    set_link(old, mid, t);
    return;
  }

  if (s->start == t->left) {
    //cout << "2\n";

    link[right1->get_trapezoid()] = right;

    set_right_up(t->left_up, up1->get_trapezoid());
    set_right_down(t->left_down, down1->get_trapezoid());

    set_right_up(up1->get_trapezoid(), right1->get_trapezoid());
    set_right_down(down1->get_trapezoid(), right1->get_trapezoid());

    set_left_up(t->right_up, right1->get_trapezoid());
    set_left_down(t->right_down, right1->get_trapezoid());
    
    set_children(mid2, mid, right);
    set_link(old, mid2, t);

  } else if (s->end == t->right) {
    //cout << "3\n";
    link[left1->get_trapezoid()] = left;

    set_left_up(t->right_up, up1->get_trapezoid());
    set_left_down(t->right_down, down1->get_trapezoid());

    set_left_up(up1->get_trapezoid(), left1->get_trapezoid());
    set_left_down(down1->get_trapezoid(), left1->get_trapezoid());

    set_right_up(t->left_up, left1->get_trapezoid());
    set_right_down(t->left_down, left1->get_trapezoid());

    set_children(mid1, left, mid);
    set_link(old, mid1, t);

  } else {
    //cout << "4\n";
    link[right1->get_trapezoid()] = right;
    link[left1->get_trapezoid()] = left;

    set_right_up(t->left_up, left1->get_trapezoid());
    set_right_down(t->left_down, left1->get_trapezoid());

    set_left_up(t->right_up, right1->get_trapezoid());
    set_left_down(t->right_down, right1->get_trapezoid());

    set_left_up(up1->get_trapezoid(), left1->get_trapezoid());
    set_left_down(down1->get_trapezoid(), left1->get_trapezoid());

    set_right_up(up1->get_trapezoid(), right1->get_trapezoid());
    set_right_down(down1->get_trapezoid(), right1->get_trapezoid());
    
    set_children(mid1, left, mid2);
    set_children(mid2, mid, right);
    set_link(old, mid1, t);
  }
}

void trapezoid_map::add_segment(segment& s){
  cout << "lolo" << endl;
  cout << s.start.x << " " << s.start.y << " " << s.end.x << " " << s.end.y << endl;
  //cout << "lolo" << endl;
    trapezoid* fst = find(s.start, &s);
    trapezoid* curr = fst;

    //cout << fst->right.x << " " << fst->right.y << endl;
    //cout << "lol" << endl;
    //cerr << fst->top->start.x << " "  << fst->top->start.y << " "  << fst->top->end.x << " " << fst->top->end.y << endl;
    //cerr << fst->bottom->start.x << " "  << fst->bottom->start.y << " "  << fst->bottom->end.x << " " << fst->bottom->end.y << endl;
    if (s.end < fst->right || fst->right == s.end) {
      //cout << "fuck" << endl;
      single_trap(fst, &s);
      return;
    } 

    auto up1 = new trapezoid_node(fst->top, &s, s.start, fst->right);
    auto up = shared_ptr<node>(up1);

    auto down1 = new trapezoid_node(&s, fst->bottom, s.start, fst->right);
    auto down = shared_ptr<node>(down1);

    auto mid = shared_ptr<node>(new segment_node(&s));

    auto left1 = new trapezoid_node(fst->top, fst->bottom, fst->left, s.start);
    auto left = shared_ptr<node>(left1);
    
    auto mid1 = shared_ptr<node>(new point_node(s.start));

    set_children(mid, up, down);
    link[up1->get_trapezoid()] = up;
    link[down1->get_trapezoid()] = down;

    auto old = link[fst];
    if (s.start == fst->left) {
      //cout << "%" << endl;
      set_right_up(fst->left_up, up1->get_trapezoid());
      set_right_down(fst->left_down, down1->get_trapezoid());
      set_link(old, mid, fst);
    } else {
      //cout << "%%" << endl;
      link[left1->get_trapezoid()] = left;
      set_right_up(fst->left_up, left1->get_trapezoid());
      set_right_down(fst->left_down, left1->get_trapezoid());

      set_left_up(up1->get_trapezoid(), left1->get_trapezoid());
      set_left_down(down1->get_trapezoid(), left1->get_trapezoid());

      set_link(old, mid1, fst);
      set_children(mid1, left, mid);
    }

    if (left_turn(s.start, s.end, fst->right) > 0) {
      //cout << "1" << endl;
      curr = fst->right_down;
      if (fst->right_up != 0)
        set_right_up(up1->get_trapezoid(), fst->right_up);
    } else {
      //cout << "2" << endl;
      curr = fst->right_up;
      if (fst->right_down != 0)
        set_right_down(down1->get_trapezoid(), fst->right_down);
    }


    shared_ptr<node> curr_up;
    shared_ptr<node> curr_down;
    trapezoid_node* curr_up1;
    trapezoid_node* curr_down1;


    
    //cout << curr->right.x << " " << curr->right.y << endl;
    
    while (!(s.end < curr->right || curr->right == s.end)) {
      //cout << "fuf" << endl;

      //cerr << curr->top->start.x << " "  << curr->top->start.y << " "  << curr->top->end.x << " " << curr->top->end.y << endl;
      //cerr << curr->bottom->start.x << " "  << curr->bottom->start.y << " "  << curr->bottom->end.x << " " << curr->bottom->end.y << endl;

      
      mid = shared_ptr<node>(new segment_node(&s));

      if (left_turn(s.start, s.end, curr->left) > 0) {
        //cout << "-" << endl;
        curr_up1 = new trapezoid_node(curr->top, &s, curr->left, curr->right);
        curr_up = shared_ptr<node>(curr_up1);
        link[curr_up1->get_trapezoid()] = curr_up;
        up1->get_trapezoid()->right = curr->left;

        curr_down = down;
        curr_down1 = down1;

        set_right_down(up1->get_trapezoid(), curr_up1->get_trapezoid());
        if (curr->left_up != 0)
          set_left_up(curr_up1->get_trapezoid(), curr->left_up);
      } else {

        //cout << "--" << endl;
        curr_up = up;
        curr_up1 = up1;

        curr_down1 = new trapezoid_node(&s, curr->bottom, curr->left, curr->right);
        curr_down = shared_ptr<node>(curr_down1);
        link[curr_down1->get_trapezoid()] = curr_down;
        down1->get_trapezoid()->right = curr->left;

        set_right_up(down1->get_trapezoid(), curr_down1->get_trapezoid());
        if (curr->left_down != 0)
          set_left_down(curr_down1->get_trapezoid(), curr->left_down);
      }

      set_children(mid, curr_up, curr_down);
      old = link[curr];
      set_link(old, mid, curr);

      if (left_turn(s.start, s.end, curr->right) > 0) {
        //cout << "?" << endl;
        if (curr->right_up != 0)
          set_right_up(curr_up1->get_trapezoid(), curr->right_up);
        curr = curr->right_down;
      } else {
        //cout << "??" << endl;
        if (curr->right_down != 0)
          set_right_down(curr_down1->get_trapezoid(), curr->right_down);
        curr = curr->right_up;
      }

      up = curr_up;
      down = curr_down;
      up1 = curr_up1;
      down1 = curr_down1;
    }

    mid = shared_ptr<node>(new segment_node(&s));

    if (left_turn(s.start, s.end, curr->left) > 0) {
      //cout << "-" << endl;
      curr_up1 = new trapezoid_node(curr->top, &s, curr->left, s.end);
      curr_up = shared_ptr<node>(curr_up1);
      link[curr_up1->get_trapezoid()] = curr_up;
      up1->get_trapezoid()->right = curr->left;
      
      curr_down = down;
      curr_down1 = down1;
      curr_down1->get_trapezoid()->right = s.end;
      link[curr_down1->get_trapezoid()] = curr_down;

      set_right_down(up1->get_trapezoid(), curr_up1->get_trapezoid());
      if (curr->left_up != 0)
        set_left_up(curr_up1->get_trapezoid(), curr->left_up);
    } else {
      //cout << "--" << endl;

      curr_up = up;
      curr_up1 = up1;
      curr_up1->get_trapezoid()->right = s.end;
      link[curr_up1->get_trapezoid()] = curr_up;
      
      curr_down1 = new trapezoid_node(&s, curr->bottom, curr->left, s.end);
      curr_down = shared_ptr<node>(curr_down1);
      link[curr_down1->get_trapezoid()] = curr_down;
      down1->get_trapezoid()->right = curr->left;

      set_right_up(down1->get_trapezoid(), curr_down1->get_trapezoid());
      if (curr->left_down != 0)
        set_left_down(curr_down1->get_trapezoid(), curr->left_down);
    }

    old = link[curr];

    set_children(mid, curr_up, curr_down);
    if (s.end == curr->right) {
      set_left_up(curr->right_up, curr_up1->get_trapezoid());
      set_left_down(curr->right_down, curr_down1->get_trapezoid());
      set_link(old, mid, curr);
    } else {
      auto right1 = new trapezoid_node(curr->top, curr->bottom, s.end, curr->right);
      auto right = shared_ptr<node>(right1);
      link[right1->get_trapezoid()] = right;

      auto mid2 = shared_ptr<node>(new point_node(s.end));

      set_children(mid2, mid, right);
      set_link(old, mid2, curr);

      set_right_up(curr_up1->get_trapezoid(), right1->get_trapezoid());
      set_right_down(curr_down1->get_trapezoid(), right1->get_trapezoid());

      set_left_up(curr->right_up, right1->get_trapezoid());
      set_left_down(curr->right_down, right1->get_trapezoid());
    }
}

trapezoid_map::trapezoid_map(vector<segment> v, double g) {
  segments = move(v);
  bound = count_bounds(segments, g);
  gap = g;

  root = create_trapezoid_node(bound.top, bound.bottom, bound.left, bound.right);
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
    //cout << res << endl;
    if (res > 0)
      curr = curr.get()->get_left();
    else
      curr = curr.get()->get_right();
  }
  return curr.get()->get_segment(bound);
}

trapezoid* trapezoid_map::find(point p, segment* s){
  std::shared_ptr<node> curr(root);
  int res;
  while ((res = curr.get()->get_direction(p, s)) != 0) {
    //cout << res << endl;
    if (res > 0)
      curr = curr.get()->get_left();
    else
      curr = curr.get()->get_right();
  }
  
  auto r = dynamic_cast<trapezoid_node*>(curr.get());
  return r->get_trapezoid();
}

