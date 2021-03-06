#include "query_executor/suchthat/SuchThatEvaluator.h"
#include <cassert>
#include <string>
#include <vector>

#include "query_executor/query_executor.h"

bool SuchThatEvaluator::evaluate() {
  // Get all relevant variables so that further work with such_that can be
  // easily done
  // Get all relevant variables so that further work with such_that can be
  // easily done
  argLeftAsSynonym = QueryExecutor::getRefAsSynonym(argLeft);
  argRightAsSynonym = QueryExecutor::getRefAsSynonym(argRight);
  argLeftIsUnderscore = QueryExecutor::isRefUnderscore(argLeft),
  argRightIsUnderscore = QueryExecutor::isRefUnderscore(argRight);
  argLeftAsBasic = QueryExecutor::getRefAsBasic(argLeft);
  argRightAsBasic = QueryExecutor::getRefAsBasic(argRight);

  if (argLeftAsSynonym) {
    // Add entire set of values for variable into the overall constraints
    QueryExecutor::addAllValuesForVariableToConstraints(
        declarations, pkb, argLeftAsSynonym->synonym, db);
  }
  if (argRightAsSynonym) {
    // Add entire set of values for variable into the overall constraints
    QueryExecutor::addAllValuesForVariableToConstraints(
        declarations, pkb, argRightAsSynonym->synonym, db);
  }

  return dispatch();
}

//! Dispatch correct such that case to correct handler based on arg types
bool SuchThatEvaluator::dispatch() {
  if (argLeftAsSynonym && argRightAsBasic) {
    // Case 1:  Follows*(s, 3)
    return dispatchLeftSynonymRightBasic();
  } else if (argRightAsSynonym && argLeftAsBasic) {
    // Case 2:  Follows*(3, s)
    return dispatchRightSynonymLeftBasic();
  } else if (argLeftAsSynonym && argRightIsUnderscore) {
    // Case 3: Follows*(s, _)
    return dispatchLeftVarSynonymRightUnderscore();
  } else if (argRightAsSynonym && argLeftIsUnderscore) {
    // Case 4: Follows*(_, s)
    return dispatchRightVarSynonymLeftUnderscore();
  } else if (argLeftAsSynonym && argRightAsSynonym) {
    // Case 5:  Follows*(s1, s2)
    return dispatchBothVarsSynonyms();
  } else if (argLeftIsUnderscore && argRightIsUnderscore) {
    // Case 6: Follows*(_, _)
    return dispatchDoubleUnderscore();
  } else if (argLeftAsBasic && argRightIsUnderscore) {
    // Case 7: Follows(3, _)
    return dispatchLeftBasicRightUnderscore();
  } else if (argRightAsBasic && argLeftIsUnderscore) {
    // Case 8:  Follows(_, 3)
    return dispatchRightBasicLeftUnderscore();
  } else if (argLeftAsBasic && argRightAsBasic) {
    // Case 9: Follows(2, 3)
    return dispatchBothBasic();
  } else {
    std::cout << "This case should not be triggered\n";
    assert(false);
  }
}

bool SuchThatEvaluator::dispatchLeftSynonymRightBasic() {
  // Case 1:  Follows*(s, 3)
  // Get PKB to give us list of possible s
  auto results = handleLeftSynonymRightBasic(*argRightAsBasic);
  if (results.empty()) return false;
  return db.addToSingleVariableConstraints(argLeftAsSynonym->synonym, results);
}

bool SuchThatEvaluator::dispatchRightSynonymLeftBasic() {
  // Case 2:  Follows*(3, s)
  // Get PKB to give us list of possible s
  auto results = handleRightSynonymLeftBasic(*argLeftAsBasic);
  if (results.empty()) return false;
  return db.addToSingleVariableConstraints(argRightAsSynonym->synonym, results);
}

bool SuchThatEvaluator::dispatchLeftVarSynonymRightUnderscore() {
  // Case 3: Follows*(s, _)
  // Iterate through all possible s, ask PKB if that relation has any results
  // Add those that have results to output
  // E.g. Follows*(3, _)? Follows*(4, _)?

  // Either get all possible DEs, or look up existing set of DEs in DB
  SingleConstraintSet lhs_designentities;
  if (db.hasVariable(argLeftAsSynonym->synonym)) {
    lhs_designentities = db.selectOneAsSet(argLeftAsSynonym->synonym);
  } else {
    lhs_designentities = QueryExecutor::getAllDesignEntityValuesByVarName(
        declarations, pkb, argLeftAsSynonym->synonym);
  }

  SingleConstraintSet results;
  for (auto de : lhs_designentities) {
    if (handleLeftSynonymRightUnderscore(de)) {
      results.insert(de);
    }
  }
  if (results.empty()) return false;
  return db.addToSingleVariableConstraints(argLeftAsSynonym->synonym, results);
}

