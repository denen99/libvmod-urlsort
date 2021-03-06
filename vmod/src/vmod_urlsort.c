#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include "vrt.h"
#include "bin/varnishd/cache.h"
#include "vcc_if.h"

struct param_node {
  char *value;
  struct param_node *right, *left;
};

typedef struct param_node Node;

int
init_function(struct vmod_priv *priv, const struct VCL_conf *conf)
{
   return (0);
}

static void insert(Node **tree, Node *item) {
  if(!(*tree)) {
    *tree = item;
    return;
  }
  if(strcmp(item->value, (*tree)->value) > 0) 
    insert(&(*tree)->left, item);
  else if(strcmp((*tree)->value, item->value) > 0)
    insert(&(*tree)->right, item);
}

static void sortparam(Node *tree, char *out) {
  if(tree->right) sortparam(tree->right, out);
  strcat(out, tree->value);
  strcat(out, "&\0");
  if(tree->left) sortparam(tree->left, out);
}

const char * vmod_sortquery(struct sess *sp, const char *in) {
  Node *root, *current;
  root = NULL;

  char *tok, *result, *sorted_url;
  sorted_url = (char*) malloc(strlen(in) + 1);
  result = (char*) malloc(strlen(in) + 1);
  result = strtok(in, "?");

  while(1) {
    current = (Node* )malloc(sizeof(Node));
    current->left = current->right = NULL;
    tok = strtok(NULL, "&");
    if(tok == NULL) break;
    current->value = tok;
    insert(&root, current);
  }

  if(root == NULL) return in;
  sorted_url[0] = 0;
  sortparam(root, sorted_url);
  strcat(result, "?");
  strcat(result, sorted_url);
  strncpy(result, result, strlen(result) - 2); 
  return result;
}
