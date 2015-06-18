// Copyright (C) 2015 Nippon Telegraph and Telephone Corporation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>

#include "saifdb.h"

#include "sai_rocker_fdb.h"
#include "../netdev/netlink.h"

static struct fib* fdb;
static int fdb_size;

int maccmp(const void* mac1, const void* mac2){
  return memcmp(mac1, mac2, 6);
}

int fib_equal(const struct fib* f1, const struct fib* f2){
  return !strcmp(f1->destination, f2->destination) && !maccmp(f1->addr, f2->addr);
}

void fdb_init(){
  fdb_size = get_fdb(&fdb);

  // comparing the mac address is enough, because no two fibs can have
  // the same mac addresses and destionation ports at the same time
  qsort(fdb, fdb_size, sizeof(struct fib), maccmp);
}

// Returns:
//   1 -> when the fdb has been updated
//   0 -> when the fdb is not updated
int get_latest_fdb(){
  struct fib* current_fdb;
  int current_fdb_size;
  int i, updated = 0;

  current_fdb_size = get_fdb(&current_fdb);
  qsort(current_fdb, current_fdb_size, sizeof(struct fib), maccmp);

  // when the new fdb has a different size from the old one, the new one is updated
  if(fdb_size != current_fdb_size){
    updated = 1;
  }
  else{
    for(i=0;i<current_fdb_size;i++){
      // if any fib in the two fdbs are different, the new one is updated
      if(!fib_equal(&fdb[i], &current_fdb[i])){
	updated = 1;
	break;
      }
    }
  }

  if(updated){
    free(fdb);
    
    fdb_size = current_fdb_size;
    fdb = current_fdb;

    return 1;
  }
  else{
    free(current_fdb);
    return 0;
  }
}

// arg: time interval (second) on which this thread is invoked
void fdb_update_notifier(void* _arg){
  struct fdb_update_nitifier_arg* arg =  (struct fdb_update_nitifier_arg*)_arg;
  fdb_init();
  
  while(1){
    sleep(arg->interval);

    int updated = get_latest_fdb();
    
    if(updated){
      arg->f(0, NULL, 0, NULL);
    }
  }
}
