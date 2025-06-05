def x_flow_plugin_get_api_version():
    return "1.0"


def x_flow_plugin_get_caption():
    return "Bypass(py)"


def x_flow_plugin_get_name():
    return "x-tools.x-flow.plugin.bypass.py"


def x_flow_plugin_get_in_ports():
    return 1


def x_flow_plugin_get_out_ports():
    return 1


def x_flow_plugin_handle_data(data: bytes, index: int):
    return data