bool SuchThatEvaluator::dispatchRightVarSynonymLeftUnderscore() {
  // Case 4: Follows*(_, s)
  // Iterate through all possible s, ask PKB if that relation has any results
  // Add those that have results to output
  // E.g. Follows*(_, 3)? Follows*(, 4)?

  // Either get all possible DEs, or look up existing set of DEs in DB
  SingleConstraintSet rhs_designentities;
  if (db.hasVariable(argRightAsSynonym->synonym)) {
    rhs_designentities = db.selectOneAsSet(argRightAsSynonym->synonym);
  } else {
    rhs_designentities = QueryExecutor::getAllDesignEntityValuesByVarName(
        declarations, pkb, argRightAsSynonym->synonym);
  }

  SingleConstraintSet results;
  for (auto de : rhs_designentities) {
    if (handleRightSynonymLeftUnderscore(de)) {
      results.insert(de);
    }
  }
  if (results.empty()) return false;
  return db.addToSingleVariableConstraints(argRightAsSynonym->synonym, results);
}

bool SuchThatEvaluator::dispatchBothVarsSynonyms() {
  // Case 5:  Follows*(s1, s2)
  // Iterate through all possible s1, s2 if that relation has any results
  // Add those that have results to output
  // E.g. Follows*(2, 3)? Follows*(2, 4)? Follows*(3, 3)? ...
  // Either get all possible DEs, or look up existing set of DEs in DB
  SingleConstraintSet lhs_designentities;
  if (db.hasVariable(argLeftAsSynonym->synonym)) {
    lhs_designentities = db.selectOneAsSet(argLeftAsSynonym->synonym);
    // lhs_designentities = QueryExecutor::getAllDesignEntityValuesByVarName(
    //    declarations, pkb, argLeftAsSynonym->synonym);
  } else {
    lhs_designentities = QueryExecutor::getAllDesignEntityValuesByVarName(
        declarations, pkb, argLeftAsSynonym->synonym);
  }
  SingleConstraintSet rhs_designentities;
  if (db.hasVariable(argRightAsSynonym->synonym)) {
    rhs_designentities = db.selectOneAsSet(argRightAsSynonym->synonym);
    // rhs_designentities = QueryExecutor::getAllDesignEntityValuesByVarName(
    //    declarations, pkb, argRightAsSynonym->synonym);
  } else {
    rhs_designentities = QueryExecutor::getAllDesignEntityValuesByVarName(
        declarations, pkb, argRightAsSynonym->synonym);
  }

  PairedConstraintSet results;
  for (auto lhs_de : lhs_designentities) {
    for (auto rhs_de : rhs_designentities) {
      if (handleBothArgsSynonyms(lhs_de, rhs_de)) {
        results.insert({lhs_de, rhs_de});
      }
    }
  }
  if (results.empty()) return false;
  return db.addToPairedVariableConstraints(argLeftAsSynonym->synonym,
                                           argRightAsSynonym->synonym, results);
}

bool SuchThatEvaluator::dispatchDoubleUnderscore() {
  // Case 6: Follows*(_, _)
  // Let PKB tell us if there are any results at all for this relation
  return handleDoubleUnderscore();
}

bool SuchThatEvaluator::dispatchLeftBasicRightUnderscore() {
  // Case 7: Follows(3, _)
  // Ask PKB to answer specific case: any results for this LHS?
  return handleLeftBasicRightUnderscore(*argLeftAsBasic);
}

bool SuchThatEvaluator::dispatchRightBasicLeftUnderscore() {
  // Case 8:  Follows(_, 3)
  // Ask PKB to answer specific case: any results for this RHS?
  return handleRightBasicLeftUnderscore(*argRightAsBasic);
}

bool SuchThatEvaluator::dispatchBothBasic() {
  // Case 9: Follows(2, 3)
  // Ask PKB for direct answer to this fully-realized query
  return handleBothArgsBasic(*argLeftAsBasic, *argRightAsBasic);
}