#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cctype>
#include <random>

using namespace std;

struct ForLoop {
    string var;
    int end;
    int start_pc;
};

vector<ForLoop> loopStack;

map<string, string> variables;

// Random number generator for `random` command
std::mt19937 rng((std::random_device())());

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

// Simple interpolation: replace occurrences of {var} with variables[var]
string interpolate(const string& s) {
    string out;
    size_t i = 0;
    while (i < s.size()) {
        if (s[i] == '{') {
            size_t j = s.find('}', i + 1);
            if (j != string::npos) {
                string key = s.substr(i + 1, j - (i + 1));
                if (variables.count(key)) out += variables[key];
                // if not found, insert empty string (could also keep placeholder)
                i = j + 1;
                continue;
            }
        }
        out.push_back(s[i]);
        i++;
    }
    return out;
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

// (No duplicate f-string helper; use `interpolate` defined above.)

void executeLine(string line, vector<string>& program, int& pc) {

    stringstream ss(line);
    string command;
    ss >> command;

    

    // debug: show executing line
    // cout << "DEBUG: exec: '" << line << "'\n";

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

        if (!arg.empty() && arg.front() == '"' && arg.back() == '"') {
            string content = arg.substr(1, arg.size() - 2);
            // perform interpolation for {var} patterns
            cout << interpolate(content) << endl;
        }
        else if (variables.count(arg)) {
            cout << variables[arg] << endl;
        }
        else {
            // If unquoted arg contains braces, try interpolation as well
            if (arg.find('{') != string::npos && arg.find('}') != string::npos) {
                cout << interpolate(arg) << endl;
            } else {
                cout << arg << endl;
            }
        }
    }

    else if (command == "input") {

        string var;
        ss >> var;

        string prompt;
        getline(ss >> ws, prompt);

        if (!prompt.empty() && prompt.front() == '"' && prompt.back() == '"') {
            cout << prompt.substr(1, prompt.size() - 2);
            cout.flush();
        }
        else if (!prompt.empty()) {
            cout << prompt;
            cout.flush();
        }

        string inputVal;
        if (!std::getline(cin, inputVal)) {
            inputVal = "";
        }

        variables[var] = inputVal;
    }

    else if (command == "random") {

        string var;
        ss >> var;

        string a, b;

        if (!(ss >> a)) {
            cout << "Error: random requires at least one bound" << endl;
            return;
        }

        if (ss >> b) {
            // syntax: random var min max
            int minv = (int)getValue(a);
            int maxv = (int)getValue(b);

            if (minv > maxv) std::swap(minv, maxv);

            std::uniform_int_distribution<int> dist(minv, maxv);
            int r = dist(rng);
            variables[var] = to_string(r);
        } else {
            // syntax: random var max  (min defaults to 0)
            int minv = 0;
            int maxv = (int)getValue(a);

            if (minv > maxv) std::swap(minv, maxv);

            std::uniform_int_distribution<int> dist(minv, maxv);
            int r = dist(rng);
            variables[var] = to_string(r);
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