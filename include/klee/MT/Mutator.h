#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <cctype>
#include "klee/Expr/Expr.h"
#include "klee/Expr/Constraints.h"

using namespace klee;


class Mutator {
  public:
    //Default constructor
    Mutator() {}

    //Constructor that receive a list of mutation operators
    Mutator(std::vector<std::string> operators) : mutationOperators(operators){}

    std::vector<ConstraintSet> mutate(ConstraintSet cs);



  private:
    //Build a new expression from a given Kind (string for general purposes)
    ref<Expr> build(ref<Expr> LHS, ref<Expr> RHS, std::string newKind);

    //Apply all possible mutations to a given contraint set
    std::vector<ConstraintSet> applyMutations(ConstraintSet cs, std::string oldOp, std::string newOp);

    //Mutation Operators
    std::vector<ConstraintSet> apply_ROR(ConstraintSet cs);
    std::vector<ConstraintSet> apply_ARB(ConstraintSet cs);
    std::vector<ConstraintSet> apply_ARS(ConstraintSet cs);
    std::vector<ConstraintSet> apply_AIU(ConstraintSet cs);
    std::vector<ConstraintSet> apply_AIS(ConstraintSet cs, std::string op);
    std::vector<ConstraintSet> apply_COR(ConstraintSet cs);
    std::vector<ConstraintSet> apply_COI(ConstraintSet cs);

    //Auxiliary methods
    void printOriginalConstraints();
    bool is_number(const std::string& s);
    bool excludedOperators(ref<Expr> node);

    //Count the possible mutations
    int countMutations(std::string op, ConstraintSet cs);
    int countMutations(std::string op, ref<Expr> e);
    ref<Expr> mutate(ref<Expr> e, std::string oldOp, std::string newOp, int currentLevel, int objectiveLevel);

    //AIS and AIU need to be counted separately
    int countMutations_AIS(ConstraintSet cs);
    int countMutations_AIS(ref<Expr> e);
    ref<Expr> mutate_AIS(ref<Expr> e, int currentLevel, int objectiveLevel, std::string op);


    int countMutations_AIU(ConstraintSet cs);
    int countMutations_AIU(ref<Expr> e);
    ref<Expr> mutate_AIU(ref<Expr> e, int currentLevel, int objectiveLevel);


    std::vector<std::string> mutationOperators;
    ConstraintSet originalConstraints;
};