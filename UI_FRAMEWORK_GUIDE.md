# 医疗软件框架设计说明

## 1. 目标

本设计聚焦于一个核心目标：明确 UI 与逻辑层边界。

设计不以 UI 与逻辑完全独立运行作为前提。

设计目标如下：

1. UI 不写业务判断。
2. 逻辑不直接操作 QWidget。
3. 场景数据有统一组织方式。
4. 后续流程、模块、样式调整时，不需要推翻整体结构。

---

## 2. 设计取舍与保留对象

为保持职责边界清晰、对象层级稳定以及实现复杂度可控，框架不引入 SurfaceNode、ParameterNode、SceneSnapshot、ModuleStateStore、ModuleViewScene、UiStateSnapshot 等扩展对象。

框架保留的核心对象包括：

1. 启动与软件装配层：RedisSoftwareResolver、SoftwareInitializerFactory、BaseSoftwareInitializer、各具体软件初始化类。
2. UI 层：MainWindow、WorkspaceShell、PageManager、ApplicationCoordinator、GlobalUiManager、各业务主模块 UI、壳层附属模块 UI。
3. 契约层：ILogicGateway、UiAction、LogicNotification。
4. 逻辑层：LogicRuntime、WorkflowStateMachine、ModuleLogicRegistry、SceneGraph、NodeBase、PointNode、LineNode、ModelNode、TransformNode。
5. 通信层：RedisGateway、MessageRouter、DataSource、PollingTask、CommunicationHub。

约束说明：文档统一采用 ModuleCoordinator 表示模块级 UI 协调对象，采用 ModuleLogicHandler 表示逻辑层模块处理入口；软件类型和运行模式是两个独立维度，前者决定模块装配组合，后者决定 local 或 redis 的运行方式。

---

## 3. 总体结构

```text
Server / Algorithm Side
├─ 算法服务
├─ 业务服务
└─ Redis

Desktop Client
├─ Startup / Software Assembly Layer
│  ├─ RedisSoftwareResolver
│  ├─ SoftwareInitializerFactory
│  ├─ BaseSoftwareInitializer
│  └─ ConcreteSoftwareInitializers
│
├─ UI Layer
│  ├─ MainWindow
│  ├─ WorkspaceShell
│  ├─ PageManager
│  ├─ ApplicationCoordinator
│  ├─ GlobalUiManager
│  └─ 各业务模块 UI
│
├─ Contract Layer
│  ├─ ILogicGateway
│  ├─ UiAction
│  └─ LogicNotification
│
├─ Logic Layer
│  ├─ LogicRuntime
│  ├─ WorkflowStateMachine
│  ├─ ModuleLogicRegistry
│  │  ├─ 参数模块处理器
│  │  ├─ 选点模块处理器
│  │  ├─ 规划模块处理器
│  │  └─ 导航模块处理器
│  ├─ SceneGraph
│  └─ Node System
│     ├─ NodeBase
│     ├─ PointNode
│     ├─ LineNode
│     ├─ ModelNode
│     └─ TransformNode
│
└─ Communication Layer
   ├─ RedisGateway
   ├─ MessageRouter
   ├─ DataSource Layer
   │  ├─ SubscriptionSource
   │  └─ PollingSource / PollingTask
   └─ CommunicationHub
```

---

## 4. 边界

### 启动与软件装配层负责

1. 在完整 UI 装配前读取软件类型或软件 profile。
2. 根据软件类型选择具体的软件初始化类。
3. 固化共享启动骨架，避免每个软件重复写整段 builder。
4. 决定软件启用哪些模块、流程顺序和初始模块。

### UI 层负责

1. 页面显示。
2. 模块显示。
3. 用户操作采集。
4. 通知、遮罩、3D 工具窗管理。

### 逻辑层负责

1. 流程状态。
2. 模块状态和按钮可用状态。
3. SceneGraph 更新。
4. 给 UI 生成需要刷新的结果。

### 通信层负责

1. 提供 Redis 连接、订阅分发、遍历轮询与基础读写能力。
2. 维护控制面和数据面的通信通路。
3. 负责协议转换、数据解析、重连与健康状态观测。
4. 在启动前提供最小的软件配置读取能力，例如读取 current software key 或 software profile json。

### 服务器负责

1. 算法执行。
2. 核心业务计算。
3. 外部设备与结果输出。

## 5. UI 结构

```text
MainWindow
├─ rootStack
│  ├─ WorkspaceShell
│  │  ├─ topWidget
│  │  ├─ centerStack
│  │  │  ├─ 参数主模块
│  │  │  ├─ 选点主模块
│  │  │  ├─ 规划主模块
│  │  │  └─ 导航主模块
│  │  ├─ rightWidget
│  │  │  ├─ 壳层菜单模块
│  │  │  └─ 当前主模块可选附属 widget
│  │  └─ bottomWidget
│  │     ├─ 壳层状态条
│  │     └─ 当前主模块可选附属 widget
│  └─ 其它整页页面
├─ globalOverlayLayer
└─ globalToolHost
```

说明：

1. 软件初始化类决定主模块、附属模块、流程顺序和初始模块，并完成注册。
2. PageManager 仅负责 centerStack 中主模块的切换；附属模块可挂载到 rightWidget 或 bottomWidget 而不进入 PageManager。
3. ApplicationCoordinator 负责壳层级切换、挂载和全局 UI 服务，GlobalUiManager 负责全局通知、遮罩、确认层与 3D 工具窗。
4. 模块细节由 ModuleLogicHandler 产出 LogicNotification，经 ModuleCoordinator 分发到主页面、附属模块和 3D 窗口。
5. 单个主模块可装配多个 3D 窗口，窗口共享节点通知并依据 windowId 执行过滤和分层渲染。
6. globalOverlayLayer 和 globalToolHost 是可交互宿主层，而不是纯视觉覆盖层；若其上承载确认层、全局通知面板或工具窗，不应通过统一的 transparent-for-mouse-events 策略让其失去交互能力。是否穿透鼠标由其内部具体部件自行决定，而不是由壳层根节点一刀切配置。

