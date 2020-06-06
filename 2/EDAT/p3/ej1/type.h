/* TYPE MODULE */
/* TYPE ENUMERATION OF SUPPORTED DATA TYPES */
#ifndef TYPE_H
#define TYPE_H

#include <stdlib.h>
#include <stdio.h>

typedef enum {
    INT, /*integer*/
    STR, /*string*/
    LLNG, /*long int*/
    DBL /*double*/
} type_t;


/*
   value_cmp(type_t type, void* value1, void* value2);

   Compares two values given the pointers to their value and their
   type. The function works like the strcmp function in C: it returns
   a value less then zero if the first value is less than the second,
   zero if the two are equal, and a value greater than zero if the
   second value is greater than the first. Note that the function
   guarantees that the sign of the return value is the appropriate,
   but it does not make any promises as to the value that is returned.


   NOTE:
   It is possible that you won't need this function for your
   implementation. However, I need it in order to implement the test
   program and that is the main reason why this function is here.
 */
int value_cmp(type_t type, void* value1, void* value2);



/*
   value_length(type_t type, void* value);

   Given a pointer to a value, and the type of this value, the
   function computes its occupancy in bytes.

   Note that if the value is of a fixed lenght type, the parameter
   "value" is not used. For example, calling

   int n;
   value_length(INT, &n);

   will return sizeof(int) regardless the type of n. On the other
   hand, for a variable lenght type, such as a string, the value
   determine the length:

   char val[] = "Hello, world"
   assert(value_length(STR, val) == strlen(val)+1);

   Note that the value returned is the occupancy of the complete
   string in bytes, that is, it includes the final 0.
*/
size_t value_length(type_t type, void* value);

/*
   Given a value and its type, prints on the given stream using the
   appropriate format. In order to print on the screen, use stdout as
   the stream:

   char c[2];
   c[0] = 0;
   c[1] = 1;
   print_value(stdout, INT, (void *) c);

   will print 256 on the screen (why?)
*/
void print_value(FILE* f, type_t type, void* value);



#endif
