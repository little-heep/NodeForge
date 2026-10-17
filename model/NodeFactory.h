//
// Created by Micheal on 2026/5/20.
//

#ifndef NODEFORGE_NODEFACTORY_H
#define NODEFORGE_NODEFACTORY_H

#include "NodeModel.h"
#include <QJsonObject>
#include <QString>
#include <functional>
#include <unordered_map>
#include <utility>
#include <QDebug>

class NodeFactory {
public:
    using CreatorFunc = std::function<NodeModel*()>;

    // 单例模式访问入口
    static NodeFactory& instance();

    // 1. 泛型注册接口（一行搞定注册，自动绑定 new T()）
    template <typename T>
    void registerType(const QString& typeName) {
        registerCreator(typeName, []() -> NodeModel* {
            return new T();
        });
    }

    // 2. 自定义 Lambda / 函数指针注册接口
    void registerCreator(const QString& typeName, CreatorFunc creator);

    // 3. 根据类型名创建实例
    NodeModel* createNode(const QString& typeName) const;

    // 4. 根据 JSON 反序列化创建节点（向后兼容原有调用）
    static NodeModel* createNodeFromJson(const QJsonObject &nodeObj);

    // 检查某种类型是否已注册
    bool hasType(const QString& typeName) const;

private:
    NodeFactory() = default;
    ~NodeFactory() = default;
    NodeFactory(const NodeFactory&) = delete;
    NodeFactory& operator=(const NodeFactory&) = delete;

    std::unordered_map<QString, CreatorFunc> m_creators;
};

// ==========================================
// 自动注册机制（Auto-Registration Helper）
// ==========================================
template <typename T>
struct NodeAutoRegister {
    explicit NodeAutoRegister(const QString& typeName) {
        NodeFactory::instance().registerType<T>(typeName);
    }
};

/**
 * 节点自注册宏：
 * 利用 C++20 inline static，即使在头文件被多次 include，也仅在全局初始化一次。
 */
#define REGISTER_NODE(Type) \
inline static const NodeAutoRegister<Type> _auto_register_##Type(#Type);

#endif //NODEFORGE_NODEFACTORY_H