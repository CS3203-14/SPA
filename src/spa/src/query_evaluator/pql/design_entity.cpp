#include "query_evaluator/pql/design_entity.h"
#include "query_evaluator/core/exceptions.h"
#include "query_evaluator/pql/util.h"

#include <stdexcept>

#include <map>

namespace QE {
// Hard to use unordered_map even though faster
// - need to define specialized hash for enum class
std::map<DesignEntity, std::string> designEntityToStringMap({
    {DesignEntity::STMT, "stmt"},
    {DesignEntity::READ, "read"},
    {DesignEntity::PRINT, "print"},
    {DesignEntity::CALL, "call"},
    {DesignEntity::WHILE, "while"},
    {DesignEntity::IF, "if"},
    {DesignEntity::ASSIGN, "assign"},
    {DesignEntity::VARIABLE, "variable"},
    {DesignEntity::CONSTANT, "constant"},
    {DesignEntity::PROCEDURE, "procedure"},
});

auto stringToDesignEntityMap =
    swapPairs<DesignEntity, std::string>(designEntityToStringMap);

DesignEntity getDesignEntity(std::string& designentity_string) {
  try {
    return stringToDesignEntityMap.at(designentity_string);
  } catch (const std::out_of_range& oor) {
    throw PQLParseException("Cannot find a Design Entity called " +
                            designentity_string);
  }
}

// An exception here would be a programmer error - no need to handle differently
std::string getDesignEntityString(DesignEntity de) {
  return designEntityToStringMap.at(de);
}

}  // namespace QE