// TODO remove print debug statements and iostream

#include "program_knowledge_base/pkb_manager.h"
#include "simple_parser/interface.h"

#include "catch.hpp"

#include <fstream>

using Simple::SimpleInterface;

TEST_CASE ("Test PKB for assign.txt") {
  auto ast = SimpleInterface::getAstFromFile("tests/simple_source/assign.txt");

  PKB::PKBManager pkb = PKB::PKBManager(ast);

  // variable
  auto var_exist_test_1 = pkb.isVariableExists("i");
  REQUIRE(var_exist_test_1 == true);
  auto var_exist_test_2 = pkb.isVariableExists("j");
  REQUIRE(var_exist_test_2 == false);

  std::unordered_set<std::string> var_get_test_1_check;
  var_get_test_1_check.insert("i");
  auto var_get_test_1_vector = pkb.getVariableList();
  std::unordered_set<Variable> var_get_test_1_set(var_get_test_1_vector.begin(),
                                                  var_get_test_1_vector.end());
  REQUIRE(var_get_test_1_set == var_get_test_1_check);

  // assign
  auto assign_exist_test_1 = pkb.isAssignExists("1");
  REQUIRE(assign_exist_test_1 == true);
  auto assign_exist_test_2 = pkb.isAssignExists("2");
  REQUIRE(assign_exist_test_2 == false);

  // statement
  auto statement_exist_test_1 = pkb.isStatementExists("1");
  REQUIRE(statement_exist_test_1 == true);
  auto statement_exist_test_2 = pkb.isStatementExists("2");
  REQUIRE(statement_exist_test_2 == false);

  // test modifies
  auto modifies_test_1 = pkb.isLineModifiesVar("1", "i");
  REQUIRE(modifies_test_1 == true);
}

