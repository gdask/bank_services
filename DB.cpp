#include "DB.h"
#include <unistd.h>
#include <stdlib.h>

#define DEF_BUCKS 40

using namespace std;


HashTable::HashTable(int n){
    if(n<1){
        this->buckets=DEF_BUCKS;
    }
    else{
        this->buckets=n;
    }

    account_bucket= new Account_List*[buckets];
    lock_arr = new pthread_mutex_t*[buckets];
    int i;
    for(i=0;i<buckets;i++){
        account_bucket[i] = new Account_List();
        lock_arr[i] = new pthread_mutex_t;
        pthread_mutex_init(lock_arr[i],NULL);
    }
}

HashTable::~HashTable(){
    int i;
    for(i=0;i<buckets;i++){
        delete account_bucket[i];
        pthread_mutex_destroy(lock_arr[i]);
        delete lock_arr[i];
    }
    delete[] account_bucket;
    delete[] lock_arr;
}

int HashTable::StringToBucket(string name){ //Hashing Function uses the ascii sum of the first 4 digits
    char char_arr[sizeof(int)];
    unsigned int i,counter=0;
    for(i=0;i<sizeof(int);i++) char_arr[i]=0; //Char_arr iniitializing,in case len(string)<sizeof(int)
    unsigned int bytes=name.length();

    if(bytes>sizeof(int)) bytes=sizeof(int); //Prevent memory error in case of len(string) < sizeof(int)

    memcpy(char_arr,name.c_str(),bytes);

    for(i=0;i<sizeof(int);i++) counter=counter + char_arr[i];

    return counter%buckets;
}

Int_List* HashTable::Convert(String_List &arg){
    Int_List *to_lock = new Int_List();
    string_node* temp=arg.head;

    while(temp){
        to_lock->Add(this->StringToBucket(temp->name));
        temp=temp->next;
    }
    return to_lock;
}

void HashTable::Lock(Int_List &arg){
    int_node *temp=arg.head;

    while(temp){
        pthread_mutex_lock(lock_arr[temp->bucket]);
        temp=temp->next;
    }
}

void HashTable::Unlock(Int_List &arg){
    int_node *temp = arg.head;

    while(temp){
        pthread_mutex_unlock(lock_arr[temp->bucket]);
        temp=temp->next;
    }
}

void HashTable::Add_Account(string name,int start,int delay,stringstream &res){
    clock_t started,stop;
    started=clock();
    int target_bucket=this->StringToBucket(name);
    //Built lock_list
    Int_List to_lock;
    to_lock.Add(target_bucket);
    //Lock target backet
    this->Lock(to_lock);
    //CS STARTS
    //Check if account already exists
    if(account_bucket[target_bucket]->Exists(name)==true){
        res << "Error. Account Creation Failed(" << name << ":" << start ;
    }
    else{
        account_bucket[target_bucket]->Add_Account(name,start);
        res << "Success. Account creation(" << name << ":" << start ;
    }
    if(delay>0){
        res << " : " << delay << ")" << endl;
    }
    else{
        res << ")" << endl;
    }
    //Sleep if necesserary
    stop=clock();
    this->Sleep((clock_t)started,(clock_t)stop,delay);
    //CS END
    //Unlock
    this->Unlock(to_lock);
    return;
}

void HashTable::Transfer(string source,int amount,int delay,String_List &targets,stringstream &res){
    clock_t started,stop;
    started = clock();
    bool err=false;
    //Built lock list
    int source_backet = this->StringToBucket(source);
    Int_List *to_lock = this->Convert(targets); //Accounts to recieve money
    to_lock->Add(source_backet); //Account to send money
    //Lock Neccessery backets
    this->Lock(*to_lock);
    //CS starts
    //Check if all accounts exists
    if(account_bucket[source_backet]->Exists(source)==false) err=true; //Source account doesnt exists
    if(account_bucket[source_backet]->Balance(source) < targets.Count()*amount) err=true; //Not enough balance
    string_node *temp=targets.head;
    while(temp){
        if(account_bucket[this->StringToBucket(temp->name)]->Exists(temp->name)==false) err=true; //Target account doesnt exists
        temp = temp->next;
    }

    if(err==true){ // Error occured
        if(targets.Count()>1){ //Multi transfer
            if(delay>0){
                res <<"Error. Multi-Transfer addition failed ("<< source << " : " << amount << " : " << delay << ")" << endl;
            }
            else{
                res << "Error. Multi-Transfer addition failed ("<< source << " : " << amount << ")" << endl;
            }
        }
        else{ //Single Transfer
            if(delay>0){
                res << "Error Transfer addition failed ("<< source << " : " << targets.head->name << " : " << amount << " : " << delay << ")" << endl;
            }
            else{
                res << "Error Transfer addition failed ("<< source << " : " << targets.head->name << " : " << amount << ")" << endl;
            }
        }
    }
    else{ //Successfull
        //Make the transfer
        account_bucket[source_backet]->Send_Money(source,amount*targets.Count());

        temp=targets.head;
        while(temp){
            account_bucket[this->StringToBucket(temp->name)]->Add_Money(source,temp->name,amount);
            temp = temp->next;
        }

        //Built Rescponse
        if(targets.Count()>1){
            if(delay>0){
                res << "Success. Multi-Transfer addition ("<< source << " : " << amount << " : " << delay << ")" << endl;
            }
            else{
                res << "Success. Multi-Transfer addition (" << source << " : " << amount << ")" << endl;
            }
        }
        else{
            if(delay>0){
                res << "Success Transfer addition (" << source << " : " << targets.head->name << " : " << amount << " : " << delay << ")" << endl;
            }
            else{
                res << "Success Transfer addition (" << source << " : " << targets.head->name << " : " << amount << ")" << endl;
            }
        }
    }
    stop=clock();
    this->Sleep(started,stop,delay);
    //CS END,Unlock
    this->Unlock(*to_lock);
    delete to_lock;
    return;
}

