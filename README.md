## 终极目标是设计一个轻量的,高并发,高性能的OLTP内存数据库.
> *可以用于缓存或嵌入到程序中作为sql接口的内存池，或独立部署的实时的事务处理数据库*

 目前初步实现了：
*   基于共享内存的存储模块
*   数据在磁盘上的持久化
*   并发安全的读写
*   一个简单的元数据管理
*   一个简单的序列实现
*   HASH 索引，用于O(1)需求的查询
*   跳表索引，用于快速范围查询
*   基于无锁队列的redo日志和内存上的undo机制
*   表与索引的 MVCC 机制
*   sql 语法解析器，目前只支持最基本sql的语法
*   一个简单的sql逻辑优化器，支持几种基本sql的改写
*   一个简单的sql物理执行器（在研，暂无物理优化器）
*   常用工具类:无锁队列、基于无锁队列的前继依赖执行器、基于无锁队列的线程池、基于无锁队列的内存池、文件操作、日志操作、cas锁。
*   目前按照实现一个库的思路，在一点一点实现各库，还未将各模块串起来。
*   本人会用业余时间会一点一点推进。
