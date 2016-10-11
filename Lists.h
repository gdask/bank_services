#ifndef LISTS_H
#define	LISTS_H
#include <string>
#include <sstream>
#include <cstring>

bool Is_Int(const char *arg);

class transaction_node{
public:
    std::string Name;
    int Amount;
    transaction_node *next;
    transaction_node(std::string nam,int money,transaction_node* n):Name(nam),Amount(money),next(n){};
};

class Transaction_List{
    transaction_node *head;
public:
    Transaction_List(){head=NULL;};
    ~Transaction_List();
    void Add(std::string name,int amount){head= new transaction_node(name,amount,head);};
    void Show();
};

class account_node{
public:
    std::string name;
    Transaction_List Income;
    int balance;
    account_node *next;
    account_node(std::string nam,int start,account_node *n):name(nam),balance(start),next(n){};
};

class Account_List{ //Before using any function programmer first have to check if a account exists
    account_node *head;
public:
    Account_List(){head=NULL;};
    ~Account_List();
    bool Exists(std::string nam);
    int Balance(std::string nam);
    void Add_Account(std::string nam,int start){head = new account_node(nam,start,head);};
    void Add_Money(std::string from,std::string to,int amount);
    void Send_Money(std::string from,int amount);
};

class int_node{
public:
    int bucket;
    int_node *next;
    int_node(int num,int_node *n):bucket(num),next(n){};
};

class Int_List{ //Ordered list
public:
    int_node *head;
    Int_List(){head=NULL;};
    ~Int_List();
    void Add(int n);
    int Pop();
};

class string_node{
public:
    std::string name;
    string_node *next;
    string_node(std::string nam,string_node *n):name(nam),next(n){};
};

class String_List{
    int counter;
public:
    string_node *head;
    String_List():counter(0){head=NULL;};
    ~String_List();
    void Add(std::string nam){head= new string_node(nam,head); counter++;};
    int StreamToList(std::stringstream &in); //Returns -1 if a syntax error occured,or else returns the delay (=0 if not provided).
    int Count(){return counter;};
};

class Fd_Queue{
    int *arr;
    unsigned int head;
    unsigned int tail;
    const int siz;
    int counter;
public:
    Fd_Queue(int maxfd);
    ~Fd_Queue(){delete[] arr;};
    void Add(int arg);
    int Pop();
    bool Empty();
    bool Full();
};


#endif	/* SLISTS_H */










