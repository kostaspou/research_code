/***************************************************************************************************************************
Header Files
****************************************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#include <assert.h>
#include <limits.h>
/***************************************************************************************************
 CUDD Package header files
***************************************************************************************************/
#include "util.h"
#include "cudd.h"
#include "cuddInt.h"
#include "dddmp.h"
/***************************************************************************************************************************
Constant Declarations 
****************************************************************************************************************************/
// VARIOUS CONSTANTS
#define Mfnam      20			// max size for a file name
#define Mnod    1000000 		        // max number of nodes in a graph/node
#define Mlin      300			// max size of characters in a line
#define Mnam       50			// max size of a node name
#define Mtyp       11			// max type of nodes/gates
#define Mout       16		        // max node out degree (Nfo)
#define Min         9			// max node in degree (Nfi)
#define Mpi       233			// max number of primary inputs
#define Mpo       140			// max number of primary outputs
#define Mpt       10			// max number of input patterns in .vec file
#define Mft       10			// max number of stuck at faults in .faults file
#define Htable	10000000		//size of the hash table
// NODE TYPE CONSTANTS 
#define INPT 1				// Primary Input
#define AND  2				// AND 
#define NAND 3				// NAND 
#define OR   4				// OR 
#define NOR  5				// NOR 
#define XOR  6				// XOR 
#define XNOR 7				// XNOR 
#define BUFF 8				// BUFFER 
#define NOT  9				// INVERTER 
#define FROM 10				// STEM BRANCH
#define DFF 11				// DFF
#define invert(X)	((X) == 1 ? 0 : 1)
#define IN_NODE(i)		fanin = graph[i].Fin; while(fanin!=NULL){ if(graph[fanin->id].Cval == 2){ in_node = fanin->id; break; }fanin = fanin->next;}
#define OUT_NODE(i)		fanout = graph[i].Fot; while(fanout!=NULL){ if(graph[fanout->id].Mark == 1){ out_node = fanout->id; break; }fanout = fanout->next;}
/***************************************************************************************************************************
Structure Declarations 
****************************************************************************************************************************/
//1.Stucture declaration for LIST
typedef struct LIST_type {
   int  id;		   //id for current element		
   struct LIST_type *next;  //pointer to next id element( if there is no element, then it will be NULL)		
} LIST;
//2.Stucture declaration for NODE
typedef struct NODE_type
{
  char Name[Mnam];                      //name of the node
  int Type,Nfi,Nfo,Po,level,dff,order;                  //type, nooffanins, nooffanouts,primaryo/p
  int Mark,Cval,Fval,low,high,weight,chain;                    //marker,correctvalue,faultvalue
  LIST *Fin,*Fot;                      //fanin members, fanout members
  DdNode *fault;
} NODE;
//3.Stucture declaration for PATTERN
typedef struct PATTERN_type
{
 char piv[Mpi];    //primary input vector(size is not declared)
} PATTERN;
//4.Stucture declaration for FAULT
typedef struct FAULT_type
{
 int Snod,Sval;      //stuck_at_node,stuck_at_value
} FAULT;

typedef struct LUT_type
{
  char name[Mnam];
  int value;
} LUT;

typedef struct LUT_type2
{
  int key;
  int value;
} KEY;

typedef struct info_type{
	int max,no_inputs;
} INFO;

typedef struct vector_info_type{
	float grade;
	int *vector;
}vector_info;

typedef struct preprocess_type{
	vector_info * vector_data;;
	float *fault_grades;
}preprocess_info;

typedef struct FAULT_LIST_TYPE{
	float value;
	int id,detected,apply_detected,shift_detected;
}fault_list_info;

typedef struct history_info_type{
	int *vector;
	int decision;
}history_info;
/***************************************************************************************************************************
Cudd Package Declarations 
****************************************************************************************************************************/
DdManager *manager;          //Manager for Cudd Package
static int view_number=0;    //Global Variable used by Manager
DdNode *onez;                //Global Variable used by Manager

/***************************************************************************************************************************
Functions in given.c
****************************************************************************************************************************/
/***************************************************************************************************************************
LIST Structure Functions
****************************************************************************************************************************/
int pop(LIST **);
void push (LIST ** , int );
void CopyList(LIST **,LIST **);
void InsertList(LIST **,int);
void PrintList(LIST *);
void FreeList(LIST **);
void Delete(LIST **,int );
int CountList(LIST *);
int check_if_marked(NODE *,int);
/***************************************************************************************************************************
 NODE Structure Functions
****************************************************************************************************************************/
int ReadIsc(FILE *,NODE *);
int Read_bench(FILE *,NODE *);
int node_table(FILE *,NODE *);
INFO Levelize(NODE *,NODE *,int );
void copy_node(NODE *,NODE *,int ,int );
void construct_stem(NODE *,int );
static unsigned long hash_function(unsigned char *);
int insert(LUT *,char *,int *);
int insert_key(KEY *

,int );
void InitializeCircuit(NODE *,int);
int AssignType(char *);
void PrintCircuit(NODE *,int);
void PrintINFO(NODE *,int );
void ClearCircuit(NODE *,int);
/***************************************************************************************************************************
 PATTERN Structure Functions
****************************************************************************************************************************/
void fault_evaluation(NODE *,int ,int *,int ,int );
int ReadVec(FILE *,PATTERN *);
/***************************************************************************************************************************
User Defined Functions in user.c
****************************************************************************************************************************/
int * Simulate(NODE *,int,int ,int ,int );
void excite_fault(NODE *,int ,int ,int ,int *,int *,LIST **);
void backtrace(NODE *,int ,int ,int *,int *,int *);
int objective(NODE *,int ,int *,int *,int ,int , LIST **);
void depth(NODE *,int );
void controllability(NODE *,int );
void x_path(NODE *,int ,int *);
void d_frontier(NODE *,int ,LIST **);
int Gates (int, int , int );
int NOT_OUT (int);
int FROM_OUT (int);
void init_node(NODE *,int);
int Evaluation(NODE *,int,int,int,int,int);

/***************************************************************************************************************************
Functions in new.c
****************************************************************************************************************************/
int ** create_fault_list(NODE *,int ,int *,int );
static void my_zdd_print_minterm_aux (DdManager *,DdNode *,int ,int *,int *);
int * my_Cudd_zddPrintMinterm(DdManager *,DdNode *);
preprocess_info fault_grading(NODE *,int,int );
int Two_Gates (int, int , int );
void algorithm(NODE *,int ,int );
int shift(NODE *,int ,int ,int *,fault_list_info *);
int apply_or_shift(NODE *,int ,int ,int *,fault_list_info *);
/***************************************************************************************************************************
Functions in extra_functions.c
****************************************************************************************************************************/
void update_graph(NODE *,int );
