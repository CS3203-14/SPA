// TODO remove debug print statements and iostream
#include "program_knowledge_base/pkb.h"
#include <iostream>
#include <stack>

PKB::PKB(const std::shared_ptr<ProcedureNode> proc) {
  ast = proc;
  setLineNumbers(ast);
  setFollowsRelations(ast);
  setParentRelations(ast);
  setUsesRelations(ast);
  setModifiesRelations(ast);
}

PKB::~PKB() {}

void PKB::setLineNumbers(const std::shared_ptr<ProcedureNode> node) {
  setLineNumbersIterator(node->StmtList->StmtList);
}

void PKB::setLineNumbers(const std::shared_ptr<IfNode> node) {
  lines.push_back(node);
  setLineNumbersIterator(node->StmtListThen->StmtList);
  setLineNumbersIterator(node->StmtListElse->StmtList);
}

void PKB::setLineNumbers(const std::shared_ptr<WhileNode> node) {
  lines.push_back(node);
  setLineNumbersIterator(node->StmtList->StmtList);
}

void PKB::setLineNumbers(const std::shared_ptr<ReadNode> node) {
  lines.push_back(node);
}

void PKB::setLineNumbers(const std::shared_ptr<PrintNode> node) {
  lines.push_back(node);
}

void PKB::setLineNumbers(const std::shared_ptr<AssignNode> node) {
  lines.push_back(node);
}

// TODO add return value
void PKB::setLineNumbersIterator(
    const std::vector<std::shared_ptr<Node>> stmt_lst) {
  // iterate through AST via DFS

  for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
    if (dynamic_cast<ProcedureNode *>((*it).get()) != 0) {
      std::shared_ptr<ProcedureNode> derived =
          std::dynamic_pointer_cast<ProcedureNode>(*it);
      setLineNumbers(derived);
    } else if (dynamic_cast<IfNode *>((*it).get()) != 0) {
      std::shared_ptr<IfNode> derived = std::dynamic_pointer_cast<IfNode>(*it);
      setLineNumbers(derived);
    } else if (dynamic_cast<WhileNode *>((*it).get()) != 0) {
      std::shared_ptr<WhileNode> derived =
          std::dynamic_pointer_cast<WhileNode>(*it);
      setLineNumbers(derived);
    } else if (dynamic_cast<ReadNode *>((*it).get()) != 0) {
      std::shared_ptr<ReadNode> derived =
          std::dynamic_pointer_cast<ReadNode>(*it);
      setLineNumbers(derived);
    } else if (dynamic_cast<PrintNode *>((*it).get()) != 0) {
      std::shared_ptr<PrintNode> derived =
          std::dynamic_pointer_cast<PrintNode>(*it);
      setLineNumbers(derived);
    } else if (dynamic_cast<AssignNode *>((*it).get()) != 0) {
      std::shared_ptr<AssignNode> derived =
          std::dynamic_pointer_cast<AssignNode>(*it);
      setLineNumbers(derived);
    }
  }
}

// TODO error handling
// TODO refactor to use hashmap instead
// would need to implement hash function of the node
// as well as a hash table
// return line number
// returns empty string if node does not exist
std::string PKB::getLineNumberFromNode(
    const std::vector<std::shared_ptr<Node>> ls,
    const std::shared_ptr<Node> node) {
  // loop through vector of nodes
  for (std::size_t i = 0; i < ls.size(); i++) {
    if (ls[i] == node) {
      return std::to_string(i + 1);  // +1 due to 0 based index
    }
  }
  return "";  // TODO error handling
}

std::shared_ptr<Node> PKB::getNodeFromLineNumber(
    const std::vector<std::shared_ptr<Node>> ls, const int line_number) {
  return ls.at(line_number - 1);  // -1 due to 0 based index
}

// TODO set a return value
// TODO error handling
void PKB::setFollowsRelations(const std::shared_ptr<ProcedureNode> node) {
  setFollowsRelationsIterator(node->StmtList->StmtList);
}

void PKB::setFollowsRelations(const std::shared_ptr<IfNode> node) {
  setFollowsRelationsIterator(node->StmtListThen->StmtList);
  setFollowsRelationsIterator(node->StmtListElse->StmtList);
}

void PKB::setFollowsRelations(const std::shared_ptr<WhileNode> node) {
  setFollowsRelationsIterator(node->StmtList->StmtList);
}

