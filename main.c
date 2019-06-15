#include <clutter/clutter.h>
#include <stdlib.h>

#define CANVAS_HEIGHT 1080
#define CANVAS_WIDTH 1920

void close_app() {
	exit(0);
}

void _start() {
	asm volatile("sub $8, %rsp\n");

	int ret = clutter_init(NULL, NULL);
	(void)ret;

	ClutterActor *stage = clutter_stage_new();
	clutter_actor_show(stage);
	clutter_stage_hide_cursor(CLUTTER_STAGE(stage));
	clutter_stage_set_user_resizable(CLUTTER_STAGE(stage), TRUE);
	clutter_stage_set_fullscreen(CLUTTER_STAGE(stage), TRUE);

	ClutterEffect *shader = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);
	clutter_shader_effect_set_shader_source(CLUTTER_SHADER_EFFECT(shader), "void main(){cogl_color_out=vec4(cogl_tex_coord_in[0].xy, 0.3, 1.0);}\n");
	clutter_actor_add_effect(stage, shader);

	g_signal_connect(stage, "delete-event", close_app, NULL);
	clutter_main();
	__builtin_unreachable();
}
