#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

struct BHNode {
	int processId;
    int initialExecutionTime;
    int newExecutionTime;
    int firstTimeArrival;
    int lastTimeArrival;
    double priorityValue;
    int totalWaitingTime;
    
    int degree;                 // Dügümün derecesi (çocuk sayisi)
    struct BHNode *parent;      // Üst dügüm
    struct BHNode *child;       // En soldaki çocuk
    struct BHNode *sibling;     // Sagdaki kardes
};

typedef struct BHNode BHNode;

struct BinomialHeap{
	BHNode*head;
};

typedef struct BinomialHeap BinomialHeap;


//current_et new_et same
double calculatePriority(int current_et,int t_arr,int e_max,int isFirstInsertion){
	double factor=1;
	double priorityValue=0;
	if(isFirstInsertion){
		priorityValue=factor*current_et;
	}
	else{
		double factor=exp(pow((2.0*current_et)/(3.0*e_max),3.0));
		priorityValue=factor*current_et;
	}
	return priorityValue;
	
}
void get_a_BT_k(BHNode *child, BHNode *parent) {
    child->parent = parent;
    child->sibling = parent->child;
    parent->child = child;
    parent->degree++;
}

BHNode *heapUnion(BinomialHeap *original, BinomialHeap *uni) {
    // 1. Adim: Kök listelerini derecelerine göre sirali tek bir liste yap (Sende var olan heapMerge)
    BHNode *new_head = heapMerge(original, uni);
    
    // Orijinal heap'lerin içini bosaltiyoruz
    original->head = NULL;
    uni->head = NULL;

    if (new_head == NULL) return NULL;

    BHNode *q = NULL;
    BHNode *x = new_head;
    BHNode *r = x->sibling;

    while (r != NULL) {
        
        if (x->degree != r->degree || 
            (r->sibling != NULL && r->sibling->degree == x->degree)) {
            prev = x;
            x = r;
        } 
        else {
            // Durum 2: Iki tane ayni dereceli agaç var, birlestirme (Link) zamani!
            if (isPrior(x, r)) {
                // x daha öncelikli (priorityValue daha küçük veya varis zamani daha eski)
                x->sibling = r->sibling;
                get_a_BT_k(r, x); // r'i x'un çocugu yap
            } else {
                // r daha öncelikli
                if (q == NULL) {
                    new_head = r;
                } else {
                    q->sibling = r;
                }
                get_a_BT_k(x, r); // x'u r'in çocugu yap
                x = r;
            }
        }
        r = x->sibling;
    }
    return new_head;
}

BHNode *nodeInit(int processId,int initialExecutionTime,int firstTimeArrival,int e_max)
{
    BHNode *node;

    node = (BHNode *)malloc(sizeof(BHNode));
    if (node == NULL)
        return NULL;

    node->processId = processId;
    node->initialExecutionTime = initialExecutionTime;
    node->firstTimeArrival=firstTimeArrival;
    node->newExecutionTime=initialExecutionTime;
    node->lastTimeArrival=firstTimeArrival;
    node->degree=0;
    node->totalWaitingTime=0;
    
    node->priorityValue = calculatePriority(node->newExecutionTime, node->firstTimeArrival, e_max, 1);
    
    node->child = NULL;
    node->parent = NULL;
    node->sibling = NULL;

    return node;
}

BinomialHeap *heapInit(void) {
    BinomialHeap *heap = (BinomialHeap *)malloc(sizeof(BinomialHeap));
    if (heap) heap->head = NULL;
    return heap;
}

int isPrior(BHNode* node1,BHNode* node2){
	if(node1->newExecutionTime == node2->newExecutionTime){
		if(node1->firstTimeArrival < node2->firstTimeArrival)
			return 1;
	}
	else{
		if(node1->priorityValue < node2->priorityValue){
			return 1;
		}
	}
	return 0;
}

BHNode *heapMerge(BinomialHeap *heap1, BinomialHeap *heap2)
{
    BHNode *head;
    BHNode *tail;
    BHNode *h1It;
    BHNode *h2It;

    if (heap1->head == NULL)
        return heap2->head;
    if (heap2->head == NULL)
        return heap1->head;

    h1It = heap1->head;
    h2It = heap2->head;

    if (h1It->degree <= h2It->degree) {
        head = h1It;
        h1It = h1It->sibling;
    } else {
        head = h2It;
        h2It = h2It->sibling;
    }

    tail = head;

    while (h1It != NULL && h2It != NULL) {
        if (h1It->degree <= h2It->degree) {
            tail->sibling = h1It;
            h1It = h1It->sibling;
        } else {
            tail->sibling = h2It;
            h2It = h2It->sibling;
        }
        tail = tail->sibling;
    }

    tail->sibling = (h1It != NULL) ? h1It : h2It;
    return head;
}

