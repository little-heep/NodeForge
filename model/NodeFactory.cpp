#include "NodeFactory.h"
#include "nodes/NumberNode.h"
#include "nodes/StringNode.h"
#include "nodes/AddNode.h"
#include "nodes/SubNode.h"
#include "nodes/MulNode.h"
#include "nodes/DivNode.h"
#include "nodes/StringAddNode.h"
#include "nodes/CustomJsNode.h"
#include <QDebug>

NodeModel* NodeFactory::createNodeFromJson(const QJsonObject &nodeObj) {
    if (!nodeObj.contains("type")) return nullptr;
    QString type = nodeObj["type"].toString();
    QJsonObject data = nodeObj.contains("data") && nodeObj["data"].isObject() ? nodeObj["data"].toObject() : QJsonObject();

    NodeModel* m = nullptr;

    if (type == "NumberNode") {
        double v = 0.0;
        if (data.contains("value")) v = data["value"].toDouble();
        m = new NumberNode(v);
    } else if (type == "StringNode") {
        QString s;
        if (data.contains("value")) s = data["value"].toString();
        m = new StringNode(s);
    } else if (type == "AddNode") {
        m = new AddNode();
    } else if (type == "SubNode") {
        m = new SubNode();
    } else if (type == "MulNode") {
        m = new MulNode();
    } else if (type == "DivNode") {
        m = new DivNode();
    } else if (type == "StringAddNode") {
        m = new StringAddNode();
    } else if (type == "CustomJsNode") {
        QString name = data.contains("name") ? data["name"].toString() : QString("Custom");
        int inC = data.contains("inputCount") ? data["inputCount"].toInt() : 0;
        int outC = data.contains("outputCount") ? data["outputCount"].toInt() : 1;
        QString js = data.contains("jsCode") ? data["jsCode"].toString() : QString();
        m = new CustomJsNode(name, inC, outC, js);
    } else {
        qWarning() << "Unknown node type in NodeFactory:" << type;
        return nullptr;
    }

    // set id if present
    if (nodeObj.contains("id")) {
        int id = nodeObj["id"].toInt();
        m->setId(id);
    }

    // let model deserialize any extra fields (fromJson expects the inner 'data' object)
    // Note: for CustomJsNode we already passed js and sizes to ctor, but fromJson will update if needed
    m->fromJson(data);

    return m;
}
