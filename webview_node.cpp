#include "webview_node.h"
#include "webview/api.h"  // Use C API
#include "core/config/engine.h"
#include "core/error/error_macros.h"

void WebViewNode::_bind_methods() {
	// Core WebView functions
	ClassDB::bind_method(D_METHOD("load_url", "url"), &WebViewNode::load_url);
	ClassDB::bind_method(D_METHOD("load_html", "html"), &WebViewNode::load_html);
	ClassDB::bind_method(D_METHOD("navigate", "url"), &WebViewNode::navigate);
	ClassDB::bind_method(D_METHOD("go_back"), &WebViewNode::go_back);
	ClassDB::bind_method(D_METHOD("go_forward"), &WebViewNode::go_forward);
	ClassDB::bind_method(D_METHOD("reload"), &WebViewNode::reload);
	ClassDB::bind_method(D_METHOD("stop"), &WebViewNode::stop);

	// JavaScript execution
	ClassDB::bind_method(D_METHOD("eval_javascript", "script"), &WebViewNode::eval_javascript);
	ClassDB::bind_method(D_METHOD("bind_function", "name", "callable"), &WebViewNode::bind_function);

	// Properties
	ClassDB::bind_method(D_METHOD("set_url", "url"), &WebViewNode::set_url);
	ClassDB::bind_method(D_METHOD("get_url"), &WebViewNode::get_url);
	ClassDB::bind_method(D_METHOD("set_debug", "debug"), &WebViewNode::set_debug);
	ClassDB::bind_method(D_METHOD("get_debug"), &WebViewNode::get_debug);

	// Window properties
	ClassDB::bind_method(D_METHOD("set_title", "title"), &WebViewNode::set_title);
	ClassDB::bind_method(D_METHOD("get_title"), &WebViewNode::get_title);
	ClassDB::bind_method(D_METHOD("set_webview_size", "size"), &WebViewNode::set_webview_size);
	ClassDB::bind_method(D_METHOD("get_webview_size"), &WebViewNode::get_webview_size);

	// Properties
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "url"), "set_url", "get_url");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug"), "set_debug", "get_debug");

	// Signals
	ADD_SIGNAL(MethodInfo("page_loaded", PropertyInfo(Variant::STRING, "url")));
	ADD_SIGNAL(MethodInfo("page_load_failed", PropertyInfo(Variant::STRING, "url"), PropertyInfo(Variant::STRING, "error")));
	ADD_SIGNAL(MethodInfo("title_changed", PropertyInfo(Variant::STRING, "title")));
}

WebViewNode::WebViewNode() {
	print_line("WebView: Constructor called");
	webview = nullptr;
	current_url = "";
	debug_enabled = false;
	is_initialized = false;
	initialization_attempted = false;
	set_clip_contents(true);
	set_focus_mode(Control::FOCUS_ALL);
	print_line("WebView: Constructor completed");
}

WebViewNode::~WebViewNode() {
	print_line("WebView: Destructor called");
	_cleanup_webview();
	print_line("WebView: Destructor completed");
}

void WebViewNode::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			print_line("WebView: NOTIFICATION_READY received");
			if (Engine::get_singleton()->is_editor_hint()) {
				print_line("WebView: In editor mode, skipping initialization");
				return; // Don't initialize in editor
			}
			print_line("WebView: Calling _initialize_webview()");
			_initialize_webview();
		} break;

		case NOTIFICATION_EXIT_TREE: {
			print_line("WebView: NOTIFICATION_EXIT_TREE received");
			_cleanup_webview();
		} break;

		case NOTIFICATION_RESIZED: {
			print_line("WebView: NOTIFICATION_RESIZED received");
			if (is_initialized && webview) {
				Vector2i size = get_size();
				print_line("WebView: Resizing to: " + String::num(size.x) + "x" + String::num(size.y));
				webview_set_size(webview, size.x, size.y, WEBVIEW_HINT_NONE);
			}
		} break;
	}
}

