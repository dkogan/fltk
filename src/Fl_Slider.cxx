//
// Slider widget for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2015 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include <FL/Fl.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Fill_Slider.H>
#include <FL/Fl_Hor_Slider.H>
#include <FL/Fl_Hor_Fill_Slider.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Nice_Slider.H>
#include <FL/fl_draw.H>
#include <math.h>
#include "flstring.h"


void Fl_Slider::_Fl_Slider() {
  slider_size_ = 0;
  slider_ = 0; // FL_UP_BOX;
}

/**
  Creates a new Fl_Slider widget using the given position,
  size, and label string. The default boxtype is FL_DOWN_BOX.
*/
Fl_Slider::Fl_Slider(int X, int Y, int W, int H, const char* L)
: Fl_Valuator(X, Y, W, H, L) {
  box(FL_DOWN_BOX);
  _Fl_Slider();
}

/**
  Creates a new Fl_Slider widget using the given type, position,
  size, and label string.
*/
Fl_Slider::Fl_Slider(uchar t, int X, int Y, int W, int H, const char* L)
  : Fl_Valuator(X, Y, W, H, L) {
  type(t);
  box(t==FL_HOR_NICE_SLIDER || t==FL_VERT_NICE_SLIDER ?
      FL_FLAT_BOX : FL_DOWN_BOX);
  _Fl_Slider();
}

void Fl_Slider::slider_size(double v) {
  if (v <  0) v = 0;
  if (v > 1) v = 1;
  if (slider_size_ != float(v)) {
    slider_size_ = float(v);
    damage(FL_DAMAGE_EXPOSE);
  }
}

/**
  Sets the minimum (a) and maximum (b) values for the valuator widget.
  if at least one of the values is changed, a partial redraw is asked.
*/
void Fl_Slider::bounds(double a, double b) {
  if (minimum() != a || maximum() != b) {
    Fl_Valuator::bounds(a, b);
    damage(FL_DAMAGE_EXPOSE);
  }
}

/**
  Sets the size and position of the sliding knob in the box.
  \param[in] pos position of first line displayed
  \param[in] size size of window in lines
  \param[in] first number of first line
  \param[in] total total number of lines
  Returns Fl_Valuator::value(p)
 */
int Fl_Slider::scrollvalue(int pos, int size, int first, int total) {
  step(1, 1);
  if (pos+size > first+total) total = pos+size-first;
  slider_size(size >= total ? 1.0 : double(size)/double(total));
  bounds(first, total-size+first);
  return value(pos);
}

// All slider interaction is done as though the slider ranges from
// zero to one, and the left (bottom) edge of the slider is at the
// given position.  Since when the slider is all the way to the
// right (top) the left (bottom) edge is not all the way over, a
// position on the widget itself covers a wider range than 0-1,
// actually it ranges from 0 to 1/(1-size).

void Fl_Slider::draw_bg(int X, int Y, int W, int H) {
  fl_push_clip(X, Y, W, H);
  draw_box();
  fl_pop_clip();

  Fl_Color black = active_r() ? FL_FOREGROUND_COLOR : FL_INACTIVE_COLOR;
  if (type() == FL_VERT_NICE_SLIDER) {
    draw_box(FL_THIN_DOWN_BOX, X+W/2-2, Y, 4, H, black);
  } else if (type() == FL_HOR_NICE_SLIDER) {
    draw_box(FL_THIN_DOWN_BOX, X, Y+H/2-2, W, 4, black);
  }
}

static double val_linear01_from_value(const double value,
                                      const double min,
                                      const double max) {
  if (min == max)
    return 0.5;

  const double val_linear01 =
    (value-min)/(max-min);
  if (val_linear01 > 1.0) return 1.0;
  if (val_linear01 < 0.0) return 0.0;
  return val_linear01;
}

