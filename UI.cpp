#include "UI.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <ranges>

#include "imgui/imgui.h"

void UI::Draw() {
	ImGui::Begin("Combat Log", &opened);

	ImGui::InputText("Player", playerBuf, sizeof(playerBuf));

	std::lock_guard<std::mutex> guard(eventsMutex);

	if (ImGui::Button("Save and Clear")) {
		// save current events to a file
		char fileNameBuf[512]{};
		typedef std::chrono::system_clock system_clock;
		snprintf(fileNameBuf, 512, "addons\\combat_log\\%lld.txt", system_clock::to_time_t(system_clock::now()));
		std::ofstream outFile(fileNameBuf);
		for (const Event& event : events) {
			outFile << event.index << "|" <<
				static_cast<int>(event.type) << "|" <<
				event.skillname << "|" <<

				event.ev.time << "|" <<
				event.ev.src_agent << "|" <<
				event.ev.dst_agent << "|" <<
				event.ev.value << "|" <<
				event.ev.buff_dmg << "|" <<
				event.ev.overstack_value << "|" <<
				event.ev.skillid << "|" <<
				event.ev.src_instid << "|" <<
				event.ev.dst_instid << "|" <<
				event.ev.src_master_instid << "|" <<
				event.ev.dst_master_instid << "|" <<
				event.ev.iff << "|" <<
				event.ev.buff << "|" <<
				event.ev.result << "|" <<
				event.ev.is_activation << "|" <<
				event.ev.is_buffremove << "|" <<
				event.ev.is_ninety << "|" <<
				event.ev.is_fifty << "|" <<
				event.ev.is_moving << "|" <<
				event.ev.is_statechange << "|" <<
				event.ev.is_flanking << "|" <<
				event.ev.is_shields << "|" <<
				event.ev.is_offcycle << "|" <<
				event.ev.pad61 << "|" <<
				event.ev.pad62 << "|" <<
				event.ev.pad63 << "|" <<
				event.ev.pad64 << "|" <<

				event.src.name << "|" <<
				event.src.id << "|" <<
				event.src.prof << "|" <<
				event.src.elite << "|" <<
				event.src.self << "|" <<
				event.src.team << "|" <<

				event.dst.name << "|" <<
				event.dst.id << "|" <<
				event.dst.prof << "|" <<
				event.dst.elite << "|" <<
				event.dst.self << "|" <<
				event.dst.team << "|" << '\n';
		}

		outFile.close();

		events.clear();
		nextIndex = 0;
	}

	const ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings |
		ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg |
		ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

	if (ImGui::BeginTable("Combat Table", 42, tableFlags)) {

		ImGui::TableSetupColumn("#");
		ImGui::TableSetupColumn("type");
		ImGui::TableSetupColumn("ev->time");
		ImGui::TableSetupColumn("ev->src_agent");
		ImGui::TableSetupColumn("ev->dst_agent");
		ImGui::TableSetupColumn("ev->value");
		ImGui::TableSetupColumn("ev->buff_dmg");
		ImGui::TableSetupColumn("ev->overstack_value");
		ImGui::TableSetupColumn("ev->skillid");
		ImGui::TableSetupColumn("ev->src_instid");
		ImGui::TableSetupColumn("ev->dst_instid");
		ImGui::TableSetupColumn("ev->src_master_instid");
		ImGui::TableSetupColumn("ev->dst_master_instid");
		ImGui::TableSetupColumn("ev->iff");
		ImGui::TableSetupColumn("ev->buff");
		ImGui::TableSetupColumn("ev->result");
		ImGui::TableSetupColumn("ev->is_activation");
		ImGui::TableSetupColumn("ev->is_buffremove");
		ImGui::TableSetupColumn("ev->is_ninety");
		ImGui::TableSetupColumn("ev->is_fifty");
		ImGui::TableSetupColumn("ev->is_moving");
		ImGui::TableSetupColumn("ev->is_statechange");
		ImGui::TableSetupColumn("ev->is_flanking");
		ImGui::TableSetupColumn("ev->is_shields");
		ImGui::TableSetupColumn("ev->is_offcycle");
		ImGui::TableSetupColumn("ev->pad61");
		ImGui::TableSetupColumn("ev->pad62");
		ImGui::TableSetupColumn("ev->pad63");
		ImGui::TableSetupColumn("ev->pad64");
		ImGui::TableSetupColumn("src->name");
		ImGui::TableSetupColumn("src->id");
		ImGui::TableSetupColumn("src->prof");
		ImGui::TableSetupColumn("src->elite");
		ImGui::TableSetupColumn("src->self");
		ImGui::TableSetupColumn("src->team");
		ImGui::TableSetupColumn("dst->name");
		ImGui::TableSetupColumn("dst->id");
		ImGui::TableSetupColumn("dst->prof");
		ImGui::TableSetupColumn("dst->elite");
		ImGui::TableSetupColumn("dst->self");
		ImGui::TableSetupColumn("dst->team");
		ImGui::TableSetupColumn("skillname");

		ImGui::TableHeadersRow();

		auto filterPlayer = [this](const Event& event) {
			if (playerBuf[0] == '\0') return true;
			return event.src.name.find(playerBuf) != std::string::npos || event.dst.name.find(playerBuf) != std::string::npos;
		};

		auto filterEventType = [this](const Event& event) {
			return event.type == EventType::PhysicalDamage;
		};
		
		for (const Event& event : events | std::views::filter(filterPlayer) | std::views::filter(filterEventType)) {
			ImGui::TableNextRow();

			if (ImGui::TableNextColumn()) {
				ImGui::Text("%d", event.index);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%s", to_string(event.type).c_str());
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%llu", event.ev.time);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%llu", event.ev.src_agent);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%llu", event.ev.dst_agent);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.value);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%d", event.ev.buff_dmg);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.overstack_value);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.skillid);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.src_instid);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.dst_instid);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.src_master_instid);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.dst_master_instid);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.iff);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.buff);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.result);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_activation);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_buffremove);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_ninety);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_fifty);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_moving);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_statechange);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_flanking);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_shields);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.is_offcycle);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.pad61);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.pad62);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.pad63);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.ev.pad64);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%s", event.src.name.c_str());
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%llu", event.src.id);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.src.prof);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.src.elite);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.src.self);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%d", event.src.team);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%s", event.dst.name.c_str());
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%llu", event.dst.id);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.dst.prof);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.dst.elite);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.dst.self);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%u", event.dst.team);
			}
			if (ImGui::TableNextColumn()) {
				ImGui::Text("%s", event.skillname.c_str());
			}
		}

		ImGui::EndTable();
	}
	
	ImGui::End();
}

