/* -*- indent-tabs-mode: nil; tab-width: 4; c-basic-offset: 4; -*-

   mainloop.h for the Openbox window manager
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

#ifndef __ob__mainloop_h
#define __ob__mainloop_h

#include <X11/Xlib.h>
#include <glib.h>

typedef struct _ObMainLoop ObMainLoop;

ObMainLoop *ob_main_loop_new(Display *display);
void        ob_main_loop_destroy(ObMainLoop *loop);

typedef void (*ObMainLoopXHandler) (const XEvent *e, gpointer data);
typedef void (*ObMainLoopXDoneHandler) (gpointer data);

void ob_main_loop_x_add(ObMainLoop *loop,
                        ObMainLoopXHandler handler,
                        ObMainLoopXDoneHandler done_handler,
                        gpointer data,
                        GDestroyNotify notify);
void ob_main_loop_x_remove(ObMainLoop *loop,
                           ObMainLoopXHandler handler);

typedef void (*ObMainLoopFdHandler) (gint fd, gpointer data);

void ob_main_loop_fd_add(ObMainLoop *loop,
                         gint fd,
                         ObMainLoopFdHandler handler,
                         gpointer data,
                         GDestroyNotify notify);
void ob_main_loop_fd_remove(ObMainLoop *loop,
                            gint fd);

typedef void (*ObMainLoopSignalHandler) (gint signal, gpointer data);

void ob_main_loop_signal_add(ObMainLoop *loop,
                             gint signal,
                             ObMainLoopSignalHandler handler,
                             gpointer data,
                             GDestroyNotify notify);
void ob_main_loop_signal_remove(ObMainLoop *loop,
                                ObMainLoopSignalHandler handler);

void ob_main_loop_timeout_add(ObMainLoop *loop,
                              gulong microseconds,
                              GSourceFunc handler,
                              gpointer data,
                              GDestroyNotify notify);
void ob_main_loop_timeout_remove(ObMainLoop *loop,
                                 GSourceFunc handler);
void ob_main_loop_timeout_remove_data(ObMainLoop *loop,
                                      GSourceFunc handler,
                                      gpointer data);

struct _ObAction;

/*! Queues an action, which will be run when there are no more X events
  to process */
void ob_main_loop_queue_action(ObMainLoop *loop, struct _ObAction *act);

void ob_main_loop_run(ObMainLoop *loop);
void ob_main_loop_exit(ObMainLoop *loop);

#endif
