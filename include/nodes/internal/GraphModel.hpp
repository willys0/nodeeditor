#pragma once

#include "Export.hpp"

#include "PortType.hpp"
#include "Definitions.hpp"

#include <unordered_set>
#include <numeric_limits>

namespace QtNodes
{

class NODE_EDITOR_PUBLIC GraphModel
{
public:

  /// @brief Returns the full set of unique Node Ids.
  /**
   * Users are responsible for generating unique unsigned int Ids for
   * all the nodes in the graph. From an Id it should be possible to
   * trace back to the model's internal representation of the node.
   */
  virtual
  std::unordered_set<NodeId>
  allNodeIds() const;

  /// @brief Returns all connected Node Ids for given port.
  virtual
  std::unordered_set<NodeId>
  connectedNodes(NodeId const & node,
                 PortType       portType,
                 PortIndex      index) const;

  /// @brief Returns node-related data for requested NodeRole.
  /**
   * Returns: Node Caption, Node Caption Visibility,
   * Node Position etc.
   */
  virtual
  QVariant
  nodeData(NodeId nodeId, NodeRole role);

  /// @brief Returns port-related data for requested NodeRole.
  /**
   * Returns: Port Data Type, Port Data, Connection Policy, Port
   * Caption.
   */
  virtual
  QVariant
  portData(NodeId    nodeId,
           PortType  portType,
           PortIndex index,
           PortRole  role) const;
};

}
