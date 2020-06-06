/** 
 * @brief Defines the functionality associated with the requests made to the server.
 * 
 * @file request_response.h
 * @author Pair number 15 Redes II Group 2361
 * @version 1.0
 * @date 11-02-2020 
 * @copyright GNU Public License
 */

#include <stdlib.h>
#include <syslog.h>
#include "../includes/picohttpparser.h"

/** @struct HTTPRequest
 * 
 *  @brief Holds the fields of a parsed HTTP request.
 */
typedef struct {
  const char *method; /*Method of the request: POST, GET, OPTIONS*/
  size_t method_len; /*Length of method*/
  const char *path; /*Everything after the first '/' character, path to the requested object*/
  size_t path_len; /*Length of path*/
  int minor_version; /*HTTP 1.0 or 1.1*/
  struct phr_header headers[100]; /*Headers of the request*/
  size_t num_headers; /*Number of headers*/
  char *body; /*Body of the request*/
} HTTPRequest;

/** @struct Parameter
 * 
 *  @brief Holds a parameter of a URL with of format key=value&...
 */
typedef struct {
  char *key;
  char *value;
} Parameter;


/** 
 * @brief Processes the request of the client associated to the given socket, parsing its fields and 
 * executing a specific functionality regarding the method of the request.
 * @param connDescr int - descriptor of the socket associated to a connection
 * @param request HTTPRequest - structure to store the fields of the parsed request
 * @return int - error code
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int process_request(int connDescr, HTTPRequest *request);

/** 
 * @brief Processes a GET petition.
 * @param connDescr int - descriptor of the socket associated to a connection
 * @param request HTTPRequest - structure to store the fields of the parsed request
 * @param serverSignature char * - name of the server
 * @param path char* - path of the petition
 * @return int - error code
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int process_GET(int connDescr, HTTPRequest *request, char* serverSignature, char *path);

/** 
 * @brief Processes a POST petition.
 * @param connDescr int - descriptor of the socket associated to a connection
 * @param request HTTPRequest - structure to store the fields of the parsed request
 * @param serverSignature char * - name of the server
 * @param path char* - path of the petition
 * @param request_body char* - body of the petition
 * @return int - error code
 *	
 * @author: Pair number 15 Redes II Group 2361
 */
int process_POST(int connDescr, HTTPRequest *request, char* serverSignature, char* path, char* request_body);

/** 
 * @brief Handles the response for an OPTIONS petition. Sends an empty response with an 
 * Allowed header field with the allowed methods.
 * @param connDescr int - descriptor of the socket associated to a connection
 * @param request HTTPRequest - structure with the fields of the parsed request
 * @param serverSignature char * - name of the server
 * @return int - error code
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int process_OPTIONS(int connDescr, HTTPRequest *request, char* serverSignature);

/** 
 * @brief Handles the response for unsupported methods in petitions. Sends a response with 418 code.
 * @param connDescr int - descriptor of the socket associated to a connection
 * @param request HTTPRequest - structure with the fields of the parsed request
 * @param serverSignature char * - name of the server
 * @return int - error code
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int process_unsupported(int connDescr, HTTPRequest *request, char* serverSignature);

/** 
 * @brief Sends a response with 404 code.
 * @param connDescr int - descriptor of the socket associated to a connection
 * @param request HTTPRequest - structure with the fields of the parsed request
 * @param serverSignature char * - name of the server
 * @return int - error code
 *	
 * @author: Pair number 15 Redes II Group 2361
 */

int process_404(int connDescr, HTTPRequest *request, char* serverSignature);


/** 
 * @brief Creates the response for a given script.
 * @param path char* - path of the script
 * @param extension char* - extension of the script
 * @param parameters Parameter* - arguments to call the scripts
 * @param nParams int - number of arguments passes through parameters
 * @param script_response char* - response of the script
 * @return int - error code
 *	
 * @author: Pair number 15 Redes II Group 2361
 */
int create_script_response(char* path, char* extension, Parameter* parameters, int nParams, char* script_response);

/** 
 * @brief Sends a binary file to the given socket.
 * @param connDescr int - descriptor of the socket associated to a connection
 * @param path char* - path of the binary field
 *	
 * @author: Pair number 15 Redes II Group 2361
 */
int send_file(int connDescr, char* path);