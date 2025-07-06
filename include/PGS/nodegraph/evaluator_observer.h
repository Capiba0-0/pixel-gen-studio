#pragma once

#include "PGS/nodegraph/types.h"

namespace PGS::NodeGraph
{
class Node;

class EvaluatorObserver
{
public:
    virtual ~EvaluatorObserver() = default;
    virtual void onNodeAdded(NodeID id, const Node& node) = 0;
    virtual void onNodeRemoved(NodeID id) = 0;
    virtual void onConnectionAdded(const Connection& connection) = 0;
    virtual void onConnectionRemoved(const Connection& connection) = 0;
};

} // namespace PGS::NodeGraph