### 5.1 模块形态

框架将模块划分为主模块和附属模块。主模块进入 centerStack，由 PageManager 切换；附属模块挂载到 rightWidget 或 bottomWidget，用于流程菜单、状态面板和辅助引导。

约束：

1. 模块集合由软件初始化类决定，不是全局固定集合。
2. 不同软件可以复用同一批模块实现，但 workflowSequence、initialModule、enabledModules 可以不同。
3. 主模块可带零个或多个附属 widget，也可在同一主页面内组合多个 3D 窗口。

### 5.2 多 3D 窗口装配规则

多窗口 3D 装配遵循以下约束：

1. 每个 3D 窗口必须具有稳定的 windowId，例如 planning_main、planning_overlay、navigation_main、navigation_follow。
2. 单个模块可注册多个 3D 窗口，但窗口仍归属于该模块，不形成新的业务模块。
3. scene_nodes_updated 保持模块级单条通知；节点通过 displayTargets 声明在不同 windowId 下的可见性和层级。
4. 每个 3D 窗口在初始化时必须声明相机参数，至少包括 position、focalPoint、viewUp，以及 parallelProjection 下的 parallelScale 或 perspective 下的 viewAngle；必要时可补充 clippingRange。
5. UI 装配层负责窗口创建、注册、销毁和相机初始参数装配，逻辑层负责节点显示策略，窗口侧按 windowId 完成过滤和渲染。

---

## 6. UI 和逻辑的通信方式

在 VTK + Qt 项目里，框架采用：

1. ILogicGateway 做边界。
2. Qt signal/slot 传消息。
3. 跨线程一律走 Qt::QueuedConnection。

以下方式不纳入框架边界：

1. UI 直接调用 LogicRuntime 的业务函数。
2. UI 直接读写 SceneGraph。
3. 逻辑线程直接操作 QWidget、QVTKOpenGLNativeWidget、vtkRenderWindow。

整体模式可以概括为：

命令向下，事件向上。

也就是：

1. UI 把动作作为命令发给逻辑层。
2. 逻辑层把处理结果作为事件通知回 UI。

标准调用链：

```text
Shell UI
   └─ emit shellAction(UiAction)
      └─ ApplicationCoordinator
         └─ ILogicGateway::sendAction(...)

Module UI / Shell Extension UI
   └─ emit moduleAction(UiAction)
      └─ 所属模块 ModuleCoordinator
         └─ ILogicGateway::sendAction(...)
            └─ Qt::QueuedConnection
               └─ LogicRuntime::onActionReceived(...)

LogicRuntime
   ├─ 更新 WorkflowStateMachine.currentModule
   └─ routeTo(ModuleLogicHandler)

ModuleLogicHandler
   └─ emit logicNotification(LogicNotification)
      └─ Qt::QueuedConnection
         ├─ ApplicationCoordinator::onShellNotification(...)
         └─ ModuleCoordinator::onModuleNotification(...)
```

关键约束：

1. UI 和逻辑之间不是直接函数互调，而是消息交互。
2. Qt 负责线程切换。
3. ApplicationCoordinator 只处理壳层通知，不吞掉模块细节通知。
4. VTK 渲染更新最终仍然回到 UI 线程执行。

### 6.1 UiAction

UI 发给逻辑层的动作。

例如：

1. 点击下一步。
2. 请求切换模块。
3. 请求确认选点。
4. 请求开始导航。

统一字段：

1. actionId：全局唯一动作 id。
2. actionType：动作类型，例如 next_step、confirm_points。
3. module：动作来源模块。
4. timestampMs：毫秒时间戳。
5. traceId：链路追踪 id。
6. payload：动作参数。

约束：

1. actionId 全局唯一，用于去重和重试。
2. payload 只放最小必要参数，不放大体量几何数据。
3. actionType 使用固定枚举，避免字符串随意扩散。

在 Qt 里，壳层动作由 ApplicationCoordinator 转给 ILogicGateway，模块细节动作由对应 ModuleCoordinator 转给 ILogicGateway。

### 6.2 LogicNotification

逻辑层发给 UI 的结果。

例如：

1. 当前模块变化。
2. 当前阶段变化。
3. 某按钮是否可用。
4. 顶部提示变化。
5. 3D 视图需要刷新。

统一字段：

1. notificationId：通知唯一 id。
2. sourceActionId：对应触发它的 actionId，可为空。
3. eventType：通知类型，例如 module_changed、scene_nodes_updated。
4. level：info、warning、error。
5. timestampMs：毫秒时间戳。
6. targetScope：shell、current_module、all_modules、module_list。
7. targetModules：当 targetScope=module_list 时生效。
8. payload：通知数据。

约束：

1. 任何会影响 UI 行为的结果都通过 LogicNotification 返回。
2. 逻辑层一次动作可返回多条通知，但要保持同一 traceId。
3. shell 级通知只由 ApplicationCoordinator 处理。
4. 模块级通知由对应模块 UI 处理，必要时可用 all_modules 广播给全部模块 UI。
5. 错误必须通过 level=error 明确返回，不允许静默失败。

在 Qt 中，这类结果由 ModuleLogicHandler 或 LogicRuntime 发 signal，经 Gateway 回到 UI 主线程，再由 ApplicationCoordinator 或 ModuleCoordinator 分发。

### 6.2.1 场景通知与多窗口显示策略

对于 3D 场景通知，需要再补一层统一约束：

1. scene_nodes_updated 仍然只是一条模块级通知，不额外按窗口拆消息。
2. payload.nodes 中的每个节点都可以携带 displayTargets 字段，描述该节点在不同 windowId 下的显示与层级规则。
3. UI 收到通知后，不直接假设所有节点都在当前窗口显示，而是先按当前窗口的 windowId 解析 displayTargets。
4. 这样同一批节点可以在窗口 A 可见、在窗口 B 不可见，也可以在两个窗口都可见但层级不同。

