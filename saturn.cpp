#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cctype>

using namespace std;

struct ForLoop {
    string var;
    int end;
    int start_pc;
};

vector<ForLoop> loopStack;

map<string, string> variables;

bool isNumber(const string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit(c) && c != '.' && c != '-') return false;
    }
    return true;
}

double getValue(string token) {

    if (variables.count(token)) {
        return stod(variables[token]);
    }

    return stod(token);
}

double evaluate(string expr) {

    stringstream ss(expr);

    string left, op, right;

    ss >> left;

    if (!(ss >> op)) {
        return getValue(left);
    }

    ss >> right;

    double a = getValue(left);
    double b = getValue(right);

    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") return a / b;

    return 0;
}

bool evaluateCondition(string cond) {

    stringstream ss(cond);

    string left, op, right;

    ss >> left >> op >> right;

    double a = getValue(left);
    double b = getValue(right);

    if (op == ">") return a > b;
    if (op == "<") return a < b;
    if (op == ">=") return a >= b;
    if (op == "<=") return a <= b;
    if (op == "==") return a == b;
    if (op == "!=") return a != b;

    return false;
}

void executeLine(string line, vector<string>& program, int& pc) {

    stringstream ss(line);
    string command;
    ss >> command;

    if (command == "let") {

        string var;
        string equals;
        string expr;

        ss >> var >> equals;

        getline(ss >> ws, expr);

        if (expr.front() == '"' && expr.back() == '"') {
            variables[var] = expr.substr(1, expr.size() - 2);
        }
        else {
            double value = evaluate(expr);
            variables[var] = to_string(value);
        }
    }

    else if (command == "print") {

        string arg;
        getline(ss >> ws, arg);

        if (arg.front() == '"' && arg.back() == '"') {
            cout << arg.substr(1, arg.size() - 2) << endl;
        }
        else if (variables.count(arg)) {
            cout << variables[arg] << endl;
        }
        else {
            cout << arg << endl;
        }
    }

    else if (command == "if") {

        string condition;
        getline(ss >> ws, condition);

        bool result = evaluateCondition(condition);

        if (!result) {

            int depth = 1;

            while (depth > 0 && pc + 1 < program.size()) {
                pc++;

                string l = program[pc];

                if (l.substr(0,2) == "if") depth++;
                if (l == "end") depth--;
            }
        }
    }
    else if (command == "for") {

        string var;
        string equals;
        string start;
        string toWord;
        string endVal;

        ss >> var >> equals >> start >> toWord >> endVal;

        int startValue = getValue(start);
        int endValue = getValue(endVal);

        variables[var] = to_string(startValue);

        ForLoop loop;
        loop.var = var;
        loop.end = endValue;
        loop.start_pc = pc;

        loopStack.push_back(loop);
    }

    else if (command == "end") {

        if (!loopStack.empty()) {

            ForLoop &loop = loopStack.back();

            double val = 0;
            if (isNumber(variables[loop.var])) {
                val = stod(variables[loop.var]);
            } else {
                cerr << "Error: loop variable is not numeric: " << loop.var << endl;
                exit(1);
            }

            val++;

            variables[loop.var] = to_string(val);

            if (val <= loop.end) {
                pc = loop.start_pc;
            } else {
                loopStack.pop_back();
            }
        }
    }

    else {
        cout << "Unknown command: " << command << endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cout << "Usage: saturn <file.sat>" << endl;
        return 1;
    }

    ifstream file(argv[1]);

    vector<string> program;
    string line;

    while (getline(file, line)) {

        if (line.size() == 0) continue;
        if (line[0] == '#') continue;

        program.push_back(line);
    }

    for (int pc = 0; pc < program.size(); pc++) {
        executeLine(program[pc], program, pc);
    }

    return 0;
}