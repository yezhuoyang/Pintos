#include "devices/input.h"
#include "threads/input.h"
int
get_line (char * buffer)
{
  int current_input_char = 0;
  char * buffer_pointer = buffer;
  while (current_input_char != '\r')
   {
    current_input_char = input_getc ();
    if (current_input_char == '\b')
     {
      if (buffer_pointer != buffer)
       {
        printf ("\b \b");
        --buffer_pointer;
       }
     }
    else 
     {
      if (current_input_char == '\r')
       {
        *buffer_pointer = '\0';
        putchar ('\n');
       } 
      else 
       {
        *(buffer_pointer++) = current_input_char;
        putchar (current_input_char);
       }
     }
   }
  return 0;
}