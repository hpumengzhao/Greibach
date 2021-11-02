// Greibach.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include<set>
#include<map>
#include<cassert>
#include"GreibachSolver.h"
#include"NPDAsolver.h"
#include"Grammar.h"
using namespace std;
//范式
int main()
{   
    FILE* stream;
    freopen_s(&stream,"input.txt","r",stdin);
    Grammar contexts =  Grammar();


    cout << "\033[32m文法加载成功!\033[0m" << endl;
    contexts.DisplayRules();

    GrebachSolver GS = GrebachSolver();
  //  cout << "\033[32m直接左递归消除成功!\033[0m" << endl;

    GS.delLeftRecursive(contexts);
    cout << "\033[32m左递归消除成功!\033[0m" << endl;
    contexts.DisplayRules();
    
  
    GS.delUnused(contexts);
    cout << "\033[32m删除无用表达式成功！\033[0m" << endl;
    contexts.DisplayRules();

    GS.delEpsilon(contexts);
    cout << "\033[32m删除空产生式成功！\033[0m" << endl;
    contexts.DisplayRules();

    GS.delSingle(contexts);
    cout << "\033[32m删除单一产生式成功！\033[0m" << endl;
    contexts.DisplayRules();

    GS.delFirst(contexts);
    cout << "\033[32m将产生式的第一个符号变为终结符！\033[0m" << endl;
    contexts.DisplayRules();

    GS.Solver(contexts);
    cout << "\033[32m格雷巴赫范式为: \033[0m" << endl;
    contexts.DisplayRules();
    

    NPDA NPDASolver =  NPDA(contexts);
    cout << "\033[32mNPDA为: \033[0m" << endl;
    NPDASolver.Display();

    //cout << NPDASolver.Accepted("aabbcccddccc") << endl;
    bool res = NPDASolver.Accepted("aabbcccddccc");
    if (res) {
        cout << "\033[32mAccepted!!\033[0m" << endl;
    }
    else {
        cout << "\033[31mRejected!!\033[0m" << endl;
    }
    fclose(stdin);

}