void HashTable::Balance(String_List &targets,stringstream &res){
    bool err=false;
    //Make lock list
    Int_List *to_lock = this->Convert(targets);
    //lock buckets
    this->Lock(*to_lock);
    //CS Starts
    //Check if all accounts exists
    string_node *temp=targets.head;
    while(temp){
        if(account_bucket[StringToBucket(temp->name)]->Exists(temp->name)==false) err=true;
        temp = temp->next;
    }

    if(err==true){
        if(targets.Count()>1){
            res << "Error. Multi-Balance failed (" ;
            temp=targets.head;
            while(temp){
                res << temp->name << " : " ;
                temp= temp->next;
            }
            res << ")" << endl;
        }
        else{
            res << "Error. Balance (" << targets.head->name << ")" << endl;
        }
    }
    else{
        if(targets.Count()>1){
            res << "Success. Multi-Balance (" ;
            temp=targets.head;
            while(temp){
                res << temp->name << " / " << account_bucket[StringToBucket(temp->name)]->Balance(temp->name) << " : " ;
                temp = temp->next;
            }
            res << ")" << endl;
        }
        else{
            res << "Success. Balance (" << targets.head->name << " : " << account_bucket[StringToBucket(targets.head->name)]->Balance(targets.head->name) << ")" << endl;
        }
    }
    //CS END,unlock
    this->Unlock(*to_lock);
    delete to_lock;
    return;
}

void HashTable::Sleep(clock_t start,clock_t stop,int delay){
    long elapsed= (long)(stop-start)*1000 / CLOCKS_PER_SEC; //Elapsed time in ms
    if(elapsed < delay){
        usleep((delay-elapsed)*1000);
    }
    return;
}

void HashTable::Service(stringstream &in,stringstream &out){ //Gets one command at a time and calls the right function of hashtable.
    string token,name;
    int amount,delay=0;
    String_List target;

    in >> token;

    if(token.compare("add_account")==0){
        in >> token;
        if(in.good()==false){ //Not enough arguments
            out << "Error. Unknown Command. (Not Enough Arguments)" << endl;
            return;
        }
        if(Is_Int(token.c_str())==false){ //User didnt provide amount
            out << "Error. Unknown Command. (No Amount)" << endl;
            return;
        }
        amount=atoi(token.c_str());

        in >> name;
        if(Is_Int(name.c_str())==true){ //Names must not be ints
            out << "Error. Unknown Command. (Names cannot be integers)" << endl;
            return;
        }
        if(name.length()<2){ //Name must be at least 2 chars long
            out << "Error. Unknown Command. (Name should be at least 2 characters long)" << endl;
            return;
        }

        if(in.good()==true){ //We are not done parsing client's msg yet
            in >> token;
            if(Is_Int(token.c_str())==true){
                delay=atoi(token.c_str());
                token.erase(token.begin(),token.end());
                in >> token;
                if(token.length()>2 || in.good()==true){ //There is junk after delay
                    out << "Error. Unknown Command." << endl;
                    return;
                }
            }
            else if(token.length()>=2){
                out << "Error. Unknown Command." << endl;
                return;
            }
        }

        this->Add_Account(name,amount,delay,out);

    }
    else if(token.compare("add_transfer")==0 || token.compare("add_multi_transfer")==0){
        in >> token;
        if(Is_Int(token.c_str())==false){ //Client didnt provide amount
            out << "Error. Unknown Command. (No Amount)" << endl;
            return;
        }

        amount=atoi(token.c_str());
        if(amount<0){
            out << "Error. Unknown Command. (Amount can't be negative)" << endl;
            return;
        }

        in >> name;
        if(Is_Int(name.c_str())==true || name.length()<2){ //Second token is not a name.
            out << "Error. Unknown Command. (Second Token isn't a name)" << endl;
            return;
        }

        delay=target.StreamToList(in);
        if(delay<0){ //Systax error occured
            out << "Error. Unknown Command. (Syntax Error at 'delay')" << endl;
            return;
        }
        this->Transfer(name,amount,delay,target,out);

    }
    else if(token.compare("print_balance")==0 || token.compare("print_multi_balance")==0){
        delay = target.StreamToList(in);

        if(delay!=0){ //If delay=-1 an syntax error occured,if delay >0 user provided delay,its not an option in that command.
            out << "Error. Unknown Command. (Delay is not an option in that command)" << endl;
            return;
        }
        this->Balance(target,out);
    }
    else{
        out << "Error. Unknown Command" << endl;
    }
    return ;
}
