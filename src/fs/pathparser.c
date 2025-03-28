#include "pathparser.h"
#include "status.h"
#include "string/string.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"

static int pathparser_path_valid_format(const char* path){
  int len = strnlen(path, PEACHOS_MAX_PATH);
  return len >= 3 && isdigit(path[0]) && memcmp((void*)&path[1], ":/", 2) == 0;
}

static int pathparser_get_drive_by_path(const char** path){
  if(!pathparser_path_valid_format(*path)){
    return -EBADPATH;
  }

  int drive_no = tonumericdigit(*path[0]);  
  //Add 3 to skip the drive number and the :/
  *path += 3;
  return drive_no;
}

static struct path_root* pathparser_create_root(int drive_no){
  
  struct path_root* path_r = kzalloc(sizeof(struct path_root));
  path_r->drive_number = drive_no;
  path_r->first = 0;
  return path_r;
}

static const char* pathparser_get_path_part(const char** path){
  char* result_path_part = kzalloc(PEACHOS_MAX_PATH);
  int i = 0;
  while (**path != '/' && **path != 0){
    result_path_part[i] = **path;
    *path += 1;
    i++;    
  }
  //Skip slash to avoid problems
  if(**path == '/'){
    *path += 1;
  }
  
  if(i == 0){
    kfree(result_path_part);
    result_path_part = 0;
  }

  return result_path_part;
}

struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path){
  
  const char* path_part_str = pathparser_get_path_part(path);
  if(!path_part_str){
    return 0;
  }

  struct path_part* part = kzalloc(sizeof(struct path_part));
  part->name = path_part_str;
  part->next = 0;

  if(last_part){
    last_part->next = part;
  }

  return part;
}

void pathparser_free(struct path_root* root){
  struct path_part* part = root->first;
  while(part){
    struct path_part* next = part->next;
    kfree((void*) part->name);
    kfree(part);
    part = next;
  }
  kfree(root);
}

struct path_root* pathparser_parse(const char* path, const char* current_directory){
  int res = 0;
  const char* tmp_path = path;
  struct path_root* path_root = 0;

  if(strlen(path) > PEACHOS_MAX_PATH){
    goto out;
  }

  res = pathparser_get_drive_by_path(&tmp_path);
  if(res < 0){
    goto out;
  }

  path_root = pathparser_create_root(res);
  if(!path_root){
    goto out;
  }

  struct path_part* first_part = pathparser_parse_path_part(NULL, &tmp_path);
  if(!first_part){
    goto out;
  }

  path_root->first = first_part;
  struct path_part* part = pathparser_parse_path_part(first_part, &tmp_path);
  while(part){
    part = pathparser_parse_path_part(part, &tmp_path);
  }

out:
  return path_root;
}