#pragma once
#include"Grammar.h"
#include<map>
#include<vector>
#include<queue>
using namespace std;
class GrebachSolver {
public:

	/*ɾ�����ñ��ʽ*/
	Grammar gra;
	map<string, bool> used;
	void DFS_Used(string u) {//�ݹ�ķ���
		if (used[u]) {
			return ;
		}
		used[u] = 1;
		for (string v : gra.rules[u]) {
			vector<string> ps = gra.parse(v);
			for (string v : ps) {
				if (v[0] >= 'A' && v[0] <= 'Z') {
					DFS_Used(v);
;				}
			}
		}
	}
	void delUnused(Grammar &Gra) {
		gra = Gra;
		DFS_Used(gra.S);
		for (string from : gra.from_set) {
			if (!used[from]) {
				//cout << "deleted: " << from << endl;
				Gra.from_set.erase(from);//ɾ�����ñ��ʽ
				Gra.rules[from].clear();
			}
		}
	}
	/*ɾ���ղ���ʽ*/
	map<string, set<string>> inv_graph;
	map<string, bool> can_reach_epsilon;//�ɿռ���

	//ͨ��DFS�������տɵ���ķ��ս��
	void search_epsilon(string u) {
		if (can_reach_epsilon[u]) {
			return ;
		}
		can_reach_epsilon[u] = 1;
		for (string v : inv_graph[u]) {
			search_epsilon(v);
		}
	}
	void delEpsilon(Grammar& gra) {
		for (string from : gra.from_set) {
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				if (ps.size() == 1) {
					if (ps[0] == "*" || (ps[0][0] >= 'A' && ps[0][0] <= 'Z')) {
						inv_graph[ps[0]].insert(from);
					}
				}
			}
		}
		search_epsilon("*");