标准 displayTargets 结构：

```json
{
   "default": {
      "visible": true,
      "layer": 1
   },
   "windows": {
      "planning_main": {
         "visible": true,
         "layer": 1
      },
      "planning_overlay": {
         "visible": true,
         "layer": 3
      },
      "navigation_follow": {
         "visible": false,
         "layer": 1
      }
   }
}
```

约束：

1. default 表示未显式指定 windowId 时的默认策略。
2. windows 是按 windowId 的覆写表。
3. visible=false 表示该节点在该窗口完全不渲染。
4. layer 只描述该节点在该窗口目标 renderer 层，不负责具体 VTK actor 的创建方式。

### 6.3 ILogicGateway

UI 层只能通过它和逻辑层交互。

UI 不直接访问 SceneGraph。

UI 不直接访问 Redis。

最小接口：

1. sendAction(action: UiAction) -> bool
2. subscribeNotification(handler) -> subscriptionId
3. unsubscribeNotification(subscriptionId)
4. getConnectionState() -> Connected | Degraded | Disconnected
5. requestResync(reason)

行为约束：

1. sendAction 返回只代表“是否收到了动作”，不代表业务成功。
2. 业务成功失败统一由 LogicNotification 返回。
3. requestResync 只用于通信异常后的主动重同步。

补充：启动阶段的软件选择不应通过 ILogicGateway 完成，而应在完整 UI 装配前由软件解析层直接读取 Redis 中的软件配置，再据此决定实例化哪个软件初始化类。

### 6.4 在 VTK + Qt 下的线程规则

线程规则：

1. QWidget 相关对象只在 UI 主线程操作。
2. QVTKOpenGLNativeWidget 相关更新只在 UI 主线程操作。
3. vtkRenderer、vtkRenderWindow、Render() 调用放在 UI 主线程。
4. 流程判断、消息路由、数据整理放在逻辑线程。

也就是说：

1. LogicRuntime 只产出普通数据。
2. UI 收到 LogicNotification 后，再更新本地 VTK 对象。
3. 禁止从后台线程直接改 actor、mapper、renderer。

### 6.5 同步和异步怎么分

规则：

1. 业务动作一律异步。
2. 连接状态这类轻量信息可同步获取。
3. 任何会触发流程判断、SceneGraph 更新、Redis 通信的动作都不要阻塞 UI。

---

## 7. UI 内部对于各个模块的调用方式

UI 模块协同遵循以下规则：

1. UI 模块之间不直接互调，统一通过 UiAction 和 LogicNotification 协同。
2. ApplicationCoordinator 仅处理壳层事件与挂载，例如 module_changed、page_changed、workflow_changed、connection_state_changed，以及全局工具窗和壳层附属模块挂载。
3. ModuleCoordinator 是模块 UI 协调入口，负责接收模块动作、调用 ILogicGateway、接收模块通知，并刷新主页面、附属 widget 和模块内 3D 窗口。
4. ModuleLogicHandler 是逻辑侧模块处理入口，负责接收模块动作和 Redis 数据、更新 SceneGraph 与模块状态，并发出 LogicNotification。
5. UI 主线程内允许 ApplicationCoordinator 到 PageManager、GlobalUiManager，以及 ModuleCoordinator 到所属 UI 的直接函数调用；跨线程和跨边界交互统一走 Gateway + signal/slot。
6. 主模块可带零个或多个附属 widget；附属 widget 不决定主模块切换，只承载展示和动作入口。
7. 3D 窗口可作为模块内部组成部分存在，不直接跨模块共享；其全局登记与工具窗能力由 GlobalUiManager 管理，具体刷新由 ModuleCoordinator 接收通知后分发执行。

---

## 8. 状态和显示数据

状态与显示数据遵循三项原则：壳层显示结果由 ApplicationCoordinator 分发，模块细节显示结果由 ModuleCoordinator 分发；流程状态保留在 WorkflowStateMachine，模块临时状态保留在 ModuleCoordinator 或 ModuleLogicHandler；3D 显示数据随 LogicNotification 下发。框架不引入完整状态快照、视图场景对象或独立 UI 状态存储。

---

## 9. LogicRuntime 的职责

LogicRuntime 是本地逻辑总入口，但不是所有模块细节的最终处理者。

它负责：

1. 接收 UiAction。
2. 调用 WorkflowStateMachine 判断当前阶段和当前模块。
3. 根据当前模块把动作或 Redis 消息路由给对应 ModuleLogicHandler。
4. 维护跨模块共享的 SceneGraph。
5. 通过通信层与 Redis 交互。

它不负责：

1. 直接操作 UI 控件。
2. 直接刷新模块细节界面。
3. 直接执行服务器算法。

### 9.1 ModuleLogicHandler 的职责

ModuleLogicHandler 是逻辑层的模块处理入口。

它负责：

1. 接收属于该模块的 UiAction。
2. 接收 Redis 返回给该模块的数据。
3. 做模块级数据整理和模块级状态收口。
4. 更新本模块涉及的 SceneGraph 数据。
5. 发出模块级 LogicNotification。
6. 必要时发出 all_modules 广播通知。

它不负责：

1. 切整个工作区页面。
2. 决定全局模块切换。
3. 直接操作别的模块 QWidget。

---

## 10. WorkflowStateMachine 的职责

WorkflowStateMachine 只管流程真相。

例如：

1. 当前阶段。
2. 当前允许进入哪个模块。
3. 下一步是否允许。
4. 某个动作是否有效。

这些判断不能放在 UI 里。

约束：

1. WorkflowStateMachine 不应把标准四步流程写死在框架里，而应由软件初始化类注入 workflowSequence 和 initialModule。
2. 当 Redis 或逻辑结果表明模块变化时，WorkflowStateMachine 负责记录这个真相。
3. ApplicationCoordinator 只根据这个真相切页。
4. 具体进入该模块后的数据刷新，由对应模块处理器和 ModuleCoordinator 接管。
5. WorkflowStateMachine 除 currentModule 外，还应维护“当前允许进入的模块集合或前沿边界”，例如 enterableModules；壳层菜单、下一步按钮和直接切模块动作都只消费该真相，不自行推导。
6. 对 request_switch_module、next_step 这类动作，若目标模块当前不可进入，逻辑层必须返回明确的 shell 级通知，说明被阻止原因；不能只依赖 UI 按钮置灰来保证流程正确。