TEST_CASE ("Test PKB for simple_1.txt") {
  auto ast = Simple::SimpleInterface::getAstFromFile(
      "tests/simple_source/simple_1.txt");

  PKB::PKBManager pkb = PKB::PKBManager(ast);

  // variable
  auto var_exist_test_1 = pkb.isVariableExists("i");
  REQUIRE(var_exist_test_1 == true);
  auto var_exist_test_2 = pkb.isVariableExists("j");
  REQUIRE(var_exist_test_2 == true);
  auto var_exist_test_3 = pkb.isVariableExists("k");
  REQUIRE(var_exist_test_3 == false);
  auto var_exist_test_4 = pkb.isVariableExists("l");
  REQUIRE(var_exist_test_4 == false);

  std::unordered_set<std::string> var_get_test_1_check;
  var_get_test_1_check.insert("i");
  var_get_test_1_check.insert("j");
  auto var_get_test_1_vector = pkb.getVariableList();
  std::unordered_set<Variable> var_get_test_1_set(var_get_test_1_vector.begin(),
                                                  var_get_test_1_vector.end());
  REQUIRE(var_get_test_1_set == var_get_test_1_check);

  // assign
  auto assign_exist_test_1 = pkb.isAssignExists("1");
  REQUIRE(assign_exist_test_1 == true);
  auto assign_exist_test_2 = pkb.isAssignExists("2");
  REQUIRE(assign_exist_test_2 == false);

  // assign
  auto statement_exist_test_1 = pkb.isStatementExists("1");
  REQUIRE(statement_exist_test_1 == true);
  auto statement_exist_test_2 = pkb.isStatementExists("2");
  REQUIRE(statement_exist_test_2 == true);
  auto statement_exist_test_3 = pkb.isStatementExists("3");
  REQUIRE(statement_exist_test_3 == true);
  auto statement_exist_test_4 = pkb.isStatementExists("4");
  REQUIRE(statement_exist_test_4 == true);
  auto statement_exist_test_5 = pkb.isStatementExists("5");
  REQUIRE(statement_exist_test_5 == true);
  auto statement_exist_test_6 = pkb.isStatementExists("6");
  REQUIRE(statement_exist_test_6 == false);
  auto statement_exist_test_7 = pkb.isStatementExists("0");
  REQUIRE(statement_exist_test_7 == false);
  auto statement_exist_test_8 = pkb.isStatementExists("-12");
  REQUIRE(statement_exist_test_8 == false);

  // print
  auto print_exist_test_1 = pkb.isPrintExists("3");
  REQUIRE(print_exist_test_1 == true);
  auto print_exist_test_2 = pkb.isPrintExists("4");
  REQUIRE(print_exist_test_2 == true);
  auto print_exist_test_3 = pkb.isPrintExists("5");
  REQUIRE(print_exist_test_3 == false);

  // read
  auto read_exist_test_1 = pkb.isReadExists("5");
  REQUIRE(read_exist_test_1 == true);
  auto read_exist_test_2 = pkb.isReadExists("2");
  REQUIRE(read_exist_test_2 == false);

  // while
  auto while_exist_test_1 = pkb.isWhileExists("2");
  REQUIRE(while_exist_test_1 == true);
  auto while_exist_test_2 = pkb.isWhileExists("3");
  REQUIRE(while_exist_test_2 == false);

  // if
  auto if_exist_test_1 = pkb.isIfExists("1");
  REQUIRE(if_exist_test_1 == false);
  auto if_exist_test_2 = pkb.isIfExists("2");
  REQUIRE(if_exist_test_2 == false);

  // constant
  auto constant_exist_test_1 = pkb.isConstantExists("5");
  REQUIRE(constant_exist_test_1 == true);
  auto constant_exist_test_2 = pkb.isConstantExists("222");
  REQUIRE(constant_exist_test_2 == false);

  // procedure
  auto proc_exist_test_1 = pkb.isProcedureExists("main");
  REQUIRE(proc_exist_test_1 == true);
  auto proc_exist_test_2 = pkb.isProcedureExists("blah");
  REQUIRE(proc_exist_test_2 == false);

  // test follows
  auto follows_test_1 = pkb.isLineFollowLine("1", "2");
  REQUIRE(follows_test_1 == true);
  auto follows_test_2 = pkb.isLineFollowLine("1", "4");
  REQUIRE(follows_test_2 == false);
  auto follows_test_3 = pkb.isLineFollowLine("1", "5");
  REQUIRE(follows_test_3 == false);
  auto follows_test_4 = pkb.isLineFollowLine("2", "4");
  REQUIRE(follows_test_4 == true);
  auto follows_test_5 = pkb.isLineFollowLine("2", "5");
  REQUIRE(follows_test_5 == false);
  auto follows_test_6 = pkb.isLineFollowLine("4", "5");
  REQUIRE(follows_test_6 == true);
  auto follows_test_7 = pkb.isLineFollowLine("1", "9");
  REQUIRE(follows_test_7 == false);
  auto follows_test_8 = pkb.isLineFollowLine("2", "3");
  REQUIRE(follows_test_8 == false);
  auto follows_test_9 = pkb.isLineFollowLine("4", "2");
  REQUIRE(follows_test_9 == false);
  auto follows_test_10 = pkb.isLineFollowLine("8", "0");
  REQUIRE(follows_test_10 == false);
  auto follows_test_11 = pkb.isLineFollowLineS("1", "2");
  REQUIRE(follows_test_11 == true);
  auto follows_test_12 = pkb.isLineFollowLineS("1", "3");
  REQUIRE(follows_test_12 == false);
  auto follows_test_13 = pkb.isLineFollowLineS("1", "4");
  REQUIRE(follows_test_13 == true);
  auto follows_test_14 = pkb.isLineFollowLineS("1", "5");
  REQUIRE(follows_test_14 == true);
  auto follows_test_15 = pkb.isLineFollowLineS("2", "3");
  REQUIRE(follows_test_15 == false);
  auto follows_test_16 = pkb.isLineFollowLineS("2", "4");
  REQUIRE(follows_test_16 == true);
  auto follows_test_17 = pkb.isLineFollowLineS("2", "5");
  REQUIRE(follows_test_17 == true);
  auto follows_test_18 = pkb.isLineFollowLineS("3", "4");
  REQUIRE(follows_test_18 == false);

  // test parent
  auto parent_test_1 = pkb.isLineParentLine("2", "3");
  REQUIRE(parent_test_1 == true);
  auto parent_test_2 = pkb.isLineParentLine("1", "2");
  REQUIRE(parent_test_2 == false);
  auto parent_test_3 = pkb.isLineParentLine("3", "3");
  REQUIRE(parent_test_3 == false);
  auto parent_test_4 = pkb.isLineParentLine("4", "3");
  REQUIRE(parent_test_4 == false);

  std::unordered_set<std::string> parent_test_5_check;
  parent_test_5_check.insert("2");
  std::unordered_set<Variable> parent_test_5_set;
  auto parent_test_5 = pkb.getParentLineS("3");
  if (parent_test_5) {
    for (const auto &elem : (*parent_test_5)) {
      parent_test_5_set.insert(elem);
    }
  }
  REQUIRE(parent_test_5_set == parent_test_5_check);

  // test uses
  auto uses_test_1 = pkb.isLineUsesVar("2", "i");
  REQUIRE(uses_test_1 == true);
  auto uses_test_2 = pkb.isLineUsesVar("1", "i");
  REQUIRE(uses_test_2 == false);
  auto uses_test_3 = pkb.isLineUsesVar("5", "i");
  REQUIRE(uses_test_3 == false);
  auto uses_test_4 = pkb.isLineUsesVar("2", "alpha");
  REQUIRE(uses_test_4 == false);

  auto uses_test_5 = pkb.isProcedureUsesVar("main", "i");
  REQUIRE(uses_test_5 == true);
  auto uses_test_6 = pkb.isProcedureUsesVar("main", "j");
  REQUIRE(uses_test_6 == true);
  auto uses_test_7 = pkb.isProcedureUsesVar("main", "k");
  REQUIRE(uses_test_7 == false);
  auto uses_test_8 = pkb.isProcedureUsesVar("test", "alpha");
  REQUIRE(uses_test_8 == false);

  std::unordered_set<std::string> uses_test_9_check;
  uses_test_9_check.insert("i");
  uses_test_9_check.insert("j");
  std::unordered_set<Variable> uses_test_9_set;
  auto uses_test_9 = pkb.getVarUsedByProcedure("main");
  if (uses_test_9) {
    for (const auto &elem : (*uses_test_9)) {
      uses_test_9_set.insert(elem);
    }
  }
  REQUIRE(uses_test_9_set == uses_test_9_check);

  // test modifies
  auto modifies_test_1 = pkb.isLineModifiesVar("5", "i");
  REQUIRE(modifies_test_1 == true);
  auto modifies_test_2 = pkb.isLineModifiesVar("1", "i");
  REQUIRE(modifies_test_2 == true);
  auto modifies_test_3 = pkb.isLineModifiesVar("1", "beta");
  REQUIRE(modifies_test_3 == false);
  auto modifies_test_4 = pkb.isLineModifiesVar("2", "alpha");
  REQUIRE(modifies_test_4 == false);
  auto modifies_test_5 = pkb.isLineModifiesVar("22", "alpha");
  REQUIRE(modifies_test_5 == false);
  auto modifies_test_6 = pkb.isLineModifiesVar("10", "i");
  REQUIRE(modifies_test_6 == false);

  auto modifies_test_7 = pkb.isProcedureModifiesVar("main", "i");
  REQUIRE(modifies_test_7 == true);
  auto modifies_test_8 = pkb.isProcedureModifiesVar("main", "j");
  REQUIRE(modifies_test_8 == false);
  auto modifies_test_9 = pkb.isProcedureModifiesVar("test", "i");
  REQUIRE(modifies_test_9 == false);
  auto modifies_test_10 = pkb.isProcedureModifiesVar("test", "asd");
  REQUIRE(modifies_test_10 == false);

  std::unordered_set<std::string> modifies_test_11_check;
  modifies_test_11_check.insert("i");
  std::unordered_set<Variable> modifies_test_11_set;
  auto modifies_test_11 = pkb.getVarModifiedByProcedure("main");
  if (modifies_test_11) {
    for (const auto &elem : (*modifies_test_11)) {
      modifies_test_11_set.insert(elem);
    }
  }
  REQUIRE(modifies_test_11_set == modifies_test_11_check);
}

TEST_CASE ("Test PKB for 10_simple_source_deep_nesting.txt") {
  auto proc = SimpleInterface::getAstFromFile(
      "tests/10_simple_source_deep_nesting.txt");

  PKB::PKBManager pkb = PKB::PKBManager(proc);

  // variable
  auto var_exist_test_1 = pkb.isVariableExists("x");
  REQUIRE(var_exist_test_1 == true);
  auto var_exist_test_2 = pkb.isVariableExists("y");
  REQUIRE(var_exist_test_2 == true);
  auto var_exist_test_3 = pkb.isVariableExists("z");
  REQUIRE(var_exist_test_3 == true);
  auto var_exist_test_4 = pkb.isVariableExists("a");
  REQUIRE(var_exist_test_4 == false);
}

TEST_CASE ("Test detection of semantic errors in AST") {
  SECTION ("procedure and variable with the same name") {
    auto ast = SimpleInterface::getAstFromFile(
        "tests/semantic_errors/proc_variable_same_name.txt");

    REQUIRE_THROWS_WITH(
        PKB::PKBManager(ast),
        "Found procedure and variable with the same name: 'main'.");
  }
}