void PKB::setFollowsRelationsIterator(
    const std::vector<std::shared_ptr<Node>> stmt_lst) {
  // add relations
  for (std::size_t i = 0; i < stmt_lst.size() - 1; i++) {
    auto cur_line_number = getLineNumberFromNode(lines, stmt_lst[i]);
    auto next_line_number = getLineNumberFromNode(lines, stmt_lst[i + 1]);
    follows_set.insert(
        std::pair<LineNumber, LineNumber>(cur_line_number, next_line_number));
    addToVectorMap(follows_map, cur_line_number, next_line_number);
    // DEBUG
    // std::cout << cur_line_number;
    // std::cout << " is followed by ";
    // std::cout << next_line_number << std::endl;
  }

  // iterate through AST via DFS
  for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
    if (dynamic_cast<ProcedureNode *>((*it).get()) != 0) {
      std::shared_ptr<ProcedureNode> derived =
          std::dynamic_pointer_cast<ProcedureNode>(*it);
      setFollowsRelations(derived);
    } else if (dynamic_cast<IfNode *>((*it).get()) != 0) {
      std::shared_ptr<IfNode> derived = std::dynamic_pointer_cast<IfNode>(*it);
      setFollowsRelations(derived);
    } else if (dynamic_cast<WhileNode *>((*it).get()) != 0) {
      std::shared_ptr<WhileNode> derived =
          std::dynamic_pointer_cast<WhileNode>(*it);
      setFollowsRelations(derived);
    }
  }
}

void PKB::setParentRelations(const std::shared_ptr<ProcedureNode> node) {
  setParentRelationsIterator(node->StmtList->StmtList, node);
}

void PKB::setParentRelations(const std::shared_ptr<IfNode> node) {
  setParentRelationsIterator(node->StmtListThen->StmtList, node);
  setParentRelationsIterator(node->StmtListElse->StmtList, node);
}

void PKB::setParentRelations(const std::shared_ptr<WhileNode> node) {
  setParentRelationsIterator(node->StmtList->StmtList, node);
}

void PKB::setParentRelationsIterator(
    const std::vector<std::shared_ptr<Node>> stmt_lst,
    const std::shared_ptr<Node> parent_node) {
  bool is_statement_node = false;

  // if there are children for the node
  for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
    // if statement node
    if (dynamic_cast<ProcedureNode *>((*it).get()) != 0) {
      is_statement_node = true;
    } else if (dynamic_cast<IfNode *>((*it).get()) != 0) {
      is_statement_node = true;
    } else if (dynamic_cast<WhileNode *>((*it).get()) != 0) {
      is_statement_node = true;
    } else if (dynamic_cast<ReadNode *>((*it).get()) != 0) {
      is_statement_node = true;
    } else if (dynamic_cast<PrintNode *>((*it).get()) != 0) {
      is_statement_node = true;
    } else if (dynamic_cast<AssignNode *>((*it).get()) != 0) {
      is_statement_node = true;
    }

    if (is_statement_node) {
      auto current_line = getLineNumberFromNode(lines, *it);
      parent_set.insert(std::pair<Parent, LineNumber>(getNodeValue(parent_node),
                                                      current_line));
      is_statement_node = false;
      // DEBUG
      // std::cout << current_line;
      // std::cout << " is the parent of ";
      // std::cout << following_line << std::endl;
    }
  }

  // iterate through AST via DFS
  for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
    if (dynamic_cast<ProcedureNode *>((*it).get()) != 0) {
      std::shared_ptr<ProcedureNode> derived =
          std::dynamic_pointer_cast<ProcedureNode>(*it);
      setParentRelations(derived);
    } else if (dynamic_cast<IfNode *>((*it).get()) != 0) {
      std::shared_ptr<IfNode> derived = std::dynamic_pointer_cast<IfNode>(*it);
      setParentRelations(derived);
    } else if (dynamic_cast<WhileNode *>((*it).get()) != 0) {
      std::shared_ptr<WhileNode> derived =
          std::dynamic_pointer_cast<WhileNode>(*it);
      setParentRelations(derived);
    }
  }
}

void PKB::setUsesRelations(std::shared_ptr<ProcedureNode> node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsIterator(node->StmtList->StmtList);
}

void PKB::setUsesRelations(std::shared_ptr<IfNode> node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsIterator(node->StmtListThen->StmtList);
  setUsesRelationsIterator(node->StmtListElse->StmtList);
}

void PKB::setUsesRelations(std::shared_ptr<WhileNode> node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsIterator(node->StmtList->StmtList);
}

void PKB::setUsesRelations(std::shared_ptr<PrintNode> node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->Var, node);
}

void PKB::setUsesRelations(std::shared_ptr<AssignNode> node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->Expr, node);
}

//

void PKB::setUsesRelationsH(std::shared_ptr<ExprNode> node,
                            std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->ExprP, parent_node);
  setUsesRelationsH(node->Term, parent_node);
}

