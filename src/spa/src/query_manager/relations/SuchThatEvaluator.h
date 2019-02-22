#pragma once
#include <optional>
#include <string>
#include <vector>
#include "program_knowledge_base/pkb_manager.h"
#include "query_evaluator/pql/pql.h"

using namespace PKB;
using namespace QE;
using BoolOrStrings = std::variant<bool, std::vector<std::string>>;

class SuchThatEvaluator {
 private:
  Query* query;
  PKBManager* pkb;

  // Calculated from the Query object
  QE::StmtOrEntRef arg1;
  QE::StmtOrEntRef arg2;
  std::optional<Synonym> arg1AsSynonym;
  std::optional<Synonym> arg2AsSynonym;
  bool arg1InSelect;
  bool arg2InSelect;
  bool arg1IsUnderscore;
  bool arg2IsUnderscore;
  std::optional<std::string> arg1AsBasic;
  std::optional<std::string> arg2AsBasic;

  //! Dispatches such that query to individual methods to handle it
  BoolOrStrings dispatch();

  BoolOrStrings dispatchSuchThatSelected();

  BoolOrStrings dispatchSuchThatNotSelected();

 public:
  SuchThatEvaluator(Query* query, PKBManager* pkb)
      : query(query),
        pkb(pkb),
        arg1(query->such_that->getFirstArg()),
        arg2(query->such_that->getSecondArg()){};
  BoolOrStrings evaluate();
};