---

## 11. SceneGraph 的职责

SceneGraph 是逻辑层里的场景对象容器。

它只负责：

1. 节点增删改查。
2. 按 id 查询节点。
3. 按类型查询节点。
4. 维护引用关系。
5. 汇总节点变化事件。

它不负责：

1. 流程判断。
2. Redis 通信。
3. UI 刷新。
4. 3D 渲染。
5. SceneSnapshot 导出。

---

## 12. 为什么保留四类 Node

框架仅保留 PointNode、LineNode、ModelNode、TransformNode 四类 Node。该集合已足以覆盖主要场景，并能控制 SceneGraph 的类型复杂度；面对象并入 ModelNode，参数和状态不建模为 Node。

---

## 13. NodeBase 设计

### 字段

1. nodeId
2. nodeTagName
3. name
4. description
5. version
6. dirtyFlag
7. attributeMap
8. referenceMap
9. defaultDisplayTarget
10. windowDisplayOverrides

说明：

NodeBase 不保存具体渲染参数，只保留通用数据能力。

颜色、线宽、点径、渲染模式等放在各子节点中。

在该设计中，NodeBase 还需要保存“显示到哪个窗口、在哪一层”的通用显示策略，因为这是所有节点都需要共享的横切能力。

### 核心方法

1. getNodeId()
2. getNodeTagName()
3. setName()
4. getName()
5. setDescription()
6. getDescription()
7. setAttribute()
8. getAttribute()
9. removeAttribute()
10. addReference()
11. removeReference()
12. getReferences()
13. startBatchModify()
14. endBatchModify()
15. touchModified()
16. setDefaultDisplayTarget()
17. setWindowDisplayTarget()
18. removeWindowDisplayTarget()
19. getDisplayTargetForWindow()

### 标准事件

1. NodeModified
2. PayloadModified
3. ReferenceChanged
4. TransformChanged

### 13.0.1 节点级显示策略

节点级显示策略用于统一描述节点在不同 3D 窗口中的显示关系。

#### 字段

1. defaultDisplayTarget：默认显示目标，至少包含 visible 和 layer。
2. windowDisplayOverrides：按 windowId 覆写的显示目标表。

#### 标准结构

```json
{
   "visible": true,
   "layer": 1
}
```

#### 规则

1. visible 为 false 时，该节点在目标窗口不参与渲染。
2. layer 固定使用 1、2、3 三个整数值。
3. 逻辑层负责产生这个真相，UI 只消费，不反向发明默认层级。
4. 所有节点都必须至少有一个 defaultDisplayTarget。
5. 当某窗口没有单独覆写时，回退到 defaultDisplayTarget。

---

## 13.1 可视化统一约定

为避免每个节点随意命名，统一可视化字段和方法风格如下。

### 统一字段命名

1. visibilityFlag
2. opacityValue
3. colorRGBA
4. renderMode

### 统一方法命名

1. setVisibility()
2. isVisible()
3. setOpacity()
4. getOpacity()
5. setColor()
6. getColor()

### 通用规则

1. colorRGBA 范围统一为 0.0 到 1.0。
2. opacityValue 范围统一为 0.0 到 1.0。
3. renderMode 统一枚举字符串：surface、wireframe、points。
4. 所有可视化参数变化都触发 PayloadModified。

## 13.2 Render Layer 统一约定

为了支持点线面分层显示，3D 窗口固定采用 3 层 renderer。

### 层级定义

1. 第 1 层：基础模型层。普通 ModelNode 默认在这一层。
2. 第 2 层：中间叠加层。用于半透明辅助面、候选模型叠加、局部高亮等可选内容。
3. 第 3 层：置顶辅助层。PointNode、LineNode、TransformNode 坐标轴以及需要避免被普通模型遮挡的辅助几何默认在这一层。

### 默认规则

1. 普通模型默认 layer = 1。
2. 点、线默认 layer = 3。
3. 变换坐标轴默认 layer = 3。
4. 需要强调的辅助面、参考面、截骨面等如果希望始终可见，可以作为 ModelNode 放到 layer = 2 或 layer = 3。
5. 同一个节点在不同窗口可以处于不同 layer。

### UI 侧约束

1. 每个 3D 窗口固定创建 3 个 renderer，而不是按节点动态增减层数。
2. 三个 renderer 共用一个 camera，避免窗口内部层间视角不一致。
3. 高层 renderer 使用透明背景，不覆盖低层模型。
4. 每个窗口的默认 camera 参数在初始化时一次性设定，并作为该窗口的标准观察位姿。
5. 交互期间允许用户临时改变 camera；当窗口交互结束后，如 3 秒内没有新的交互输入，窗口应自动将共享 camera 恢复到初始化时设定的参数。
6. 只有窗口自己知道当前 windowId，因此层级解析必须在窗口侧完成；相机恢复规则同样由窗口侧负责执行。
7. “恢复到初始化相机”指恢复到窗口装配时记录的 position、focalPoint、viewUp、projection 和 clippingRange 等标准参数，而不是简单调用一次通用 ResetCamera；否则不同软件或窗口预设视角会被运行时包围盒重算破坏。

---

## 14. PointNode 设计

### 适用场景

1. 关键点。
2. 选点结果。
3. 规划点。
4. 采集点。

### PointItem 结构

1. pointId
2. label
3. position
4. selectedFlag
5. visibleFlag
6. lockedFlag
7. associatedNodeId
8. colorRGBA
9. sizeValue

### 字段

1. controlPoints
2. pointLabelFormat
3. selectedPointIndex
4. pointRole
5. parentTransformId
6. defaultPointColor
7. defaultPointSize
8. showPointLabelFlag

