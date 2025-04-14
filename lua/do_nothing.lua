local function x_flow_plugin_get_api_version()
    return "1.0";
end

local function x_flow_plugin_get_caption()
    return "Do Nothing";
end

local function x_flow_plugin_get_name()
    return "Do Nothing";
end

local function x_flow_plugin_get_in_ports()
    return 1;
end

local function x_flow_plugin_get_out_ports()
    return 1;
end

local function x_flow_plugin_handle_data(str)
    return str;
end
