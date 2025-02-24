/*
 *  This file is part of XForms.
 *
 *  XForms is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1, or
 *  (at your option) any later version.
 *
 *  XForms is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with XForms; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 *  MA 02111-1307, USA.
 */


/*
 * InOut mode of input field demonstration 
 *
 *  W.Heisch 2106-12-29
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include "include/forms.h"

#include "fd/inout_gui.h"

    FD_f *fd_f;
    int incs1 = 1;
    int incs2 = 1;
    int val1 = 0;
    int val2 = 0;


/***************************************
 * ----- idle_callback ---------*
 ***************************************/

static int
plc_running( XEvent * ev  FL_UNUSED_ARG,
			 void   * b   FL_UNUSED_ARG )
{
    char hstr1[ 20 ];
    char hstr2[ 20 ];

    /* generate the changed values */

    if ( val1 >= 10000 )
        incs1 = -1;
    else if ( val1 <=  0 )
        incs1 = 1;
	
    val1 = val1 + incs1;

    sprintf( hstr1, "%9d",val1 );
    fl_set_object_label( fd_f->plc_pv1, hstr1 );

    if ( val2 >= 10000 )
        incs2 = -1;
    else
		if ( val2 <=  0 )
			incs2 = 1;

    val2 = val2 + incs2;

    sprintf( hstr2, "%9d", val2 );
    fl_set_object_label( fd_f->plc_pv2, hstr2 );

   /* ==== write the values to the Terminals ===== */

    /* terminal 1 */

    fl_set_input( fd_f->term1_pv1, hstr1 );
    fl_set_input( fd_f->term1_pv2, hstr2 );

    /* terminal 2, not updated when under the mouse */

    if ( ! fd_f->term2_pv1->belowmouse )
        fl_set_input( fd_f->term2_pv1, hstr1 );
    if ( ! fd_f->term2_pv2->belowmouse )
        fl_set_input ( fd_f->term2_pv2, hstr2 );

    /* terminal 3 */

    fl_set_input( fd_f->term3_pv1, hstr1 );
    fl_set_input( fd_f->term3_pv2, hstr2 );
   
    return 0;
}  


/***************************************
 ***************************************/

int
main( int    argc,
      char * argv[ ] )
{
    fl_initialize( &argc, argv, 0, 0, 0 );

    fd_f = create_form_f( );

    fl_set_idle_callback( plc_running, NULL);
    fl_set_input_inout_mode( fd_f->term3_pv1, 1 );
    fl_set_input_inout_mode( fd_f->term3_pv2, 1 );

    /* Show the first form */

    fl_show_form( fd_f->f, FL_PLACE_CENTERFREE, FL_FULLBORDER, "InOut_input" );

    fl_do_forms( );

    if ( fl_form_is_visible( fd_f->f ) )
        fl_hide_form( fd_f->f );
    fl_free( fd_f );
    fl_finish( );

    return 0;
}

/***************************************
 ***************************************/

void
cb_write_to_plc( FL_OBJECT * obj,
                 long        data )
{
    if ( data == 1)
        val1 = atoi( fl_get_input( obj ) );
    else
        val2 = atoi( fl_get_input( obj ) );
}


/***************************************
 ***************************************/

void
cb_term3_onoff( FL_OBJECT * obj,
                long        data  FL_UNUSED_ARG )
{
    int mode = fl_get_choice( obj ) == 1;

	fl_set_input_inout_mode( fd_f->term3_pv1, mode );
	fl_set_input_inout_mode( fd_f->term3_pv2, mode );

	printf("SWITCH %d\n", mode);
}
