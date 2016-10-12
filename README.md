# bank_services: Project description in a nutshell.
Project implemented in C++, using POSIX API. Data structures are 'tailor-made', i didn't use STL.

BankClient:  Sents commands at 'BankServer' and recieves response, using tcp.

BankServer:  Provides a web service. 'Master' thread listening at a web socket for incoming connections. 'Worker' threads recieve and execute commands from 'BankClient'.

### Supported Commands:
   *  add_account [init_amount] [name]
   *  add_transfer [amount] [source_name] [destination_name]*
   *  print_balance [name]*

### 'BankServer' Components
  
  *  main:  Arguments: -p Server listening port   -s Thread pool size    -q Queue of incomining connections size <br>
            Main checks arguments,setting up the server's listening port and init 'System'.
          
  *  System:  Create,destroy and manage:
    * Thread-safe queue of file descriptors : Stores the incoming connections until a 'Worker' is available.
    * 'Worker' threads : Recieve messege from client,forward it at 'DB', respond to client. Repeat until client disconnects.
    * Database: Storing and query anwsering about bank accounts,amounts,transfers.
    
  *  DB:  A thread-safe hash-table. Hashing function use accounts' names. Every bucket of hashtable is protected from a different a mutex. Different threads cannot access data from the same bucket at the same time. In post parse stage of the command,an ordered list of mutexes that needs to be locked,is created. Mutex locking in an order from lowest bucket to highest bucket. This is very usefull to avoid any possible deadlock situation.

  * Lists: Implementation of data structures for storing bank clients' information.
