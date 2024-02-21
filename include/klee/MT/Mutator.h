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
    //Constructor that receive a list of mutation operators
    Mutator(std::vector<std::string> operators) : mutationOperators(operators){}

    //Default constructors
    Mutator(){}
    Mutator(ConstraintSet cs) : originalConstraints(cs){}

    std::vector<ConstraintSet> mutate(ConstraintSet cs);

    //std::vector<ConstraintSet> mutate(ConstraintSet cs, int type);
    //std::vector<ConstraintSet> mutate(int type);

    //std::multimap<std::string, std::vector<ConstraintSet>> mutate(ConstraintSet cs);
    //std::multimap<std::string, std::vector<ConstraintSet>> mutate();

    //Count the possible mutations
    int countMutations(std::string op, ConstraintSet cs);
    int countMutations(std::string op, ref<Expr> e);

    int countMutations_AIS(ConstraintSet cs);
    int countMutations_AIS(ref<Expr> e);

    int countMutations_AIU(ConstraintSet cs);
    int countMutations_AIU(ref<Expr> e);

    //Apply all possible mutations to a given contraint set
    std::vector<ConstraintSet> applyMutations(ConstraintSet cs, std::string oldOp, std::string newOp);
    std::vector<ConstraintSet> apply_AIS(ConstraintSet cs, std::string op);
    std::vector<ConstraintSet> apply_AIU(ConstraintSet cs);

    //All the previous methods for the original constraint set
    int countMutations(std::string op);
    int countMutations_AIS();
    int countMutations_AIU();
    std::vector<ConstraintSet> applyMutations(std::string oldOp, std::string newOp);

    void printOriginalConstraints();

    ref<Expr> mutate(ref<Expr> e, std::string oldOp, std::string newOp, int currentLevel, int objectiveLevel);
    ref<Expr> mutate_AIS(ref<Expr> e, int currentLevel, int objectiveLevel, std::string op);
    ref<Expr> mutate_AIU(ref<Expr> e, int currentLevel, int objectiveLevel);

    bool is_number(const std::string& s) {
        return !s.empty() && std::find_if(s.begin(),
            s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
    }

    bool excludedOperators(ref<Expr> node);



  private:
    //Build a new expression from a given Kind (string for general purposes)
    ref<Expr> build(ref<Expr> LHS, ref<Expr> RHS, std::string newKind);
    int count_ARU(ConstraintSet cs);

    //Mutation Operators
    std::vector<ConstraintSet> apply_ROR(ConstraintSet cs);

    std::vector<ConstraintSet> apply_ARB(ConstraintSet cs);
    std::vector<ConstraintSet> apply_ARU(ConstraintSet cs);
    std::vector<ConstraintSet> apply_ARS(ConstraintSet cs);
    //std::vector<ConstraintSet> apply_AIU(ConstraintSet cs);
    //std::vector<ConstraintSet> apply_AIS(ConstraintSet cs);
    std::vector<ConstraintSet> apply_ADS(ConstraintSet cs);
    std::vector<ConstraintSet> apply_COR(ConstraintSet cs);
    std::vector<ConstraintSet> apply_COI(ConstraintSet cs);
    std::vector<ConstraintSet> apply_COD(ConstraintSet cs);
    std::vector<ConstraintSet> apply_LOR(ConstraintSet cs);
    std::vector<ConstraintSet> apply_ASR(ConstraintSet cs);

    std::vector<std::string> mutationOperators;
    ConstraintSet originalConstraints;
};