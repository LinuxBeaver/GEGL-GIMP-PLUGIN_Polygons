/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <https://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 * GEGL polygon, 2022 Beaver 
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double  (scale, _("Scale of Polygons"), 0.10)
    description  (_("The scale of the noise function"))
    value_range  (0.05, 0.19)


property_int     (rank, _("Rank of Polygons"), 2)
    description  (_("Select the n-th closest point"))
    value_range  (2, 3)
    ui_meta     ("role", "output-extent")

property_seed    (seed, _("Random seed"), rand)
    description  (_("The random seed for the noise function"))

property_double (azimuth, _("Light Rotation"), 2.0)
    description (_("Light angle (degrees)"))
    value_range (0, 360)
    ui_meta ("unit", "degree")
    ui_meta ("direction", "ccw")

property_int (depth, _("Depth of Polygons"), 20)
    description (_("Filter width"))
    value_range (6, 30)

property_color (value, _("Color"), "transparent")
    description (_("The color to paint over the input"))
    ui_meta     ("role", "color-primary")

property_double (std_dev, _("Radius of Sharpen"), 2.5)
    description(_("Expressed as standard deviation, in pixels"))
    value_range (2.5, 7.0)
    ui_range    (2.5, 7.0)
    ui_gamma    (3.0)
    ui_meta     ("unit", "pixel-distance")

property_double (strength, _("Scaling Factor of Sharpen"), 1)
    description(_("Scaling factor for unsharp-mask, the strength of effect"))
    value_range (0.0, 5.0)
    ui_range    (0.0, 5.0)
    ui_gamma    (3.0)


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     polygon
#define GEGL_OP_C_SOURCE polygon.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *over, *cellnoise, *emboss, *multiply, *color, *crop, *sharpen;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");


  over = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);

  cellnoise = gegl_node_new_child (gegl,
                                  "operation", "gegl:cell-noise",
                                  NULL);

  emboss = gegl_node_new_child (gegl,
                                  "operation", "gegl:emboss",
                                  NULL);

multiply = gegl_node_new_child (gegl,
                                    "operation", "gimp:layer-mode", "layer-mode", 30, NULL);

  color = gegl_node_new_child (gegl,
                                  "operation", "gegl:color",
                                  NULL);

  crop = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);

  sharpen = gegl_node_new_child (gegl,
                                  "operation", "gegl:unsharp-mask",
                                  NULL);


  gegl_operation_meta_redirect (operation, "scale", cellnoise, "scale");
  gegl_operation_meta_redirect (operation, "rank", cellnoise, "rank");
  gegl_operation_meta_redirect (operation, "seed", cellnoise, "seed");
  gegl_operation_meta_redirect (operation, "depth", emboss, "depth");
  gegl_operation_meta_redirect (operation, "azimuth", emboss, "azimuth");
  gegl_operation_meta_redirect (operation, "value", color, "value");
  gegl_operation_meta_redirect (operation, "std_dev", sharpen, "std-dev");
  gegl_operation_meta_redirect (operation, "strength", sharpen, "scale");




  gegl_node_link_many (input, over, emboss, crop, sharpen, multiply, output, NULL);
  gegl_node_connect_from (over, "aux", cellnoise, "output");
  gegl_node_link_many (input, cellnoise, NULL);
  gegl_node_connect_from (multiply, "aux", color, "output");
  gegl_node_link_many (input, color, NULL);



}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "gegl:polygons",
    "title",       _("Polygon Generator"),
    "categories",  "Aristic",
    "reference-hash", "3p6j6bf40dd50f2345sf27ac",
    "description", _("Generator simple polygonal backgrounds with GEGL   "
                     ""),
    NULL);
}

#endif

