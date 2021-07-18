#include <Windows.h>

#include "arcdps_structs.h"
#include "UI.h"
#include "imgui/imgui.h"

arcdps_exports arc_exports;
char* arcvers;
UI ui;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

/* combat callback -- may be called asynchronously. return ignored */
/* one participant will be party/squad, or minion of. no spawn statechange events. despawn statechange only on marked boss npcs */
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision) {
	/* ev is null. dst will only be valid on tracking add. skillname will also be null */
	if (!ev) {

		/* notify tracking change */
		if (!src->elite) {

			/* add */
			if (src->prof) {
				ui.addEvent(EventType::AgentAdded, ev, src, dst, skillname);
				// p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				// p += _snprintf_s(p, 400, _TRUNCATE, "agent added: %s:%s (%0llx), instid: %u, prof: %u, elite: %u, self: %u, team: %u, subgroup: %u\n", src->name, dst->name, src->id, dst->id, dst->prof, dst->elite, dst->self, src->team, dst->team);
			}

			/* remove */
			else {
				ui.addEvent(EventType::AgentRemoved, ev, src, dst, skillname);
				// p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				// p += _snprintf_s(p, 400, _TRUNCATE, "agent removed: %s (%0llx)\n", src->name, src->id);
			}
		}

		/* notify target change */
		else if (src->elite == 1) {
			ui.addEvent(EventType::NewTarget, ev, src, dst, skillname);
			// p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
			// p += _snprintf_s(p, 400, _TRUNCATE, "new target: %0llx\n", src->id);
		}
	}

	/* combat event. skillname may be null. non-null skillname will remain static until module is unloaded. refer to evtc notes for complete detail */
	else {

		/* default names */
		if (!src->name || !strlen(src->name))
			src->name = const_cast<char*>("(area)");
		if (!dst->name || !strlen(dst->name))
			dst->name = const_cast<char*>("(area)");

		EventType eType;
		
		/* common */
		// p += _snprintf_s(p, 400, _TRUNCATE, "combatdemo: ==== cbtevent %u at %llu ====\n", cbtcount, ev->time);
		// p += _snprintf_s(p, 400, _TRUNCATE, "source agent: %s (%0llx:%u, %lx:%lx), master: %u\n", src->name, ev->src_agent, ev->src_instid, src->prof, src->elite, ev->src_master_instid);
		// if (ev->dst_agent) p += _snprintf_s(p, 400, _TRUNCATE, "target agent: %s (%0llx:%u, %lx:%lx)\n", dst->name, ev->dst_agent, ev->dst_instid, dst->prof, dst->elite);
		// else p += _snprintf_s(p, 400, _TRUNCATE, "target agent: n/a\n");

		/* statechange */
		if (ev->is_statechange) {
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_statechange: %u\n", ev->is_statechange);
			eType = EventType::StateChange;
		}

		/* activation */
		else if (ev->is_activation) {
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_activation: %u\n", ev->is_activation);
			// p += _snprintf_s(p, 400, _TRUNCATE, "skill: %s:%u\n", skillname, ev->skillid);
			// p += _snprintf_s(p, 400, _TRUNCATE, "ms_expected: %d\n", ev->value);
			eType = EventType::Activation;
		}

		/* buff remove */
		else if (ev->is_buffremove) {
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_buffremove: %u\n", ev->is_buffremove);
			// p += _snprintf_s(p, 400, _TRUNCATE, "skill: %s:%u\n", skillname, ev->skillid);
			// p += _snprintf_s(p, 400, _TRUNCATE, "ms_duration: %d\n", ev->value);
			// p += _snprintf_s(p, 400, _TRUNCATE, "ms_intensity: %d\n", ev->buff_dmg);
			eType = EventType::BuffRemove;
		}

		/* buff */
		else if (ev->buff) {

			/* damage */
			if (ev->buff_dmg) {
				// p += _snprintf_s(p, 400, _TRUNCATE, "is_buff: %u\n", ev->buff);
				// p += _snprintf_s(p, 400, _TRUNCATE, "skill: %s:%u\n", skillname, ev->skillid);
				// p += _snprintf_s(p, 400, _TRUNCATE, "dmg: %d\n", ev->buff_dmg);
				// p += _snprintf_s(p, 400, _TRUNCATE, "is_shields: %u\n", ev->is_shields);
				eType = EventType::BuffDamage;
			}

			/* application */
			else {
				// p += _snprintf_s(p, 400, _TRUNCATE, "is_buff: %u\n", ev->buff);
				// p += _snprintf_s(p, 400, _TRUNCATE, "skill: %s:%u\n", skillname, ev->skillid);
				// p += _snprintf_s(p, 400, _TRUNCATE, "raw ms: %d\n", ev->value);
				// p += _snprintf_s(p, 400, _TRUNCATE, "overstack ms: %u\n", ev->overstack_value);
				eType = EventType::BuffApplication;
			}
		}

		/* physical */
		else {
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_buff: %u\n", ev->buff);
			// p += _snprintf_s(p, 400, _TRUNCATE, "skill: %s:%u\n", skillname, ev->skillid);
			// p += _snprintf_s(p, 400, _TRUNCATE, "dmg: %d\n", ev->value);
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_moving: %u\n", ev->is_moving);
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_ninety: %u\n", ev->is_ninety);
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_flanking: %u\n", ev->is_flanking);
			// p += _snprintf_s(p, 400, _TRUNCATE, "is_shields: %u\n", ev->is_shields);
			eType = EventType::PhysicalDamage;
		}

		/* common */
		// p += _snprintf_s(p, 400, _TRUNCATE, "iff: %u\n", ev->iff);
		// p += _snprintf_s(p, 400, _TRUNCATE, "result: %u\n", ev->result);

		ui.addEvent(eType, ev, src, dst, skillname);
	}

	return 0;
}

uintptr_t mod_options() {
	ImGui::Checkbox("Combat Log", &ui.opened);

	return 0;
}

uintptr_t mod_imgui(uint32_t not_charsel_or_loading) {
	if (!not_charsel_or_loading) return 0;

	if (ui.opened) {
		ui.Draw();
	}

	return 0;
}

/* initialize mod -- return table that arcdps will use for callbacks */
arcdps_exports* mod_init() {
    /* for arcdps */
    memset(&arc_exports, 0, sizeof(arcdps_exports));
    arc_exports.sig = 0xFFFA;
    arc_exports.imguivers = IMGUI_VERSION_NUM;
    arc_exports.size = sizeof(arcdps_exports);
    arc_exports.out_name = "combatdemo";
    arc_exports.out_build = "0.1";
    arc_exports.combat = mod_combat;
	arc_exports.imgui = mod_imgui;
	arc_exports.options_end = mod_options;
    //arc_exports.size = (uintptr_t)"error message if you decide to not load, sig must be 0";
    return &arc_exports;
}

/* release mod -- return ignored */
uintptr_t mod_release() {
    FreeConsole();
    return 0;
}


/* export -- arcdps looks for this exported function and calls the address it returns on client load */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext * imguictx, void* id3dd9, HANDLE arcdll, void* mallocfn, void* freefn) {
    arcvers = arcversion;
    ImGui::SetCurrentContext((ImGuiContext*)imguictx);
    ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))mallocfn, (void (*)(void*, void*))freefn); // on imgui 1.80+
    return mod_init;
}

/* export -- arcdps looks for this exported function and calls the address it returns on client exit */
extern "C" __declspec(dllexport) void* get_release_addr() {
    arcvers = 0;
    return mod_release;
}
