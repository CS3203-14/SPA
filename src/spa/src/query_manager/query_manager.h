#pragma once
#include <optional>
#include <string>
#include <vector>
#include "program_knowledge_base/pkb_manager.h"
#include "query_evaluator/pql/pql.h"

using namespace PKB;
using BoolOrStrings = std::variant<bool, std::vector<std::string>>;

class QueryManager {
 private:
  PKBManager* pkb;

  //! Makes the actual query - but returns the result unsorted
  std::vector<std::string> makeQueryUnsorted(QE::Query* query);

  //! Returns true if this SuchThat has no variables to fill
  bool isBooleanSuchThat(QE::SuchThat*);
  //! Evaluates the SuchThat clause as a boolean
  bool isBooleanSuchThatTrue(QE::SuchThat*);
  //! Evaluates SuchThat clauses that don't return a simple boolean
  BoolOrStrings handleNonBooleanSuchThat(QE::Query*);

 public:
  QueryManager(PKBManager* pkb) : pkb(pkb){};
  std::vector<std::string> makeQuery(QE::Query* query);

  //! Convert a StmtOrEntRef to a string to pass to PKB
  static std::string suchThatArgToString(QE::StmtOrEntRef);
  static std::string stmtRefToString(QE::StmtRef);
  static std::string entRefToString(QE::EntRef);
  static std::optional<QE::Synonym> getSuchThatArgAsSynonym(QE::StmtOrEntRef);
  static bool isSuchThatArgUnderscore(QE::StmtOrEntRef);
  static std::optional<std::string> getSuchThatArgAsBasic(QE::StmtOrEntRef);
  //! Gets the result of a select query (everything from a design entity)
  static std::vector<std::string> getSelect(PKBManager* pkb, QE::DesignEntity);
};