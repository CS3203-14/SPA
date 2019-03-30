#include "query_executor/clause_prioritizer.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "query_executor/clause_functions.cpp"

std::vector<Clause> ClausePrioritizer::getClauses() {
  // Not worth sorting - little to no gain
  if (tooFewClauses()) return getClausesFromQuery();

  // Initializes all clauses to a default starting score and group
  auto weighted_grouped_clauses = getInitialWeightedGroupedClauses();

  prioritizeClauses(weighted_grouped_clauses);

  return getClausesFromWeightedGroupedClauses(weighted_grouped_clauses);
}

void ClausePrioritizer::prioritizeClauses(
    std::vector<WeightedGroupedClause>& clauses) {
  /* Prioritization algorithm: https://github.com/jethrokuan/SPA-T14/issues/200
   * 1. Apply weightages on clauses: different penalties and modifiers.
   * 2. Group clauses: sort them so that groups are executed one after another
   */
  weightClauses(clauses);
  std::sort(clauses.begin(), clauses.end());
}

// Weight calculation
void ClausePrioritizer::weightClauses(
    std::vector<WeightedGroupedClause>& clauses) {
  // For each clause, attempt to apply a clause type checking function
  // If it matches, update it with the appropriate weighting function
  for (auto& clause : clauses) {
    for (auto [clauseTypeChecker, weightFunction] : weightUpdaters) {
      if (clauseTypeChecker(clause)) weightFunction(clause);
    }
  }
}

// Actual definition of the weightUpdaters static variables
std::vector<std::pair<ClauseTypeChecker, WeightFunction>>
    ClausePrioritizer::weightUpdaters = {
        {isBooleanClause, weightBooleanClause}};

std::vector<WeightedGroupedClause>
ClausePrioritizer::getInitialWeightedGroupedClauses() {
  std::vector<WeightedGroupedClause> clauses;
  // Add all clauses to a vector of them
  if (!query->rel_cond->empty()) {
    for (auto& rel_cond : *(query->rel_cond)) {
      clauses.push_back({rel_cond, STARTING_WEIGHT, DEFAULT_GROUP});
    }
  }
  if (!query->patternb->empty()) {
    for (auto& pattern : *(query->patternb)) {
      clauses.push_back({pattern, STARTING_WEIGHT, DEFAULT_GROUP});
    }
  }
  if (!query->with_cond->empty()) {
    for (auto& with_cond : *(query->with_cond)) {
      clauses.push_back({with_cond, STARTING_WEIGHT, DEFAULT_GROUP});
    }
  }
  return clauses;
}

std::vector<Clause> ClausePrioritizer::getClausesFromQuery() {
  std::vector<Clause> clauses;
  // Add all clauses to a vector of them
  if (!query->rel_cond->empty()) {
    for (auto& rel_cond : *(query->rel_cond)) {
      clauses.push_back(rel_cond);
    }
  }
  if (!query->patternb->empty()) {
    for (auto& pattern : *(query->patternb)) {
      clauses.push_back(pattern);
    }
  }
  if (!query->with_cond->empty()) {
    for (auto& with_cond : *(query->with_cond)) {
      clauses.push_back(with_cond);
    }
  }
  return clauses;
}

bool ClausePrioritizer::tooFewClauses() {
  size_t total_clauses = (query->rel_cond ? query->rel_cond->size() : 0) +
                         (query->patternb ? query->patternb->size() : 0) +
                         (query->with_cond ? query->with_cond->size() : 0);
  return total_clauses < MIN_CLAUSES_TO_SORT;
}

std::vector<Clause> ClausePrioritizer::getClausesFromWeightedGroupedClauses(
    const std::vector<WeightedGroupedClause>& clauses) {
  std::vector<Clause> out_clauses;
  for (const auto& clause : clauses) {
    out_clauses.push_back(clause.clause);
  }
  return out_clauses;
}