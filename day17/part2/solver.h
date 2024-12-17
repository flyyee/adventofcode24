#include <iostream>
#include <vector>
#include <cassert>
#include <optional>
#include <variant>
#include <functional>
#include <unordered_map>
#include <stack>
#include <cmath>
#include <memory>

using namespace std;

enum class Operation {
    Mod8,
    Exp2,
    Div,
    Xor
};

class Expression;
using Instruction = pair<Operation, optional<Expression*>>;
class SolverV2;

class Expression {
public:
    using Identifier = unsigned long;
private:
    Identifier id_;
    // root
    optional<Expression*> parent_;
    // an application
    optional<Instruction> instruction_;

public:
    explicit Expression(Identifier id) : id_(id) {}

    bool is_root() const {
        return !parent_.has_value();
    }

    Instruction get_instruction() const {
        return *instruction_;
    }

    Expression* get_parent() const {
        return *parent_;
    }

    Identifier get_id() const {
        return id_;
    }

    void add_child(Expression* child, const Instruction& instruction) {
        child->parent_ = this;
        child->instruction_ = instruction;
    }
};

string instruction_to_string(const Instruction& instruction, optional<function<string(Expression*)>> get_name = nullopt) {
    const auto& [operation, operand] = instruction;
    switch (operation) {
    case Operation::Div:
        return "div " + (get_name ? (*get_name)(*operand) : "?");
    case Operation::Exp2:
        return "exp2";
    case Operation::Mod8:
        return "mod8";
    case Operation::Xor:
        return "xor " + (get_name ? (*get_name)(*operand) : "?");
    default:
        assert(false);
    }
}

using RawValue = long;
enum class ConstraintType {
    Eq,
    Neq
};

struct Constraint {
    ConstraintType type;
    Expression::Identifier lhs;
    variant<Expression::Identifier, RawValue> rhs;
};

struct SaveState;
class SolverV2 {
public:
    using ExpressionName = string;
private:
    vector<Constraint> constraints;
    unordered_map<Expression::Identifier, pair<ExpressionName, Expression*>> expressions;
    Expression::Identifier current_expression_id = 0;
    Expression::Identifier generate_expression_id() {
        return current_expression_id++;
    }

    // returns all constraints that have target id. all results' lhs will be id.
    vector<Constraint> search_constraints(Expression::Identifier id) {
        vector<Constraint> results;
        for (const auto& constraint : constraints) {
            if (constraint.lhs == id) {
                results.push_back(constraint);
            }
            else if (holds_alternative<Expression::Identifier>(constraint.rhs) && get<Expression::Identifier>(constraint.rhs) == id) {
                auto constraint_flipped = constraint;
                swap(constraint_flipped.lhs, get<Expression::Identifier>(constraint_flipped.rhs));
                results.push_back(constraint_flipped);
            }
        }
        return results;
    }

    struct SaveState {
        size_t constraint_count;
        size_t current_expression_id;
    };

public:
    unique_ptr<SaveState> save_state() const {
        auto state = make_unique<SaveState>();
        state->constraint_count = constraints.size();
        state->current_expression_id = current_expression_id;
        return state;
    }

    void restore_state(unique_ptr<SaveState> previous_state) {
        // The previous current_expression_id was previous_state
        size_t constraint_count_diff = constraints.size() - previous_state->constraint_count;
        // TODO: just erase in one go
        for (size_t i = 0; i < constraint_count_diff; i++) {
            constraints.pop_back();
        }

        for (auto id = previous_state->current_expression_id; id < current_expression_id; id++) {
            auto& [name, expression] = expressions.at(id);
            delete expression;
            expressions.erase(id);
        }
        current_expression_id = previous_state->current_expression_id;
    }

    ExpressionName get_name(const Expression* const expression) const {
        return expressions.at(expression->get_id()).first;
    }

    ExpressionName get_name_stripped(const Expression* const expression) const {
        auto full_name = get_name(expression);
        return full_name.substr(0, full_name.rfind("_"));
    }

    Expression* symbol(const ExpressionName& name) {
        const auto id = generate_expression_id();
        auto expression = new Expression(id);
        expressions[expression->get_id()] = { name + "_" + to_string(id), expression };
        return expression;
    }

    void add_constraint(Expression* lhs, ConstraintType condition, variant <Expression*, RawValue> rhs) {
        variant<Expression::Identifier, RawValue> rhs_;
        if (holds_alternative<Expression*>(rhs)) {
            rhs_ = get<Expression*>(rhs)->get_id();
        }
        else {
            rhs_ = get<RawValue>(rhs);
        }

        constraints.emplace_back(Constraint{
            .type = condition,
            .lhs = lhs->get_id(),
            .rhs = rhs_
            });
    }

    Expression* constant(const ExpressionName& name, const RawValue& value) {
        auto expression = symbol(name);
        add_constraint(expression, ConstraintType::Eq, value);
        return expression;
    }

    Expression* mod8(Expression* expression) {
        auto child = symbol(get_name_stripped(expression));
        expression->add_child(child, { Operation::Mod8, nullopt });
        return child;
    }

