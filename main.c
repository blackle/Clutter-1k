#include <clutter/clutter.h>
#include <stdlib.h>
#include "shader.h"

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
	clutter_actor_show(stage);
	clutter_stage_hide_cursor(CLUTTER_STAGE(stage));
	clutter_stage_set_user_resizable(CLUTTER_STAGE(stage), TRUE);
	clutter_stage_set_fullscreen(CLUTTER_STAGE(stage), TRUE);

	ClutterEffect *shader = clutter_shader_effect_new(CLUTTER_FRAGMENT_SHADER);
	clutter_shader_effect_set_shader_source(CLUTTER_SHADER_EFFECT(shader), shader_frag);
	clutter_actor_add_effect(stage, shader);

	g_signal_connect(stage, "delete-event", G_CALLBACK(close_app), NULL);

	clutter_main();
	__builtin_unreachable();
}
