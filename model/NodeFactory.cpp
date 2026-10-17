#include "NodeFactory.h"
#include <QDebug>

NodeFactory& NodeFactory::instance() {
    static NodeFactory s_instance;
    return s_instance;
}

void NodeFactory::registerCreator(const QString& typeName, CreatorFunc creator) {
    if (m_creators.find(typeName) != m_creators.end()) {
        return; // 已存在则忽略，保证幂等性
    }
    m_creators[typeName] = std::move(creator);
}

bool NodeFactory::hasType(const QString& typeName) const {
    return m_creators.find(typeName) != m_creators.end();
}

NodeModel* NodeFactory::createNode(const QString& typeName) const {
    auto it = m_creators.find(typeName);
    if (it != m_creators.end()) {
        return it->second();
    }
    qWarning() << "[NodeFactory] 未注册的节点类型:" << typeName;
    return nullptr;
}

NodeModel* NodeFactory::createNodeFromJson(const QJsonObject &nodeObj) {
    if (!nodeObj.contains("type")) return nullptr;
    QString type = nodeObj["type"].toString();

    // 1. 创建实例
    NodeModel* m = instance().createNode(type);
    if (!m) return nullptr;

    // 2. 还原通用 ID
    if (nodeObj.contains("id")) {
        m->setId(nodeObj["id"].toInt());
    }

    // 3. 让节点多态地反序列化自身的业务字段
    if (nodeObj.contains("data") && nodeObj["data"].isObject()) {
        m->fromJson(nodeObj["data"].toObject());
    }

    return m;
}