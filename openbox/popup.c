#include "popup.h"

#include "openbox.h"
#include "frame.h"
#include "client.h"
#include "stacking.h"
#include "screen.h"
#include "render/render.h"
#include "render/theme.h"

ObPopup *popup_new(gboolean hasicon)
{
    XSetWindowAttributes attrib;
    ObPopup *self = g_new0(ObPopup, 1);

    self->obwin.type = Window_Internal;
    self->hasicon = hasicon;
    self->gravity = NorthWestGravity;
    self->x = self->y = self->w = self->h = 0;
    self->a_bg = RrAppearanceCopy(ob_rr_theme->app_hilite_bg);
    self->a_text = RrAppearanceCopy(ob_rr_theme->app_hilite_label);

    attrib.override_redirect = True;
    self->bg = XCreateWindow(ob_display, RootWindow(ob_display, ob_screen),
                             0, 0, 1, 1, 0, RrDepth(ob_rr_inst),
                             InputOutput, RrVisual(ob_rr_inst),
                             CWOverrideRedirect, &attrib);
    
    self->text = XCreateWindow(ob_display, self->bg,
                               0, 0, 1, 1, 0, RrDepth(ob_rr_inst),
                               InputOutput, RrVisual(ob_rr_inst), 0, NULL);

    XMapWindow(ob_display, self->text);

    stacking_add(INTERNAL_AS_WINDOW(self));
    return self;
}

void popup_free(ObPopup *self)
{
    if (self) {
        XDestroyWindow(ob_display, self->bg);
        XDestroyWindow(ob_display, self->text);
        RrAppearanceFree(self->a_bg);
        RrAppearanceFree(self->a_text);
        stacking_remove(self);
        g_free(self);
    }
}

void popup_position(ObPopup *self, gint gravity, gint x, gint y)
{
    self->gravity = gravity;
    self->x = x;
    self->y = y;
}

void popup_size(ObPopup *self, gint w, gint h)
{
    self->w = w;
    self->h = h;
}

void popup_size_to_string(ObPopup *self, gchar *text)
{
    gint textw, texth;
    gint iconw;

    self->a_text->texture[0].data.text.string = text;
    RrMinsize(self->a_text, &textw, &texth);
    /*XXX textw += ob_rr_theme->bevel * 2;*/
    texth += ob_rr_theme->padding * 2;

    self->h = texth + ob_rr_theme->padding * 2;
    iconw = (self->hasicon ? texth : 0);
    self->w = textw + iconw + ob_rr_theme->padding * (self->hasicon ? 3 : 2);
}

void popup_set_text_align(ObPopup *self, RrJustify align)
{
    self->a_text->texture[0].data.text.justify = align;
}

void popup_show(ObPopup *self, gchar *text)
{
    gint l, t, r, b;
    gint x, y, w, h;
    gint textw, texth;
    gint iconw;

    RrMargins(self->a_bg, &l, &t, &r, &b);

    XSetWindowBorderWidth(ob_display, self->bg, ob_rr_theme->bwidth);
    XSetWindowBorder(ob_display, self->bg, ob_rr_theme->b_color->pixel);

    /* set up the textures */
    self->a_text->texture[0].data.text.string = text;

    /* measure the shit out */
    RrMinsize(self->a_text, &textw, &texth);
    /*XXX textw += ob_rr_theme->padding * 2;*/
    texth += ob_rr_theme->padding * 2;

    /* set the sizes up and reget the text sizes from the calculated
       outer sizes */
    if (self->h) {
        h = self->h;
        texth = h - (t+b + ob_rr_theme->padding * 2);
    } else
        h = t+b + texth + ob_rr_theme->padding * 2;
    iconw = (self->hasicon ? texth : 0);
    if (self->w) {
        w = self->w;
        textw = w - (l+r + iconw + ob_rr_theme->padding *
                     (self->hasicon ? 3 : 2));
    } else
        w = l+r + textw + iconw + ob_rr_theme->padding *
            (self->hasicon ? 3 : 2);
    /* sanity checks to avoid crashes! */
    if (w < 1) w = 1;
    if (h < 1) h = 1;
    if (textw < 1) textw = 1;
    if (texth < 1) texth = 1;

    /* set up the x coord */
    x = self->x;
    switch (self->gravity) {
    case NorthGravity:
    case CenterGravity:
    case SouthGravity:
        x -= w / 2;
        break;
    case NorthEastGravity:
    case EastGravity:
    case SouthEastGravity:
        x -= w;
        break;
    }

    /* set up the y coord */
    y = self->y;
    switch (self->gravity) {
    case WestGravity:
    case CenterGravity:
    case EastGravity:
        y -= h / 2;
        break;
    case SouthWestGravity:
    case SouthGravity:
    case SouthEastGravity:
        y -= h;
        break;
    }

    /* set the windows/appearances up */
    XMoveResizeWindow(ob_display, self->bg, x, y, w, h);

    self->a_text->surface.parent = self->a_bg;
    self->a_text->surface.parentx = l + iconw +
        ob_rr_theme->padding * (self->hasicon ? 2 : 1);
    self->a_text->surface.parenty = t + ob_rr_theme->padding;
    XMoveResizeWindow(ob_display, self->text,
                      l + iconw + ob_rr_theme->padding *
                      (self->hasicon ? 2 : 1),
                      t + ob_rr_theme->padding, textw, texth);

    RrPaint(self->a_bg, self->bg, w, h);
    RrPaint(self->a_text, self->text, textw, texth);

    if (self->hasicon) {
        if (iconw < 1) iconw = 1; /* sanity check for crashes */
        if (self->draw_icon)
            self->draw_icon(l + ob_rr_theme->padding, t + ob_rr_theme->padding,
                            iconw, texth, self->draw_icon_data);
    }

    if (!self->mapped) {
        XMapWindow(ob_display, self->bg);
        stacking_raise(INTERNAL_AS_WINDOW(self));
        self->mapped = TRUE;
    }
}

