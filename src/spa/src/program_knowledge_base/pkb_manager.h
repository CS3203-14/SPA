#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "program_knowledge_base/pkb_definitions.h"
#include "program_knowledge_base/pkb_preprocessor.h"
#include "program_knowledge_base/pkb_storage.h"
#include "structs/node.h"
#include "utils/utils.h"

using namespace Utils;

namespace PKB {

class PKBManager {
 private:
  std::shared_ptr<PKBStorage> pkb_storage = std::make_shared<PKBStorage>();

 public:
  PKBManager(const std::shared_ptr<ProcedureNode> ast);
  ~PKBManager();

  // API exposed to Query Manager

  // get design entities
  std::vector<Variable> getVariableList();
  std::vector<Line> getAssignList();
  std::vector<Line> getStatementList();
  std::vector<Line> getPrintList();
  std::vector<Line> getReadList();
  std::vector<Line> getWhileList();
  std::vector<Line> getIfList();
  std::vector<Constant> getConstantList();
  std::vector<Procedure> getProcedureList();

  bool isVariableExists(Variable);
  bool isAssignExists(Line);
  bool isStatementExists(Line);
  bool isPrintExists(Line);
  bool isReadExists(Line);
  bool isWhileExists(Line);
  bool isIfExists(Line);
  bool isConstantExists(Constant);
  bool isProcedureExists(Procedure);

  // get mapping
  ParentLine getParentLine(const ChildLine);
  std::vector<ChildLine> getChildLine(const ParentLine);
  std::vector<ParentLine> getParentLineS(const ChildLine);
  std::vector<ChildLine> getChildLineS(const ParentLine);

  LineAfter getFollowingLine(const LineBefore);
  LineBefore getBeforeLine(const LineAfter);
  std::vector<LineAfter> getFollowingLineS(const LineBefore);
  std::vector<LineBefore> getBeforeLineS(const LineAfter);

  Procedure getProcedureModifiesVar(const Variable);
  Line getLineModifiesVar(const Variable);
  std::vector<Variable> getVarModifiedByProcedure(const Procedure);
  Variable getVarModifiedByLine(const Line);

  std::vector<Procedure> getProcedureUsesVar(const Variable);
  std::vector<Line> getLineUsesVar(const Variable);
  std::vector<Variable> getVarUsedByProcedure(const Procedure);
  std::vector<Variable> getVarUsedByLine(const Line);

  // is relationship valid
  bool isLineFollowLine(const LineBefore, const LineAfter);
  bool isLineParentLine(const ParentLine, const ChildLine);
  bool isProcedureModifiesVar(const Procedure, const Variable);
  bool isLineModifiesVar(const Line, const Variable);
  bool isProcedureUsesVar(const Procedure, const Variable);
  bool isLineUsesVar(const Line, const Variable);
};

}  // namespace PKB
