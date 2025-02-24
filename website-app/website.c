#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "include/forms.h"

const char* GITHUB_LINK = "https://github.com/fstachura";
const char* EMAIL_LINK = "mailto://fbstachura@gmail.com";
const char* ABOUT_THIS_LINK = "https://github.com/fstachura/website";

// welcome to fbstc.org
// go to my github
// email me
// information about this
// exit to console

// https://stackoverflow.com/a/16457921
int get_screen_size(struct fb_var_screeninfo* info) {
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("failed to open framebuffer device");
        return fbfd;
    }

    int result = ioctl(fbfd, FBIOGET_VSCREENINFO, info);
    if(result == -1) {
        perror("failed to read info");
        return result;
    }

    return 0;
}

struct app_context {
    int serial_fb;
    int width;
    int height;
};

#define MSG_BUF_LEN 1024

void open_link(struct app_context* ctx, const char* link) {
    char* msg = malloc(MSG_BUF_LEN);
    // quote escapes...
    int result = snprintf(msg, MSG_BUF_LEN, "\n{\"type\": \"open\", \"link\": \"%s\"}\n", link);
    if (result > 0 && result < MSG_BUF_LEN) {
        // TODO locking
        int written = write(ctx->serial_fb, msg, result);
        if (written <= 0) {
            fprintf(stderr, "failed to write link %s %d\n", link, ctx->serial_fb);
            perror("write error");
        }
    } else {
        fprintf(stderr, "failed to write link %s %d\n", link, result);
        perror("snprintf error");
    }
    free(msg);
}

#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a < b ? b : a)

static void open_github(FL_OBJECT* obj, long event) {
    if (event == ButtonPress) {
        open_link((struct app_context*)obj->u_vdata, GITHUB_LINK);
    }
}

static void email_me(FL_OBJECT* obj, long event) {
    if (event == ButtonPress) {
        open_link((struct app_context*)obj->u_vdata, EMAIL_LINK);
    }
}

static void about_this(FL_OBJECT* obj, long event) {
    if (event == ButtonPress) {
        open_link((struct app_context*)obj->u_vdata, ABOUT_THIS_LINK);
    }
}

static void exit_to_console(FL_OBJECT* obj, long event) {
    if (event == ButtonPress) {
        exit(0);
    }
}

static void form_callback(FL_OBJECT* obj, void*) {
}

FL_FORM* create_main_form(struct app_context* ctx) {
    FL_OBJECT* obj;

    int form_width = MIN(480, ctx->width);
    int form_height = (ctx->height*4)/5;

    FL_FORM* form = fl_bgn_form(FL_NO_BOX, form_width, form_height);

    int button_width = (4*form_width)/5;
    int button_height = form_height/10;
    int button_x_offset = form_width/10;
    int button_padding = form_height/40;
    int button_start = button_padding;

    fl_add_box(FL_UP_BOX, 0, 0, form_width, form_height, "");

    obj = fl_add_box(FL_ROUNDED3D_UPBOX, button_x_offset, 20, button_width, 2*button_height, "welcome to fbstc.org");
    fl_set_object_lstyle(obj, FL_BOLD_STYLE);
    fl_set_object_lsize(obj, FL_HUGE_SIZE);

    obj = fl_add_button(FL_NORMAL_BUTTON, button_x_offset, button_start+2*(button_height+button_padding), 
            button_width, button_height, "my github");
    obj->u_vdata = ctx;
    fl_set_object_callback(obj, open_github, ButtonPress);

    obj = fl_add_button(FL_NORMAL_BUTTON, button_x_offset, button_start+3*(button_height+button_padding), 
            button_width, button_height, "email me");
    obj->u_vdata = ctx;
    fl_set_object_callback(obj, email_me, ButtonPress);

    obj = fl_add_button(FL_NORMAL_BUTTON, button_x_offset, button_start+4*(button_height+button_padding), 
            button_width, button_height, "about this");
    obj->u_vdata = ctx;
    fl_set_object_callback(obj, about_this, ButtonPress);

    obj = fl_add_button(FL_NORMAL_BUTTON, button_x_offset, form_height-button_start-button_padding-button_height, button_width, button_height, "exit to console");
    obj->u_vdata = ctx;
    fl_set_object_callback(obj, exit_to_console, ButtonPress);

    fl_end_form();

    fl_winposition((ctx->width-form_width)/2, (ctx->height-form_height)/2);

    return form;
}

int main(int argc, char** argv) {
    struct fb_var_screeninfo screen_info;
    int width, height;
    if (get_screen_size(&screen_info) == 0) {
        width = screen_info.xres;
        height = screen_info.yres;
    } else {
        width = 1024;
        height = 768;
    }

    fprintf(stderr, "%d %d\n", width, height);

    struct app_context* ctx = malloc(sizeof(struct app_context));
    ctx->serial_fb = open("/dev/hvc1", O_WRONLY);
    if (ctx->serial_fb <= 0) {
        perror("failed to open hvc1\n");
    } else {
        int written = write(ctx->serial_fb, "\n", 1);
        if (written <= 0) {
            perror("failed test serial_fb write");
        }
    }
    fprintf(stderr, "serial fb fd %d\n", ctx->serial_fb);

    ctx->width = width;
    ctx->height = height;

    fl_initialize(&argc, argv, "fbstc.org", 0, 0);

    FL_FORM* form = create_main_form(ctx);
    fl_set_form_callback(form, form_callback, 0);

    fl_show_form(form, FL_PLACE_FREE, FL_FULLBORDER, "fbstc.org");
    fl_do_forms();

    fl_finish();

    free(ctx);
    return 0;
}
