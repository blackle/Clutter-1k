#include <clutter/clutter.h>
#include <stdlib.h>
#include "shader.h"

#define ANIMATE
#define OFFSET_MS (0*1000)
#define FULL_SCREEN

#if defined(ANIMATE)
static void on_new_frame(ClutterTimeline *timeline, gint msecs, ClutterEffect *shader) {
	(void)timeline;
	clutter_shader_effect_set_uniform(CLUTTER_SHADER_EFFECT(shader), VAR_ITIME, G_TYPE_FLOAT, 1, (msecs+OFFSET_MS)/1000.0);
}
#endif

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
	typedef void (*voidWithOneParam)(int*);
	voidWithOneParam clutter_init_one_param = (voidWithOneParam)clutter_init;
	(*clutter_init_one_param)(NULL);
#pragma GCC diagnostic pop

	ClutterActor *stage = clutter_stage_new();
	ClutterEffect *shader = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);
	ClutterTimeline *timeline = clutter_timeline_new(48000-OFFSET_MS);

#if defined(ANIMATE)
	g_signal_connect(timeline, "new-frame", G_CALLBACK(on_new_frame), shader);
	g_signal_connect(timeline, "completed", G_CALLBACK(clutter_main_quit), NULL);
#endif
	g_signal_connect(stage, "delete-event", G_CALLBACK(clutter_main_quit), NULL);
	g_signal_connect(stage, "key-press-event", G_CALLBACK(clutter_main_quit), NULL);

#if !defined(FULL_SCREEN)
	clutter_actor_set_size(stage, 640, 360);
#endif
	clutter_actor_show(stage);
#if defined(FULL_SCREEN)
	clutter_stage_set_user_resizable(CLUTTER_STAGE(stage), TRUE);
	clutter_stage_set_fullscreen(CLUTTER_STAGE(stage), TRUE);
	clutter_stage_hide_cursor(CLUTTER_STAGE(stage));
#endif

	clutter_shader_effect_set_shader_source(CLUTTER_SHADER_EFFECT(shader), shader_frag);
#if defined(ANIMATE)
	clutter_actor_add_effect(stage, clutter_blur_effect_new());
#endif
	clutter_actor_add_effect(stage, shader);

#if defined(ANIMATE)
	clutter_timeline_start(timeline);
#endif
	clutter_main();
	asm volatile(".intel_syntax noprefix");
	asm volatile("push 60"); //exit
	asm volatile("pop rax");
	asm volatile("xor edi, edi");
	asm volatile("syscall");
	asm volatile(".att_syntax prefix");
	__builtin_unreachable();
}
