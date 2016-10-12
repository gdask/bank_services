# bank_services: Project description in a nutshell.
Project implemented in C++, using POSIX API. Data structures are 'tailor-made', i didn't use STL.

BankClient:  Sends commands at 'BankServer' and receives response, using tcp.

BankServer:  Provides a web service. 'Master' thread listening at a web socket for incoming connections. 'Worker' threads receive and execute commands from 'BankClient'.

### Supported Commands:
   *  add_account [init_amount] [name]
   *  add_transfer [amount] [source_name] [destination_name]*
   *  print_balance [name]*

### 'BankServer' Components
  
  *  main:  Arguments: -p Server listening port   -s Thread pool size    -q Queue of incoming connections size <br>
            Main checks arguments,setting up the server's listening port and init 'System'.
          
  *  System:  Create,destroy and manage:
    * Thread-safe queue of file descriptors : Stores the incoming connections until a 'Worker' is available.
    * 'Worker' threads : Receive message from client,forward it at 'DB', respond to client. Repeat until client disconnects.
    * Database: Storing and query anwsering about bank accounts,amounts,transfers.
    
  *  DB:  A thread-safe hash-table. Hashing function uses accounts' names. Every bucket of hashtable is protected from a different mutex. Different threads cannot access data from the same bucket at the same time. In the post parsing stage of the command,an ordered list of mutexes that needs to be locked,is created. Mutex locking is done in ascending order of buckets. This is very useful to avoid any possible deadlock situations.

  * Lists: Implementation of data structures for storing bank clients' information.