void WebViewNode::_initialize_webview() {
	print_line("WebView: _initialize_webview() called");
	
	if (is_initialized) {
		print_line("WebView: Already initialized, returning");
		return;
	}
	
	if (Engine::get_singleton()->is_editor_hint()) {
		print_line("WebView: In editor hint mode, returning");
		return;
	}

	print_line("WebView: Initializing webview on MAIN THREAD (as required by WebKit)");
	
	// Add a safety check - defer initialization if we're not ready
	if (!is_inside_tree()) {
		print_line("WebView: Not in tree yet, deferring initialization");
		call_deferred("_initialize_webview");
		return;
	}
	
	print_line("WebView: Current URL to load: '" + current_url + "'");
	print_line("WebView: Debug enabled: " + String::num(debug_enabled));
	
	// Create webview on MAIN THREAD (as required by WebKit on macOS)
	print_line("WebView: About to call webview_create() on main thread");
	
	webview = webview_create(debug_enabled ? 1 : 0, nullptr);
	
	print_line("WebView: webview_create() call completed!");
	
	if (!webview) {
		print_line("WebView: ERROR - webview_create() returned nullptr");
		ERR_PRINT("Failed to create WebView instance");
		return;
	}
	
	print_line("WebView: SUCCESS - webview_create() returned a valid webview instance!");
	is_initialized = true;
	
	// Set window properties
	print_line("WebView: Setting window title and size");
	webview_set_title(webview, "Godot WebView - Test Window");
	webview_set_size(webview, 1024, 768, WEBVIEW_HINT_NONE);
	
	// Load initial content
	if (!current_url.is_empty()) {
		print_line("WebView: Loading initial URL: " + current_url);
		webview_navigate(webview, current_url.utf8().get_data());
	} else {
		print_line("WebView: Loading default content");
		webview_set_html(webview, "<h1>WebView initialized successfully!</h1><p>This should be visible in a separate window.</p><p>Initialized on main thread without webview_run().</p>");
	}
	
	print_line("WebView: WebView initialized successfully on main thread");
	print_line("WebView: NOTE - NOT calling webview_run() to avoid blocking Godot's main thread");
	print_line("WebView: The webview window should be visible now");
}

void WebViewNode::_cleanup_webview() {
	print_line("WebView: _cleanup_webview() called");
	
	if (webview) {
		print_line("WebView: Destroying webview instance");
		webview_destroy(webview);
		webview = nullptr;
		print_line("WebView: Webview instance destroyed");
	} else {
		print_line("WebView: No webview instance to destroy");
	}
	
	is_initialized = false;
	print_line("WebView: _cleanup_webview() completed");
}

void WebViewNode::load_url(const String &p_url) {
	print_line("WebView: load_url() called with: " + p_url);
	current_url = p_url;
	
	if (!is_initialized) {
		print_line("WebView: Not initialized yet, URL will be loaded when ready");
		return; // Will be loaded when webview is ready
	}
	
	if (!webview) {
		print_line("WebView: ERROR - webview instance is null");
		return;
	}

	print_line("WebView: Calling webview_navigate() on main thread");
	webview_navigate(webview, p_url.utf8().get_data());
	print_line("WebView: webview_navigate() completed");
	
	emit_signal("page_loaded", p_url);
	print_line("WebView: page_loaded signal emitted");
}

void WebViewNode::load_html(const String &p_html) {
	if (!is_initialized || !webview) {
		return;
	}

	webview_set_html(webview, p_html.utf8().get_data());
}

void WebViewNode::navigate(const String &p_url) {
	load_url(p_url);
}

void WebViewNode::go_back() {
	if (!is_initialized || !webview) {
		return;
	}
	// Note: The webview C API doesn't have built-in back/forward
	// This would need to be implemented with JavaScript
	eval_javascript("history.back()");
}

void WebViewNode::go_forward() {
	if (!is_initialized || !webview) {
		return;
	}
	// Note: The webview C API doesn't have built-in back/forward
	// This would need to be implemented with JavaScript
	eval_javascript("history.forward()");
}

void WebViewNode::reload() {
	if (!is_initialized || !webview) {
		return;
	}
	eval_javascript("location.reload()");
}

void WebViewNode::stop() {
	if (!is_initialized || !webview) {
		return;
	}
	eval_javascript("window.stop()");
}

void WebViewNode::eval_javascript(const String &p_script) {
	if (!is_initialized || !webview) {
		return;
	}

	webview_eval(webview, p_script.utf8().get_data());
}

void WebViewNode::bind_function(const String &p_name, const Callable &p_callable) {
	if (!is_initialized || !webview) {
		return;
	}

	// Note: This is a simplified implementation
	// A full implementation would need to store the callable and create a callback
	print_line("WebView: bind_function called for: " + p_name);
}

void WebViewNode::set_url(const String &p_url) {
	load_url(p_url);
}

String WebViewNode::get_url() const {
	return current_url;
}

void WebViewNode::set_debug(bool p_debug) {
	debug_enabled = p_debug;
	// Note: Debug flag is set during webview creation, can't change at runtime
}

bool WebViewNode::get_debug() const {
	return debug_enabled;
}

void WebViewNode::set_title(const String &p_title) {
	if (!is_initialized || !webview) {
		return;
	}

	webview_set_title(webview, p_title.utf8().get_data());
	emit_signal("title_changed", p_title);
}

String WebViewNode::get_title() const {
	// Note: The webview C API doesn't provide a way to get the current title
	// This would need to be tracked via JavaScript callbacks
	return "";
}

void WebViewNode::set_webview_size(const Vector2i &p_size) {
	if (!is_initialized || !webview) {
		return;
	}

	webview_set_size(webview, p_size.x, p_size.y, WEBVIEW_HINT_NONE);
}

Vector2i WebViewNode::get_webview_size() const {
	return Vector2i(get_size());
}

Size2 WebViewNode::get_minimum_size() const {
	return Size2(100, 100);
}