void Fl_Slider::draw(int X, int Y, int W, int H) {

  const double val_linear01 =
    val_linear01_from_value(value(), minimum(), maximum());

  int length_px = (horizontal() ? W : H);
  int handle_start_edge_px, handle_size_px;
  if (type()==FL_HOR_FILL_SLIDER || type() == FL_VERT_FILL_SLIDER) {
    handle_size_px = int(val_linear01*length_px+.5);
    if (minimum()>maximum()) {handle_size_px = length_px-handle_size_px; handle_start_edge_px = length_px-handle_size_px;}
    else handle_start_edge_px = 0;
  } else {
    handle_size_px = int(slider_size_*length_px+.5);
    int handle_size_min_px = (horizontal() ? H : W)/2+1;
    if (type()==FL_VERT_NICE_SLIDER || type()==FL_HOR_NICE_SLIDER) handle_size_min_px += 4;
    if (handle_size_px < handle_size_min_px) handle_size_px = handle_size_min_px;
    handle_start_edge_px = int(val_linear01*(length_px-handle_size_px)+.5);
  }
  int xsl, ysl, wsl, hsl;
  if (horizontal()) {
    xsl = X+handle_start_edge_px;
    wsl = handle_size_px;
    ysl = Y;
    hsl = H;
  } else {
    ysl = Y+handle_start_edge_px;
    hsl = handle_size_px;
    xsl = X;
    wsl = W;
  }

  draw_bg(X, Y, W, H);

  Fl_Boxtype box1 = slider();
  if (!box1) {box1 = (Fl_Boxtype)(box()&-2); if (!box1) box1 = FL_UP_BOX;}
  if (type() == FL_VERT_NICE_SLIDER) {
    draw_box(box1, xsl, ysl, wsl, hsl, FL_GRAY);
    int d = (hsl-4)/2;
    draw_box(FL_THIN_DOWN_BOX, xsl+2, ysl+d, wsl-4, hsl-2*d,selection_color());
  } else if (type() == FL_HOR_NICE_SLIDER) {
    draw_box(box1, xsl, ysl, wsl, hsl, FL_GRAY);
    int d = (wsl-4)/2;
    draw_box(FL_THIN_DOWN_BOX, xsl+d, ysl+2, wsl-2*d, hsl-4,selection_color());
  } else {
    if (wsl>0 && hsl>0) draw_box(box1, xsl, ysl, wsl, hsl, selection_color());

    if (type() != FL_HOR_FILL_SLIDER && type() != FL_VERT_FILL_SLIDER &&
        Fl::is_scheme("gtk+")) {
      if (W>H && wsl>(hsl+8)) {
        // Draw horizontal grippers
        int yy, hh;
        hh = hsl-8;
        handle_start_edge_px = xsl+(wsl-hsl-4)/2;
        yy = ysl+3;

        fl_color(fl_darker(selection_color()));
        fl_line(handle_start_edge_px, yy+hh, handle_start_edge_px+hh, yy);
        fl_line(handle_start_edge_px+6, yy+hh, handle_start_edge_px+hh+6, yy);
        fl_line(handle_start_edge_px+12, yy+hh, handle_start_edge_px+hh+12, yy);

        handle_start_edge_px++;
        fl_color(fl_lighter(selection_color()));
        fl_line(handle_start_edge_px, yy+hh, handle_start_edge_px+hh, yy);
        fl_line(handle_start_edge_px+6, yy+hh, handle_start_edge_px+hh+6, yy);
        fl_line(handle_start_edge_px+12, yy+hh, handle_start_edge_px+hh+12, yy);
      } else if (H>W && hsl>(wsl+8)) {
        // Draw vertical grippers
        int yy;
        handle_start_edge_px = xsl+4;
        length_px = wsl-8;
        yy = ysl+(hsl-wsl-4)/2;

        fl_color(fl_darker(selection_color()));
        fl_line(handle_start_edge_px, yy+length_px, handle_start_edge_px+length_px, yy);
        fl_line(handle_start_edge_px, yy+length_px+6, handle_start_edge_px+length_px, yy+6);
        fl_line(handle_start_edge_px, yy+length_px+12, handle_start_edge_px+length_px, yy+12);

        yy++;
        fl_color(fl_lighter(selection_color()));
        fl_line(handle_start_edge_px, yy+length_px, handle_start_edge_px+length_px, yy);
        fl_line(handle_start_edge_px, yy+length_px+6, handle_start_edge_px+length_px, yy+6);
        fl_line(handle_start_edge_px, yy+length_px+12, handle_start_edge_px+length_px, yy+12);
      }
    }
  }

  draw_label(xsl, ysl, wsl, hsl);
  if (Fl::focus() == this) {
    if (type() == FL_HOR_FILL_SLIDER || type() == FL_VERT_FILL_SLIDER) draw_focus();
    else draw_focus(box1, xsl, ysl, wsl, hsl);
  }
}

void Fl_Slider::draw() {
  if (damage()&FL_DAMAGE_ALL) draw_box();
  draw(x()+Fl::box_dx(box()),
       y()+Fl::box_dy(box()),
       w()-Fl::box_dw(box()),
       h()-Fl::box_dh(box()));
}

