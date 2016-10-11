#include "Lists.h"
#include <stdlib.h>

using namespace std;

bool Is_Int(const char* arg){
    char *temp=(char*)arg;
    while(*temp){
        if(*temp!='0' && *temp!='1' && *temp!='2' && *temp!='3' && *temp!='4' && *temp!='5' && *temp!='6' && *temp!='7' && *temp!='8' && *temp!='9' && *temp!='-') return false;
        temp++;
    }
    return true;
}

Transaction_List::~Transaction_List(){
    transaction_node *temp=head;

    while(head){
        temp=head;
        head=head->next;
        delete temp;
    }
}

void Transaction_List::Show(){
    transaction_node *temp=head;

    while(temp){
        cout << temp->Name << temp->Amount << endl;
        temp=temp->next;
    }
}

Account_List::~Account_List(){
    account_node *temp=head;

    while(head){
        temp=head;
        head=head->next;
        delete temp;
    }
}

bool Account_List::Exists(string nam){
    account_node *temp=head;

    while(temp){
        if(temp->name.compare(nam)==0) return true;
        temp=temp->next;
    }
    return false;
}


int Account_List::Balance(string nam){
    account_node *temp=head;

    while(temp){
        if(temp->name.compare(nam)==0) return temp->balance;
        temp=temp->next;
    }
    return -1;
}

void Account_List::Add_Money(string from,string to,int amount){
    account_node *temp=head;

    while(temp){
        if(temp->name.compare(to)==0){
            temp->Income.Add(from,amount);
            temp->balance = temp->balance + amount;
            return;
        }
        temp=temp->next;
    }
    return ;
}

void Account_List::Send_Money(string from,int amount){
    account_node *temp=head;

    while(temp){
        if(temp->name.compare(from)==0){
            temp->balance = temp->balance - amount;
            return;
        }
        temp=temp->next;
    }
    return;
}


void Int_List::Add(int n){ //Distinct Ordered int list
    int_node *temp=head;
    int_node *target=NULL;

    if(head==NULL){
        head = new int_node(n,head);
    }
    else if(head->bucket >= n){
        if(head->bucket == n) return;
        head = new int_node(n,head);
    }
    else{
        while(temp){
            if(temp->bucket == n) return;
            if(temp->bucket > n) break;
            target=temp;
            temp = temp->next;
        }
        target->next = new int_node(n,temp);
    }
}

Int_List::~Int_List(){
    int_node *temp=head;

    while(head){
        temp=head;
        head=head->next;
        delete temp;
    }
}

String_List::~String_List(){
    string_node *temp=head;

    while(head){
        temp=head;
        head=head->next;
        delete temp;
    }
}

int String_List::StreamToList(stringstream &in){
    string token;
    int delay=0;

    in >> token;
    while(in.good()==true){ //All tokens except last one must not being ints
        if(Is_Int(token.c_str())==true){
            return -1;
        }
        if(token.length()<2) return -1;
        this->Add(token);
        in >> token;
    }
    //Check the last token
    if(Is_Int(token.c_str())==true){ //Its int,its delay
        delay=atoi(token.c_str());
    }
    else if(token.length()>=2){ //Its name
        this->Add(token);
    }
    //Else its ignored,it could be enter character,but not a name.

    if(this->Count()==0) return -1; //No name Provided!
    return delay;
}

Fd_Queue::Fd_Queue(int maxfd):siz(maxfd){
    head=0;
    tail=0;
    counter=0;
    arr = new int[siz];
}

bool Fd_Queue::Empty(){
    if(counter==0) return true;
    return false;
}

bool Fd_Queue::Full(){
    if(counter==siz) return true;
    return false;
}

void Fd_Queue::Add(int arg){
    if(this->Full()) return;
    arr[tail]=arg;
    tail++;
    tail=tail%siz;
    counter++;
}

int Fd_Queue::Pop(){
    if(this->Empty()) return -1;
    int temp=arr[head];
    head++;
    head=head%siz;
    counter--;
    return temp;
}