### 核心方法

1. addPoint()
2. insertPoint()
3. removePoint()
4. removeAllPoints()
5. getPointCount()
6. getPointByIndex()
7. getPointById()
8. setPointPosition()
9. getPointPosition()
10. setPointLabel()
11. setPointSelected()
12. setPointLocked()
13. setParentTransform()
14. getParentTransform()
15. setPointColor()
16. getPointColor()
17. setPointSize()
18. getPointSize()
19. setDefaultPointColor()
20. setDefaultPointSize()
21. setShowPointLabel()
22. isShowPointLabel()

### 可视化补充

1. 支持按点单独颜色，也支持节点级默认颜色。
2. 支持按点设置点径，用于强调关键点。
3. 标签显示开关由 PointNode 管理，不放到 UI 控件里。
4. PointNode 默认应将 defaultDisplayTarget.layer 设为 3，以避免被普通模型遮挡。

---

## 15. LineNode 设计

### 适用场景

1. 路径。
2. 轨迹。
3. 测量线。

### 字段

1. polylinePoints
2. closedFlag
3. lineRole
4. cachedLength
5. parentTransformId
6. colorRGBA
7. opacityValue
8. lineWidthValue
9. renderMode
10. dashedFlag

### 核心方法

1. setPolyline()
2. appendVertex()
3. removeVertex()
4. clearVertices()
5. getVertexCount()
6. getVertex()
7. setClosed()
8. isClosed()
9. recalculateLength()
10. getLength()
11. setParentTransform()
12. getParentTransform()
13. setColor()
14. getColor()
15. setOpacity()
16. getOpacity()
17. setLineWidth()
18. getLineWidth()
19. setRenderMode()
20. getRenderMode()
21. setDashed()
22. isDashed()

### 可视化补充

1. 规划路径可通过线宽和颜色区分主路径与候选路径。
2. 导航轨迹可通过 dashedFlag 表示预测轨迹或历史轨迹。
3. LineNode 默认应将 defaultDisplayTarget.layer 设为 3，以便轨迹和路径不被基础模型压住。

---

## 16. ModelNode 设计

### 适用场景

1. 骨模型。
2. 器械模型。
3. 植入物模型。
4. 参考面和截骨面这类需要显示的面对象。

### 字段

1. polyDataPayload
2. vertices
3. indices
4. visibilityFlag
5. opacityValue
6. colorValue
7. modelRole
8. cachedBounds
9. parentTransformId
10. renderMode
11. showEdgesFlag
12. edgeColorRGBA
13. edgeWidthValue
14. backfaceCullingFlag
15. useScalarColorFlag
16. scalarColorMapName

### 核心方法

1. setPolyData()
2. getPolyData()
3. clearPolyData()
4. setMeshData()
5. getVertices()
6. getIndices()
7. getBoundingBox()
8. setVisibility()
9. isVisible()
10. setOpacity()
11. getOpacity()
12. setColor()
13. getColor()
14. setParentTransform()
15. getParentTransform()
16. setRenderMode()
17. getRenderMode()
18. setShowEdges()
19. isShowEdges()
20. setEdgeColor()
21. getEdgeColor()
22. setEdgeWidth()
23. getEdgeWidth()
24. setBackfaceCulling()
25. isBackfaceCulling()
26. setUseScalarColor()
27. isUseScalarColor()
28. setScalarColorMap()
29. getScalarColorMap()

### 可视化补充

1. 同一个模型可切换 surface、wireframe、points 三种模式。
2. 显示边线用于术前几何检查和轮廓确认。
3. 标量着色开关用于显示曲率、误差或强度类信息。

说明：

ModelNode 必须保留 setPolyData 这种强语义接口，不能退化成只有 setAttribute 的通用容器。
普通 ModelNode 默认位于第 1 层；如果是参考面、辅助面或必须置顶显示的面对象，则通过节点显示策略覆写到第 2 或第 3 层。

---

## 17. TransformNode 设计

### 适用场景

1. 坐标系关系。
2. 配准结果。
3. 父子变换链。

### 字段

1. matrixToParent
2. inverseDirtyFlag
3. transformKind
4. sourceSpaceName
5. targetSpaceName
6. parentTransformId
7. showAxesFlag
8. axesLengthValue
9. axesColorX
10. axesColorY
11. axesColorZ

### 核心方法

1. setMatrixTransformToParent()
2. getMatrixTransformToParent()
3. getInverseMatrix()
4. multiplyBy()
5. inverse()
6. isIdentity()
7. transformPoint()
8. transformVector()
9. setTransformKind()
10. getTransformKind()
11. setParentTransform()
12. getParentTransform()
13. setShowAxes()
14. isShowAxes()
15. setAxesLength()
16. getAxesLength()
17. setAxesColorX()
18. setAxesColorY()
19. setAxesColorZ()
20. getAxesColorX()
21. getAxesColorY()
22. getAxesColorZ()

说明：

父子关系主要通过 ParentTransform 引用表达，不靠额外继承层。
坐标轴可视化默认位于第 3 层，以保证在复杂模型遮挡下仍然可被识别。

---

## 18. 3D 对接方式

3D 对接保持“共享场景、模块分发、窗口渲染”的结构：LogicRuntime 维护跨模块共享 SceneGraph，ModuleLogicHandler 整理本模块需要显示的节点数据，节点几何与可视化参数通过 LogicNotification 下发，GlobalUiManager 仅在全局 3D 工具窗场景介入。框架不单独引入 ModuleViewScene。

### 18.1 多窗口与三层 renderer

多窗口 3D 渲染遵循以下固定约束：

