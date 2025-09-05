#ifndef WEBVIEW_NODE_H
#define WEBVIEW_NODE_H

#include "scene/gui/control.h"

// Forward declare the C webview handle
typedef void *webview_t;

class WebViewNode : public Control {
	GDCLASS(WebViewNode, Control);

private:
	// Use C API webview handle
	webview_t webview;
	
	String current_url;
	bool debug_enabled;
	bool is_initialized;
	bool initialization_attempted;
	
	void _initialize_webview();
	void _cleanup_webview();
	void _try_initialize_safe();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	WebViewNode();
	virtual ~WebViewNode();

	// Core WebView functions
	void load_url(const String &p_url);
	void load_html(const String &p_html);
	void navigate(const String &p_url);
	void go_back();
	void go_forward();
	void reload();
	void stop();

	// JavaScript execution
	void eval_javascript(const String &p_script);
	void bind_function(const String &p_name, const Callable &p_callable);

	// Properties
	void set_url(const String &p_url);
	String get_url() const;
	void set_debug(bool p_debug);
	bool get_debug() const;

	// Window properties
	void set_title(const String &p_title);
	String get_title() const;
	void set_webview_size(const Vector2i &p_size);
	Vector2i get_webview_size() const;

	// Overrides
	Size2 get_minimum_size() const override;
};

#endif // WEBVIEW_NODE_H
