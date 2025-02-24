/*
 *
 * This file is part of XForms.
 *
 * XForms is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1, or
 * (at your option) any later version.
 *
 * XForms is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with XForms.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef SP_SCROLLBAR_H_
#define SP_SCROLLBAR_H_

#include "include/forms.h"
#include <stdio.h>


void scrollbar_change_type( FL_OBJECT * obj,
                            int         new_type );
FL_FORM * scrollbar_create_spec_form( void );
void scrollbar_adjust_spec_form( FL_OBJECT * obj );
void scrollbar_fill_in_spec_form( FL_OBJECT * obj );
void scrollbar_reread_spec_form( FL_OBJECT * obj );
void scrollbar_emit_spec_fd_code( FILE      * fp,
 								 FL_OBJECT * obj );
void scrollbar_emit_spec_c_code( FILE      * fp,
                                 FL_OBJECT * obj );

#endif


/*
 * Local variables:
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
