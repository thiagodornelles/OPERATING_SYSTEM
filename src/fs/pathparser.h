#ifndef PATH_PARSER_H
#define PATH_PARSER_H

struct path_root
{
  int drive_number;
  struct path_part* first;
};

struct path_part
{
  const char* name;
  struct path_part* next;
};

void pathparser_free(struct path_root* root);
struct path_root* pathparser_parse(const char* path, const char* current_directory);

#endif PATH_PARSER_H