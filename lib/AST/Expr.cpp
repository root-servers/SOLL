// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#include "soll/AST/Expr.h"
#include "soll/AST/Decl.h"
#include "soll/AST/Stmt.h"

namespace soll {

bool Identifier::isStateVariable() const {
  if (auto *D = dynamic_cast<const VarDecl *>(getCorrespondDecl())) {
    return D->isStateVariable();
  }
  return false;
}

bool CastExpr::isStateVariable() const {
  return getSubExpr()->isStateVariable();
}

bool MemberExpr::isStateVariable() const {
  return getBase()->isStateVariable();
}

bool IndexAccess::isStateVariable() const {
  return getBase()->isStateVariable();
}
bool ParenExpr::isStateVariable() const {
  return getSubExpr()->isStateVariable();
}

///
/// TupleExpr
///
std::vector<Expr *> TupleExpr::getComponents() {
  std::vector<Expr *> Comps;
  for (auto &Ele : Components)
    Comps.emplace_back(Ele.get());
  return Comps;
}

std::vector<const Expr *> TupleExpr::getComponents() const {
  std::vector<const Expr *> Comps;
  for (auto &Ele : Components)
    Comps.emplace_back(Ele.get());
  return Comps;
}

///
/// CallExpr
///
std::vector<Expr *> CallExpr::getArguments() {
  std::vector<Expr *> Args;
  if (isNamedCall()) {
    auto FnTy = dynamic_cast<const FunctionType *>(CalleeExpr->getType().get());
    std::unordered_map<std::string, size_t> ParamNamesIndex;
    size_t ParamSize = 0;
    for (const auto &ParamName : *FnTy->getParamNames()) {
      ParamNamesIndex[ParamName] = ParamSize++;
    }
    Args.resize(ParamSize);
    for (size_t i = 0; i < ParamSize; ++i) {
      Args[ParamNamesIndex.at(Names->at(i))] = Arguments.at(i).get();
    }
  } else {
    for (auto &Arg : Arguments)
      Args.emplace_back(Arg.get());
  }
  return Args;
}

std::vector<const Expr *> CallExpr::getArguments() const {
  std::vector<const Expr *> Args;
  if (isNamedCall()) {
    auto FnTy = dynamic_cast<const FunctionType *>(CalleeExpr->getType().get());
    std::unordered_map<std::string, size_t> ParamNamesIndex;
    size_t ParamSize = 0;
    for (const auto &ParamName : *FnTy->getParamNames()) {
      ParamNamesIndex[ParamName] = ParamSize++;
    }
    Args.resize(ParamSize);
    for (size_t i = 0; i < ParamSize; ++i) {
      Args[ParamNamesIndex.at(Names->at(i))] = Arguments.at(i).get();
    }
  } else {
    for (auto &Arg : Arguments)
      Args.emplace_back(Arg.get());
  }
  return Args;
}

BinaryOperator::BinaryOperator(SourceRange L, ExprPtr &&lhs, ExprPtr &&rhs,
                               Opcode opc)
    : Expr(L), Opc(opc) {
  SubExprs[LHS] = std::move(lhs);
  SubExprs[RHS] = std::move(rhs);
  if (this->isAssignmentOp())
    setValueKind(ValueKind::VK_LValue);
}

///
/// Identifier
///
Identifier::Identifier(const Token &T)
    : Expr(SourceRange(T.getLocation(), T.getEndLoc()), ValueKind::VK_LValue,
           nullptr),
      T(T), D() {}

Identifier::Identifier(const Token &T, Decl *D)
    : Expr(SourceRange(T.getLocation(), T.getEndLoc()), ValueKind::VK_LValue,
           nullptr),
      T(T), D(D) {
  updateTypeFromCurrentDecl();
}

Identifier::Identifier(const Token &T, TypePtr Ty)
    : Expr(SourceRange(T.getLocation(), T.getEndLoc()), ValueKind::VK_LValue,
           std::move(Ty)),
      T(T), D() {}

Identifier::Identifier(const Token &T, SpecialIdentifier D, TypePtr Ty)
    : Expr(SourceRange(T.getLocation(), T.getEndLoc()), ValueKind::VK_LValue,
           std::move(Ty)),
      T(T), D(D) {}

void Identifier::updateTypeFromCurrentDecl() {
  Decl *D = getCorrespondDecl();
  if (auto VD = dynamic_cast<const VarDecl *>(D)) {
    setType(VD->GetType());
  } else if (auto FD = dynamic_cast<const FunctionDecl *>(D)) {
    setType(FD->getType());
  } else if (dynamic_cast<const EventDecl *>(D)) {
    setType(nullptr);
  } else {
    assert(false && "unknown decl");
    __builtin_unreachable();
  }
}

} // namespace soll
