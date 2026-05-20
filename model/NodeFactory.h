//
// Created by Micheal on 2026/5/20.
//

#ifndef NODEFORGE_NODEFACTORY_H
#define NODEFORGE_NODEFACTORY_H

#include <QJsonObject>

class NodeModel;

namespace NodeFactory {
    // create a NodeModel* from a JSON object that contains at least "type" and optional "data"
    // Returns nullptr on unknown type
    NodeModel* createNodeFromJson(const QJsonObject &nodeObj);
}

#endif //NODEFORGE_NODEFACTORY_H