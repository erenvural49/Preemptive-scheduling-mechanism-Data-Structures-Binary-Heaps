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
//parent will be root
void get_a_BT_k(BHNode *child, BHNode *parent) {
    child->parent = parent;
    child->sibling = parent->child;
    parent->child = child;
    parent->degree++;
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
            q = x;
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

void heapInsert(BinomialHeap *heap1, int PID, int initial_et, int arrival, int e_max) {
    BHNode* node1 = nodeInit(PID, initial_et, arrival, e_max);
    if (node1 == NULL) 
	return;

    BinomialHeap* tempHeap = heapInit();
    if (tempHeap == NULL) 
	return;

    tempHeap->head = node1;
    heap1->head = heapUnion(heap1, tempHeap);
    free(tempHeap);
}

void heapRemove(BinomialHeap *heap, BHNode *node, BHNode *before)
{
    BinomialHeap *temp;
    BHNode *child;
    BHNode *new_head;
    BHNode *next;

    if (node == heap->head)
        heap->head = node->sibling;
    else if (before != NULL)
        before->sibling = node->sibling;

    new_head = NULL;
    child = node->child;

    while (child != NULL) {
        next = child->sibling;
        child->sibling = new_head;
        child->parent = NULL;
        new_head = child;
        child = next;
    }

    temp = heapInit();
    if (temp == NULL)
        return;

    temp->head = new_head;
    heap->head = heapUnion(heap, temp);
    free(temp);
}
BHNode* heapDeleteMin(BinomialHeap* heap){
	
	if (heap == NULL || heap->head == NULL) {
        return NULL;
    }
    BHNode *minNode = heap->head; // Su ana kadar bulunan en öncelikli
    BHNode *minPrev = NULL;       // minNode'un bir öncesi
    
    BHNode *curr = heap->head;    // Listeyi tarayan
    BHNode *prev = NULL;          // curr'un bir öncesi
    
    while(curr!=NULL){
    	if(isPrior(curr,minNode)){
    		minNode=curr;
    		minPrev=prev;
		}
    	prev=curr;
    	curr=curr->sibling;
	}
	heapRemove(heap, minNode, minPrev);
    return minNode;
}
int main(void){
	
	// 1. Heap ve Degisken Baslatma
    BinomialHeap *heap = heapInit();
    int currentTime = 0;
    int completedCount = 0;
    int totalProcesses = 3; // Test için 3 tane diyelim
    int e_max = 3;        // Örnek max süre
    int q=1;
    // Test verileri (Dosyadan okuma yerine simdilik elinle gir)
    int IDs[] = {1, 2, 3};
    int arrivalTime[] = {0, 2, 3};
    int initial_et[] = {3, 1, 2};
    int isAdded[] = {0, 0, 0};

    printf("Simulasyon Basliyor (q=1)...\n\n");
	
	
	int i;
	while (completedCount < totalProcesses || heap->head != NULL) {
        
        // A. Yeni Gelen Isleri Kontrol Et
        for (i = 0; i < totalProcesses; i++) {
            if (arrivalTime[i] == currentTime && isAdded[i] == 0) {
                heapInsert(heap,IDs[0],initial_et[0],arrivalTime[0],e_max);
                isAdded[i] = 1;
                printf("[T=%d] Process %d sisteme eklendi.\n", currentTime, IDs[i]);
            }
        }
	}
return 0;
}

