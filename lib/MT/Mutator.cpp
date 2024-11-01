#include "klee/MT/Mutator.h"
#include "klee/Expr/Expr.h"
#include "klee/Expr/Constraints.h"
#include "klee/Expr/ExprBuilder.h"
#include "klee/Support/ErrorHandling.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace klee;

Mutator::Mutator(std::string operators) {
  //Try to open the file included in the operators string
  //std::cout << "Operators: " << operators << std::endl;
  std::ifstream file(operators);
  std::string line;
  if(file.is_open()) {
    while(getline(file, line)) {
      mutationOperators.push_back(line);
    }
    file.close();
  } else {
    klee_warning("MISE: Could not open file with mutation operators. Using default operators.");
    mutationOperators = {"ROR", "ARB", "AIU", "AIS", "COR", "COI"};
  }
}

//Function that return the current timestamp as a string
std::string currentDateTime() {
  time_t now = time(0);
  struct tm tstruct;
  char buf[80];
  tstruct = *localtime(&now);
  strftime(buf, sizeof(buf), "%d-%m-%Y.%X", &tstruct);
  return buf;
}

void Mutator::printOriginalConstraints() {
  originalConstraints.printConstraints();
}

std::vector<std::pair<ConstraintSet, std::string>> Mutator::mutate(ConstraintSet cs) {
  std::vector<std::pair<ConstraintSet, std::string>> res; // Ahora guarda ConstraintSet + tipo de mutación
  std::vector<ConstraintSet> aux;

    //Explanation on missing mutation operators:
  //ARS: There is not increment and decrement operator in Query expressions.
  //ASR: There is not coumpound assignment operator in Query expressions.
  //ARU: As there is not negative operator in Query expressions, we cannot apply this mutation.
  //     although its behaviour would be the exact same as AIU.
  //ADS: There is not increment and decrement operator in Query expressions.
  //COD: KLEE will never generate constraints with explicit negations, so we cannot apply this mutation.
  //LOR: Its behavior is the same as COR, so we will not apply this mutation.

  for(auto &op : mutationOperators) {
    if(op == "ROR") {
      aux = apply_ROR(cs);
      for (const auto &mutant : aux)
        res.emplace_back(mutant, "ROR"); // Guarda el mutante con la etiqueta "ROR"
      klee_message("MISE: Generated %ld ROR mutants at %s.", aux.size(), currentDateTime().c_str());

    } else if(op == "ARB") {
      aux = apply_ARB(cs);
      for (const auto &mutant : aux)
        res.emplace_back(mutant, "ARB"); // Guarda el mutante con la etiqueta "ARB"
      klee_message("MISE: Generated %ld ARB mutants at %s.", aux.size(), currentDateTime().c_str());

    } else if(op == "AIU") {
      aux = apply_AIU(cs);
      for (const auto &mutant : aux)
        res.emplace_back(mutant, "AIU"); // Guarda el mutante con la etiqueta "AIU"
      klee_message("MISE: Generated %ld AIU mutants at %s.", aux.size(), currentDateTime().c_str());

    } else if(op == "AIS") {
      aux = apply_AIS(cs, "Add");
      for (const auto &mutant : aux)
        res.emplace_back(mutant, "AIS_Add"); // Guarda el mutante con la etiqueta "AIS_Add"
      
      aux = apply_AIS(cs, "Sub");
      for (const auto &mutant : aux)
        res.emplace_back(mutant, "AIS_Sub"); // Guarda el mutante con la etiqueta "AIS_Sub"
      
      klee_message("MISE: Generated %ld AIS mutants at %s.", aux.size()*2, currentDateTime().c_str());

    } else if(op == "COR") {
      aux = apply_COR(cs);
      for (const auto &mutant : aux)
        res.emplace_back(mutant, "COR"); // Guarda el mutante con la etiqueta "COR"
      klee_message("MISE: Generated %ld COR mutants at %s.", aux.size(), currentDateTime().c_str());

    } else if(op == "COI") {
      aux = apply_COI(cs);
      for (const auto &mutant : aux)
        res.emplace_back(mutant, "COI"); // Guarda el mutante con la etiqueta "COI"
      klee_message("MISE: Generated %ld COI mutants at %s.", aux.size(), currentDateTime().c_str());

    } else {
      klee_warning("MISE: Operator %s not supported.", op.c_str());
    }
  }

  klee_message("MISE: Total mutants generated: %ld", res.size());

  return res;
}

