def x_flow_plugin_get_api_version():
    return "1.0"


def x_flow_plugin_get_caption():
    return "Bypass(js)"


def x_flow_plugin_get_name():
    return "xtools.xflow.plugin.bypass.js"


def x_flow_plugin_get_in_ports():
    return 1


def x_flow_plugin_get_out_ports():
    return 1


def x_flow_plugin_handle_data(str: str, index: int):
    return str
