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
	string to_string(char c) {
		string res = "";
		res += c;
		return res;
	}
	bool DFS(string status,int id,stack<string> now_stack,const int& len,const string& s) {//当前状态,字符串下标,
		//cout << status << ' ' << id << ' ' << len << ' ' << s << endl;
		if (status == "q1"&&id==len&&now_stack.top()=="z") return 1;
		if (id == len) return 0;
		bool jud = 0;
		string ss;
		if (id == -1) ss = "*";
		else ss = to_string(s[id]);
		/*搜索下面的状态*/
		//cout << status << ' ' << ss << ' ' << now_stack.top() << endl;
		for (auto v : trans_rules[{status, { ss,now_stack.top() }}]) {
			string nxt_status = v.first;
			string replaces = v.second;
			stack<string> new_stack = now_stack;
			new_stack.pop();
			vector<string> ps = parse(replaces);
			int siz = (int)ps.size();
			for (int i = siz - 1; i >= 0; --i) {
				if (ps[i] != "*") new_stack.push(ps[i]);
			}
			jud|=DFS(nxt_status, id + 1, new_stack, len, s);
		}
		return jud;
	}
	bool Accepted(string s) {
		stack<string> sta;
		sta.push("z");
		string now_status = "q0";
		int siz = (int)s.size();
		return DFS(now_status, -1, sta, siz,s);
	}
};