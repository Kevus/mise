#include "klee/MT/Mutator.h"
#include "klee/Expr/Expr.h"
#include "klee/Expr/Constraints.h"
#include "klee/Expr/ExprBuilder.h"
#include "klee/Support/ErrorHandling.h"

#include <iostream>
#include <vector>
#include <string>

using namespace klee;

void Mutator::printOriginalConstraints() {
  originalConstraints.printConstraints();
}

std::vector<ConstraintSet> Mutator::mutate(ConstraintSet cs) {
  std::vector<ConstraintSet> res;// = apply_ROR(cs);
  std::vector<ConstraintSet> aux;

  //ROR operator
  aux = apply_ROR(cs);
  res.insert(res.end(), aux.begin(), aux.end());
  klee_message("MISE: Generated %ld ROR mutants.", aux.size());

  //ARB operator
  aux = apply_ARB(cs);
  res.insert(res.end(), aux.begin(), aux.end());
  klee_message("MISE: Generated %ld ARB mutants.", aux.size());

  //COR operator
  aux = apply_COR(cs);
  res.insert(res.end(), aux.begin(), aux.end());
  klee_message("MISE: Generated %ld COR mutants.", aux.size());

  return res;
}

/*std::vector<ConstraintSet> Mutator::mutate(ConstraintSet cs, int type) {
  std::vector<ConstraintSet> res;
  std::vector<ConstraintSet> aux;

  //Original constraint
  //res.push_back(cs);

  switch(type) {
    case 1: //1 is ROR
      aux = apply_ROR(cs);
      res.insert(res.end(), aux.begin(), aux.end());

      std::cout << "Generated " << aux.size() << " ROR mutants.\n";
      break;

    case 2: //2 is ARB
      aux = apply_ARB(cs);
      res.insert(res.end(), aux.begin(), aux.end());

      std::cout << "Generated " << aux.size() << " ARB mutants.\n";
      break;

    case 3: //3 is AIS
      aux = apply_AIS(cs, "Add");
      res.insert(res.end(), aux.begin(), aux.end());

      aux = apply_AIS(cs, "Sub");
      res.insert(res.end(), aux.begin(), aux.end());

      std::cout << "Generated " << aux.size()*2 << " AIS mutants.\n";
      break;

    case 4: //4 is AIU
      aux = apply_AIU(cs);
      res.insert(res.end(), aux.begin(), aux.end());

      std::cout << "Generated " << aux.size() << " AIU mutants.\n";
      break;

    case 5: //5 is COI
      aux = apply_COI(cs);
      res.insert(res.end(), aux.begin(), aux.end());

      std::cout << "Generated " << aux.size() << " COI mutants.\n";
      break;

    case 6: //6 is COR
      aux = apply_COR(cs);
      res.insert(res.end(), aux.begin(), aux.end());

      std::cout << "Generated " << aux.size() << " COR mutants.\n";
      break;

    default: std::cout << "ERROR: bad mutant type";
  }

  return res;
}

std::vector<ConstraintSet> Mutator::mutate(int type) {
  return mutate(originalConstraints, type);
}*/

/*std::multimap<std::string, std::vector<ConstraintSet>> Mutator::mutate(ConstraintSet cs) {
  std::multimap<std::string, std::vector<ConstraintSet>> result;
  std::vector<ConstraintSet> aux;

    aux.push_back(cs);
    result.insert(std::pair<std::string, std::vector<ConstraintSet>>("", aux));
    result.insert(std::pair<std::string, std::vector<ConstraintSet>>(".ROR", mutate(cs, 1)));
    result.insert(std::pair<std::string, std::vector<ConstraintSet>>(".ARB", mutate(cs, 2)));
    result.insert(std::pair<std::string, std::vector<ConstraintSet>>(".AIS", mutate(cs, 3)));
    result.insert(std::pair<std::string, std::vector<ConstraintSet>>(".AIU", mutate(cs, 4)));
    result.insert(std::pair<std::string, std::vector<ConstraintSet>>(".COI", mutate(cs, 5)));
    result.insert(std::pair<std::string, std::vector<ConstraintSet>>(".COR", mutate(cs, 6)));
  return result;
}

std::multimap<std::string, std::vector<ConstraintSet>> Mutator::mutate() {
  return mutate(originalConstraints);
}*/

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