void UI::addEvent(EventType type, cbtevent* ev, ag* src, ag* dst, char* skillname) {
	cbtevent new_ev{};
	if (ev) {
		new_ev.time = ev->time;
		new_ev.src_agent = ev->src_agent;
		new_ev.dst_agent = ev->dst_agent;
		new_ev.value = ev->value;
		new_ev.buff_dmg = ev->buff_dmg;
		new_ev.overstack_value = ev->overstack_value;
		new_ev.skillid = ev->skillid;
		new_ev.src_instid = ev->src_instid;
		new_ev.dst_instid = ev->dst_instid;
		new_ev.src_master_instid = ev->src_master_instid;
		new_ev.dst_master_instid = ev->dst_master_instid;
		new_ev.iff = ev->iff;
		new_ev.buff = ev->buff;
		new_ev.result = ev->result;
		new_ev.is_activation = ev->is_activation;
		new_ev.is_buffremove = ev->is_buffremove;
		new_ev.is_ninety = ev->is_ninety;
		new_ev.is_fifty = ev->is_fifty;
		new_ev.is_moving = ev->is_moving;
		new_ev.is_statechange = ev->is_statechange;
		new_ev.is_flanking = ev->is_flanking;
		new_ev.is_shields = ev->is_shields;
		new_ev.is_offcycle = ev->is_offcycle;
		new_ev.pad61 = ev->pad61;
		new_ev.pad62 = ev->pad62;
		new_ev.pad63 = ev->pad63;
		new_ev.pad64 = ev->pad64;
	}

	ag2 new_src{};
	if (src) {
		if (src->name) {
			new_src.name = src->name;
		}
		new_src.id = src->id;
		new_src.prof = src->prof;
		new_src.elite = src->elite;
		new_src.self = src->self;
		new_src.team = src->team;
	}

	ag2 new_dst{};
	if (dst) {
		if (dst->name) {
			new_dst.name = dst->name;
		}
		new_dst.id = dst->id;
		new_dst.prof = dst->prof;
		new_dst.elite = dst->elite;
		new_dst.self = dst->self;
		new_dst.team = dst->team;
	}

	std::string skillnameString;
	if (skillname) {
		skillnameString = skillname;
	}

	std::lock_guard<std::mutex> guard(eventsMutex);
	events.emplace_back(++nextIndex, type, new_ev, new_src, new_dst, skillnameString);
}
