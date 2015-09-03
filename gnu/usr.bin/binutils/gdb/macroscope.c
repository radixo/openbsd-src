/* Functions for deciding which macros are currently in scope.
   Copyright 2002 Free Software Foundation, Inc.
   Contributed by Red Hat, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"

#include "macroscope.h"
#include "symtab.h"
#include "source.h"
#include "target.h"
#include "frame.h"
#include "inferior.h"
#include "complaints.h"


struct macro_scope *
sal_macro_scope (struct symtab_and_line sal)
{
  struct macro_source_file *main, *inclusion;
  struct macro_scope *ms;

  if (! sal.symtab
      || ! sal.symtab->macro_table)
    return 0;

  ms = (struct macro_scope *) xmalloc (sizeof (*ms));

  main = macro_main (sal.symtab->macro_table);
  inclusion = macro_lookup_inclusion (main, sal.symtab->filename);

  if (inclusion)
    {
      ms->file = inclusion;
      ms->line = sal.line;
    }
  else
    {
      /* There are, unfortunately, cases where a compilation unit can
         have a symtab for a source file that doesn't appear in the
         macro table.  For example, at the moment, Dwarf doesn't have
         any way in the .debug_macinfo section to describe the effect
         of #line directives, so if you debug a YACC parser you'll get
         a macro table which only mentions the .c files generated by
         YACC, but symtabs that mention the .y files consumed by YACC.

         In the long run, we should extend the Dwarf macro info
         representation to handle #line directives, and get GCC to
         emit it.

         For the time being, though, we'll just treat these as
         occurring at the end of the main source file.  */
      ms->file = main;
      ms->line = -1;

      complaint (&symfile_complaints,
                 "symtab found for `%s', but that file\n"
                 "is not covered in the compilation unit's macro information",
                 sal.symtab->filename);
    }

  return ms;
}


struct macro_scope *
default_macro_scope (void)
{
  struct symtab_and_line sal;
  struct macro_source_file *main;
  struct macro_scope *ms;

  /* If there's a selected frame, use its PC.  */ 
  if (deprecated_selected_frame)
    sal = find_pc_line (get_frame_pc (deprecated_selected_frame), 0);
  
  /* If the target has any registers at all, then use its PC.  Why we
     would have registers but no stack, I'm not sure.  */
  else if (target_has_registers)
    sal = find_pc_line (read_pc (), 0);

  /* If all else fails, fall back to the current listing position.  */
  else
    {
      /* Don't call select_source_symtab here.  That can raise an
         error if symbols aren't loaded, but GDB calls the expression
         evaluator in all sorts of contexts.

         For example, commands like `set width' call the expression
         evaluator to evaluate their numeric arguments.  If the
         current language is C, then that may call this function to
         choose a scope for macro expansion.  If you don't have any
         symbol files loaded, then get_current_or_default would raise an
         error.  But `set width' shouldn't raise an error just because
         it can't decide which scope to macro-expand its argument in.  */
      struct symtab_and_line cursal = 
      			get_current_source_symtab_and_line ();
      
      sal.symtab = cursal.symtab;
      sal.line = cursal.line;
    }

  return sal_macro_scope (sal);
}


/* Look up the definition of the macro named NAME in scope at the source
   location given by BATON, which must be a pointer to a `struct
   macro_scope' structure.  */
struct macro_definition *
standard_macro_lookup (const char *name, void *baton)
{
  struct macro_scope *ms = (struct macro_scope *) baton;

  return macro_lookup_definition (ms->file, ms->line, name);
}
