/* -*- indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*-

   stacking.h for the Openbox window manager
   Copyright (c) 2003        Ben Jansens

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   See the COPYING file for a copy of the GNU General Public License.
*/

#ifndef __stacking_h
#define __stacking_h

#include "window.h"

#include <glib.h>
#include <X11/Xlib.h>

/*! The possible stacking layers a client window can be a part of */
typedef enum {
    OB_STACKING_LAYER_DESKTOP,     /*!< 0 - desktop windows */
    OB_STACKING_LAYER_BELOW,       /*!< 1 - normal windows w/ below */
    OB_STACKING_LAYER_NORMAL,      /*!< 2 - normal windows */
    OB_STACKING_LAYER_ABOVE,       /*!< 3 - normal windows w/ above */
    OB_STACKING_LAYER_FULLSCREEN,  /*!< 4 - fullscreeen windows */
    OB_STACKING_LAYER_INTERNAL,    /*!< 5 - openbox windows/menus */
    OB_NUM_STACKING_LAYERS
} ObStackingLayer;

/* list of ObWindow*s in stacking order from highest to lowest */
extern GList *stacking_list;

/*! Sets the window stacking list on the root window from the
  stacking_list */
void stacking_set_list();

void stacking_add(ObWindow *win);
void stacking_add_nonintrusive(ObWindow *win);
#define stacking_remove(win) stacking_list = g_list_remove(stacking_list, win);

/*! Raises a window above all others in its stacking layer */
void stacking_raise(ObWindow *window);

/*! Lowers a window below all others in its stacking layer */
void stacking_lower(ObWindow *window);

/*! Moves a window below another if its in the same layer.
  This function does not enforce stacking rules IRT transients n such, and so
  it should really ONLY be used to restore stacking orders from saved sessions
*/
void stacking_below(ObWindow *window, ObWindow *below);

#endif