1. 单个模块可拥有多个 3D 窗口，每个窗口固定为三层 renderer，并共享 camera。
2. scene_nodes_updated 仅传节点数据；节点以 displayTargets 声明在不同 windowId 下的可见性和 layer。
3. 每个窗口必须在初始化阶段绑定一组相机参数，作为该窗口的标准观察位姿；该配置属于窗口装配信息，不属于节点显示策略。
4. 窗口侧按 windowId 过滤节点，并将其路由到第 1、2、3 层 renderer。
5. 用户交互可临时改变视角，但交互结束后应启动 3 秒恢复计时；若计时期间无新的交互，窗口自动恢复到预设相机参数。
6. 高频 transform 或 pose 更新场景下，窗口侧应优先复用现有 actor/mapper，仅更新 transform、可见性和必要属性；禁止对实时流中的每次 scene_nodes_updated 都执行 RemoveAllViewProps 后全量重建。
7. scene_nodes_updated 可以保持模块级全量语义，但渲染实现必须允许按 nodeId 做增量复用，否则 demo 可以工作，真实模型规模下会出现明显卡顿。

### 18.2 节点显示配置能力

节点显示配置能力用于查看和修改 displayTargets，通过 UiAction 提交，由逻辑层更新后再通过 scene_nodes_updated 回流刷新。该能力不直接操作 vtkActor，不直接修改 QWidget 内部渲染状态，也不绕过 LogicRuntime 改 SceneGraph；其承载形式可以是壳层附属区域、独立工具窗或模块内配置入口。

---

## 19. 通信层设计

通信层采用“控制面 + 数据面”的双模式结构。控制面负责可靠命令、结果通知、重同步和错误回传；数据面负责高频状态输入，并同时支持订阅分发和遍历轮询两种 Redis 数据获取方式。通信层不直接触碰 UI，只向 LogicRuntime、ModuleLogicHandler 暴露标准化输入。

### 19.0 分层结构

通信层按职责拆分为四层：

1. RedisGateway：负责 Redis 连接、pub/sub、key-value 读取、轮询读操作和重连。
2. MessageRouter：负责控制面消息信封、协议转换、版本兼容和字段校验。
3. SourceBase、SubscriptionSource、PollingSource、PollingTask：负责运行订阅分发源和遍历轮询源，并输出 StateSample。
4. CommunicationHub：负责整合控制面消息和数据面状态输入，再交给 LogicRuntime 或对应 ModuleLogicHandler。

### 19.1 启动前的软件解析

客户端在完整 UI 装配前，先执行最小通信初始化：

1. 按运行模式初始化最小 Redis 访问能力。
2. 从 Redis 读取当前软件标识或 software profile。
3. 通过 SoftwareInitializerFactory 选择具体的软件初始化类。
4. 再由该初始化类创建 MainWindow、PageManager、WorkflowStateMachine、模块处理器和各模块 UI。

约束：

1. 运行模式决定是否连接 Redis。
2. 软件类型决定装配哪套模块。
3. 这两个维度不得混在同一个枚举里。

### 19.2 控制面设计

控制面承载以下内容：

1. UiAction 请求。
2. LogicNotification 结果通知。
3. ack、heartbeat、resync_request、resync_response。
4. 连接状态与错误信息。

控制面特点：

1. 强调可靠性、幂等性、顺序性和 traceId 追踪。
2. 适用于流程推进、模块切换、确认操作、错误回传和壳层通知。
3. 不用于承载每秒几十次的高频状态采样。

### 19.3 控制面消息信封与通道

控制面消息统一使用消息信封，业务数据放在 body：

```json
{
   "msgId": "uuid",
   "msgType": "ui_action_req",
   "traceId": "uuid",
   "source": "desktop-client",
   "target": "logic-runtime|server",
   "timestampMs": 0,
   "version": "1.0",
   "needAck": true,
   "body": {}
}
```

约束：

1. msgId 必填且全局唯一。
2. version 必填，默认 1.0。
3. body 只放对应 msgType 的业务字段。

控制面通道命名约定：

1. ui.action.req：客户端动作请求。
2. logic.notify.push：逻辑结果通知。
3. logic.ack：关键消息确认。
4. logic.heartbeat：心跳。
5. logic.resync.req：客户端请求重同步。
6. logic.resync.resp：重同步响应。

控制面消息类型约定：

1. ui_action_req
2. logic_notification
3. ack
4. heartbeat
5. resync_request
6. resync_response

### 19.4 数据面设计

数据面承载高频、可覆盖、最新值优先的状态输入，例如：

1. tracking pose。
2. transform 更新。
3. 设备采样状态。
4. 高频空间位姿或传感器数值。

数据面特点：

1. 同时支持订阅分发和遍历轮询两种获取方式。
2. 优先保证低延迟和最新值可达，不采用控制面的逐条 ack 模型。
3. 不要求每个样本都形成完整 LogicNotification。
4. 输入先转换为 StateSample，再由 ModuleLogicHandler 决定是否更新 SceneGraph 或向 UI 派发通知。

### 19.5 订阅分发与遍历轮询

两种数据获取方式并存时，通信层遵循以下规则：

1. 订阅分发适用于服务端主动推送的事件、状态变化和低延迟广播。
2. 遍历轮询适用于服务端仅提供 key、hash、stream 或共享缓存，客户端需要主动读取的高频数据。
3. 两种模式都必须输出统一的 StateSample，而不是把原始 Redis 数据直接抛给 UI。
4. 数据来源差异由 SourceBase、SubscriptionSource 和 PollingSource 吸收，ModuleCoordinator 和 UI 不感知“来自订阅还是轮询”。

### 19.6 PollingTask、SourceBase 与 StateSample

遍历轮询由 PollingTask 驱动，至少包含以下字段：

1. taskId。
2. module。
3. redisKey 或 keyPattern。
4. pollIntervalMs。
5. parser。
6. mergeStrategy。
7. changeDetection。
8. priority。
9. maxDispatchRateHz。

datasource 目录至少提供以下标准对象：

1. SourceBase：定义统一的数据源生命周期、回调和错误处理约束。
2. SubscriptionSource：基于 Redis pub/sub 被动接收数据。
3. PollingSource：基于 Redis 主动读取接口执行周期轮询。
4. PollingTask：描述轮询目标、频率、解析和合并策略。
5. StateSample：定义标准化状态样本，例如 TransformStateSample、DeviceStateSample 或 SceneDelta。

