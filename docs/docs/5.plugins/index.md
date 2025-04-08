---
title: 插件开发
nav_order: 5
---

## 插件开发

> 插件开发可以参考[插件模板]

[插件模板]: https://github.com/x-tools-author/x-flow/src

## 目录框架

## 配置文件

### 配置文件示例

```json
{
    "plugin-id": "c-plugin-crc32-calc",
    "plugin-api-prefix": "crc32_calc",
    "plugin-category": "Data Processing",
    "plugin-name": "CRC32 Calculator",
    "plugin-version": "1.0.0",
    "plugin-api-version": "1.0",
    "plugin-type": "c",
    "plugin-author": "x-tools-author",
    "plugin-repository": "https://github.com/x-tools-author/x-flow/tree/master/src/c/crccalculator",
    "plugin-description": "A plugin to calculate CRC32 values for given data."
}
```

### 配置选项说明

| 参数 | 类型 | 是否必须 | 说明 |
| --- | --- | --- | --- |
| plugin-id | string | 是 | 插件的唯一标识符 |
| plugin-api-prefix | string | 是 | 插件的API前缀 |
| plugin-category | string | 是 | 插件的分类 |
| plugin-name | string | 是 | 插件的名称 |
| plugin-version | string | 是 | 插件的版本 |
| plugin-api-version | string | 是 | 插件的API版本，目前只支持"v1.0" |
| plugin-type | string | 是 | 插件类型，详情查看**插件类型**|
| plugin-author | string | 否 | 插件的作者 |
| plugin-repository | string | 否 | 插件的代码仓库地址 |
| plugin-description | string | 否 | 插件的描述 |

#### 插件类型

- `c`：基于C语言的插件，对应的文件后缀为`.so`或`.dll`
- `qt`：基于Qt的插件，对应的文件后缀为`.so`或`.dll`
- `lua`：基于Lua的插件，对应的文件后缀为`lua`
- `python`：基于Python的插件，对应的文件后缀为`py`
- `javascript`：基于JavaScript的插件，对应的文件后缀为`js`
