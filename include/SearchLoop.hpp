#pragma once

#include <cstddef>
#include <optional>
#include <utility>

template <typename Node>
struct CommonSearchResult {
  std::optional<Node> finalNode;
  std::size_t visitedStates = 0;

  bool solved() const {
    return finalNode.has_value();
  }
};

template <typename Structure, typename Evaluate, typename GenerateSuccessors>
CommonSearchResult<typename Structure::NodeType>
executeSearch(typename Structure::NodeType initialState,
              Structure &structure,
              Evaluate evaluate,
              GenerateSuccessors generateSuccessors) {
  using Node = typename Structure::NodeType;

  CommonSearchResult<Node> result;
  structure.add(std::move(initialState));

  while (!structure.empty()) {
    Node current = structure.removeNext();
    ++result.visitedStates;

    if (evaluate(current)) {
      result.finalNode = std::move(current);
      return result;
    }

    for (Node successor : generateSuccessors(current)) {
      structure.add(std::move(successor));
    }
  }

  return result;
}