约束：

1. RedisGateway 负责底层访问，不在其监听循环中混入业务轮询调度。
2. PollingSource 运行在独立轮询线程或独立调度器中，不与 pub/sub 监听线程混合。
3. SourceBase、SubscriptionSource 和 PollingSource 最终都只输出 StateSample，不直接输出原始 Redis 数据。
4. SourceBase 除样本回调外，还必须提供统一的错误回调；CommunicationHub 负责接入该错误通路并向上游逻辑层转发，不能让数据源错误停留在 datasource 内部静默丢失。

### 19.7 更新合并、节流与背压

高频状态进入逻辑层前，必须先经过合并与节流：

1. 同一实体的连续样本默认采用 latest-wins 策略。
2. 当旧样本尚未消费完成时，新样本覆盖旧样本，而不是累积排队。
3. 可按位置、姿态或数值阈值执行变化检测；变化低于阈值时不继续派发。
4. 可按时间窗或 maxDispatchRateHz 限制向 LogicRuntime 和 UI 的派发频率。
5. 高频 transform 更新应优先更新模块内部状态或 TransformNode，再按需触发 scene_nodes_updated。

### 19.8 模块级路由与状态适配

控制面和数据面进入逻辑层后，处理方式不同：

1. 控制面消息由 MessageRouter 解析后进入 LogicRuntime，再按模块路由给对应 ModuleLogicHandler。
2. StateSample 由 CommunicationHub 直接交给对应 ModuleLogicHandler 或共享实时状态缓存。
3. ModuleLogicHandler 负责把高频状态样本转换为模块内部状态、SceneGraph 更新或轻量通知。
4. ApplicationCoordinator 只接收壳层级结果，不直接处理数据面原始样本。
5. 由数据面适配器派生出的 synthetic scene_nodes_updated 只能作为下游刷新输出，不能再次回写为基线场景输入；此类通知必须带有明确 sourceTag 或 traceId，并在上游缓存更新路径中被过滤，以避免形成同步自调用链或反馈环。
6. 当 CommunicationHub 收到数据源错误时，应将其转换为标准化上行结果，例如 shell 级 error 通知或等价的逻辑错误输入，再由 ApplicationCoordinator 和 GlobalUiManager 统一向用户呈现；UI 不应自行订阅底层 datasource 错误。

### 19.8.1 Demo 阶段适配约束

在完整的 SourceBase、PollingSource、CommunicationHub 尚未全部落地前，demo 可以存在模块专用的数据面适配器，但必须满足以下约束：

1. 适配器只负责 latest-wins 合并、限频、阈值过滤和标准化输出，不承担模块准入、阶段推进或其他业务真相判断。
2. 适配器输出必须保持为可替换的过渡层，不得让 UI 直接读 Redis 后发明业务状态，也不得形成“客户端一套真相、服务端一套真相”的长期结构。
3. demo 阶段的高频轮询默认应限制在可接受的演示频率，例如 30Hz 左右；若需要更高频率，必须先证明渲染链路采用了增量更新而非全量重建。
4. 适配器产生的 synthetic 通知必须与服务端基线通知严格区分，并保证未来替换为 CommunicationHub 输出时，不影响上层 ModuleCoordinator 和 UI 的消费方式。

### 19.9 可靠性策略

控制面和数据面的可靠性策略分开定义：

1. 控制面关键消息需要 ack 和重试，非关键消息允许被后续消息覆盖。
2. 控制面关键消息示例：confirm_points、start_navigation、module_changed、error 通知。
3. 数据面默认不做逐条 ack，只要求最新值及时可达。
4. 控制面重试必须使用同一个 msgId，保证幂等。
5. LogicRuntime 对控制面消息维护去重窗口；数据面则维护样本合并和最新值缓存。

控制面默认值：

1. ackTimeoutMs = 1500
2. maxRetry = 3
3. retryBackoffMs = 300, 800, 1500

### 19.10 顺序、重连与重同步

顺序规则：

1. 同一模块的控制面动作按 seq 串行处理。
2. 不同模块的控制面动作允许并行，但共享资源访问必须加锁。
3. 数据面样本不保证逐条保序，保证“最新有效样本最终可见”。

重连后流程：

1. RedisGateway 进入 reconnecting 状态。
2. 连接恢复后自动恢复订阅，并重新启动轮询任务。
3. 客户端自动发送 resync_request。
4. LogicRuntime 先恢复当前模块和全局流程状态。
5. 对应 ModuleLogicHandler 再恢复模块最小必要状态和实时缓存。
6. UI 分别刷新壳层区域和对应模块页面、3D 窗口。

约束：

1. 重同步过程不依赖 SceneSnapshot。
2. 重同步内容由 LogicRuntime 直接拼装并下发。
3. 高频状态恢复优先重建最新值，不要求回放全部历史样本。

### 19.11 健康状态与错误上报

通信层需要同时观测控制面和数据面健康状态。

最少应观测以下指标：

1. Redis 连接状态。
2. 订阅源最近成功时间。
3. 轮询任务最近成功时间。
4. 轮询延迟与样本丢弃数。
5. 解析失败数与字段校验失败数。
6. 数据源错误是否已经被 CommunicationHub 成功上送到逻辑层与壳层。

错误码分三类：

1. COMM_xxx：通信错误。
2. LOGIC_xxx：业务逻辑错误。
3. DATA_xxx：数据校验错误。

LogicNotification 的 error 负载包含：

1. errorCode
2. message
3. recoverable
4. suggestedAction
5. 可选的 channel、source 或 subsystem 字段，用于标明错误来自 RedisGateway、SourceBase、PollingSource 或其他通信子层。

补充约束：

1. shell 级 error 或 critical 通知不能只存在于日志；ApplicationCoordinator 必须负责把这类通知交给 GlobalUiManager 做统一展示。
2. 这样壳层错误呈现路径保持唯一：逻辑层产生真相，壳层协调器负责呈现，避免各模块 UI 分别发明自己的全局错误提示机制。

