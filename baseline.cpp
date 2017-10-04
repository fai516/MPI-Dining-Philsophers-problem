#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <map>
#include <unistd.h>
#include <string>
#include "mpi.h"

//run compiled code (for 5 philosophers) with mpirun -n 6 program

using namespace std;

std::string decodeMsg(int n){
  std::string out;
  switch(n){
  case 0:out = "requesting to eat";break;
  case 1:out = "stopping to eat(wanting to think)";break;
  case 2:out = "allowed to eat";break;
  default:out ="waiting";break;
  }
  return out;
}

int main ( int argc, char *argv[] )
{
  int id; //my MPI ID
  int p;  //total MPI processes
  MPI::Status status;
  int tag = 1;

  //  Initialize MPI.
  MPI::Init ( argc, argv );

  //  Get the number of processes.
  p = MPI::COMM_WORLD.Get_size ( );

  //  Determine the rank of this process.
  id = MPI::COMM_WORLD.Get_rank ( );

  //Safety check - need at least 2 philosophers to make sense
  if (p < 3) {
	    MPI::Finalize ( );
	    std::cerr << "Need at least 2 philosophers! Try again" << std::endl;
	    return 1; //non-normal exit
  }

  srand(id + time(NULL)); //ensure different seeds...

//  cout << "This is process " << id << " of " << p
//       << " I'll give you a random number: " << rand() << endl;

  //  Setup Fork Master (Ombudsman) and Philosophers
  if ( id == 0 ) //Master
  {
	int msgIn; //messages are integers
	//let the philosophers check in

    /* Philosophers eating status */
    map<int,bool> eatingStat;


    //cout << forks[0]+1234 << endl;
    for (int i = 1; i < p; i++) {
  		MPI::COMM_WORLD.Recv ( &msgIn, 1, MPI::INT, MPI::ANY_SOURCE, tag, status );
  		std::cout << "Receiving message " << decodeMsg(msgIn) << " from Philosopher ";
      std::cout << status.Get_source() << std::endl;

      /* Starting waiter method*/
      int target = status.Get_source(); //get the # of philosopher
      if(msgIn == 0){ //requesting to eat
        int left = target+1; //his left side
        int right = target-1; //his right side
        /* handling edge case */
        if(left > 5) left = 1; //if the left side is philosopher 6, that means it actually is philosopher 1
        if(right < 1) right = 5; //same idea as above
        if(eatingStat[left]==false || eatingStat[right]==false){ //adjesent not eating => forks is available
          cout << "@@Philosopher " << target << " is allowed to eat!!" << endl;;
          eatingStat[target]=true;
        }
        else{
          cout << "@@Philosopher " << target << " can't eat because his neighbour is eating!!" << endl;
        }
      }
      else if(msgIn == 1){
        if(eatingStat[target]==true){
          cout << "@@Philosopher " << target << " stopping to eat!";
          eatingStat[target]=false;
        }
        else{
          cout << "@@Philosopher " << target << " can't stop eating because he isn't even eating!";
        }
      }
      else{
        cout << "@@Waiter: You can do whatever you want but eat!" << endl;
      }
	  }
  }
  else //I'm a philosopher
  {
	//cout << id << endl;
/* NOTE: The following code sends a random integer (message) back to the server. You do
 *
 *                            * * * * * * * * NOT * * * * * * * *
 *
 * want to send a random message in the solution... you want a specific message (integer)
 * to mean something specific (like requesting to eat, being allowed to eat, returning
 * resources, etc.)
 */
    int msgOut = rand() % p; //pick a number between 0 and the number of philosophers

    msgOut = 0;
	std::cout << "This is Philosopher " << id << " sending message of " << decodeMsg(msgOut) << std::endl;

   	//check in with master - send a random number (node 0 = master)
	MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag );
  }
  //  Terminate MPI.
  MPI::Finalize ( );
  return 0;
}
