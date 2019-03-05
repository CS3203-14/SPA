#pragma once
#include <optional>
#include <set>
#include <string>
#include <vector>
#include "program_knowledge_base/pkb_manager.h"
#include "query_builder/pql/pql.h"
#include "query_executor/constraint_solver/constraint_solver.h"
#include "query_executor/constraint_solver/query_constraints.h"

using namespace PKB;

//! \brief Clauses can either return constraints or indicate no constraints
//! bool == true ==> no constraints.
//! bool == false or empty allowed pair ==> no valid results
//! Otherwise, need to constrain other relevant values
// using AllowedValuesPairOrBool = std::variant<TupledConstraint, bool>;

class QueryExecutor {
 private:
  PKBManager* pkb;

  //! Makes the actual query - but returns the result unsorted
  std::vector<std::string> makeQueryUnsorted(QE::Query* query);
  //! Evaluates any SuchThat clause
  bool handleSuchThat(QE::Query*, QueryConstraints&);
  //! Evaluates any Pattern clause
  bool handlePattern(QE::Query*, QueryConstraints&);

 public:
  QueryExecutor(PKBManager* pkb) : pkb(pkb){};

  //! Actual external interface called to make the PQL query
  std::vector<std::string> makeQuery(QE::Query* query);

  //! Gets the result of an unconditional select query (Select <designentity>)
  static std::vector<std::string> getSelect(PKBManager* pkb, QE::DesignEntity);

  //! Convert a StmtOrEntRef to a string to pass to PKB
  static std::string suchThatArgToString(QE::StmtOrEntRef);
  static std::string stmtRefToString(QE::StmtRef);
  static std::string entRefToString(QE::EntRef);
  static std::optional<QE::Synonym> getSuchThatArgAsSynonym(QE::StmtOrEntRef);
  static bool isSuchThatArgUnderscore(QE::StmtOrEntRef);
  static std::optional<std::string> getSuchThatArgAsBasic(QE::StmtOrEntRef);
};