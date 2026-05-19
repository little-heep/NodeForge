//
// Created by Micheal on 2026/5/19.
//

#ifndef NODEFORGE_CUSTOMJSNODE_H
#define NODEFORGE_CUSTOMJSNODE_H

#include "../NodeModel.h"

#include <QJSEngine>
#include <QJSValue>
#include <QDebug>
#include <algorithm>

// 用户自定义 JS 节点：要求脚本中定义 function compute(inputs) { return [...]; }
class CustomJsNode : public NodeModel {
public:
    CustomJsNode(const QString& nodeName, int inputCount, int outputCount, const QString& jsCode)
        : m_name(nodeName), m_jsCode(jsCode) {
        inputs.assign(std::max(0, inputCount), QVariant());
        outputs.assign(std::max(0, outputCount), QVariant());
    }

    QString caption() const override { return m_name; }

    void compute() override {
        QJSEngine engine;

        // 执行脚本（定义 compute 函数）
        QJSValue evalResult = engine.evaluate(m_jsCode);
        if (evalResult.isError()) {
            qWarning() << "[CustomJsNode] JS evaluate error:"
                       << evalResult.toString();
            return;
        }

        QJSValue fn = engine.globalObject().property("compute");
        if (!fn.isCallable()) {
            qWarning() << "[CustomJsNode] function compute(inputs) not found.";
            return;
        }

        // C++ inputs -> JS array
        QJSValue jsInputs = engine.newArray(static_cast<uint>(inputs.size()));
        for (int i = 0; i < static_cast<int>(inputs.size()); ++i) {
            jsInputs.setProperty(i, engine.toScriptValue(inputs[i]));
        }

        // 调用 compute(inputs)
        QJSValue result = fn.call(QJSValueList() << jsInputs);
        if (result.isError()) {
            qWarning() << "[CustomJsNode] compute call error:" << result.toString();
            return;
        }

        if (!result.isArray()) {
            qWarning() << "[CustomJsNode] compute must return an array.";
            return;
        }

        // JS result -> outputs
        for (int i = 0; i < static_cast<int>(outputs.size()); ++i) {
            outputs[i] = result.property(i).toVariant();
        }
    }

private:
    QString m_name;
    QString m_jsCode;
};


#endif //NODEFORGE_CUSTOMJSNODE_H