std::vector<ConstraintSet> Mutator::applyMutations(ConstraintSet cs, std::string oldOp, std::string newOp) {

  std::vector<ConstraintSet> res;
  ref<Expr> auxiliar_constraint;

  //res.push_back(cs);
  for(auto &constraint : cs) {
    int mutations = countMutations(oldOp, constraint);
    for(int i = 0; i < mutations; i++) {
      ConstraintSet mutatedConstraint;
      for(auto &subConstraint : cs) {
        if(subConstraint == constraint) {
          //std::cout << "Executor.cpp4957 newOp: " << newOp << std::endl;
          auxiliar_constraint = mutate(constraint, oldOp, newOp, 0, i);
        } else auxiliar_constraint = subConstraint;

        mutatedConstraint.push_back(auxiliar_constraint);
      }
      res.push_back(mutatedConstraint);
    }
  }

  return res;
}

  int Mutator::countMutations(std::string op, ConstraintSet cs) {
    int i = 0;
    for(auto& constraint : cs) {
      i += countMutations(op, constraint);
    }
    return i;
  }

  int Mutator::countMutations_AIS(ConstraintSet cs) {
    int i = 0;
    for(auto& constraint : cs) {
      i += countMutations_AIS(constraint);
    }
    return i;
  }

  int Mutator::countMutations_AIU(ConstraintSet cs) {
    int i = 0;
    for(auto& constraint : cs) {
      i += countMutations_AIU(constraint);
    }
    return i;
  }


  int Mutator::countMutations(std::string op, ref<Expr> e) {
    if(e->getNumKids() == 0 && e->printKindtoString() == "Constant" && op == "Constant" && is_number(e->printValuetoString())){
      return 1;
    } else if(e->getNumKids() == 0 || e->getNumKids() != 2) {
      return 0;
    } else {
      if(e->printKindtoString() == op) {
        return 1 + countMutations(op, e->getKid(0)) + countMutations(op, e->getKid(1));
      } else {
        return countMutations(op, e->getKid(0)) + countMutations(op, e->getKid(1));
      }
    }
  }

  int Mutator::countMutations_AIS(ref<Expr> e) {
    if(e->getNumKids() == 2) {
      if( is_number(e->getKid(0)->printValuetoString()) || is_number(e->getKid(1)->printValuetoString()) ) { //This means it is an arithmetic op.
        return 1;
      } else {
        return countMutations_AIS(e->getKid(0)) + countMutations_AIS(e->getKid(1));
      }
    } else return 0;
  }

  int Mutator::countMutations_AIU(ref<Expr> e) {

    if(e->getNumKids() == 2) {
      std::string left = e->getKid(0)->printValuetoString();
      std::string leftType = e->getKid(0)->printKindtoString();
      std::string right = e->getKid(1)->printValuetoString();
      std::string rightType = e->getKid(0)->printKindtoString();

      if( ((is_number(left) && left != "0") && leftType == "Constant")
       || ((is_number(right) && right != "0") && rightType == "Constant") ) {
        return 1;
      } else {
        return countMutations_AIU(e->getKid(0)) + countMutations_AIU(e->getKid(1));
      }
    } else return 0;
  }

  //AIS need special treatment
  std::vector<ConstraintSet> Mutator::apply_AIS(ConstraintSet cs, std::string op) {

    std::vector<ConstraintSet> res;
    ref<Expr> auxiliar_constraint;

    //res.push_back(cs);
    for(auto &constraint : cs) {
      int mutations = countMutations_AIS(constraint); //MISE - FIX: countMutations_AIS(constraint);
      for(int i = 0; i < mutations; i++) {
        ConstraintSet mutatedConstraint;
        for(auto &subConstraint : cs) {
          if(subConstraint == constraint) {
            auxiliar_constraint = mutate_AIS(constraint, 0, i, op);
          } else auxiliar_constraint = subConstraint;

          mutatedConstraint.push_back(auxiliar_constraint);
        }
        res.push_back(mutatedConstraint);
      }
    }

    return res;
  }

  std::vector<ConstraintSet> Mutator::apply_AIU(ConstraintSet cs) {

    std::vector<ConstraintSet> res;
    ref<Expr> auxiliar_constraint;

    //res.push_back(cs);
    for(auto &constraint : cs) {
      int mutations = countMutations_AIU(constraint);
      for(int i = 0; i < mutations; i++) {
        ConstraintSet mutatedConstraint;
        for(auto &subConstraint : cs) {
          if(subConstraint == constraint) {
            auxiliar_constraint = mutate_AIU(constraint, 0, i);
          } else auxiliar_constraint = subConstraint;

          mutatedConstraint.push_back(auxiliar_constraint);
        }
        res.push_back(mutatedConstraint);
      }
    }

    return res;
  }

  ref<Expr> Mutator::mutate(ref<Expr> e, std::string oldOp, std::string newOp, int currentLevel, int objectiveLevel) {
    std::string currentOp = e->printKindtoString();

    if(currentOp == oldOp) { //base case, found mutation
      if(currentLevel == objectiveLevel) {
        if(e->getKid(0)->printWidthtoString() == e->getKid(1)->printWidthtoString()) {
          return build(e->getKid(0), e->getKid(1), newOp);
        }
      } else currentLevel++;
    }

    if(e->getNumKids() == 2 && !excludedOperators(e)) {
      return build(
        mutate(e->getKid(0), oldOp, newOp, currentLevel, objectiveLevel),
        mutate(e->getKid(1), oldOp, newOp, currentLevel, objectiveLevel),
        currentOp
      );
    } else if (e->getNumKids() == 1 && !excludedOperators(e)) {
      return mutate(e->getKid(0), oldOp, newOp, currentLevel, objectiveLevel);
    } else return e; //base case, mutation not found

  }

  //AIS and AIU need special treatment.
  ref<Expr> Mutator::mutate_AIS(ref<Expr> e, int currentLevel, int objectiveLevel, std::string op) {
      ref<Expr> left = e->getKid(0), right = e->getKid(1), intValue;
      std::string currentOp = e->printKindtoString();
      std::string Constant = "Constant";
      std::string Forbidden = "Extract";



      if(e->getNumKids() == 2) {
        if( is_number(left->printValuetoString()) || is_number(right->printValuetoString()) ) { //This means it is an arithmetic op.
          if( currentLevel == objectiveLevel ) {

            ExprBuilder *Builder = 0;
            Builder = createDefaultExprBuilder(); //this will build the value

            if(is_number(left->printValuetoString()) && right->printKindtoString() == Constant \
               && Forbidden.find(right->printKindtoString()) == std::string::npos){
              //In this case, apply operator to the left side
              //(Slt (ReadLSB w32 0 a) 1)) -> (Slt (Add (ReadLSB w32 0 a) 1) 1)
              intValue = Builder->Constant((uint64_t)(1), e->getKid(1)->getWidth());
              return build(e->getKid(0), build(e->getKid(1), intValue, op), currentOp);

            } else if(left->printKindtoString() == Constant && Forbidden.find(left->printKindtoString()) == std::string::npos){
              //In this case, apply operator to the right side
              intValue = Builder->Constant((uint64_t)(1), e->getKid(0)->getWidth());
              return build(build(e->getKid(0), intValue, op), e->getKid(1), currentOp);
            }
          } else currentLevel++;
        }
        left = mutate_AIS(left, currentLevel, objectiveLevel, op);
        right = mutate_AIS(right, currentLevel, objectiveLevel, op);

        return build(left, right, currentOp);
        
        } else if (e->getNumKids() == 1 && !excludedOperators(e)) {
        return mutate_AIS(e->getKid(0), currentLevel, objectiveLevel, op);
      } else return e;

    }

  ref<Expr> Mutator::mutate_AIU(ref<Expr> e, int currentLevel, int objectiveLevel) {
      ref<Expr> left = e->getKid(0), right = e->getKid(1), intValue;
      std::string currentOp = e->printKindtoString();
      std::string op = "Sub";
      std::string Constant = "Constant";
      std::string Zero = "0";

      if(e->getNumKids() == 2) {
        if( is_number(left->printValuetoString()) || is_number(right->printValuetoString()) ) { //This means it is an arithmetic op.
          if( currentLevel == objectiveLevel ) {

            ExprBuilder *Builder = 0;
            Builder = createDefaultExprBuilder(); //this will build the value

            if(is_number(left->printValuetoString()) && 
                         left->printValuetoString() != Zero &&
                         right->printKindtoString() == Constant) {
              //In this case, apply operator to the left side
              //(Slt (ReadLSB w32 0 a) 1)) -> (Slt (Sub 0 (ReadLSB w32 0 a)) 1)
              intValue = Builder->Constant((uint64_t)(0), e->getKid(1)->getWidth());
              return build(e->getKid(0), build(intValue, e->getKid(1), op), currentOp);

            } else if(right->printValuetoString() != Zero &&
                      left->printKindtoString() == Constant) {
              //In this case, apply operator to the right side
              intValue = Builder->Constant((uint64_t)(0), e->getKid(0)->getWidth());
              return build(build(intValue, e->getKid(0), op), e->getKid(1), currentOp);
            }
          } else currentLevel++;
        }
        left = mutate_AIU(left, currentLevel, objectiveLevel);
        right = mutate_AIU(right, currentLevel, objectiveLevel);

        return build(left, right, currentOp);
        
        } else if (e->getNumKids() == 1 && !excludedOperators(e)) {
        return mutate_AIU(e->getKid(0), currentLevel, objectiveLevel);
      } else return e;

  }

  ref<Expr> Mutator::build(ref<Expr> LHS, ref<Expr> RHS, std::string newKind) {
  
    ref<Expr> result = LHS;

    ExprBuilder *Builder = 0;
    Builder = createDefaultExprBuilder();

    // Compare
    if(newKind == "Eq") result = Builder->Eq(LHS, RHS);
    else if(newKind == "Ne") result = build(Builder->Eq(LHS, RHS), RHS, "Not");
    else if(newKind == "Ult") result = Builder->Ult(RHS, LHS);
    else if(newKind == "Ule") result = Builder->Ule(RHS, LHS);
    else if(newKind == "Ugt") result = Builder->Ult(LHS, RHS);
    else if(newKind == "Uge") result = Builder->Ule(LHS, RHS);
    else if(newKind == "Slt") result = Builder->Slt(LHS, RHS);
    else if(newKind == "Sle") result = Builder->Sle(LHS, RHS);
    else if(newKind == "Sgt") result = Builder->Slt(RHS, LHS);
    else if(newKind == "Sge") result = Builder->Sle(RHS, LHS);
    //Arithmetic
    else if(newKind == "Add") result = Builder->Add(LHS, RHS);
    else if(newKind == "Sub") result = Builder->Sub(LHS, RHS);
    else if(newKind == "Mul") result = Builder->Mul(LHS, RHS);
    else if(newKind == "UDiv") result = Builder->UDiv(LHS, RHS);
    else if(newKind == "SDiv") result = Builder->SDiv(LHS, RHS);
    else if(newKind == "URem") result = Builder->URem(LHS, RHS);
    else if(newKind == "SRem") result = Builder->SRem(LHS, RHS);
    //Logical-binary
    else if (newKind == "And") result = Builder->And(LHS, RHS);
    else if (newKind == "Or") result = Builder->Or(LHS, RHS);
    else if (newKind == "Xor") result = Builder->Xor(LHS, RHS);
    else if (newKind == "Shl") result = Builder->Shl(LHS, RHS);
    else if (newKind == "LShr") result = Builder->LShr(LHS, RHS);
    else if (newKind == "AShr") result = Builder->AShr(LHS, RHS);
    //Unary-Experimental
    else if (newKind == "NotOptimized") result = Builder->NotOptimized(LHS);
    else if (newKind == "Not") result = Builder->Not(LHS);
    else if (newKind == "ZExt") result = Builder->ZExt(LHS, LHS->getWidth());
    else if (newKind == "SExt") result = Builder->SExt(LHS, LHS->getWidth());
    //Other operators
    else if (newKind == "Concat") result = Builder->Concat(LHS, RHS); //result = Builder->Concat(LHS, RHS);
    else if (newKind == "Extract") klee_warning("kevin: detected EXTRACT expr., which shouldnt happen..."); //result = Builder->Extract(LHS, 0, LHS->getWidth());
    else if (newKind == "Constant") klee_warning("kevin: detected CONSTANT expr., which shouldnt happen..."); //Constant (const llvm::APInt &Value)
    else if (newKind == "Select") klee_warning("kevin: detected SELECT expr., which shouldnt happen..."); //Select (const ref< Expr > &Cond, const ref< Expr > &LHS, const ref< Expr > &RHS)
    else if (newKind == "Read") klee_warning("kevin: detected READ expr., which shouldnt happen..."); //Read (const UpdateList &Updates, const ref< Expr > &Index)
    //Special Negation Op.
    else if (newKind == "NOTSlt") result = build(Builder->Slt(LHS, RHS), RHS, "Not"); //Se descarta RHS y niega la expresion recibida
    else if (newKind == "NOTSle") result = build(Builder->Sle(LHS, RHS), RHS, "Not");
    else if (newKind == "NOTAnd") result = build(Builder->And(LHS, RHS), RHS, "Not");
    else if (newKind == "NOTOr") result = build(Builder->Or(LHS, RHS), RHS, "Not");
    else if (newKind == "NOTXor") result = build(Builder->Xor(LHS, RHS), RHS, "Not");
    else {
      klee_warning("kevin: operator not supported");
    }

    return result;
  }

  bool Mutator::excludedOperators(ref<Expr> node) {
    std::string type = node->printKindtoString();

    return (type == "Concat" ||
            type == "Extract" ||
            type == "Constant" ||
            type == "Select" ||
            type == "Read" );
  }

  //Unique operator: it doesnt cover any other
  std::vector<ConstraintSet> Mutator::apply_ROR(ConstraintSet cs) {
    std::vector<ConstraintSet> res;
    std::vector<ConstraintSet> aux;

    /*As KLEE dont generate constraints with >, >= or !=,
    we generate mutants supposing that C/C++ code could
    be using them */

    //< -> >=
    //aux = applyMutations(cs, "Sle", "Sgt");
    //res.insert(res.end(), aux.begin(), aux.end());

    //<= -> >
    //aux = applyMutations(cs, "Slt", "Sge");
    //res.insert(res.end(), aux.begin(), aux.end());

    //<= -> <
    aux = applyMutations(cs, "Slt", "Sle");
    res.insert(res.end(), aux.begin(), aux.end());

    //< -> <=
    aux = applyMutations(cs, "Sle", "Slt");
    res.insert(res.end(), aux.begin(), aux.end());

    //== -> !=
    aux = applyMutations(cs, "Eq", "Ne");
    res.insert(res.end(), aux.begin(), aux.end());

    return res;
  }

  //It covers ARS and ASR
  std::vector<ConstraintSet> Mutator::apply_ARB(ConstraintSet cs) {
    std::vector<ConstraintSet> res;
    std::vector<ConstraintSet> aux;

    aux = applyMutations(cs, "Add", "Sub");
    res.insert(res.end(), aux.begin(), aux.end());

    //aux = applyMutations(cs, "Sub", "Add");
    //res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "Mul", "SDiv");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "SDiv", "Mul");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "SRem", "SDiv");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "URem", "UDiv");
    res.insert(res.end(), aux.begin(), aux.end());

    return res;
  }

  // It covers LOR
  std::vector<ConstraintSet> Mutator::apply_COR(ConstraintSet cs) {
    std::vector<ConstraintSet> res;
    std::vector<ConstraintSet> aux;

    aux = applyMutations(cs, "And", "Or");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "Or", "And");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "Xor", "Or");
    res.insert(res.end(), aux.begin(), aux.end());

    return res;
  }

  //It covers COD
  std::vector<ConstraintSet> Mutator::apply_COI(ConstraintSet cs) {
    std::vector<ConstraintSet> res;
    std::vector<ConstraintSet> aux;

    //MISE: Negates all conditions. We added five special cases for this.

    aux = applyMutations(cs, "Slt", "NOTSlt");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "Sle", "NOTSle");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "Eq", "Ne");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "And", "NOTAnd");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "Or", "NOTOr");
    res.insert(res.end(), aux.begin(), aux.end());

    aux = applyMutations(cs, "Xor", "NOTXor");
    res.insert(res.end(), aux.begin(), aux.end());
    
    return res;
  }

  bool Mutator::is_number(const std::string& s) {
        return !s.empty() && std::find_if(s.begin(),
            s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }
