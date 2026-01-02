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
    
    int degree;                 
    struct BHNode *parent;      
    struct BHNode *child;       
    struct BHNode *sibling;     
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
    } 
	else {
        head = h2It;
        h2It = h2It->sibling;
    }

    tail = head;

    while (h1It != NULL && h2It != NULL) {
        if (h1It->degree <= h2It->degree) {
            tail->sibling = h1It;
            h1It = h1It->sibling;
        } 
		else {
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
            
            if (isPrior(x, r)) {
                x->sibling = r->sibling;
                get_a_BT_k(r, x); 
                
            } 
			else{
                
                if (q == NULL) {
                    new_head = r;
                } 
				else {
                    q->sibling = r;
					}
                get_a_BT_k(x, r); 
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
    BHNode *minNode = heap->head;
    BHNode *minPrev = NULL;       
    
    BHNode *curr = heap->head;
    BHNode *prev = NULL;
    
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
    if (mode == 0){ //print middle part
        len += printf("P%d ", node->processId);
    }
    else{ //print right part
        printf("P%d: %.3f ", node->processId, node->priorityValue);
    }
	
    len += printHeapProcesses(node->child, mode);
    len += printHeapProcesses(node->sibling, mode);
    
    return len;
}
void updateWaitingTimes(BHNode *node) {
    if (node == NULL) return;
    node->totalWaitingTime++;
    updateWaitingTimes(node->child);
    updateWaitingTimes(node->sibling);
}

double DifferentQAnalysis(int q, int totalProcesses, int IDs[], int arrivalTime[], int initial_et[], int e_max, int printTable) {
    BinomialHeap *heap = heapInit();
    int currentTime = 0, completedCount = 0, totalWaitTime = 0;
    int isAdded[10] = {0};
    BHNode* cpuProcess = NULL;
    int timeInCpu = 0;
    int i;

    if (printTable) {
        printf("\nSelected quantum value q: %d\n", q);
        printf("%-10s %-25s %-30s\n", "Time", "Processes in BH", "Priority value of processes in BH");
    }

    while (completedCount < totalProcesses || heap->head != NULL || cpuProcess != NULL) {
        for (i = 0; i < totalProcesses; i++) {
            if (arrivalTime[i] == currentTime && isAdded[i] == 0) {
                heapInsert(heap, IDs[i], initial_et[i], arrivalTime[i], e_max);
                isAdded[i] = 1;
            }
        }
        
        //  Process in the cpu continuing but q is expired process will send to the BH again.
        if (cpuProcess != NULL && timeInCpu == q) {
            cpuProcess->priorityValue = calculatePriority(cpuProcess->newExecutionTime, cpuProcess->firstTimeArrival, e_max, 0);
            BinomialHeap *temp = heapInit();
            cpuProcess->child = cpuProcess->sibling = cpuProcess->parent = NULL;
            cpuProcess->degree = 0;
            temp->head = cpuProcess;
            heap->head = heapUnion(heap, temp);
            free(temp);
            cpuProcess = NULL;
            timeInCpu = 0;
        }

        if (cpuProcess == NULL && heap->head != NULL) {
            cpuProcess = heapDeleteMin(heap);
            timeInCpu = 0;
        }
        
        if (printTable) {
                      
            if (cpuProcess == NULL) {
            	printf("%-10d ", currentTime);
                printf("%-25s %-30s\n", "-", "-");
            }
            else if (timeInCpu > 0 && timeInCpu < q) {
                //I chosed to blank here because process in the cpu will continue , no print in the output.
            } 
            else {
            	printf("%-10d ", currentTime);
                
                int len = 0;
                
                len += printf("P%d ", cpuProcess->processId);
                len += printHeapProcesses(heap->head, 0);

                int space = 26 - len;
                if (space < 0) space = 0;
                printf("%*s", space, "");

                printf("P%d: %.3f ", cpuProcess->processId, cpuProcess->priorityValue);
                printHeapProcesses(heap->head, 1);
                printf("\n");
            }  
        }
        
        if (cpuProcess != NULL) {
            updateWaitingTimes(heap->head);
            cpuProcess->newExecutionTime--;
            timeInCpu++;
            
            if (cpuProcess->newExecutionTime == 0) {
                totalWaitTime += cpuProcess->totalWaitingTime;
                completedCount++;
                free(cpuProcess);
                cpuProcess = NULL;
                timeInCpu = 0;
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
    
    int IDs[20];
    int arrivalTime[20];
    int initial_et[20];
    int totalProcesses = 0;
    int e_max = 0;

    FILE *fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("input.txt can't open'\n");
        return 1;
    }
    char idStr[5];
    int et, arr;
    
    while (fscanf(fp, "%s %d %d", idStr, &et, &arr) == 3) {
        
        int idVal = 0;
        if (idStr[0] == 'P' || idStr[0] == 'p') {
            idVal = atoi(&idStr[1]);
        } else {
            idVal = atoi(idStr);
        }

        IDs[totalProcesses] = idVal;
        initial_et[totalProcesses] = et;
        arrivalTime[totalProcesses] = arr;

        
        if (et > e_max) {
            e_max = et;
        }

        totalProcesses++;
               
        if (totalProcesses >= 20) break;
    }
    fclose(fp);

    double minAWT = 100;
	double currentAWT;
    int bestQ = 1;
    int q;
    
    for (q= 1; q <= 5; q++) {
        int showTable = (q <= 5); 
        currentAWT = DifferentQAnalysis(q, totalProcesses, IDs, arrivalTime, initial_et, e_max, showTable);
        if (currentAWT < minAWT) {
            minAWT = currentAWT;
            bestQ = q;
        }
    }
    printf("Min Average Waiting Time is %.2f and q = %d \n",minAWT,bestQ);
    return 0;
}
