# Live2D 模型规范

以下模型规范是参考自带的Haru模型写的，如果有哪里写的不好或者有问题或者看不懂，请直接联系我

## 文件结构说明
```json5
{
  "FileReferences": {
    "DisplayInfo": "Haru.cdi3.json",  // 显示辅助文件，显示模型部位信息，非必须，一般情况下没用
    "Expressions": [                   // 表情列表，非必须，有的模型没有，用于播放表情，
                                      //和motion的区别在于这个是在当前状态下播放表情，motion是换到一个新状态
      {
        "File": "expressions/F01.exp3.json",     // 表情文件路径，model3.json中所有的路径都相对于当前目录
        "Name": "F01"          // 表情名字，暂时不知道有什么用，但和上面的File字段应该都是必须的
      },
      {
        "File": "expressions/F02.exp3.json",     
        "Name": "F02"          
      }
    ],
    "Moc": "Haru.moc3",          // 模型核心文件路径，该文件包含了模型的很多信息，一般来说是改不了的
    "Motions": {                       // 动作定义，非必须
      "Idle": [                 // 动作名字，SDK是根据动作名字判断调用哪个动作的，
                                // Idle是闲时动作，没有触发其他动作时就会播放，让你的模型看起来像活的
        {
          "File": "motions/haru_g_idle.motion3.json", // 动作文件路径，必须
          "FadeInTime": 1, // 淡入时间，单位是秒，可选，指的是动作开始播放时的淡入时间
          "FadeOutTime": 1,// 淡出时间，可选
          "Sound": "sounds/haru_normal_04.wav",  // 音频文件路径，可选，在动作播放时会播放这个音频，音频文件有多种格式，WAVE,RIFF,fmt，其他格式都不支持
          "Doc": "阁下，我今天很开心哦！" // 文本描述，可选，这个是我自己加的，官方sdk里面不支持，用于播放音频时显示字幕
        }
      ],
      "TapBody": [//除了Idle以外，其余的动作名字都要以TapAbcdef的形式命名，在触碰名字为Abcdef的部位时会自动播放对应动作
        {
          "FadeInTime": 0.5,
          "FadeOutTime": 0.5,
          "File": "motions/haru_g_m26.motion3.json",
          "Sound": "sounds/haru_normal_04.wav"
        },
        {
          "FadeInTime": 0.5,
          "FadeOutTime": 0.5,
          "File": "motions/haru_g_m06.motion3.json",
          "Sound": "sounds/haru_normal_01.wav"
        }
      ]
    },
    "Physics": "Haru.physics3.json",          // 物理模拟文件路径，非必须，用处顾名思义
    "Pose": "Haru.pose3.json",            // 姿势文件路径，不太清楚有什么用，非必须
    "Textures": [                     // 纹理贴图列表，必须
      "Haru.2048/texture_00.png",                   // 文件路径
      "Haru.2048/texture_01.png"
    ],
    "UserData": "Haru.userdata3.json"     // 不太清楚有什么用，非必须
  },
  "Groups": [                            // 不清楚有啥用，好像是必须的
    {
      "Ids": [
        "ParamEyeLOpen",
        "ParamEyeROpen"
      ],          
      "Name": "EyeBlink",             
      "Target": "Parameter"           
    }
  ],
  "HitAreas": [                         // 可交互区域定义
    {
      "Id": "HitArea",               // 区域id，必须，实际上没用，保持每个区域id不同就行
      "Name": "Head"             // 区域名字，必须，点击区域时会根据区域名字查找对应的动作
                                  // 这个区域名字是Head，点击时会查找TapHead动作，然后从动作组中抽一个播放
                                  // 如果区域名字是Chest，就会查找TapChest动作
    },
    {
      "Id": "HitArea2",
      "Name": "Body"
    }
  ],
  "Version": 3                          // 固定值，表示模型版本
}
```

## 字段说明

### FileReferences

| 字段          | 类型     | 是否必须 | 说明              |
|-------------|--------|------|-----------------|
| DisplayInfo | string | 否    | 模型显示辅助文件        |
| Expressions | array  | 否    | 表情文件列表          |
| Moc         | string | 是    | 模型核心文件(.moc3)   |
| Motions     | object | 否    | 动作文件列表，key为动作类型 |
| Physics     | string | 否    | 物理模拟文件          |
| Pose        | string | 否    | 姿势文件            |
| Textures    | array  | 是    | 贴图文件列表          |
| UserData    | string | 否    | 用户数据文件          |

### Motions 动作类型

| 类型      | 说明           |
|---------|--------------|
| Idle    | 空闲状态随机动作列表   |
| TapAbcd | 点击Abcd部位触发动作 |
| ...     | 其他自定义动作类型    |

### HitAreas 交互区域

| 字段   | 类型     | 说明                       |
|------|--------|--------------------------|
| Id   | string | 区域唯一标识符                  |
| Name | string | 区域显示名称（Head/Body/Chest等） |

## 目录结构示例

```
模型根目录/
├── Haru.cdi3.json         # 显示辅助文件，可选
├── Haru.model3.json       # 主配置文件，必须
├── Haru.moc3              # 模型核心文件，必须
├── Haru.physics3.json     # 物理配置，可选
├── Haru.pose3.json        # 姿势文件，可选
├── Haru.userdata3.json    # 用户数据文件，可选
├── expressions/           # 表情目录，可选
│   └── F01.exp3.json
├── motions/               # 动作目录，可选
│   └── haru_g_idle.motion3.json
├── Haru.2048/             # 材质目录，必须
│   └── texture_00.png
└── sounds/                # 音效目录，可选
    └── haru_normal_01.wav
```