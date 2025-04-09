---
title: 插件开发
nav_order: 5
---

## 插件开发

{: .important-title }
> 特别声明
>
> 为鼓励用户开发自己的插件，项目作者向广大用户承诺，xFLow的插件模块始终保持向下兼容。

## 目录框架

插件目录下必须包含一个`conf`和`lib`目录（基于Qt开发的插件除外），`conf`目录下必须包含一个`conf.json`文件；`lib`目录下必须包含一个且仅一个库文件或者脚本文件。

以crc32计算器插件为例，其目录结构如下：

```txt
.crc32_calc
├── conf
│    └── conf.json
└── lib
     └── crc32_calc.dll
```

> 如果lib文件夹内包含多个文件，将会导致插件加载失败。

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
