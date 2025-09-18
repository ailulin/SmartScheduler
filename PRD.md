# 📄 智能日程规划助手（Smart Schedule Planning Assistant）  
**版本：V1.0**  
**编写人：产品经理（ChatGPT）**  
**目标用户：大学生、自由职业者、项目团队**  
**技术栈：C++ 后端 + MySQL 数据库 + JavaScript 前端（FullCalendar）**

---

## 1. 产品概述
智能日程规划助手是一个基于优先级和可用时间的智能推荐系统，能够帮助用户自动安排任务到合理的时间段。  
系统由 **数据库层**、**后端逻辑层** 和 **前端展示层** 组成，支持基础任务管理和自动化推荐。  

---

## 2. 产品目标
1. 帮助用户快速录入任务，减少手动排程时间。  
2. 提供基于 **优先队列 + 贪心算法** 的智能调度推荐。  
3. 直观的日历 UI（FullCalendar）展示结果，提升用户体验。  
4. 提供可扩展架构，后续支持更多高级功能（提醒、协作、移动端）。  

---

## 3. 功能需求  

### 3.1 用户管理  
- 用户注册/登录（本地账户即可，V1 不需要 OAuth）。  
- 用户可以查看自己的任务和日程。  

### 3.2 任务管理  
- 添加任务（标题、描述、优先级、预计时长、截止时间、任务类型）。  
- 查询任务列表。  
- 删除任务、修改任务信息。  

### 3.3 智能推荐（日程生成）  
- 输入：任务列表（来自数据库）。  
- 处理逻辑：  
  - 按截止日期升序排序；  
  - 按优先级用 **最小堆（priority queue）** 排序；  
  - 贪心算法分配任务到时间片；  
  - 保存到 **Schedule 表**。  
- 输出：推荐的日程表（任务 → 时间段）。  

### 3.4 日历展示  
- 前端（FullCalendar）调用后端接口 `/schedule`。  
- 显示每日/每周任务安排。  

---

## 4. 技术需求  

### 4.1 数据库设计（MySQL）  
**User 表**  
- id (PK, AI)  
- username  
- password  

**Task 表**  
- id (PK, AI)  
- user_id (FK → User.id)  
- name  
- description  
- priority (int)  
- duration (int, 分钟)  
- deadline (datetime)  
- type (enum: study, work, personal)  

**Schedule 表**  
- id (PK, AI)  
- user_id (FK → User.id)  
- task_id (FK → Task.id)  
- start_time (datetime)  
- end_time (datetime)  

---

### 4.2 后端（C++）  
- 使用 **MySQL Connector/C++** 与数据库交互。  
- 采用模块化结构：  
  - `db.h / db.cpp`：数据库操作  
  - `algo.h / algo.cpp`：调度算法  
  - `models.h`：数据结构定义  
  - `main.cpp`：程序入口  

- 提供以下 API：  
  - `getTasks()` → 返回任务列表  
  - `addTask(task)` → 插入新任务  
  - `generateSchedule()` → 返回推荐日程（并存入数据库）  

---

### 4.3 前端（JavaScript + FullCalendar）  
- 调用后端 API `/tasks` 和 `/schedule`。  
- 显示任务列表和推荐日程。  
- 支持基本交互（增删改任务）。  

---

## 5. 非功能需求  
- **性能**：支持至少 1000 条任务数据，推荐生成时间 < 1 秒。  
- **安全**：密码加密存储（可用 MD5/SHA256）。  
- **可扩展性**：后续支持多人协作、任务提醒、移动端适配。  

---

## 6. 项目里程碑  
### 阶段一：数据库设计（已完成 ✅）  
- 使用 MySQL Workbench 建库建表。  
- 验证外键约束。  

### 阶段二：后端开发（进行中 🚀）  
- 搭建 C++ 项目结构（db.cpp + algo.cpp）。  
- 实现任务 CRUD。  
- 实现推荐算法（优先队列 + 贪心）。  

### 阶段三：前端接入  
- 使用 FullCalendar 调用 API。  
- 显示推荐日程。  

### 阶段四：测试与优化  
- 单元测试：任务增删改查。  
- 集成测试：生成日程是否合理。  

---

## 7. 成果展示（Demo 流程）  
1. 用户在界面输入任务（标题、截止时间、优先级）。  
2. C++ 后端接收任务 → 存入 MySQL。  
3. 用户点击「生成日程」。  
4. C++ 后端调用算法 → 将推荐结果写入 Schedule 表。  
5. 前端日历（FullCalendar）显示完整的日程表。  