void popup_hide(ObPopup *self)
{
    if (self->mapped) {
        XUnmapWindow(ob_display, self->bg);
        self->mapped = FALSE;
    }
}

static void icon_popup_draw_icon(gint x, gint y, gint w, gint h, gpointer data)
{
    ObIconPopup *self = data;

    self->a_icon->surface.parent = self->popup->a_bg;
    self->a_icon->surface.parentx = x;
    self->a_icon->surface.parenty = y;
    XMoveResizeWindow(ob_display, self->icon, x, y, w, h);
    RrPaint(self->a_icon, self->icon, w, h);
}

ObIconPopup *icon_popup_new()
{
    ObIconPopup *self;

    self = g_new0(ObIconPopup, 1);
    self->popup = popup_new(TRUE);
    self->a_icon = RrAppearanceCopy(ob_rr_theme->a_clear_tex);
    self->icon = XCreateWindow(ob_display, self->popup->bg,
                               0, 0, 1, 1, 0,
                               RrDepth(ob_rr_inst), InputOutput,
                               RrVisual(ob_rr_inst), 0, NULL);
    XMapWindow(ob_display, self->icon);

    self->popup->draw_icon = icon_popup_draw_icon;
    self->popup->draw_icon_data = self;

    return self;
}

void icon_popup_free(ObIconPopup *self)
{
    if (self) {
        XDestroyWindow(ob_display, self->icon);
        RrAppearanceFree(self->a_icon);
        popup_free(self->popup);
        g_free(self);
    }
}

void icon_popup_show(ObIconPopup *self,
                     gchar *text, struct _ObClientIcon *icon)
{
    if (icon) {
        self->a_icon->texture[0].type = RR_TEXTURE_RGBA;
        self->a_icon->texture[0].data.rgba.width = icon->width;
        self->a_icon->texture[0].data.rgba.height = icon->height;
        self->a_icon->texture[0].data.rgba.data = icon->data;
    } else
        self->a_icon->texture[0].type = RR_TEXTURE_NONE;

    popup_show(self->popup, text);
}