    Expression* exp2(Expression* expression) {
        auto child = symbol(get_name_stripped(expression));
        expression->add_child(child, { Operation::Exp2, nullopt });
        return child;
    }

    // TODO: other should be of type: const Expression*
    Expression* div(Expression* expression, Expression* other) {
        auto child = symbol(get_name_stripped(expression));
        expression->add_child(child, { Operation::Div, other });
        return child;
    }

    // TODO: other should be of type: const Expression*
    Expression* exclusive_or(Expression* expression, Expression* other) {
        auto child = symbol(get_name_stripped(expression));
        expression->add_child(child, { Operation::Xor, other });
        return child;
    }

    void print(const Expression* const expression) {
        cout << "Printing " << get_name(expression) << ": \n";

        const auto print_constraints = [&](const Expression* expression) {
            for (const auto& constraint : search_constraints(expression->get_id())) {
                switch (constraint.type) {
                case ConstraintType::Eq:
                    cout << " == ";
                    break;
                case ConstraintType::Neq:
                    cout << " != ";
                    break;
                default:
                    assert(false);
                }
                if (holds_alternative<RawValue>(constraint.rhs)) {
                    cout << get<RawValue>(constraint.rhs);
                }
                else {
                    cout << expressions.at(get<Expression::Identifier>(constraint.rhs)).first;
                }

                cout << ", ";
            }
            };

        const Expression* curr = expression;
        stack<const Expression*> ancestry;
        ancestry.push(curr);
        while (!curr->is_root()) {
            curr = curr->get_parent();
            ancestry.push(curr);
        }

        cout << "{" << get_name_stripped(ancestry.top()) << "}";
        print_constraints(ancestry.top());
        ancestry.pop(); // root

        while (!ancestry.empty()) {
            auto curr = ancestry.top();
            const auto instruction = curr->get_instruction();
            cout << "\n\t| " << instruction_to_string(instruction, [&](Expression* e) {
                return "<" + expressions.at(e->get_id()).first + ">";
                });
            print_constraints(curr);
            ancestry.pop();
        }
        cout << endl;
    }

    void print_detailed(const Expression* const expression, size_t indentation = 0) {
        const string space = string(indentation, '\t');
        if (indentation)
            cout << endl;
        cout << space << "[detailed] Printing " << get_name(expression) << ": \n";

        const auto print_constraints = [&](const Expression* expression) {
            for (const auto& constraint : search_constraints(expression->get_id())) {
                switch (constraint.type) {
                case ConstraintType::Eq:
                    cout << " == ";
                    break;
                case ConstraintType::Neq:
                    cout << " != ";
                    break;
                default:
                    assert(false);
                }
                if (holds_alternative<RawValue>(constraint.rhs)) {
                    cout << get<RawValue>(constraint.rhs);
                }
                else {
                    cout << expressions.at(get<Expression::Identifier>(constraint.rhs)).first;
                }

                cout << ", ";
            }
            };

        const Expression* curr = expression;
        stack<const Expression*> ancestry;
        ancestry.push(curr);
        while (!curr->is_root()) {
            curr = curr->get_parent();
            ancestry.push(curr);
        }

        cout << space << "{" << get_name_stripped(ancestry.top()) << "}";
        print_constraints(ancestry.top());
        ancestry.pop(); // root

        vector<const Expression*> dependencies_to_print;
        while (!ancestry.empty()) {
            auto curr = ancestry.top();
            const auto instruction = curr->get_instruction();
            cout << "\n" << space << "\t| " << instruction_to_string(instruction, [&](Expression* e) {
                try {
                    auto value = evaluate(e);
                    return to_string(value);
                }
                catch (...) {
                    // failed to evaluate
                    dependencies_to_print.push_back(e);
                    return "<" + expressions.at(e->get_id()).first + ">";
                }
                });
            print_constraints(curr);
            ancestry.pop();
        }
        cout << endl;

        for (const auto& dependency : dependencies_to_print) {
            print_detailed(dependency, indentation + 1);
        }
    }

    RawValue evaluate(const Expression* expression) {
        const Expression* curr = expression;
        stack<const Expression*> ancestry;
        ancestry.push(curr);
        while (!curr->is_root()) {
            curr = curr->get_parent();
            ancestry.push(curr);
        }

        const auto root = ancestry.top();
        const auto root_constraints = search_constraints(root->get_id());
        if (root_constraints.size() != 1 || root_constraints[0].type != ConstraintType::Eq) {
            throw runtime_error("Could not evaluate non-constant value!");
        }

        RawValue curr_value;
        if (holds_alternative<RawValue>(root_constraints[0].rhs)) {
            curr_value = get<RawValue>(root_constraints[0].rhs);
        }
        else {
            curr_value = evaluate(
                expressions.at(
                    get<Expression::Identifier>(root_constraints[0].rhs)
                ).second);
        }

        ancestry.pop();
        while (!ancestry.empty()) {
            const auto curr = ancestry.top();
            const auto [operation, operand] = curr->get_instruction();
            switch (operation) {
            case Operation::Exp2:
                curr_value = pow(2, curr_value);
                break;
            case Operation::Mod8:
                curr_value %= 8;
                break;
            case Operation::Div:
                curr_value /= evaluate(*operand);
                break;
            case Operation::Xor:
                curr_value ^= evaluate(*operand);
                break;
            }
            ancestry.pop();
        }

        return curr_value;
    }