int Fl_Slider::handle(int event, int X, int Y, int W, int H) {
  // Fl_Widget_Tracker wp(this);
  switch (event) {
  case FL_PUSH: {
    Fl_Widget_Tracker wp(this);
    if (!Fl::event_inside(X, Y, W, H)) return 0;
    handle_push();
    if (wp.deleted()) return 1; }
    // fall through ...
  case FL_DRAG: {

    const double val_linear01 =
      val_linear01_from_value(value(), minimum(), maximum());

    int length_px = (horizontal() ? W : H);
    int mx = (horizontal() ? Fl::event_x()-X : Fl::event_y()-Y);
    int handle_size_px;
    static int offcenter;

    if (type() == FL_HOR_FILL_SLIDER || type() == FL_VERT_FILL_SLIDER) {

      handle_size_px = 0;
      if (event == FL_PUSH) {
        int handle_start_edge_px = int(val_linear01*length_px+.5);
        offcenter = mx-handle_start_edge_px;
        if (offcenter < -10 || offcenter > 10) offcenter = 0;
        else return 1;
      }

    } else {

      handle_size_px = int(slider_size_*length_px+.5); if (handle_size_px >= length_px) return 0;
      int T = (horizontal() ? H : W)/2+1;
      if (type()==FL_VERT_NICE_SLIDER || type()==FL_HOR_NICE_SLIDER) T += 4;
      if (handle_size_px < T) handle_size_px = T;
      if (event == FL_PUSH) {
        int handle_start_edge_px = int(val_linear01*(length_px-handle_size_px)+.5);
        offcenter = mx-handle_start_edge_px;
        if (offcenter < 0) offcenter = 0;
        else if (offcenter > handle_size_px) offcenter = handle_size_px;
        else return 1;
      }
    }

    int handle_start_edge_px = mx-offcenter;
    double v = 0;
    char tryAgain = 1;
    while (tryAgain)
    {
      tryAgain = 0;
      if (handle_start_edge_px < 0) {
        handle_start_edge_px = 0;
        offcenter = mx; if (offcenter < 0) offcenter = 0;
      } else if (handle_start_edge_px > (length_px-handle_size_px)) {
        handle_start_edge_px = length_px-handle_size_px;
        offcenter = mx-handle_start_edge_px; if (offcenter > handle_size_px) offcenter = handle_size_px;
      }
      v = round(handle_start_edge_px*(maximum()-minimum())/(length_px-handle_size_px) + minimum());
      // make sure a click outside the sliderbar moves it:
      if (event == FL_PUSH && v == value()) {
        offcenter = handle_size_px/2;
        event = FL_DRAG;
        tryAgain = 1;
      }
    }
    handle_drag(clamp(v));
    } return 1;
  case FL_RELEASE:
    handle_release();
    return 1;
  case FL_KEYBOARD:
    { Fl_Widget_Tracker wp(this);
      switch (Fl::event_key()) {
        case FL_Up:
          if (horizontal()) return 0;
          handle_push();
          if (wp.deleted()) return 1;
          handle_drag(clamp(increment(value(),-1)));
          if (wp.deleted()) return 1;
          handle_release();
          return 1;
        case FL_Down:
          if (horizontal()) return 0;
          handle_push();
          if (wp.deleted()) return 1;
          handle_drag(clamp(increment(value(),1)));
          if (wp.deleted()) return 1;
          handle_release();
          return 1;
        case FL_Left:
          if (!horizontal()) return 0;
          handle_push();
          if (wp.deleted()) return 1;
          handle_drag(clamp(increment(value(),-1)));
          if (wp.deleted()) return 1;
          handle_release();
          return 1;
        case FL_Right:
          if (!horizontal()) return 0;
          handle_push();
          if (wp.deleted()) return 1;
          handle_drag(clamp(increment(value(),1)));
          if (wp.deleted()) return 1;
          handle_release();
          return 1;
        default:
          return 0;
      }
    }
    // break not required because of switch...
  case FL_FOCUS :
  case FL_UNFOCUS :
    if (Fl::visible_focus()) {
      redraw();
      return 1;
    } else return 0;
  case FL_ENTER :
  case FL_LEAVE :
    return 1;
  default:
    return 0;
  }
}

int Fl_Slider::handle(int event) {
  if (event == FL_PUSH && Fl::visible_focus()) {
    Fl::focus(this);
    redraw();
  }

  return handle(event,
                x()+Fl::box_dx(box()),
                y()+Fl::box_dy(box()),
                w()-Fl::box_dw(box()),
                h()-Fl::box_dh(box()));
}


Fl_Fill_Slider::Fl_Fill_Slider(int X,int Y,int W,int H,const char *L)
: Fl_Slider(X,Y,W,H,L)
{
  type(FL_VERT_FILL_SLIDER);
}


Fl_Hor_Slider::Fl_Hor_Slider(int X,int Y,int W,int H,const char *l)
: Fl_Slider(X,Y,W,H,l) {
  type(FL_HOR_SLIDER);
}


Fl_Hor_Fill_Slider::Fl_Hor_Fill_Slider(int X,int Y,int W,int H,const char *L)
: Fl_Slider(X,Y,W,H,L)
{
  type(FL_HOR_FILL_SLIDER);
}


Fl_Hor_Nice_Slider::Fl_Hor_Nice_Slider(int X,int Y,int W,int H,const char *L)
: Fl_Slider(X,Y,W,H,L)
{
  type(FL_HOR_NICE_SLIDER);
  box(FL_FLAT_BOX);
}


Fl_Nice_Slider::Fl_Nice_Slider(int X,int Y,int W,int H,const char *L)
: Fl_Slider(X,Y,W,H,L) {
  type(FL_VERT_NICE_SLIDER);
  box(FL_FLAT_BOX);
}
