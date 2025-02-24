/* Header file generated by fdesign on Sat Sep 17 15:12:01 2016 */

#ifndef FD_twheelattrib_h_
#define FD_twheelattrib_h_

#include  "../lib/include/forms.h" 

#if defined __cplusplus
extern "C"
{
#endif

/* Callbacks, globals and object handlers */

void twheel_minmax_change( FL_OBJECT *, long );
void twheel_returnsetting_change( FL_OBJECT *, long );
void twheel_initialvalue_change( FL_OBJECT *, long );
void twheel_step_change( FL_OBJECT *, long );


/* Forms and Objects */

typedef struct {
    FL_FORM   * twheelattrib;
    void      * vdata;
    char      * cdata;
    long        ldata;
    FL_OBJECT * background;
    FL_OBJECT * minval;
    FL_OBJECT * maxval;
    FL_OBJECT * returnsetting;
    FL_OBJECT * initial_val;
    FL_OBJECT * step;
} FD_twheelattrib;

FD_twheelattrib * create_form_twheelattrib( void );

#if defined __cplusplus
}
#endif

#endif /* FD_twheelattrib_h_ */
