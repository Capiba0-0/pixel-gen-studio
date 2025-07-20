#include "PGS/node_graph/nodes/mapping_node.h"

#include "PGS/core/buffers/vector_field_buffer.h"
#include "PGS/node_graph/helpers.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

PGS::NodeGraph::MappingNode::MappingNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_type", "Type", DataType::Number,
        ValueList{0, {"Point", "Texture", "Vector"}}});

    registerInputPort({"in_vector", "Vector", DataType::VectorField});
    registerInputPort({"in_location", "Location", DataType::VectorField});
    registerInputPort({"in_rotation", "Rotation", DataType::VectorField});
    registerInputPort({"in_scale", "Scale", DataType::VectorField});

    // Output
    registerOutputPort({"out_vector", "Vector", DataType::VectorField});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> 
PGS::NodeGraph::MappingNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outVector = std::make_shared<VectorFieldBuffer>(bufferSize);

    const auto typeIndex = static_cast<int>(getRequiredInput<float>(inputs, "in_type", bufferSize));
    const auto mappingType = static_cast<MappingType>(std::clamp(typeIndex, 0, 2));

    // Getting buffers
    std::shared_ptr<VectorFieldBuffer> inVector = nullptr;
    if (inputs.count("in_vector")) 
        inVector = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);
    
    std::shared_ptr<VectorFieldBuffer> inLocation = nullptr;
    if (inputs.count("in_location")) 
        inLocation = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_location", bufferSize);

    std::shared_ptr<VectorFieldBuffer> inRotation = nullptr;
    if (inputs.count("in_rotation"))
        inRotation = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_rotation", bufferSize);

    std::shared_ptr<VectorFieldBuffer> inScale = nullptr;
    if (inputs.count("in_scale"))
        inScale = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_scale", bufferSize);

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            const sf::Vector2u pos = {x, y};

            sf::Vector2f vec;
            if (inVector) {
                vec = inVector->getVector(pos);
            } else {
                vec = {
                    static_cast<float>(x) / static_cast<float>(bufferSize.x),
                    static_cast<float>(y) / static_cast<float>(bufferSize.y)
                };
            }

            const sf::Vector2f location = inLocation ? inLocation->getVector(pos) : sf::Vector2f(0.f, 0.f);
            const float rotation = inRotation ? inRotation->getVector(pos).x : 0.f;
            const sf::Vector2f scale = inScale ? inScale->getVector(pos) : sf::Vector2f(1.f, 1.f);

            sf::Vector2f transformedVec = vec;

            if (mappingType == MappingType::Point) {
                transformedVec.x *= scale.x;
                transformedVec.y *= scale.y;

                if (rotation != 0.f) {
                    const float angleRad = rotation * (M_PI / 180.0f);
                    const float cosA = std::cos(angleRad);
                    const float sinA = std::sin(angleRad);
                    const float newX = transformedVec.x * cosA - transformedVec.y * sinA;
                    const float newY = transformedVec.x * sinA + transformedVec.y * cosA;
                    transformedVec = {newX, newY};
                }

                transformedVec += location;

            } else if (mappingType == MappingType::Texture) {
                transformedVec -= location;

                if (rotation != 0.f) {
                    const float angleRad = -rotation * (M_PI / 180.0f);
                    const float cosA = std::cos(angleRad);
                    const float sinA = std::sin(angleRad);
                    const float newX = transformedVec.x * cosA - transformedVec.y * sinA;
                    const float newY = transformedVec.x * sinA + transformedVec.y * cosA;
                    transformedVec = {newX, newY};
                }
                
                transformedVec.x /= (scale.x != 0.f ? scale.x : 1.f);
                transformedVec.y /= (scale.y != 0.f ? scale.y : 1.f);
            }
            else if (mappingType == MappingType::Vector) {
                transformedVec.x *= scale.x;
                transformedVec.y *= scale.y;

                if (rotation != 0.f) {
                    const float angleRad = rotation * (M_PI / 180.0f);
                    const float cosA = std::cos(angleRad);
                    const float sinA = std::sin(angleRad);
                    const float newX = transformedVec.x * cosA - transformedVec.y * sinA;
                    const float newY = transformedVec.x * sinA + transformedVec.y * cosA;
                    transformedVec = {newX, newY};
                }
            }


            outVector->setVector(pos, transformedVec);
        }
    }

    return {{"out_vector", std::move(outVector)}};
}
