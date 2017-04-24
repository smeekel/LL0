#include "avltree.h"


static AVLNode* insert      (AVLTree* tree, AVLNode* node, AVLNode* toinsert);
static AVLNode* delete      (AVLTree* tree, AVLNode* node, AVLNode* todelete);
static void     clear       (AVLTree*, AVLNode*);
static int      max         (const int, const int);
static int      height      (const AVLNode*);
static int      getbalance  (const AVLNode*);
static AVLNode* rightrotate (AVLNode*);
static AVLNode* leftrotate  (AVLNode*);


int avltree_initialize(AVLTree* tree, avlnode_compare pfcompare, avlnode_delete pfdelete)
{
  errstate_initialize(&tree->errors);  

  tree->root      = NULL;
  tree->ncompare  = pfcompare;
  tree->ndelete   = pfdelete;

  return SUCCESS;
}

int avltree_terminate(AVLTree* tree)
{
  avltree_clear(tree);
  errstate_terminate(&tree->errors);
  return SUCCESS;
}

int avltree_insert(AVLTree* tree, AVLNode* node)
{
  errstate_clear_all(&tree->errors);

  node->left    = NULL;
  node->right   = NULL;
  node->parent  = NULL;
  node->height  = 1;

  tree->root = insert(tree, tree->root, node);
  if( errstate_haserror(&tree->errors) )
    return ERROR;

  return SUCCESS;
}

int avl_delete(AVLTree* tree, AVLNode* node)
{
  errstate_clear_all(&tree->errors);

  tree->root = delete(tree, tree->root, node);
  
  if( errstate_haserror(&tree->errors) )
    return ERROR;

  return SUCCESS;
}

void avltree_clear(AVLTree* tree)
{
  if( tree->root) clear(tree, tree->root);
  tree->root = NULL;
}

void clear(AVLTree* tree, AVLNode* node)
{
  if( node->left ) 
  {
    clear(tree, node->left);
    node->left = NULL;
  }

  if( node->right ) 
  {
    clear(tree, node->right);
    node->right = NULL;
  }
  
  tree->ndelete(node);
}


int max(const int A, const int B)
{
  return A>B ? A : B ;
}

int height(const AVLNode* node)
{
  if( !node ) return 0;
  return node->height;
}

int getbalance(const AVLNode* node)
{
  if( !node ) return 0;
  return height(node->left) - height(node->right);
}

AVLNode* rightrotate(AVLNode* node)
{
  AVLNode* x  = node->left;
  AVLNode* xr = x->right;

  x->right    = node;
  node->left  = xr;

  node->height  = max(height(node->left), height(node->right))+1;
  x->height     = max(height(x->left),    height(x->right))+1;

  return x;
}

AVLNode* leftrotate(AVLNode* node)
{
  AVLNode* x  = node->right;
  AVLNode* xl = x->left;

  x->left     = node;
  node->right = xl;

  node->height  = max(height(node->left), height(node->right))+1;
  x->height     = max(height(x->left),    height(x->right))+1;

  return x;
}

AVLNode* insert(AVLTree* tree, AVLNode* node, AVLNode* toinsert)
{
  if( !node )
    return toinsert;

  if( tree->ncompare(toinsert, node)<0 )
  {
    node->left = insert(tree, node->left, toinsert);
  }
  else if( tree->ncompare(toinsert, node)>0 )
  {
    node->right = insert(tree, node->right, toinsert);
  }
  else
  {
    errstate_adderror(&tree->errors, "Error inserting duplicate key");
    return NULL;
  }

  node->height = max(height(node->left), height(node->right)) + 1;

  const int balance = getbalance(node);
  
  if( balance>1 && tree->ncompare(node->left, toinsert)>0 )
    return rightrotate(node);

  if( balance<-1 && tree->ncompare(node->right, toinsert)<0 )
    return leftrotate(node);

  if( balance>1 && tree->ncompare(node->left, toinsert)<0 )
  {
    node->left = leftrotate(node->left);
    return rightrotate(node);
  }

  if( balance<-1 && tree->ncompare(node->right, toinsert)>0 )
  {
    node->right = rightrotate(node->right);
    return leftrotate(node);
  }

  return node;
}

AVLNode* delete(AVLTree* tree, AVLNode* node, AVLNode* todelete)
{
  if( !node ) return NULL;

  if( tree->ncompare(todelete, node)<0 )
  {
    node->left = delete(tree, node->left, todelete);
  }
  else if( tree->ncompare(todelete, node)>0 )
  {
    node->right = delete(tree, node->right, todelete);
  }
  else
  {
    if( node->left==NULL || node->right==NULL )
    {
      AVLNode* temp = node->left ? node->left : node->right ;

      if( !temp )
      {
        // No children
        tree->ndelete(node);
        node = NULL;
      }
      else
      {
        tree->ndelete(node);
        node = temp;
      }
    }
    else
    {
      
    }
  }

  if( !node ) reutnr NULL;


  return NULL;
}
