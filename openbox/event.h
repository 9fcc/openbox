/* -*- indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*-

   event.h for the Openbox window manager
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

#ifndef __events_h
#define __events_h

#include <X11/Xlib.h>
#include <glib.h>

struct _ObClient;

/*! Time at which the last event with a timestamp occured. */
extern Time event_lasttime;

/*! The value of the mask for the NumLock modifier */
extern guint NumLockMask;
/*! The value of the mask for the ScrollLock modifier */
extern guint ScrollLockMask;

void event_startup(gboolean reconfig);
void event_shutdown(gboolean reconfig);

/*! Make as if the mouse just entered the client, use only when using focus
  follows mouse */
void event_enter_client(struct _ObClient *client);

/*! Request that any queued EnterNotify events not be used for distributing
  focus */
void event_ignore_queued_enters();

#endif