---

## 20. 一个完整流程

以“选点完成，进入规划”为例。

1. 选点底部流程菜单模块发出 UiAction。
2. 选点模块的 ModuleCoordinator 调用 ILogicGateway。
3. LogicRuntime 调用 WorkflowStateMachine 判断是否允许进入下一步，并确认当前主模块即将切到规划模块。
4. LogicRuntime 把模块切换结果通知 ApplicationCoordinator。
5. ApplicationCoordinator 调用 PageManager 切到规划主模块，并清理上一个模块的 bottomWidget 挂载。
6. 规划模块的 ModuleLogicHandler 接收 Redis 或本地返回结果，更新 PointNode、ModelNode、TransformNode。
7. 规划模块的 ModuleCoordinator 接收 LogicNotification 并分发刷新。
8. 规划主页面和需要感知该变化的附属模块 UI 收到通知后各自刷新。

该流程始终保持三项边界：UI 不做业务判断，SceneGraph 不直接驱动 UI，Redis 不直接控制 QWidget。

## 21. 目录结构

第 21 章给出的是建议落位，用于表达职责边界；小型项目可合并文件，但不应打乱控制面、数据面和 Redis 访问层的职责分隔。

```text
src/
├─ app/
│  ├─ software/
│  │  ├─ software_resolver.py
│  │  ├─ software_initializer.py
│  │  ├─ configured_software_initializer.py
│  │  └─ concrete_initializers.py
│  ├─ application_context.py
│  ├─ application_builder.py
│  └─ main.py
├─ shell/
├─ ui/
│  ├─ coordination/
│  ├─ globalui/
│  ├─ pages/
│  └─ modules/
├─ logic/
│  ├─ runtime/
│  ├─ workflow/
│  ├─ scene/
│  │  └─ nodes/
│  └─ gateway/
├─ communication/
│  ├─ hub/
│  │  └─ communication_hub.py
│  ├─ datasource/
│  │  ├─ source_base.py
│  │  ├─ subscription_source.py
│  │  ├─ polling_source.py
│  │  ├─ polling_task.py
│  │  └─ state_samples.py
│  ├─ redis/
│  │  └─ redis_gateway.py
│  └─ routing/
│     └─ message_router.py
├─ contracts/
├─ config/
├─ resources/
└─ tests/
```

---

## 22. 落地顺序

1. 先搭最小启动解析层，明确 Redis 中的软件标识或 software profile 协议。
2. 再抽 BaseSoftwareInitializer，把共享启动骨架固化下来。
3. 再把 WorkflowStateMachine 改成接受软件注入的 workflowSequence 和 initialModule。
4. 再为各模块实现可复用的装配件，避免每个软件重复写注册逻辑。
5. 再落具体软件初始化类，例如不同业务软件的具体装配实现。
6. 再完善 MainWindow、WorkspaceShell、PageManager、GlobalUiManager 与模块 UI 的装配细节。
7. 最后再让服务端按同一软件类型对齐，保持客户端与服务端一致。

---

## 23. 关键设计决定

1. 框架以边界稳定和装配可变为优先，不以 UI 与逻辑完全独立运行为前提。
2. 对象集合保持最小，仅保留 PointNode、LineNode、ModelNode、TransformNode 四类场景节点，不引入 SurfaceNode、ParameterNode、SceneSnapshot、ModuleStateStore、ModuleViewScene、UiStateSnapshot、UiStateStore。
3. 启动阶段先解析软件类型，再由 BaseSoftwareInitializer、SoftwareInitializerFactory 和具体初始化类完成模块组合、流程顺序与初始模块装配。
4. UI 与逻辑边界统一通过 ILogicGateway、UiAction、LogicNotification 建立；UI 侧协调入口为 ModuleCoordinator，逻辑侧模块处理入口为 ModuleLogicHandler。
5. WorkflowStateMachine 维护流程真相，ApplicationCoordinator 处理壳层事件，模块细节由 ModuleCoordinator 与 ModuleLogicHandler 收口。
6. 3D 采用共享 SceneGraph、模块级节点通知、窗口侧按 windowId 过滤并使用固定三层 renderer 的方案。
7. 通信层采用控制面与数据面分离的双模式结构；控制面承载可靠命令与通知，数据面承载高频状态输入。
8. Redis 数据获取同时支持订阅分发与遍历轮询；两种来源先统一转换为标准化状态样本，再进入 ModuleLogicHandler，不直接暴露给 UI。
9. 高频 transform、pose 与设备状态采用 latest-wins、节流和阈值过滤策略，不按逐条完整通知回流 UI。
10. 数据面派生出的 synthetic 通知必须与服务端基线通知分离，并在缓存回灌路径中显式去环，避免 synthetic scene_nodes_updated 被再次当作基线输入处理。
11. WorkflowStateMachine 必须同时负责模块准入真相，而不仅是 currentModule 记录；壳层导航状态和 direct switch 行为都必须受该真相约束。
12. demo 的实时 3D 渲染必须遵守“逻辑层产出数据、UI 侧增量消费”的原则；在高频流场景下优先复用 actor，而不是把 scene_nodes_updated 实现成每帧全量重建渲染对象。
13. 壳层中的 globalOverlayLayer 与 globalToolHost 默认按“可承载交互组件”设计；不要把其根层永久配置为鼠标穿透，否则确认层、全局面板和工具窗会在结构上失去交互能力。
14. CommunicationHub 不能只负责样本合流，还必须承担 datasource 错误上送职责；否则通信层会形成“数据成功路径可见、错误路径不可见”的架构缺口。
15. shell 级错误呈现必须经由 ApplicationCoordinator + GlobalUiManager 统一收口，不能依赖某个业务模块页面偶然感知并展示。
16. 多窗口 3D 中的“恢复标准视角”必须恢复到初始化记录参数，而不是退化为通用 ResetCamera 语义。

以上构成该框架的最小可用设计方案。