void PKB::setUsesRelationsH(const std::shared_ptr<ExprPNode> node,
                            const std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->ExprP, parent_node);
  setUsesRelationsH(node->Term, parent_node);
}

void PKB::setUsesRelationsH(const std::shared_ptr<TermNode> node,
                            const std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->TermP, parent_node);
  setUsesRelationsH(node->Factor, parent_node);
}

void PKB::setUsesRelationsH(const std::shared_ptr<TermPNode> node,
                            const std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->TermP, parent_node);
  setUsesRelationsH(node->Factor, parent_node);
}

void PKB::setUsesRelationsH(const std::shared_ptr<FactorNode> node,
                            const std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->Var, parent_node);
  // doesn't seem necessary for now
  // setUsesRelationsH(node->Val, parent_node);
  setUsesRelationsH(node->Expr, parent_node);
}

void PKB::setUsesRelationsH(const std::shared_ptr<CondExprNode> node,
                            const std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->RelExpr, parent_node);
  setUsesRelationsH(node->CondLHS, parent_node);
  setUsesRelationsH(node->CondRHS, parent_node);
}

void PKB::setUsesRelationsH(const std::shared_ptr<RelExprNode> node,
                            const std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  setUsesRelationsH(node->LHS, parent_node);
  setUsesRelationsH(node->RHS, parent_node);
}

// void PKB::setUsesRelationsH(const std::shared_ptr<RelFactorNode> node,
//                             const std::shared_ptr<Node> parent_node) {
//   setUsesRelationsH(node->Var, parent_node);
//   // doesn't seem necessary for now
//   // setUsesRelationsH(node->Val, parent_node);
//   setUsesRelationsH(node->Expr, parent_node);
// }

void PKB::setUsesRelationsH(const std::shared_ptr<VariableNode> node,
                            const std::shared_ptr<Node> parent_node) {
  if (node == 0) {
    return;
  }
  variables_set.insert(node->Name);
  uses_set.insert(std::pair<LineNumber, VariableName>(
      getLineNumberFromNode(lines, parent_node), node->Name));
  addToVectorMap(uses_map, getLineNumberFromNode(lines, parent_node),
                 node->Name);
}

void PKB::setUsesRelationsIterator(
    const std::vector<std::shared_ptr<Node>> stmt_lst) {
  // iterate through AST via DFS

  // DFS
  for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
    if (dynamic_cast<ProcedureNode *>((*it).get()) != 0) {
      std::shared_ptr<ProcedureNode> derived =
          std::dynamic_pointer_cast<ProcedureNode>((*it));
      setUsesRelations(derived);
    } else if (dynamic_cast<IfNode *>((*it).get()) != 0) {
      std::shared_ptr<IfNode> derived =
          std::dynamic_pointer_cast<IfNode>((*it));
      setUsesRelations(derived);
    } else if (dynamic_cast<WhileNode *>((*it).get()) != 0) {
      std::shared_ptr<WhileNode> derived =
          std::dynamic_pointer_cast<WhileNode>((*it));
      setUsesRelations(derived);
    } else if (dynamic_cast<PrintNode *>((*it).get()) != 0) {
      std::shared_ptr<PrintNode> derived =
          std::dynamic_pointer_cast<PrintNode>((*it));
      setUsesRelations(derived);
    } else if (dynamic_cast<AssignNode *>((*it).get()) != 0) {
      std::shared_ptr<AssignNode> derived =
          std::dynamic_pointer_cast<AssignNode>((*it));
      setUsesRelations(derived);
    }
  }
}

