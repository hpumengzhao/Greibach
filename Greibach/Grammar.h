#pragma once
#include<iostream>
#include<set>
#include<map>
#include<unordered_set>
#include<cassert>
using namespace std;
class Grammar {
    //��From�Ƶ�To
public:
    map<string, unordered_set<string> > rules;
    unordered_set<string> from_set;
    vector<string> input_order;
    //Ĭ��ʹ��S��Ϊ��ʼ����
    string S="S";
    Grammar() {
        string formula;
        //��ȡ��ʽ�����������ɹ���ʹ��rules���洢
        while (cin >> formula) {
            int siz = (int)formula.size();
            string From = "";
            int cut_id = -1;
            for (int i = 0; i < siz; i++) {
                if (formula[i] == '-') {
                    cut_id = i;
                    break;
                }
                else {
                    From += formula[i];
                }
            }
            from_set.insert(From);
            input_order.push_back(From);
            assert(cut_id != -1);
            assert(cut_id + 1 < siz && formula[cut_id + 1] == '>');
            string To = "";
            for (int i = cut_id + 2; i < siz; i++) {
                if (formula[i] == '|') {
                    assert(To.length());
                    rules[From].insert(To);
                    To.clear();
                }
                else {
                    To += formula[i];
                }
            }
            assert(To.length());
            rules[From].insert(To);
        }
    }

    /*��ӡ�Ƶ�����*/
    void DisplayRules() {
        /*
        for (string from : input_order) {
            if (from_set.find(from) != from_set.end()) {
                cout << from << " -> ";
                for (string to : rules[from]) {
                    cout << to << "|";
                }
                cout << endl;
            }
        }
        */
        for (string from : from_set) {
            cout << from << " -> ";
            int siz = (int)rules[from].size();
            int cnt = 0;

            for (string to : rules[from]) {
                cout << to ;
                ++cnt;
                if (cnt == siz) {
                    cout << endl;
                }
                else {
                    cout << "|";
                }
            }
            cout << endl;
        }
    }
    /*��������ʽ��ÿ���ս��/���ս��*/
    /*
    * A1bB1B2->{A1,b,B1,B2}
    */
};
vector<string> parse(string to) {
    vector<string>  res;
    string now = "";
    for (char c : to) {
        if (c >= '0' && c <= '9' || c == '\'') {
            now += c;
        }
        else {
            if (now.size() == 0) {
                now += c;
            }
            else {
                res.push_back(now);
                now = "";
                now += c;
            }
        }
    }
    if (now.size()) {
        res.push_back(now);
    }
    return res;
}