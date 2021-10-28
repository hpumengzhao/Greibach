#pragma once
#include "GreibachSolver.h"
#include "Grammar.h"
#include <map>
#include <set>
#include <vector>
#include <stack>
#include <string>
using namespace std;
/*为输入的文法构造一个NPDA*/
class NPDA {
public:
	set<string> Q;//NPDA状态集合
	//eg: (q1,a,A)->{(q1,ABC),(q1,*)}
	map<pair<string, pair<string, string> >, vector<pair<string, string> > > trans_rules;
	set<string> alpha_set;//输入字母表
	set<string> stack_set;//栈字母表
	string q0 = "q0";
	string z = "z";
	set<string> F;//终止状态集合ww


	NPDA(Grammar& greibach) {
		Q.insert("q0");
		Q.insert("q1");
		Q.insert("qf");

		F.insert("qf");
		trans_rules[{"q0", { "*","z" }}].push_back({ "q1","Sz" });
		trans_rules[{"q1", { "*","z" }}].push_back({ "qf","z" });

		for (string from : greibach.from_set) {
			for (string to : greibach.rules[from]) {
				vector<string> ps = parse(to);
				string temp = "";
				for (int i = 1; i < (int)ps.size(); i++) {
					temp += ps[i];
				}
				if (temp == "") {
					temp = "*";
				}
				trans_rules[{"q1", { ps[0],from }}].push_back({ "q1",temp });
				
			}
		}
	}
	void Display() {
		for (auto v : trans_rules) {
			cout << '(' << v.first.first << ',' << v.first.second.first << ',' << v.first.second.second << ')' <<
				" = ";
			cout << "{";
			int siz = (int)v.second.size();
			for (int i = 0; i < siz-1; i++) {
				cout << "(" << v.second[i].first << "," << v.second[i].second << "),";
			}
			cout << "(" << v.second[siz-1].first << "," << v.second[siz-1].second << ")";
			cout << "}";
			cout << endl;
		}
	}


	
	bool Accepted(string& s) {
		stack<string> sta;
		sta.push("z");
		string now_status = "q0";
		for (char c : s) {
			string temp = ""; temp += c;
			string top = sta.top();
			if (trans_rules[{now_status, { temp,top }}].size()) {

			}
		}
	}
};