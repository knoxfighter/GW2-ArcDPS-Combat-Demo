#include "arcdps_structs.h"

std::string to_string(EventType event) {
	switch (event) {
	case EventType::AgentAdded: 
		return "Agent added";
	case EventType::AgentRemoved: 
		return "Agent removed";
	case EventType::NewTarget: 
		return "New Target";
	case EventType::StateChange: 
		return "State change";
	case EventType::Activation: 
		return "Activation";
	case EventType::BuffRemove: 
		return "Buff remove";
	case EventType::BuffDamage: 
		return "Buff damage";
	case EventType::BuffApplication: 
		return "Buff application";
	case EventType::PhysicalDamage:
		return "Physical";
	default: return "Unknown";
	}
}
