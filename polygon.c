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
 * Credit to Øyvind Kolås (pippin) for major GEGL contributions
 * GEGL polygon, 2022 Beaver 
 */

/* 
GEGL Graph of Polygons to test without installing.

id=0
over aux=[ ref=0
cell-noise scale=0.10 rank=2
emboss azimuth=44 elevation=44 depth=19
rgb-clip
id=1
multiply aux=[ ref=1 color-overlay value=#61aded    ]   ]
crop
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#ifdef GEGL_PROPERTIES

property_double  (scale, _("Scale of Polygons"), 0.10)
    description  (_("The scale of the noise function"))
    value_range  (0.05, 0.25)

property_seed    (seed, _("Random seed"), rand)
    description  (_("The random seed for the noise function"))

property_double (azimuth, _("Light Rotation"), 2.0)
    description (_("Light angle (degrees)"))
    value_range (0, 360)
    ui_meta ("unit", "degree")
    ui_meta ("direction", "ccw")

property_int (depth, _("Depth of Polygons"), 30)
    description (_("Filter width"))
    value_range (6, 50)

property_color (value, _("Color"), "Red")
    description (_("The color to paint over the input"))


#else

#define GEGL_OP_META
#define GEGL_OP_NAME     polygon
#define GEGL_OP_C_SOURCE polygon.c

#include "gegl-op.h"

static void attach (GeglOperation *operation)
{
  GeglNode *gegl = operation->node;
  GeglNode *input, *output, *cellnoise, *emboss, *multiply, *over, *crop, *rgbclip, *color;

  input    = gegl_node_get_input_proxy (gegl, "input");
  output   = gegl_node_get_output_proxy (gegl, "output");



  cellnoise      = gegl_node_new_child (gegl, "operation", "gegl:cell-noise",
                                         "rank", 2,
                                         NULL);

  emboss = gegl_node_new_child (gegl,
                                  "operation", "gegl:emboss",
                                  NULL);

  multiply = gegl_node_new_child (gegl,
                                  "operation", "gegl:multiply",
                                  NULL);

  color = gegl_node_new_child (gegl,
                                  "operation", "gegl:color-overlay",
                                  NULL);

  over = gegl_node_new_child (gegl,
                                  "operation", "gegl:over",
                                  NULL);

     
 rgbclip = gegl_node_new_child (gegl,
                                  "operation", "gegl:rgb-clip",
                                  NULL);

  crop = gegl_node_new_child (gegl,
                                  "operation", "gegl:crop",
                                  NULL);



  gegl_operation_meta_redirect (operation, "scale", cellnoise, "scale");
  gegl_operation_meta_redirect (operation, "seed", cellnoise, "seed");
  gegl_operation_meta_redirect (operation, "depth", emboss, "depth");
  gegl_operation_meta_redirect (operation, "azimuth", emboss, "azimuth");
  gegl_operation_meta_redirect (operation, "value", color, "value");

  gegl_node_link_many (input, over, emboss, rgbclip, multiply, crop, output, NULL);
  gegl_node_connect (over, "aux", cellnoise, "output");
  gegl_node_connect (multiply, "aux", color, "output");
  gegl_node_link_many (input, color, NULL);
  gegl_node_connect (crop, "aux", input, "output");

}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass *operation_class;

  operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->attach = attach;

  gegl_operation_class_set_keys (operation_class,
    "name",        "lb:polygons",
    "title",       _("Polygon Generator"),
    "reference-hash", "3p6j6bf40dd50f2345sf27ac",
    "description", _("Generator simple polygonal backgrounds with GEGL   "
                     ""),
    "gimp:menu-path", "<Image>/Filters/Render/Fun",
    "gimp:menu-label", _("Polygons..."),
    NULL);
}

#endif
