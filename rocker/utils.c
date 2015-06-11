#include "utils.h"
#include <stdlib.h>
#include <time.h>

unsigned char get_random_byte(){
  static int init = 0;

  if(!init){
    srand(time(NULL));
    init = 1;
  }

  return (rand() % 0xff);
}

int make_netmask_bits(sai_ip4_t netmask){
  int n;
  
  for(n=32;n>0;n--){
    if( (netmask >> (n-1)) & 1 ){
      ;
    }
    else
      break;
  }

  return (32 - n);
}
