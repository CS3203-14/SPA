#pragma once
#include <cassert>
#include <optional>
#include <string>
#include <vector>
#include "program_knowledge_base/pkb_manager.h"
#include "query_evaluator/pql/pql.h"
#include "query_manager/query_manager.h"
#include "query_manager/relations/SuchThatEvaluator.h"

using namespace PKB;
using namespace QE;

class ParentEvaluator : public SuchThatEvaluator {
 public:
  ParentEvaluator(Query* query, PKBManager* pkb)
      : SuchThatEvaluator(query, pkb){};

  // Handle cases with at least one variable selected

  BoolOrStrings handleLeftVarSelectedRightBasic() override {
    // Parent(s, 3)
    if (auto parentLine = pkb->getParentLine(*arg2AsBasic)) {
      return std::vector<std::string>{*parentLine};
    } else {
      return std::vector<std::string>();
    }
  }
  BoolOrStrings handleRightVarSelectedLeftBasic() override {
    // Parent(3, s)
    if (auto childLines = pkb->getChildLine(*arg2AsBasic)) {
      return *childLines;
    } else {
      return std::vector<std::string>();
    }
  }
  BoolOrStrings handleLeftVarSelectedRightUnderscore() override {
    // Parent(s, _)
    // Note that this should select all whiles and ifs
    auto all_selected_designentities = QueryManager::getSelect(
        pkb, query->selected_declaration->getDesignEntity());
    std::vector<std::string> results;
    for (auto de : all_selected_designentities) {
      if (pkb->getChildLine(de)) {
        results.push_back(de);
      }
    }
    return results;
  }
  BoolOrStrings handleRightVarSelectedLeftUnderscore() override {
    // Parent(_, s)
    auto all_selected_designentities = QueryManager::getSelect(
        pkb, query->selected_declaration->getDesignEntity());
    std::vector<std::string> results;
    for (auto de : all_selected_designentities) {
      if (pkb->getParentLine(de)) {
        results.push_back(de);
      }
    }
    return results;
  }
  BoolOrStrings handleLeftVarSelectedRightVarUnselected() override {
    // Parent(s, s1)
    if (arg1AsSynonym == arg2AsSynonym) {
      // Cannot be a parent of yourself
      return std::vector<std::string>();
    }
    auto all_selected_designentities = QueryManager::getSelect(
        pkb, query->selected_declaration->getDesignEntity());
    auto right_arg_de = Declaration::findDeclarationForSynonym(
                            query->declarations, *arg2AsSynonym)
                            ->getDesignEntity();
    auto all_unselected_designentities =
        QueryManager::getSelect(pkb, right_arg_de);
    std::vector<std::string> results;
    for (auto de : all_selected_designentities) {
      for (auto unselect_de : all_unselected_designentities) {
        if (pkb->isLineParentLine(de, unselect_de) &&
            std::find(results.begin(), results.end(), de) == results.end()) {
          results.push_back(de);
        }
      }
    }
    return results;
  }
  BoolOrStrings handleRightVarSelectedLeftVarUnselected() override {
    // Parent(s1, s)
    if (arg1AsSynonym == arg2AsSynonym) {
      // Cannot parent yourself
      return std::vector<std::string>();
    }
    auto all_selected_designentities = QueryManager::getSelect(
        pkb, query->selected_declaration->getDesignEntity());
    auto left_arg_de = Declaration::findDeclarationForSynonym(
                           query->declarations, *arg1AsSynonym)
                           ->getDesignEntity();
    auto all_unselected_designentities =
        QueryManager::getSelect(pkb, left_arg_de);
    std::vector<std::string> results;
    for (auto de : all_selected_designentities) {
      for (auto unselect_de : all_unselected_designentities) {
        if (pkb->isLineParentLine(unselect_de, de) &&
            std::find(results.begin(), results.end(), de) == results.end()) {
          results.push_back(de);
        }
      }
    }
    return results;
  }

  // Handle cases with no variables selected

  BoolOrStrings handleDoubleUnderscore() override {
    return !pkb->isLineParentLineSetEmpty();
  }
  BoolOrStrings handleBothVarsUnselected() override {
    // Parent(s1, s2)
    if (arg1AsSynonym == arg2AsSynonym) {
      // Cannot parent yourself (hyuk)
      return false;
    }
    auto left_arg_de = Declaration::findDeclarationForSynonym(
                           query->declarations, *arg1AsSynonym)
                           ->getDesignEntity();
    auto right_arg_de = Declaration::findDeclarationForSynonym(
                            query->declarations, *arg1AsSynonym)
                            ->getDesignEntity();

    auto all_left_designentities = QueryManager::getSelect(pkb, left_arg_de);
    auto all_right_designentities = QueryManager::getSelect(pkb, right_arg_de);
    std::vector<std::string> results;
    for (auto left_de : all_left_designentities) {
      for (auto right_de : all_right_designentities) {
        // Any satisfied relation would mean this clause is true overall
        if (pkb->isLineParentLine(left_de, right_de)) {
          return true;
        }
      }
    }
    return false;
  }
  BoolOrStrings handleLeftVarUnselectedRightBasic() override {
    // Parent(s1, 3)
    return pkb->getParentLine(*arg2AsBasic).has_value();
  }
  BoolOrStrings handleRightVarUnselectedLeftBasic() override {
    // Parent(3, s1)
    return pkb->getChildLine(*arg1AsBasic).has_value();
  }

  BoolOrStrings handleLeftBasicRightUnderscore() override {
    // Parent(3, _)
    return handleRightVarUnselectedLeftBasic();
  }
  BoolOrStrings handleRightBasicLeftUnderscore() override {
    // Parent(_, 3)
    return handleLeftVarUnselectedRightBasic();
  }
  BoolOrStrings handleLeftVarUnselectedRightUnderscore() override {
    // Parent(s1, _) --> is there a statement that is a parent of anything?
    // Reuse the left-var selected results until an optimized PKB query can help
    return !std::get<std::vector<std::string>>(
                handleLeftVarSelectedRightUnderscore())
                .empty();
  }
  BoolOrStrings handleRightVarUnselectedLeftUnderscore() override {
    // Parent(_, s1) --> is there a statement that is a child of anything?
    // Reuse the left-var selected results until an optimized PKB query can help
    return !std::get<std::vector<std::string>>(
                handleRightVarSelectedLeftUnderscore())
                .empty();
  }
};