static void pager_popup_draw_icon(gint px, gint py, gint w, gint h,
                                  gpointer data)
{
    ObPagerPopup *self = data;
    gint x, y;
    guint i;
    guint rown, n;
    guint horz_inc;
    guint vert_inc;
    guint r, c;
    gint eachw, eachh;

    eachw = (w - ob_rr_theme->bwidth -
             (screen_desktop_layout.columns * ob_rr_theme->bwidth))
        / screen_desktop_layout.columns;
    eachh = (h - ob_rr_theme->bwidth -
             (screen_desktop_layout.rows * ob_rr_theme->bwidth))
        / screen_desktop_layout.rows;
    /* make them squares */
    eachw = eachh = MIN(eachw, eachh);
    g_message("dif %d %d %d %d ",
              (screen_desktop_layout.columns * (eachw + ob_rr_theme->bwidth) +
               ob_rr_theme->bwidth), w,
              (screen_desktop_layout.rows * (eachh + ob_rr_theme->bwidth) +
               ob_rr_theme->bwidth), h);

    /* center */
    px += (w - (screen_desktop_layout.columns * (eachw + ob_rr_theme->bwidth) +
                ob_rr_theme->bwidth)) / 2;
    py += (h - (screen_desktop_layout.rows * (eachh + ob_rr_theme->bwidth) +
                ob_rr_theme->bwidth)) / 2;

    g_message("%d %d %d %d", px, py, eachw, eachh);

    if (eachw <= 0 || eachh <= 0)
        return;

    switch (screen_desktop_layout.start_corner) {
    case OB_CORNER_TOPLEFT:
        n = 0;
        switch (screen_desktop_layout.orientation) {
        case OB_ORIENTATION_HORZ:
            horz_inc = 1;
            vert_inc = screen_desktop_layout.columns;
            break;
        case OB_ORIENTATION_VERT:
            horz_inc = screen_desktop_layout.rows;
            vert_inc = 1;
            break;
        }
        break;
    case OB_CORNER_TOPRIGHT:
        n = screen_desktop_layout.columns;
        switch (screen_desktop_layout.orientation) {
        case OB_ORIENTATION_HORZ:
            horz_inc = -1;
            vert_inc = screen_desktop_layout.columns;
            break;
        case OB_ORIENTATION_VERT:
            horz_inc = -screen_desktop_layout.rows;
            vert_inc = 1;
            break;
        }
        break;
    case OB_CORNER_BOTTOMLEFT:
        n = screen_desktop_layout.rows;
        switch (screen_desktop_layout.orientation) {
        case OB_ORIENTATION_HORZ:
            horz_inc = 1;
            vert_inc = -screen_desktop_layout.columns;
            break;
        case OB_ORIENTATION_VERT:
            horz_inc = screen_desktop_layout.rows;
            vert_inc = -1;
            break;
        }
        break;
    case OB_CORNER_BOTTOMRIGHT:
        n = MAX(self->desks,
                screen_desktop_layout.rows * screen_desktop_layout.columns);
        switch (screen_desktop_layout.orientation) {
        case OB_ORIENTATION_HORZ:
            horz_inc = -1;
            vert_inc = -screen_desktop_layout.columns;
            break;
        case OB_ORIENTATION_VERT:
            horz_inc = -screen_desktop_layout.rows;
            vert_inc = -1;
            break;
        }
        break;
    }

    g_message("%d %d %d", n, horz_inc, vert_inc);

    rown = n;
    i = 0;
    for (r = 0, y = 0; r < screen_desktop_layout.rows;
         ++r, y += eachh + ob_rr_theme->bwidth)
    {
        for (c = 0, x = 0; c < screen_desktop_layout.columns;
             ++c, x += eachw + ob_rr_theme->bwidth)
        {
            RrAppearance *a;

            g_message("i %d n %d", i, n);

            if (i >= self->desks)
                break;

            a = (n == self->curdesk ? self->hilight : self->unhilight);

            a->surface.parent = self->popup->a_bg;
            a->surface.parentx = x + px;
            a->surface.parenty = y + py;
            XMoveResizeWindow(ob_display, self->wins[i],
                              x + px, y + py, eachw, eachh);
            RrPaint(a, self->wins[i], eachw, eachh);

            n += horz_inc;

            ++i;
        }
        n = rown += vert_inc;
    }
}

ObPagerPopup *pager_popup_new()
{
    ObPagerPopup *self;

    self = g_new(ObPagerPopup, 1);
    self->popup = popup_new(TRUE);

    self->desks = 0;
    self->wins = g_new(Window, self->desks);
    self->hilight = RrAppearanceCopy(ob_rr_theme->app_hilite_fg);
    self->unhilight = RrAppearanceCopy(ob_rr_theme->app_unhilite_fg);

    self->popup->draw_icon = pager_popup_draw_icon;
    self->popup->draw_icon_data = self;

    return self;
}

void pager_popup_free(ObPagerPopup *self)
{
    if (self) {
        guint i;

        for (i = 0; i < self->desks; ++i)
            XDestroyWindow(ob_display, self->wins[i]);
        g_free(self->wins);
        RrAppearanceFree(self->hilight);
        RrAppearanceFree(self->unhilight);
        popup_free(self->popup);
        g_free(self);
    }
}

void pager_popup_show(ObPagerPopup *self, gchar *text, guint desk)
{
    guint i;

    if (screen_num_desktops < self->desks)
        for (i = screen_num_desktops; i < self->desks; ++i)
            XDestroyWindow(ob_display, self->wins[i]);

    if (screen_num_desktops != self->desks)
        self->wins = g_renew(Window, self->wins, screen_num_desktops);

    if (screen_num_desktops > self->desks)
        for (i = self->desks; i < screen_num_desktops; ++i) {
            XSetWindowAttributes attr;

            attr.border_pixel = RrColorPixel(ob_rr_theme->b_color);
            self->wins[i] = XCreateWindow(ob_display, self->popup->bg,
                                          0, 0, 1, 1, ob_rr_theme->bwidth,
                                          RrDepth(ob_rr_inst), InputOutput,
                                          RrVisual(ob_rr_inst), CWBorderPixel,
                                          &attr);
            XMapWindow(ob_display, self->wins[i]);
        }

    self->desks = screen_num_desktops;
    self->curdesk = desk;

    popup_show(self->popup, text);
}
