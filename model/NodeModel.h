//
// Created by Micheal on 2026/5/1.
//

#ifndef NODEFORGE_NODEMODEL_H
#define NODEFORGE_NODEMODEL_H

#include <vector>
#include <QVariant>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

class QWidget;

class NodeModel {
public:
    NodeModel(): m_id(0) {}
    virtual ~NodeModel() = default;

    // 基本接口
    virtual QString caption() const = 0; // 节点显示名
    virtual void compute() = 0;         // 计算逻辑

    // 可编辑接口（默认不可编辑）
    virtual bool isEditable() const { return false; }
    virtual bool editValue(QWidget* parent) { Q_UNUSED(parent); return false; }

    // 标识与类型（用于序列化/反序列化）
    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    // 派生类需返回类型名（用于反序列化工厂）
    virtual QString typeName() const = 0;

    // JSON 序列化（派生类应 override 扩展 data）
    virtual QJsonObject toJson() const {
        QJsonObject obj;
        // 默认序列化 inputs 和 outputs（如果需要更复杂，请override）
        QJsonArray inArr;
        for (const auto &v : inputs) {
            // 这里只处理能转为基本 JSON 的 QVariant（数值/字符串）
            if (v.canConvert<int>()) inArr.append(v.toInt());
            else if (v.canConvert<double>()) inArr.append(v.toDouble());
            else if (v.canConvert<QString>()) inArr.append(v.toString());
            else inArr.append(QString()); // 占位
        }
        QJsonArray outArr;
        for (const auto &v : outputs) {
            if (v.canConvert<int>()) outArr.append(v.toInt());
            else if (v.canConvert<double>()) outArr.append(v.toDouble());
            else if (v.canConvert<QString>()) outArr.append(v.toString());
            else outArr.append(QString());
        }
        obj["inputs"] = inArr;
        obj["outputs"] = outArr;
        return obj;
    }

    // JSON 反序列化（派生类可 override）
    virtual void fromJson(const QJsonObject &obj) {
        inputs.clear();
        outputs.clear();
        if (obj.contains("inputs") && obj["inputs"].isArray()) {
            QJsonArray arr = obj["inputs"].toArray();
            for (auto v : arr) {
                if (v.isDouble()) inputs.push_back(QVariant(v.toDouble()));
                else if (v.isString()) inputs.push_back(QVariant(v.toString()));
                else inputs.push_back(QVariant());
            }
        }
        if (obj.contains("outputs") && obj["outputs"].isArray()) {
            QJsonArray arr = obj["outputs"].toArray();
            for (auto v : arr) {
                if (v.isDouble()) outputs.push_back(QVariant(v.toDouble()));
                else if (v.isString()) outputs.push_back(QVariant(v.toString()));
                else outputs.push_back(QVariant());
            }
        }
    }

    // 数据接口：输入端和输出端的数据存储
    std::vector<QVariant> inputs;
    std::vector<QVariant> outputs;

private:
    int m_id;
};

#endif //NODEFORGE_NODEMODEL_H