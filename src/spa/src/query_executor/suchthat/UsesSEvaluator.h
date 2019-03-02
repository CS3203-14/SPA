#pragma once
#include <cassert>
#include <optional>
#include <string>
#include <vector>
#include "program_knowledge_base/pkb_manager.h"
#include "query_builder/pql/pql.h"
#include "query_executor/query_executor.h"
#include "query_executor/suchthat/SuchThatEvaluator.h"

using namespace PKB;
using namespace QE;

class UsesSEvaluator : public SuchThatEvaluator {
 public:
  UsesSEvaluator(Query* query, PKBManager* pkb, QueryConstraints& qc)
      : SuchThatEvaluator(query, pkb, qc){};

  // Handle cases with at least one variable selected

  AllowedValuesPairOrBool handleLeftVarSelectedRightBasic() override {
    // Uses(s, "x")
    auto results =
        pkb->getLineUsesVar(*arg2AsBasic).value_or(std::vector<std::string>());
    return ConstraintSolver::makeAllowedValues(*arg1AsSynonym, results);
  }
  AllowedValuesPairOrBool handleRightVarSelectedLeftBasic() override {
    // Uses(3, v)
    auto results = pkb->getVarUsedByLine(*arg1AsBasic)
                       .value_or(std::vector<std::string>());
    return ConstraintSolver::makeAllowedValues(*arg2AsSynonym, results);
  }
  AllowedValuesPairOrBool handleLeftVarSelectedRightUnderscore() override {
    // Uses(s, _)
    // Note that this should select all whiles and ifs
    auto all_selected_designentities = QueryExecutor::getSelect(
        pkb, query->selected_declaration->getDesignEntity());
    std::vector<std::string> results;
    for (auto de : all_selected_designentities) {
      if (pkb->getVarUsedByLine(de)) {
        results.push_back(de);
      }
    }
    return ConstraintSolver::makeAllowedValues(*arg1AsSynonym, results);
    ;
  }
  AllowedValuesPairOrBool handleRightVarSelectedLeftUnderscore() override {
    std::cout << "Should not happen: UsesS first arg cannot be _\n";
    assert(false);
  }
  AllowedValuesPairOrBool handleLeftVarSelectedRightVarUnselected() override {
    // Uses*(s, v)
    if (arg1AsSynonym == arg2AsSynonym) {
      std::cout << "Should not happen: UsesS cannot have identical args\n";
      assert(false);
    }
    auto all_selected_designentities = QueryExecutor::getSelect(
        pkb, query->selected_declaration->getDesignEntity());
    auto right_arg_de = Declaration::findDeclarationForSynonym(
                            query->declarations, *arg2AsSynonym)
                            ->getDesignEntity();
    auto all_unselected_designentities =
        QueryExecutor::getSelect(pkb, right_arg_de);
    AllowedValuePairSet results;
    for (auto de : all_selected_designentities) {
      for (auto unselect_de : all_unselected_designentities) {
        if (pkb->isLineUsesVar(de, unselect_de)) {
          results.insert({de, unselect_de});
        }
      }
    }
    return ConstraintSolver::makeAllowedValues(*arg1AsSynonym, *arg2AsSynonym,
                                               results);
  }
  AllowedValuesPairOrBool handleRightVarSelectedLeftVarUnselected() override {
    // Uses(s1, s)
    if (arg1AsSynonym == arg2AsSynonym) {
      std::cout << "Should not happen: UsesS cannot have identical args\n";
      assert(false);
    }
    auto all_selected_designentities = QueryExecutor::getSelect(
        pkb, query->selected_declaration->getDesignEntity());
    auto left_arg_de = Declaration::findDeclarationForSynonym(
                           query->declarations, *arg1AsSynonym)
                           ->getDesignEntity();
    auto all_unselected_designentities =
        QueryExecutor::getSelect(pkb, left_arg_de);
    AllowedValuePairSet results;
    for (auto de : all_selected_designentities) {
      for (auto unselect_de : all_unselected_designentities) {
        if (pkb->isLineUsesVar(unselect_de, de)) {
          results.insert({unselect_de, de});
        }
      }
    }
    return ConstraintSolver::makeAllowedValues(*arg1AsSynonym, *arg2AsSynonym,
                                               results);
  }

  // Handle cases with no variables selected

  AllowedValuesPairOrBool handleDoubleUnderscore() override {
    return !pkb->isLineUsesVarSetEmpty();
  }
  AllowedValuesPairOrBool handleBothVarsUnselected() override {
    // Uses(s1, s2)
    if (arg1AsSynonym == arg2AsSynonym) {
      std::cout << "Should not happen: UsesS cannot have identical args\n";
      assert(false);
    }
    auto left_arg_de = Declaration::findDeclarationForSynonym(
                           query->declarations, *arg1AsSynonym)
                           ->getDesignEntity();
    auto right_arg_de = Declaration::findDeclarationForSynonym(
                            query->declarations, *arg1AsSynonym)
                            ->getDesignEntity();

    auto all_left_designentities = QueryExecutor::getSelect(pkb, left_arg_de);
    auto all_right_designentities = QueryExecutor::getSelect(pkb, right_arg_de);
    AllowedValuePairSet results;
    for (auto left_de : all_left_designentities) {
      for (auto right_de : all_right_designentities) {
        // Any satisfied relation would mean this clause is true overall
        if (pkb->isLineUsesVar(left_de, right_de)) {
          results.insert({left_de, right_de});
        }
      }
    }
    return ConstraintSolver::makeAllowedValues(*arg1AsSynonym, *arg2AsSynonym,
                                               results);
  }
  AllowedValuesPairOrBool handleLeftVarUnselectedRightBasic() override {
    // Uses(s1, "x")
    return handleLeftVarSelectedRightBasic();
  }
  AllowedValuesPairOrBool handleRightVarUnselectedLeftBasic() override {
    // Uses(3, v1)
    return handleRightVarSelectedLeftBasic();
  }

  // Unlikely that these last 4 will need to be changed
  AllowedValuesPairOrBool handleLeftBasicRightUnderscore() override {
    // Uses(3, _)
    return pkb->getVarUsedByLine(*arg1AsBasic).has_value();
  }
  AllowedValuesPairOrBool handleRightBasicLeftUnderscore() override {
    // Uses(_, "x")
    return pkb->getLineUsesVar(*arg2AsBasic).has_value();
  }
  AllowedValuesPairOrBool handleLeftVarUnselectedRightUnderscore() override {
    // Uses*(s1, _) --> is there a statement that uses any variable?
    // Reuse the left-var selected results until an optimized PKB query can help
    return handleLeftVarSelectedRightUnderscore();
  }
  AllowedValuesPairOrBool handleRightVarUnselectedLeftUnderscore() override {
    // Uses*(_, v) --> is there a variable that is used?
    // Reuse the left-var selected results until an optimized PKB query can help
    return handleRightVarSelectedLeftUnderscore();
  }
};