std::vector<ConstraintSet> Mutator::applyMutations(std::string oldOp, std::string newOp) {
  return applyMutations(originalConstraints, oldOp, newOp);
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

  int Mutator::countMutations(std::string op) {
    return countMutations(op, originalConstraints);
  }

  int Mutator::countMutations_AIS() {
    return countMutations_AIS(originalConstraints);
  }

  int Mutator::countMutations_AIU() {
    return countMutations_AIU(originalConstraints);
  }

  //kevin: private auxiliar method
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
       || ((is_number(right) && right != "0") && rightType == "Constant") ) { //This means it is an arithmetic op and its not 0.
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
      int mutations = countMutations_AIS(constraint);
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

    //std::cout << "Current -> " << currentOp << " Old -> " << oldOp << " New -> " << newOp << std::endl;

    if(currentOp == oldOp) { //base case, found mutation
      if(currentLevel == objectiveLevel) {
        //Añadido para el tema de los asertos...
        if(e->getKid(0)->printWidthtoString() == e->getKid(1)->printWidthtoString()) {
          //std::cout << "OldOP: " << oldOp << " NewOp: " << newOp << std::endl;
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

  //AIS and AIU need special treatment. TO-DO: refactor code to unify both functions
  ref<Expr> Mutator::mutate_AIS(ref<Expr> e, int currentLevel, int objectiveLevel, std::string op) {
      ref<Expr> left = e->getKid(0), right = e->getKid(1), intValue;
      std::string currentOp = e->printKindtoString();

      if(e->getNumKids() == 2) {
        if( is_number(left->printValuetoString()) || is_number(right->printValuetoString()) ) { //This means it is an arithmetic op.
          if( currentLevel == objectiveLevel ) {

            ExprBuilder *Builder = 0;
            Builder = createDefaultExprBuilder(); //this will build the value
            intValue = Builder->Constant((uint64_t)(1), Expr::Int32);

            if(is_number(left->printValuetoString()) &&
               right->printKindtoString() == "Constant") {

              if(!(intValue->getWidth()==right->getWidth() && "type mismatch")) {
                  return build(left, right, currentOp);
              } else return build(e->getKid(0), build(e->getKid(1), intValue, op), currentOp);
            } else if (is_number(right->printValuetoString()) &&
              left->printKindtoString() == "Constant"){
              if(!(intValue->getWidth()==right->getWidth() && "type mismatch")) {
                  return build(left, right, currentOp);
              } else return build(build(e->getKid(0), intValue, op), e->getKid(1), currentOp);
            }
          } else currentLevel++;
        }
        left = mutate_AIS(left, currentLevel, objectiveLevel, op);
        right = mutate_AIS(right, currentLevel, objectiveLevel, op);

        if(!(left->getWidth()==right->getWidth() && "type mismatch"))
          return build(e->getKid(0), e->getKid(1), currentOp);
        else return build(left, right, currentOp);
            } else if (e->getNumKids() == 1 && !excludedOperators(e)) {
        return mutate_AIS(e->getKid(0), currentLevel, objectiveLevel, op);
      } else return e;

    }

  ref<Expr> Mutator::mutate_AIU(ref<Expr> e, int currentLevel, int objectiveLevel) {
    ref<Expr> left = e->getKid(0), right = e->getKid(1), intValue;
    std::string currentOp = e->printKindtoString();
    int num;

    if(e->getNumKids() == 2) {
      if( is_number(left->printValuetoString()) || is_number(right->printValuetoString()) ) { //This means it is an arithmetic op.
        if( currentLevel == objectiveLevel ) {

          ExprBuilder *Builder = 0;
          Builder = createDefaultExprBuilder(); //this will build the value

          if(is_number(left->printValuetoString()) &&
             right->printKindtoString() == "Constant") {
            std::stringstream auxNum(left->printValuetoString());
            auxNum >> num;
            intValue = Builder->Constant((uint64_t)(-num), Expr::Int32);

            if(!(intValue->getWidth()==right->getWidth() && "type mismatch") && (currentOp == "Sle" || currentOp == "Ule")) {
              return build(left, right, currentOp);
            } else return build(intValue, right, currentOp);
          } else if (is_number(right->printValuetoString()) &&
            left->printKindtoString() == "Constant"){
            std::stringstream auxNum(right->printValuetoString());
            auxNum >> num;
            intValue = Builder->Constant((uint64_t)(-num), Expr::Int32);

            if(!(intValue->getWidth()==left->getWidth() && "type mismatch") && (currentOp == "Sle" || currentOp == "Ule")) {
              return build(left, right, currentOp);
            } else return build(left, intValue, currentOp);
          }
        } else currentLevel++;
      }
      left = mutate_AIU(left, currentLevel, objectiveLevel);
      right = mutate_AIU(right, currentLevel, objectiveLevel);

      //std::cout << "Voy a hacer build de " << currentOp << std::endl;
      //std::cout << "left = " << left->printValuetoString() << "\nright = " << right->printValuetoString() << std::endl;

      if(!(left->getWidth()==right->getWidth() && "type mismatch") && (currentOp == "Sle" || currentOp == "Ule"))
        return build(e->getKid(0), e->getKid(1), currentOp);
      else return build(left, right, currentOp);

      //return build(mutate_AIU(e->getKid(0), currentLevel, objectiveLevel), mutate_AIU(e->getKid(1), currentLevel, objectiveLevel), currentOp);
    } else if (e->getNumKids() == 1 && !excludedOperators(e)) {
      return mutate_AIU(e->getKid(0), currentLevel, objectiveLevel);
    } else return e;

  }

  ref<Expr> Mutator::build(ref<Expr> LHS, ref<Expr> RHS, std::string newKind) {
    


    //DEBUG
    //std::cout << "RECIBO TIPO: " << newKind << std::endl;
    //std::cout << "LHS: " << LHS->printWidthtoString() << " -- RHS: " << RHS->printWidthtoString() << std::endl;
    //std::cout << "LHS: " << LHS->printKindtoString() << " -- RHS: " << RHS->printKindtoString() << std::endl;
    //std::cout << "LHS: " << LHS->printValuetoString() << " -- RHS: " << RHS->printValuetoString() << std::endl;

    ref<Expr> result = LHS;

    ExprBuilder *Builder = 0;
    Builder = createDefaultExprBuilder();

    /*if(LHS->printWidthtoString() != RHS->printWidthtoString()) {
      std::cout << "Los tipos son diferentes, a ver si hay un aserto.\n";
      std::cout << "LHS: " << LHS->printWidthtoString() << " -- RHS: " << RHS->printWidthtoString() << std::endl;
      std::cout << "LHS: " << LHS->printKindtoString() << " -- RHS: " << RHS->printKindtoString() << std::endl;
      std::cout << "LHS: " << LHS->printValuetoString() << " -- RHS: " << RHS->printValuetoString() << std::endl;
      std::cout << "LLAMADA DENTRO: Y el tipo que voy a crear es: " << newKind << std::endl;
    } */
    //if(!(l->getWidth()==r->getWidth() && "type mismatch"))


    //std::cout << "Y ANTES DE LA COMPARACION ES: " << newKind << std::endl;
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

    aux = applyMutations(cs, "Sub", "Add");
    res.insert(res.end(), aux.begin(), aux.end());

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

    /* Debe negar todos los condicionales.
    Crea una función de aplicar mutaciones que reciba un Expr
    y lo niegue por completo, no solo el LHS */

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