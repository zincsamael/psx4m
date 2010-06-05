// -*- Mode: c; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
#ifndef __CELL_RENDERER_KEY_H__
#define __CELL_RENDERER_KEY_H__

#include <gtk/gtkcellrenderertext.h>

G_BEGIN_DECLS

#define TYPE_CELL_RENDERER_KEY		(cell_renderer_key_get_type ())
#define CELL_RENDERER_KEY(obj)		(GTK_CHECK_CAST ((obj), TYPE_CELL_RENDERER_KEY, CellRendererKey))
#define CELL_RENDERER_KEY_CLASS(klass)	(GTK_CHECK_CLASS_CAST ((klass), TYPE_CELL_RENDERER_KEY, CellRendererKeyClass))
#define IS_CELL_RENDERER_KEY(obj)		(GTK_CHECK_TYPE ((obj), TYPE_CELL_RENDERER_KEY))
#define IS_CELL_RENDERER_KEY_CLASS(klass)	(GTK_CHECK_CLASS_TYPE ((klass), TYPE_CELL_RENDERER_KEY))
#define CELL_RENDERER_KEY_GET_CLASS(obj)   (GTK_CHECK_GET_CLASS ((obj), TYPE_CELL_RENDERER_KEY, CellRendererKeyClass))

typedef struct _CellRendererKey      CellRendererKey;
typedef struct _CellRendererKeyClass CellRendererKeyClass;

struct _CellRendererKey
{
  GtkCellRendererText parent;
  gint scancode;
  GtkWidget *edit_widget;
  GtkWidget *grab_widget;
  GtkWidget *sizing_label;
};

struct _CellRendererKeyClass
{
  GtkCellRendererTextClass parent_class;

  void (* accel_edited) (CellRendererKey    *keys,
			 const char             *path_string,
			 guint                   scancode);

  void (* accel_cleared) (CellRendererKey    *keys,
			  const char             *path_string);
};

GType            cell_renderer_key_get_type        (void);
GtkCellRenderer *cell_renderer_key_new             (void);

void cell_renderer_key_set_scancode(CellRendererKey * key, gint scancode);
gint cell_renderer_key_get_scancode(CellRendererKey * key);

G_END_DECLS


#endif /* __GTK_CELL_RENDERER_KEYS_H__ */