    vector<RawValue> backsolve(const Expression* expression) {
        optional<RawValue> tail_value = nullopt;
        for (const auto& constraint : search_constraints(expression->get_id())) {
            if (constraint.type == ConstraintType::Eq) {
                if (holds_alternative<RawValue>(constraint.rhs)) {
                    tail_value = get<RawValue>(constraint.rhs);
                }
                else {
                    tail_value = evaluate(
                        expressions.at(
                            get<Expression::Identifier>(constraint.rhs)
                        ).second);
                }
                break;
            }
        }

        if (!tail_value) {
            throw runtime_error("backsolve(): Failed to determine tail value. Did you add a constraint?");
        }

        if (expression->is_root()) {
            return { *tail_value };
        }

        vector<RawValue> solutions = {};
        RawValue curr_value = *tail_value;
        // perform op
        const auto& [operation, operand] = expression->get_instruction();
        switch (operation) {
        case Operation::Exp2:
        {
            RawValue prev_value = log2(curr_value);
            if (pow(2, prev_value) != curr_value) {
                break;
            }
            add_constraint(expression->get_parent(), ConstraintType::Eq, prev_value);
            const auto new_solutions = backsolve(expression->get_parent());
            solutions.insert(end(solutions), begin(new_solutions), end(new_solutions));
            constraints.pop_back();
            break;
        }
        case Operation::Mod8:
            for (size_t i = 0; i < 10; i++) {
                RawValue prev_value = i * 8 + curr_value;
                add_constraint(expression->get_parent(), ConstraintType::Eq, prev_value);
                const auto new_solutions = backsolve(expression->get_parent());
                solutions.insert(end(solutions), begin(new_solutions), end(new_solutions));
                constraints.pop_back();
            }
            break;
        case Operation::Div:
        {
            const auto divisor = evaluate(*operand);
            for (RawValue i = 0; i < divisor; i++) {
                RawValue prev_value = curr_value * divisor + i;
                add_constraint(expression->get_parent(), ConstraintType::Eq, prev_value);
                const auto new_solutions = backsolve(expression->get_parent());
                solutions.insert(end(solutions), begin(new_solutions), end(new_solutions));
                constraints.pop_back();
            }
            break;
        }
        case Operation::Xor:
        {
            {
                const auto other = evaluate(*operand);
                RawValue prev_value = curr_value ^ other;
                add_constraint(expression->get_parent(), ConstraintType::Eq, prev_value);
                const auto new_solutions = backsolve(expression->get_parent());
                solutions.insert(end(solutions), begin(new_solutions), end(new_solutions));
                constraints.pop_back();
                break;
            }
        }
        default:
            assert(false);
        }

        return solutions;
    }

    ~SolverV2() {
        for (auto& [name, name_and_expression] : expressions) {
            delete name_and_expression.second;
        }
    }
};

void test_constants() {
    SolverV2 s;
    auto seven = s.constant("seven", 7);
    auto seven_exp2 = s.exp2(seven);
    auto thirteen = s.constant("thirteen", 13);
    auto thirteen_mod8 = s.mod8(thirteen);

    auto val = s.div(seven_exp2, thirteen_mod8);
    assert(s.evaluate(val) == 25);
}

void test_symbols() {
    SolverV2 s;
    auto reg = s.symbol("reg");
    const auto blackbox = [&](auto input) {
        auto three = s.constant("three", 3);
        auto input_div_three = s.div(input, three);
        return s.exp2(input_div_three);
        };
    auto result = blackbox(reg);
    s.add_constraint(result, ConstraintType::Eq, 8);
    s.print(result);

    const auto solutions = s.backsolve(result);
    assert(solutions.size() == 3);
    assert(find(begin(solutions), end(solutions), 9) != end(solutions));
    assert(find(begin(solutions), end(solutions), 10) != end(solutions));
    assert(find(begin(solutions), end(solutions), 11) != end(solutions));
}

void test_solver() {
    SolverV2 s;
    auto a = s.symbol("register_a");
    const auto blackbox = [&](auto input) {
        auto a = s.mod8(input);
        auto three = s.constant("const", 3);
        auto b = s.exclusive_or(a, three);

        auto a6 = s.exp2(b);
        auto a7 = s.div(input, a6);

        auto five = s.constant("const", 5);
        auto c = s.exclusive_or(b, five);
        auto d = s.exclusive_or(c, a7);
        auto a14 = s.mod8(d);

        return a14;
        };

    auto result = blackbox(a); // equivalent to running the testcase loop once
    s.print_detailed(result);
    s.add_constraint(result, ConstraintType::Eq, 2);
    auto solutions = s.backsolve(result);
    for (const auto& solution : solutions) {
        cout << "Found solution: " << solution << endl;
    }
}