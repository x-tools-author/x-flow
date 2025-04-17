export function x_flow_plugin_get_api_version() {
    return "1.0";
}

export function x_flow_plugin_get_caption() {
    return "Bypass(js)";
}

export function x_flow_plugin_get_name() {
    return "x-tools.x-flow.plugin.bypass.js";
}

export function x_flow_plugin_get_in_ports() {
    return 1;
}

export function x_flow_plugin_get_out_ports() {
    return 1;
}

export function x_flow_plugin_handle_data(str, index) {
    return str;
}