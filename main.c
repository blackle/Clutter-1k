#include <clutter/clutter.h>
#include <stdlib.h>

#define CANVAS_HEIGHT 1080
#define CANVAS_WIDTH 1920

static void close_app() {
	asm volatile(".intel_syntax noprefix");
	asm volatile("push 231"); //exit_group
	asm volatile("pop rax");
	asm volatile("xor edi, edi");
	asm volatile("syscall");
	asm volatile(".att_syntax prefix");
	__builtin_unreachable();
}

static void on_new_frame(ClutterTimeline *timeline, gint msecs, ClutterEffect *shader) {
	(void)timeline;
	clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shader), "iTime", G_TYPE_FLOAT, 1, msecs/1000.0);
}

void _start() {
	asm volatile("pop %rax\n");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
	typedef void (*voidWithOneParam)(int*);
	voidWithOneParam clutter_init_one_param = (voidWithOneParam)clutter_init;
	(*clutter_init_one_param)(NULL);
#pragma GCC diagnostic pop

	ClutterActor *stage = clutter_stage_new();
	clutter_actor_show(stage);
	clutter_stage_hide_cursor(CLUTTER_STAGE(stage));
	clutter_stage_set_user_resizable(CLUTTER_STAGE(stage), TRUE);
	clutter_stage_set_fullscreen(CLUTTER_STAGE(stage), TRUE);

	ClutterTimeline *timeline = clutter_timeline_new(1000);

	ClutterEffect *shader = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);
	clutter_shader_effect_set_shader_source(CLUTTER_SHADER_EFFECT(shader), "uniform float iTime;void main(){cogl_color_out=vec4(cogl_tex_coord_in[0].xy,0.5,1.0);if(cogl_tex_coord_in[0].y < iTime){cogl_color_out.x=0.0;}}\n");
	clutter_actor_add_effect(stage, shader);

	// on_new_frame(timeline, 0, CLUTTER_SHADER_EFFECT(shader));
	g_signal_connect(timeline, "new-frame", G_CALLBACK(on_new_frame), shader);
	g_signal_connect(timeline, "completed", G_CALLBACK(close_app), NULL);
	g_signal_connect(stage, "delete-event", G_CALLBACK(close_app), NULL);

	clutter_timeline_start(timeline);
	clutter_main();
	__builtin_unreachable();
}
