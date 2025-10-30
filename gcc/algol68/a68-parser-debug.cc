/* Debug facilities for the Algol 68 parser.
   Copyright (C) 2025 Jose E. Marchesi.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#define INCLUDE_MEMORY
#include "config.h"
#define INCLUDE_VECTOR
#include "system.h"
#include "coretypes.h"
#include "diagnostic.h"
#include "text-art/types.h"
#include "text-art/dump.h"
#include "text-art/dump-widget-info.h"
#include "text-art/canvas.h"
#include "text-art/theme.h"
#include "text-art/tree-widget.h"

#include "a68.h"

/* Write a printable representation of the parse tree with top node P to the
   standard output.  */

static void
a68_dump_parse_tree_1 (NODE_T *p, const text_art::dump_widget_info &dwi,
		       text_art::tree_widget &widget)
{
  for (; p != NO_NODE; FORWARD (p))
    {
      char *symbol;
      if (ATTRIBUTE (p) == IDENTIFIER
	  || ATTRIBUTE (p) == DEFINING_IDENTIFIER
	  || ATTRIBUTE (p) == DEFINING_OPERATOR
	  || ATTRIBUTE (p) == BOLD_TAG)
	symbol = xasprintf (" %s", NSYMBOL (p));
      else
	symbol = xstrdup ("");

      char mode[BUFFER_SIZE];
      if (MOID (p) != NO_MOID)
	{
	  MOID_T *moid = MOID (p);
	  mode[0] = '\0';

	  a68_bufcat (mode, " (", 2);
	  if (IS (moid, SERIES_MODE))
	    {
	      if (PACK (moid) != NO_PACK && NEXT (PACK (moid)) == NO_PACK)
		a68_bufcat (mode, a68_moid_to_string (MOID (PACK (moid)), MOID_ERROR_WIDTH, p),
			    BUFFER_SIZE);
	      else
		a68_bufcat (mode, a68_moid_to_string (moid, MOID_ERROR_WIDTH, p), BUFFER_SIZE);
	    }
	  else
	    a68_bufcat (mode, a68_moid_to_string (moid, MOID_ERROR_WIDTH, p), BUFFER_SIZE);
	  a68_bufcat (mode, ")", 2);
	}

      location_t loc = a68_get_node_location (p);
      std::unique_ptr<text_art::tree_widget> cwidget
	= text_art::tree_widget::from_fmt (dwi, nullptr,
					   "%s:%d:%d [%d] %s%s%s",
					   LOCATION_FILE (loc),
					   LOCATION_LINE (loc),
					   LOCATION_COLUMN (loc),
					   NUMBER (p),
					   a68_attribute_name (ATTRIBUTE (p)),
					   symbol,
					   mode);
      free (symbol);

      a68_dump_parse_tree_1 (SUB (p), dwi, *cwidget);
      widget.add_child (std::move (cwidget));
    }
}

void
a68_dump_parse_tree (NODE_T *p)
{
  text_art::style_manager sm;
  text_art::style::id_t default_style_id (sm.get_or_create_id (text_art::style ()));
  text_art::ascii_theme theme;
  text_art::dump_widget_info dwi (sm, theme, default_style_id);
  std::unique_ptr<text_art::tree_widget> widget
    = text_art::tree_widget::from_fmt (dwi, nullptr, "Parse Tree");

  a68_dump_parse_tree_1 (p, dwi, *widget);

  text_art::canvas c (widget->to_canvas (sm));
  pretty_printer *const pp = global_dc->get_reference_printer ();
  c.print_to_pp (pp);
  printf ("%s", pp_formatted_text (pp));
}

void
a68_dump_modes (MOID_T *moid)
{
  for (; moid != NO_MOID; FORWARD (moid))
    {
      printf ("%p %s\n", (void *) moid,
	      a68_moid_to_string (moid, MOID_ERROR_WIDTH, NODE (moid),
				  true /* indicant_value */));
    }
}
