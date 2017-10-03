#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <unistd.h>
#include "mpi.h"

//run compiled code (for 5 philosophers) with mpirun -n 6 program

using namespace std;

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
    for (int i = 1; i < p; i++) {
		MPI::COMM_WORLD.Recv ( &msgIn, 1, MPI::INT, MPI::ANY_SOURCE, tag, status );
		std::cout << "Receiving message " << msgIn << " from Philosopher ";
//OLD SYNTAX		std::cout << status.getSource() << std::endl;
		std::cout << status.Get_source() << std::endl;
	}
  }
  else //I'm a philosopher
  {
/* NOTE: The following code sends a random integer (message) back to the server. You do 
 *
 *                            * * * * * * * * NOT * * * * * * * * 
 *
 * want to send a random message in the solution... you want a specific message (integer)
 * to mean something specific (like requesting to eat, being allowed to eat, returning 
 * resources, etc.)
 */
    int msgOut = rand() % p; //pick a number between 0 and the number of philosophers
	
	std::cout << "This is Philosopher " << id << " sending message of " << msgOut << std::endl;
	
   	//check in with master - send a random number (node 0 = master)
	MPI::COMM_WORLD.Send ( &msgOut, 1, MPI::INT, 0, tag ); 
  }
  
  //  Terminate MPI.
  MPI::Finalize ( );
  return 0;
}