		//������ö���滻
		for (string from : gra.from_set) {
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				vector<int> id;//�洢����ʽ�ɿձ�����λ��
				int siz = (int)ps.size();
				for (int i = 0; i < siz; i++) {
					if (can_reach_epsilon[ps[i]]) {
						id.push_back(i);
					}
				}
				int cnt = (int)id.size();
				for (int i = 0; i < (1 << cnt); i++) {//���б�յķ���
					map<int, bool> del;
					for (int j = 0; j < cnt; j++) {
						if ((i >> j) & 1) {
							del[id[j]] = 1;
						}
					}
					string new_gen = "";//�µĲ���ʽ
					for (int j = 0; j < siz; j++) {
						if (!del[j]) {
							new_gen += ps[j];
						}
					}
					gra.rules[from].insert(new_gen);
				}

			}
		}
		/*
		cout << (int)gra.rules["A"].size() << endl;
		for (string v : gra.rules["A"]) {
			cout << "A: " << v <<  ' '<<v.size()<<endl;
		}
		*/
		for (string from : gra.from_set) {
			if (gra.rules[from].find("*") != gra.rules[from].end()) {
				gra.rules[from].erase("*");
			}
			if (gra.rules[from].find("") != gra.rules[from].end()) {
				gra.rules[from].erase("");
			}
		}
		//cout << (int)gra.rules["A"].size() << endl;
	}
	/*������һ����ʽ*/
	/*��Ҫ��������������滻*/
	/*
		S->A
		A->B
		B->bb
		to
		S->bb
		A->bb
		B->bb
		�����B->A->S����������ϵ
	*/
	map<string, set<string>> top_graph;//����ͼ
	map<string, int> ind;//����

	void delSingle(Grammar& gra) {
		for (string from : gra.from_set) {
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				if (ps.size() == 1 && gra.from_set.find(ps[0]) != gra.from_set.end()) {//Ѱ�ҵ�from�����Ƶ����ĵ�һ����ʽ
					top_graph[ps[0]].insert(from);
					ind[from]++;
				}
			}
		}
		queue<string> q;
		for (string from : gra.from_set) {
			if (ind[from] == 0) {
				q.push(from);
			}
		}
		/// <summary>
		/// ��������
		/// </summary>
		/// <param name="gra"></param>
		while (!q.empty()) {
			string from = q.front();
			q.pop();
			set<string> add_tmp;
			set<string> del;
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				if (ps.size() == 1 && gra.from_set.find(ps[0]) != gra.from_set.end()) {
					del.insert(ps[0]);
					for (string v : gra.rules[ps[0]]) {
						add_tmp.insert(v);
					}
				}
			}
			/// <summary>
			/// ����һ����ʽ�滻Ϊ���Ƶ����Ĳ���ʽ
			/// </summary>
			/// <param name="gra"></param>
			for (string v : add_tmp) {
				gra.rules[from].insert(v);
			}
			/// <summary>
			/// ɾ����һ�ķ��ս��
			/// </summary>
			/// <param name="gra"></param>
			for (string d : del) {
				assert(gra.rules[from].find(d) != gra.rules[from].end());
				gra.rules[from].erase(d);
			}
			for (string nxt : top_graph[from]) {
				--ind[nxt];
				if (ind[nxt] == 0) {
					q.push(nxt);
				}
			}
		}
	}
	/* ����ֱ����ݹ�
		P->Pa|b
		תΪ
		P->bP'
		P'->P'a|*
	*/
	void delDirectLeftRecursive(Grammar& gra) {
		set<string> from_add_temp;
		for (string from : gra.from_set) {
			vector<string> no_leftR;//��Ų�������ݹ�Ĳ���ʽ
			vector<string> yes_leftR;
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				if (ps[0] == from) {//������ݹ�
					yes_leftR.push_back(to);
				}
				else {
					no_leftR.push_back(to);
				}
			}
			if (yes_leftR.size()) {//����ֱ����ݹ������
				gra.rules[from].clear();
				string new_from = from + "'";
				gra.from_set.insert(new_from);
				for (string v : no_leftR) {
					gra.rules[from].insert(v+new_from);
				}
				gra.rules[new_from].insert("*");
				for (string v : yes_leftR) {
					vector<string> pss = gra.parse(v);
					string temp = "";
					int siz = (int)pss.size();
					for (int i = 1; i < siz; i++) {
						temp += pss[i];
					}
					temp += new_from;
					gra.rules[new_from].insert(temp);
				}
			}
		}
	}
	/*���������ݹ�*/
	//����DFS��
	map<string, set<string>> Graph;
	vector<string> orders;
	map<string, bool> vis;
	void Order(string u) {
		if (vis[u]) return;
		vis[u] = 1;
		orders.push_back(u);
		for (string v : Graph[u]) {
			Order(v);
		}
	}
	void delLeftRecursive(Grammar& gra) {
		map<string, bool> has;//���Ա��滻��

		for (string from : gra.from_set) {
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				int siz = (int)ps.size();
				for (int i = 0; i < siz; i++) {
					if (ps[i][0] >= 'A' && ps[i][0] <= 'Z') {
						Graph[from].insert(ps[i]);
					}
				}
			}
		}
		Order(gra.S);
		reverse(orders.begin(), orders.end());
		//for (int steps = 1; steps <= 2; steps++) {
			for (string from : orders) {
				set<string> new_to;
				set<string> need_del;
				for (string to : gra.rules[from]) {
					vector<string> ps = gra.parse(to);
					if (ps[0][0] >= 'A' && ps[0][0] <= 'Z' && has[ps[0]]) {//�����Ƿ��ս��
						need_del.insert(to);
						for (string nxt : gra.rules[ps[0]]) {
							string new_gen = nxt;
							for (int i = 1; i < (int)ps.size(); i++) {
								new_gen += ps[i];
							}
							new_to.insert(new_gen);
						}
					}
				}
				for (string v : need_del) {
					gra.rules[from].erase(v);
				}
				for (string v : new_to) {
					gra.rules[from].insert(v);
				}
				has[from] = 1;
			}
			delDirectLeftRecursive(gra);
		//}
	}
	/*���Ƶ�ʽ����һ��Ϊ���ս�����滻��*/
	/*��������
		A->Bb
		B->Ca
		C->c
		תΪ
		A->cab
		B->ca
		C->c
		����C->B->A��������
	*/
	map<string, set<string> > top_graph_2;
	map<string, int> ind2;
	void delFirst(Grammar& gra) {
		for (string from : gra.from_set) {
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				if (ps[0][0] >= 'A' && ps[0][0] <= 'Z') {
					top_graph_2[ps[0]].insert(from);
					ind2[from]++;
				}
			}
		}
		queue<string> q;
		for (string from : gra.from_set) {
			if (ind2[from] == 0) {
				q.push(from);
			}
		}
		while (!q.empty()) {
			string from = q.front(); 
			q.pop();
			
			set<string> add_tmp;//�²�����ʽ��
			set<string> del;
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				if (ps[0][0] >= 'A' && ps[0][0] <= 'Z') {//�滻
					del.insert(to);
					string p = ps[0];
					int len = (int)to.length();
					for (string nxt : gra.rules[p]) {//��p�Ƶ��������滻to[0]
						string new_gen = nxt;
						int siz = (int)ps.size();
						for (int i = 1; i < siz; i++) new_gen += ps[i];
						//new_gen += to.substr(1,len-1);
						add_tmp.insert(new_gen);
					}
				}
			}
			for (string v : add_tmp) {
				gra.rules[from].insert(v);
			}
			for (string to : del) {
				gra.rules[from].erase(to);
			}
			for (string nxt : top_graph_2[from]) {
				--ind2[nxt];
				if (ind2[nxt] == 0) {
					q.push(nxt);
				}
			}
		}
	}


	/*����Greibach��ʽ*/
	string connect(string s, int x) {//�����ַ���������
		string t = "";
		while (x) {
			t += char(x % 10 + '0');
			x /= 10;
		}
		reverse(t.begin(), t.end());
		s += t;
		return s;
	}
	map<string, string> new_key;
	void Solver(Grammar& gra) {
		set<string> add_new; 
		for (string from : gra.from_set) {
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				int siz = (int)ps.size();
				for (int i = 1; i < siz; i++) {
					if (ps[i][0] >= 'a' && ps[i][0] <= 'z') {
						string base = "";
						base += ps[i][0] - ('a' - 'A');
						for (int cnt = 1;; cnt++) {
							string new_base = connect(base, cnt);
							if (gra.from_set.find(new_base) == gra.from_set.end()) {
								new_key[ps[i]] = new_base;
								//gra.from_set.insert(new_base);
								add_new.insert(new_base);
								string temp = "";
								temp += ps[i];
								gra.rules[new_base].insert(temp);
								//cout << new_base << ' ' << temp << endl;
								break;
							}
						}
					}
				}
			}
		}
		for (string v : add_new) {
			gra.from_set.insert(v);
		}
		/*
		for (string v : gra.from_set) {
			cout << "xx: " << v << endl;
		}
		*/
		for (string from : gra.from_set) {
			set<string> tmp;
			for (string to : gra.rules[from]) {
				vector<string> ps = gra.parse(to);
				string now = "";
				int siz = (int)ps.size();
				now += ps[0];
				for (int i = 1; i < siz; i++) {
					if (ps[i][0] >= 'a' && ps[i][0] <= 'z') {
						now += new_key[ps[i]];
					}
					else {
						now += ps[i];
					}
				}
				tmp.insert(now);
			}
			//cout << from << ' ' << (int)tmp.size() << endl;
			gra.rules[from].clear();
			for (string v : tmp) {
				gra.rules[from].insert(v);
			}
		}
	}
};