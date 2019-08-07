#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

typedef struct fibHeapNode_s{
  int key;
  int degree;
  bool mark;
  struct fibHeapNode_s* parent;
  struct fibHeapNode_s*  left; 
  struct fibHeapNode_s*  right;
  struct fibHeapNode_s* child;
} fibHeapNode;

fibHeapNode* _init_fibHeapNode(int key){
  fibHeapNode* node = malloc(sizeof(fibHeapNode));
  node->key = key;
  node->mark = false;
  node->degree = 0;
  node->child = NULL;
  node->parent = NULL;
  node->left = node;
  node->right = node;
  return node;
}

void freeFibHeapNode(fibHeapNode *node){
  free(node);
}

typedef struct fibHeap_s{
  struct fibHeapNode_s* root;
  struct fibHeapNode_s* min;
  int n;
  
} fibHeap;

fibHeap* _init_fibHeap(){
  fibHeap* heap = malloc(sizeof(fibHeap));
  heap->min = NULL;
  heap->root = NULL;
  heap->n = 0;
  return heap;
}

void printFibHeap(fibHeap* H){
  fibHeapNode* n = H->root;
  printf("The roots: ");
  while(n->right != H->root){
    printf("%d-->", n->key);
    n = n->right;
  }
  printf("%d | # nodes: %d\n", n->key, H->n);
  
}

void fibHeapInsertNode(fibHeap* H, fibHeapNode* node){
  if(H->min == NULL){
    H->min = node;
    H->root = node;
  }
  else{
    if(node->key < H->min->key){
      H->min = node;
    }
    fibHeapNode* firstNode = H->root;
    fibHeapNode* lastNode = H->root->left;
    node->left = lastNode;
    node->right = firstNode;
    firstNode->left = node;
    lastNode->right = node;
  }
  H->n++;
}

void fibHeapInsertKey(fibHeap* H, int key){
  fibHeapNode* node = _init_fibHeapNode(key);
  fibHeapInsertNode(H, node);
}

fibHeap* fibHeapUnion(fibHeap* H1, fibHeap* H2){
  fibHeap* H = _init_fibHeap();
  H->min = H1->min;
  H->root = H1->root;
  if(H->min->key > H2->min->key){
    H->min = H2->min;
  }
  fibHeapNode* H1First = H1->root;
  fibHeapNode* H1Last = H1->root->left;
  fibHeapNode* H2First = H2->root;
  fibHeapNode* H2Last = H2->root->left;
  H1First->left = H2Last;
  H2Last->right = H1First;
  H1Last->right = H2First;
  H2First->left = H1Last;
  H->n = H1->n + H2->n;
  free(H1);
  free(H2);
  return H;
}

void fibHeapLink(fibHeap* H, fibHeapNode* n1, fibHeapNode* n2){
  // Remove n1 from the root list of H:
  fibHeapNode* prevNode = n1->left;
  fibHeapNode* nextNode = n1->right;
  prevNode->right = nextNode;
  nextNode->left = prevNode;
  n1->left = n1;
  n1->right = n1;
  
  // Make n1 child of n2, increment n2.degree:
  n1->parent = n2;
  if(n2->child == NULL){
    n2->child = n1;
  }
  // Connect the children of the new tree:
  fibHeapNode* firstChild = n2->child;
  fibHeapNode* lastChild = firstChild->left;
  firstChild->left = n1;
  lastChild->right = n1;
  n1->right = firstChild;
  n1->left = lastChild;
  
  n2->degree++;
  n1->mark = false;
  if(n1 == H->root){
    H->root = n2;
  }
}

void consolidate(fibHeap* H){
  int maxDegree = H->n;
  fibHeapNode* A[maxDegree];
  for(int i = 0; i <= maxDegree; i++){
    A[i] = NULL;
  }
  fibHeapNode* node = H->root;
  int deltaN = 0;
  do{
    int d = node->degree;
    while(A[d] != NULL){
      fibHeapNode* y = A[d];
      if(node->key > y->key) { // Exchange y and node
        fibHeapNode* temp = y;
        y = node;
        node = temp;
      }
      fibHeapLink(H, y, node); // Node becomes the parent of y and y is being removes from the root list
      A[d] = NULL;
      d++;
    }
    A[d] = node;
    node = node->right;
  } while(node != H->root);
  
  // Find the new min:
  H->min = H->root;
  fibHeapNode* min = H->min;
  for(int i = 0; i < H->n; i++){
    if (H->min->key > min->key){
      H->min = min;
    }
    min = min->right;
  }
}

