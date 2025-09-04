#include "modules/register_module_types.h"

#include "webview_node.h"

void initialize_godot_webview_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	
	GDREGISTER_CLASS(WebViewNode);
}

void uninitialize_godot_webview_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}
