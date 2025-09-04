# Godot WebView Module

A native WebView implementation for Godot using the lightweight [webview](https://github.com/webview/webview) library.

## Features

- Native WebView integration for Windows (WebView2), macOS (WebKit), and Linux (WebKitGTK)
- Simple C++ API similar to other Godot Control nodes
- JavaScript execution support
- URL navigation and HTML loading
- Debug mode support
- Cross-platform compatibility

## Platform Requirements

### Windows
- WebView2 runtime (usually pre-installed on Windows 10/11)

### macOS
- WebKit framework (built-in)

### Linux
- GTK3/4 and WebKitGTK development packages:
  ```bash
  # Ubuntu/Debian (GTK4 + WebKitGTK 6.0 - preferred)
  sudo apt-get install libgtk-4-dev libwebkitgtk-6.0-dev
  
  # Ubuntu/Debian (GTK3 + WebKitGTK 4.1 - fallback)
  sudo apt-get install libgtk-3-dev libwebkit2gtk-4.1-dev
  
  # Ubuntu/Debian (GTK3 + WebKitGTK 4.0 - older systems)
  sudo apt-get install libgtk-3-dev libwebkit2gtk-4.0-dev
  ```

## Usage

```gdscript
# Create a WebView node
var webview = WebViewNode.new()
add_child(webview)

# Set properties
webview.debug = true
webview.url = "https://godotengine.org"

# Or load content directly
webview.load_url("https://example.com")
webview.load_html("<html><body><h1>Hello World!</h1></body></html>")

# Execute JavaScript
webview.eval_javascript("alert('Hello from Godot!');")

# Connect to signals
webview.page_loaded.connect(_on_page_loaded)
webview.page_load_failed.connect(_on_page_load_failed)
```

## API Reference

### Methods

- `load_url(url: String)` - Load a URL
- `load_html(html: String)` - Load HTML content directly
- `navigate(url: String)` - Navigate to URL (alias for load_url)
- `go_back()` - Navigate back (via JavaScript)
- `go_forward()` - Navigate forward (via JavaScript)  
- `reload()` - Reload current page
- `stop()` - Stop loading
- `eval_javascript(script: String)` - Execute JavaScript code
- `bind_function(name: String, callable: Callable)` - Bind Godot function to JavaScript (planned)

### Properties

- `url: String` - Current URL
- `debug: bool` - Enable developer tools

### Signals

- `page_loaded(url: String)` - Emitted when page loads successfully
- `page_load_failed(url: String, error: String)` - Emitted when page load fails
- `title_changed(title: String)` - Emitted when page title changes

## Building

This module is automatically built when compiling Godot if the webview submodule is properly initialized:

```bash
# Initialize the webview submodule
git submodule update --init --recursive

# Build Godot with the module
scons platform=<platform> module_godot_webview_enabled=yes
```

## License

This module is licensed under the MIT License. The webview library is also MIT licensed.