void PKB::setModifiesRelations(std::shared_ptr<Node> node) {
  // iterate through AST via DFS

  // call helper function to traverse down the nodes to form relationships
  std::vector<std::shared_ptr<Node>> stmt_lst;

  if (dynamic_cast<AssignNode *>(node.get()) != 0) {
    std::shared_ptr<AssignNode> derived =
        std::dynamic_pointer_cast<AssignNode>(node);
    setModifiesRelationsHelper(derived->Var,
                               getLineNumberFromNode(lines, derived));
  } else if (dynamic_cast<IfNode *>(node.get()) != 0) {
    std::shared_ptr<IfNode> derived = std::dynamic_pointer_cast<IfNode>(node);
    std::vector<std::shared_ptr<Node>> then_stmt_lst =
        derived->StmtListThen->StmtList;
    std::vector<std::shared_ptr<Node>> else_stmt_lst =
        derived->StmtListElse->StmtList;
    then_stmt_lst.insert(then_stmt_lst.end(), else_stmt_lst.begin(),
                         else_stmt_lst.end());  // concat
    stmt_lst = then_stmt_lst;
    for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
      setModifiesRelationsHelper(*it, getLineNumberFromNode(lines, derived));
    }
  } else if (dynamic_cast<WhileNode *>(node.get()) != 0) {
    std::shared_ptr<WhileNode> derived =
        std::dynamic_pointer_cast<WhileNode>(node);
    stmt_lst = derived->StmtList->StmtList;
    for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
      setModifiesRelationsHelper(*it, getLineNumberFromNode(lines, derived));
    }
  } else if (dynamic_cast<ReadNode *>(node.get()) != 0) {
    std::shared_ptr<ReadNode> derived =
        std::dynamic_pointer_cast<ReadNode>(node);
    setModifiesRelationsHelper(derived->Var,
                               getLineNumberFromNode(lines, derived));
  } else {
    // TODO throw error
  }

  // DFS
  if (dynamic_cast<ProcedureNode *>(node.get()) != 0) {
    std::shared_ptr<ProcedureNode> derived =
        std::dynamic_pointer_cast<ProcedureNode>(node);
    stmt_lst = derived->StmtList->StmtList;
  } else if (dynamic_cast<WhileNode *>(node.get()) != 0) {
    std::shared_ptr<WhileNode> derived =
        std::dynamic_pointer_cast<WhileNode>(node);
    stmt_lst = derived->StmtList->StmtList;
  } else if (dynamic_cast<IfNode *>(node.get()) != 0) {
    std::shared_ptr<IfNode> derived = std::dynamic_pointer_cast<IfNode>(node);
    std::vector<std::shared_ptr<Node>> then_stmt_lst =
        derived->StmtListThen->StmtList;
    std::vector<std::shared_ptr<Node>> else_stmt_lst =
        derived->StmtListElse->StmtList;
    then_stmt_lst.insert(then_stmt_lst.end(), else_stmt_lst.begin(),
                         else_stmt_lst.end());  // concat
    stmt_lst = then_stmt_lst;
  }
  // reverse iterator to do DFS
  for (auto it = stmt_lst.begin(); it != stmt_lst.end(); it++) {
    setModifiesRelations(*it);
  }
}

// recursive function
// node and vector as arguments
void PKB::setModifiesRelationsHelper(std::shared_ptr<Node> node,
                                     LineNumber line_number) {
  if (dynamic_cast<VariableNode *>(node.get()) != 0) {
    std::shared_ptr<VariableNode> derived =
        std::dynamic_pointer_cast<VariableNode>(node);
    variables_set.insert(derived->Name);
    modifies_set.insert(
        std::pair<LineNumber, VariableName>(line_number, derived->Name));
    addToVectorMap(modifies_map, line_number, derived->Name);
  } else {
    // throw error
  }
}

ProcedureName PKB::getNodeValue(std::shared_ptr<Node> node) {
  if (dynamic_cast<ProcedureNode *>(node.get()) != 0) {
    std::shared_ptr<ProcedureNode> derived =
        std::dynamic_pointer_cast<ProcedureNode>(node);
    return derived->Name;
  } else {
    return getLineNumberFromNode(lines, node);
  }
}

// TODO deprecate temp testing methods
bool PKB::testFollows(LineNumber a, LineNumber b) {
  return follows_set.find(std::pair<LineNumber, LineNumber>(a, b)) !=
         follows_set.end();
}

bool PKB::testParent(Parent a, LineNumber b) {
  return parent_set.find(std::pair<Parent, LineNumber>(a, b)) !=
         parent_set.end();
}

bool PKB::testUses(LineNumber a, VariableName b) {
  return uses_set.find(std::pair<LineNumber, VariableName>(a, b)) !=
         uses_set.end();
}

bool PKB::testModifies(LineNumber a, VariableName b) {
  return modifies_set.find(std::pair<LineNumber, VariableName>(a, b)) !=
         modifies_set.end();
}

std::vector<LineNumber> PKB::getFollows(LineNumber line) {
  std::vector<LineNumber> res;
  try {
    res = follows_map.at(line);
    return res;
  } catch (const std::out_of_range &e) {
    return res;
  }
}

std::vector<LineNumber> PKB::getUses(LineNumber line) {
  std::vector<LineNumber> res;
  try {
    res = uses_map.at(line);
    return res;
  } catch (const std::out_of_range &e) {
    return res;
  }
}

// TODO shift this out to a utils component
void PKB::addToVectorMap(
    std::unordered_map<std::string, std::vector<std::string>> umap,
    std::string index, std::string data) {
  if (umap.find(index) == umap.end()) {
    // create new vector
    std::vector<std::string> v;
    v.push_back(data);
    umap[index] = v;
  } else {
    // retrieve vector and add element
    umap.at(index).push_back(data);
  }
}