fibHeapNode* fibHeapExtractMin(fibHeap* H){
  if (H->min != NULL){
    fibHeapNode* min = H->min;
    if (min->child){ // Add min's children to the root array.
      fibHeapNode* firstChild = min->child;
      fibHeapNode* lastChild = firstChild->left;
      fibHeapNode * firstRoot = H->root;
      fibHeapNode* lastRoot = firstRoot->left;
      firstRoot->left = lastChild;
      lastRoot->right = firstChild;
      lastChild->right = firstRoot;
      firstChild->left = lastRoot;
      
      fibHeapNode* x = min->child;
      do {  
        fibHeapNode *x = min->child;
        x->parent = NULL;
        x = x->right;
      } while(x != min->child);
    }
    
    // Remove the min:
    min->left->right = min->right;
    min->right->left = min->left;
    if (min == min->right){ // Node is the only node in the root array
      H->min = NULL;
      H->root = NULL;
    }
    
    else {
      H->min = min->right;
      H->root = H->min;
      consolidate(H);
    }
    
    H->n--;
    return min;
  }
  return NULL;
}

void cut(fibHeap* H, fibHeapNode* node, fibHeapNode* parent){
  // Remove node form child list of parent:
  if(node->right == node){ // Node is an only child
    parent->child == NULL;
  }
  else{
    (node->right)->left = node->left;
    (node->left)->right = node->right;
  }
  // Add node to the root list
  node->left = H->root->left;
  node->right = H->root;
  (H->root->left)->right = node;
  (H->root)->left = node;
  
  node->parent = NULL;
  node->mark = false;
}

void cascadingCut(fibHeap* H, fibHeapNode* parent){
  fibHeapNode* grandParent = parent->parent;
  if(grandParent != NULL){
    if (parent->mark == false){
      parent->mark = true;
    }
    else {
      cut(H, parent, grandParent); // Move parent up to root list
      cascadingCut(H, grandParent); // Mark grandparent and move it to root list if nessecary.
    }
  }
}

void fibHeapDecreaseKey(fibHeap* H, fibHeapNode* node, int val){
  if (val > node->key){
    fprintf(stderr, "Error: Value greater than key");
    exit(-1);
  }
  node->key = val;
  fibHeapNode* parent = node->parent;
  if(parent != NULL){
    if(parent->key > node->key){
      cut(H, node, parent);
      cascadingCut(H, parent); // Mark parent as a child looser, and promot it to the root list if nessecary.
    }
  }
  if (node->key < H->min->key){
    H->min = node;
  }
}

void fibHeapDelete(fibHeap* H, fibHeapNode* node){
  if(H->min == NULL){
    printf("Heap is empty");
  }
  else{
    fibHeapDecreaseKey(H, node, -INT_MAX);
    fibHeapExtractMin(H);
  }
}

int main(){
  fibHeap* h1 = _init_fibHeap();
  fibHeapInsertKey(h1, 7);
  fibHeapInsertKey(h1, 2);
  fibHeapInsertKey(h1, 3);
  fibHeapInsertKey(h1, 4);
  printf("Fibonacci Heap 1: ");
  printFibHeap(h1);
  fibHeap* h2 =_init_fibHeap();
  fibHeapInsertKey(h2, 5);
  fibHeapInsertKey(h2, 10);
  fibHeapInsertKey(h2, -2);
  fibHeapInsertKey(h2, 6);
  printf("Fibonacci Heap 2: ");
  printFibHeap(h2);
  
  fibHeap* h = fibHeapUnion(h1, h2);
  printf("Union of heap 1 and 2: ");
  printFibHeap(h);
  
  printf("Extracting the min: %d\n", fibHeapExtractMin(h)->key);
  printf("Extracting the min: %d\n", fibHeapExtractMin(h)->key);
  printf("Extracting the min: %d\n", fibHeapExtractMin(h)->key);
  printf("The fibonacci heap after extraction: ");
  printFibHeap(h);
  return 0;
}














