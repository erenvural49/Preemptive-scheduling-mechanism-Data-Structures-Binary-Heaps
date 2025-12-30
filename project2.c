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
    
    int degree;                 // D?g?m?n derecesi (?ocuk sayisi)
    struct BHNode *parent;      // ?st d?g?m
    struct BHNode *child;       // En soldaki ?ocuk
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
    // 1. Adim: K?k listelerini derecelerine g?re sirali tek bir liste yap (Sende var olan heapMerge)
    BHNode *new_head = heapMerge(original, uni);
    
    // Orijinal heap'lerin i?ini bosaltiyoruz
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
            // Durum 2: Iki tane ayni dereceli aga? var, birlestirme (Link) zamani!
            if (isPrior(x, r)) {
                // x daha ?ncelikli (priorityValue daha k???k veya varis zamani daha eski)
                x->sibling = r->sibling;
                get_a_BT_k(r, x); // r'i x'un ?ocugu yap
                //2 processin hangisi daha priority öncelikli onu yukarý alma islemi
            } else {
                // r daha ?ncelikli
                if (q == NULL) {
                    new_head = r;
                } else {
                    q->sibling = r;
                }
                get_a_BT_k(x, r); // x'u r'in ?ocugu yap
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
int isPrior(BHNode* node1, BHNode* node2) {
    
    if (node1->priorityValue < node2->priorityValue) {
        return 1;
    } 
    else if (node1->priorityValue > node2->priorityValue) {
        return 0;
    }
   
    else {
        if (node1->firstTimeArrival < node2->firstTimeArrival) {
            return 1;
        }
        return 0;
    }
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
    BHNode *minNode = heap->head; // Su ana kadar bulunan en ?ncelikli
    BHNode *minPrev = NULL;       // minNode'un bir ?ncesi
    
    BHNode *curr = heap->head;    // Listeyi tarayan
    BHNode *prev = NULL;          // curr'un bir ?ncesi
    
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
int printHeapProcesses(BHNode *node, int mode) {
	
    if (node == NULL) return;
    int len=0;
    if (mode == 0) // Sadece ID'leri yazdir (P1, P2...)
        len += printf("P%d ", node->processId);
    else // ID ve Priority degerlerini yazdir (P1: 3.00...)
        printf("P%d: %.3f ", node->processId, node->priorityValue);
	
    len += printHeapProcesses(node->child, mode);
    len += printHeapProcesses(node->sibling, mode);
    
    return len;
}
// CPU çalisirken heap'te bekleyenlerin süresini artirmak için
void updateWaitingTimes(BHNode *node) {
    if (node == NULL) return;
    node->totalWaitingTime++;
    updateWaitingTimes(node->child);
    updateWaitingTimes(node->sibling);
}

double runSimulation(int q, int totalProcesses, int IDs[], int arrivalTime[], int initial_et[], int e_max, int printTable) {
    BinomialHeap *heap = heapInit();
    int currentTime = 0, completedCount = 0, totalWaitTime = 0;
    int isAdded[10] = {0};
    BHNode* cpuProcess = NULL;
    int quantumTimer = 0;

    if (printTable) {
        printf("\nSelected quantum value q: %d\n", q);
        printf("%-10s %-25s %-30s\n", "Time", "Processes in BH", "Priority value of processes in BH");
    }

    while (completedCount < totalProcesses || heap->head != NULL || cpuProcess != NULL) {
        // 1. Yeni Gelenler
        int i;
        for (i = 0; i < totalProcesses; i++) {
            if (arrivalTime[i] == currentTime && isAdded[i] == 0) {
                heapInsert(heap, IDs[i], initial_et[i], arrivalTime[i], e_max);
                isAdded[i] = 1;
            }
        }
        
        // cpudaki process is continuing but q=islemin cpuda kaldigi sure so BH'e tekrar send
        if (cpuProcess != NULL && quantumTimer == q) {
            cpuProcess->priorityValue = calculatePriority(cpuProcess->newExecutionTime, cpuProcess->firstTimeArrival, e_max, 0);
            BinomialHeap *temp = heapInit();
            cpuProcess->child = cpuProcess->sibling = cpuProcess->parent = NULL;
            cpuProcess->degree = 0;
            temp->head = cpuProcess;
            heap->head = heapUnion(heap, temp);
            free(temp);
            cpuProcess = NULL;
            quantumTimer = 0;
        }

        // 4. CPU'ya Süreç Al
        if (cpuProcess == NULL && heap->head != NULL) {
            cpuProcess = heapDeleteMin(heap);
            quantumTimer = 0;
        }
        
        // 4. EKRANA YAZDIRMA MANTIGI (Istedigin T=5 Düzenlemesi)
        if (printTable) {
                      
            if (cpuProcess == NULL) {
            	printf("%-10d ", currentTime);
                printf("%-25s %-30s\n", "-", "-");
            } //<= olunca p1 is process olur dedim ama q=1 no ?
            else if (quantumTimer > 0 && quantumTimer < q) {
                //I chosed to blank here because process in the cpu will continue no print in the output.
            } 
            else {
            	printf("%-10d ", currentTime);
                // KARAR ANI VEYA BASLANGIÇ: Tam tabloyu yazdir
                int len = 0;
                // Mevcut çalisan ve heap'tekiler
                len += printf("P%d ", cpuProcess->processId);
                len += printHeapProcesses(heap->head, 0);

                int space = 26 - len;
                if (space < 0) space = 0;
                printf("%*s", space, "");

                // Priority Degerleri
                printf("P%d: %.3f ", cpuProcess->processId, cpuProcess->priorityValue);
                printHeapProcesses(heap->head, 1);
                printf("\n");
            }  
        }
        // 5. Zaman Ilerlemesi
        if (cpuProcess != NULL) {
            updateWaitingTimes(heap->head);
            cpuProcess->newExecutionTime--;
            quantumTimer++;
            
            if (cpuProcess->newExecutionTime == 0) {
                totalWaitTime += cpuProcess->totalWaitingTime;
                completedCount++;
                free(cpuProcess);
                cpuProcess = NULL;
                quantumTimer = 0;
            }
        }
        currentTime++;
        if (currentTime > 100) break;
    }
    if (printTable) {
        printf("%-10d %-25s %-30s\n", currentTime, "EMPTY", "");
    }
    
    double awt = (double)totalWaitTime / totalProcesses;
    printf("Average Waiting Time (AWT): %.2f\n", awt);
    printf("\n");
    free(heap);
    return awt;
}

int main(void) {
    int IDs[] = {1, 2, 3,4,5,6};
    int arrivalTime[] = {0, 2, 3,5,6,7};
    int initial_et[] = {3, 1,2,2,2,4};
    int totalProcesses = 6;
    int e_max = 4;

    double minAWT = 10000, currentAWT;
    int bestQ = 1;
    int q;
    // q=1'den q=5'e kadar tablolu, sonrasinda sadece sonuç odakli
    for (q= 1; q <= 5; q++) {
        int showTable = (q <= 5); 
        currentAWT = runSimulation(q, totalProcesses, IDs, arrivalTime, initial_et, e_max, showTable);
        if (currentAWT < minAWT) {
            minAWT = currentAWT;
            bestQ = q;
        }
    }

    printf("\n**************************************************\n");
    printf("OPTIMIZATION COMPLETE: Best Quantum q = %d (AWT = %.2f)\n", bestQ, minAWT);
    printf("**************************************************\n");

    return 0;
}
