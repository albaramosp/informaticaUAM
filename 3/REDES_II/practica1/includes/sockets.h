/** 
 * @brief Defines the functionality related to the use of sockets and connections.
 * 
 * @file sockets.h
 * @author Pair number 15 Redes II Group 2361
 * @version 1.0
 * @date 11-02-2020 
 * @copyright GNU Public License
 */

#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>


/** 
 * @brief Opens a socket of the specified domain, type and protocol.
 * @param domain int - family type of the socket
 * @param type int - type of the socket
 * @param protocol int - protocol of the socket
 * @return int - socket descriptor
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int openSocket(int domain, int type, int protocol);

/** 
 * @brief Binds a socket to a certain port number and address.
 * @param socket int - socket to be bound
 * @param dpmain int - domain of the socket
 * @param port int - protocol for the socket
 * @param addr int - address for the socket
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

void bindSocket(int socket, int domain, int port, int addr);

/** 
 * @brief Initiates the socket for the server. It opens the socket for INET and STREAM mode, 
 * binds it to the server's port and creates the queue for unattended petitions.
 * @return int - socket descriptor of the server
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int init_server();

/** 
 * @brief Accepts a queued connection on the server, generating a new socket for it.
 * @param serverSocket int - descriptor of the socket where the server is listening
 * @return int - descriptor of the socket opened for that connection
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int accept_connection(int serverSocket);

/** 
 * @brief Transforms a process into a daemon, killing its parent and being adopted by the main process.
 * To keep the system secure, standard file descriptors associated to the daemon are closed.
 * 	
 * @author: Pair number 15 Redes II Group 2361
 */

void toDaemon();
    
