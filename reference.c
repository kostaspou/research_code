#include "graph.h"


void fault_grading(NODE *graph,int Max,DdManager *manager)
{
LIST *fanin;
int previous,i,k,j;
 DdNode *temp1,*temp2;
 DdNode *list1,*list2;
struct timespec seconds;
FILE *fp1;

int *list;
float *fault_grades;
//char *buffer;
list1 = NULL;
list2 = NULL;


	fault_grades = ALLOC(float, 3*Mnod);
for(i=0;i<manager->sizeZ;i++){
	fault_grades[i]=0;
}

for(k=0;k<=1;k++){
		for(i=0;i<=Max;i++){
// Switch statement to manipulate every node according to their type
if(graph[i].Type != 0){
printf("\ni am at NODE =  %d",i);
			switch (graph[i].Type)  {
			    case INPT  :
				//generate random input vector
				//gettimeofday(&seconds,NULL);
				//srand(seconds.tv_nsec);
				//graph[i].Cval = rand()%2;
//if(i==5) graph[i].Cval = 0;
//else graph[i].Cval = 1;
graph[i].Cval = 0;
				if(graph[i].Cval == 0){printf("\ni am at rising input %d",i);
					graph[i].fault = Cudd_zddChange(manager,onez,2*i);
					Cudd_Ref(graph[i].fault);
				} else if (graph[i].Cval == 1){printf("\ni am at falling input %d",i);
					graph[i].fault = Cudd_zddChange(manager,onez,2*i+1);
					Cudd_Ref(graph[i].fault);
				}
				break;

			// For each gate i pass the fan-in list and take from its members the value
			//Then i use the respective function to derive the new value of the gate
			    case AND   :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(0,graph[fanin->id].Cval,previous);
						fanin = fanin->next;
					}
					graph[i].Cval = previous;
				}

				//ZDD construction
					if(graph[i].Cval == 0){
						fanin = graph[i].Fin;
						while(fanin!=NULL){
							if(graph[fanin->id].Cval == 1){
								if(list1 == NULL){
									list1 = graph[fanin->id].fault;
									Cudd_Ref(list1);
								}else{
									temp1 = Cudd_zddProduct(manager,list1,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									temp1 = Cudd_zddDivide(manager,list1,temp2);
									list1 = temp1;
									Cudd_RecursiveDerefZdd(manager,temp1);
								}
							}else if(graph[fanin->id].Cval == 0){
								if(list2 == NULL){
									list2 = graph[fanin->id].fault;
									Cudd_Ref(list2);
								}else{
									temp1 = Cudd_zddProduct(manager,list2,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									temp1 = Cudd_zddDivide(manager,list2,temp2);
									list2 = temp1;
									Cudd_RecursiveDerefZdd(manager,temp1);
								}
							}
						fanin = fanin->next;
						}
						if(list1 == NULL){
							temp2 = list2;
							Cudd_RecursiveDerefZdd(manager,list2);
						}else if(list2 == NULL){
							temp2 = list1;
							Cudd_RecursiveDerefZdd(manager,list1);
						} else { // find the difference
							temp1 = Cudd_zddProduct(manager,list2,list1);
							Cudd_Ref(temp1);
							temp2 = Cudd_zddDivide(manager,temp1,list1);
								Cudd_RecursiveDerefZdd(manager,temp1);
								Cudd_RecursiveDerefZdd(manager,list1);
								Cudd_RecursiveDerefZdd(manager,list2);
						}
						Cudd_Ref(temp2);
						graph[i].fault = Cudd_zddChange(manager,temp2,2*i);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp2);
						list1 = NULL;
						list2 = NULL;
					} else if(graph[i].Cval == 1){
						fanin = graph[i].Fin;
						temp1 = graph[fanin->id].fault;
						Cudd_Ref(temp1);
						fanin = fanin->next;
						while(fanin!=NULL){
								temp2 = Cudd_zddProduct(manager,graph[fanin->id].fault,temp1);
								Cudd_Ref(temp2);
								Cudd_RecursiveDerefZdd(manager,temp1);
							temp1 = temp2;
							Cudd_Ref(temp1);
							Cudd_RecursiveDerefZdd(manager,temp2);
							fanin = fanin->next;
						}
						graph[i].fault = Cudd_zddChange(manager,temp1,2*i+1);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp1);
					}
				break;

			    case NAND  :
					//simulation
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(0,graph[fanin->id].Cval,previous);
						fanin = fanin->next;
					}
					graph[i].Cval = NOT_OUT(previous);

					//ZDD construction
					if(graph[i].Cval == 1){
						fanin = graph[i].Fin;
						while(fanin!=NULL){
							if(graph[fanin->id].Cval == 1){
								if(list1 == NULL){
									list1 = graph[fanin->id].fault;
									//Cudd_Ref(list1);
								}else{
									temp1 = Cudd_zddProduct(manager,list1,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									Cudd_RecursiveDerefZdd(manager,list1);
									Cudd_RecursiveDerefZdd(manager,graph[fanin->id].fault);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									Cudd_RecursiveDerefZdd(manager,graph[fanin->id].fault);
									Cudd_RecursiveDerefZdd(manager,temp1);
									temp1 = Cudd_zddDivide(manager,list1,temp2);
									Cudd_Ref(temp1);
									Cudd_RecursiveDerefZdd(manager,list1);
									Cudd_RecursiveDerefZdd(manager,temp2);
									list1 = temp1;
								}
							}else if(graph[fanin->id].Cval == 0){
								if(list2 == NULL){
									list2 = graph[fanin->id].fault;
									//Cudd_Ref(list2);
								}else{
									temp1 = Cudd_zddProduct(manager,list2,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									Cudd_RecursiveDerefZdd(manager,list2);
									Cudd_RecursiveDerefZdd(manager,graph[fanin->id].fault);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									Cudd_RecursiveDerefZdd(manager,temp1);
									Cudd_RecursiveDerefZdd(manager,graph[fanin->id].fault);
									temp1 = Cudd_zddDivide(manager,list2,temp2);
									Cudd_Ref(temp1);
									Cudd_RecursiveDerefZdd(manager,list2);
									Cudd_RecursiveDerefZdd(manager,temp2);
									list2 = temp1;

								}
							}
						fanin = fanin->next;
						}
						if(list1 == NULL){
							temp2 = list2;
							//Cudd_Ref(temp2);
							//Cudd_RecursiveDerefZdd(manager,list2);
						} else { // find the difference
							temp1 = Cudd_zddProduct(manager,list2,list1);
							Cudd_Ref(temp1);
							Cudd_RecursiveDerefZdd(manager,list2);
							Cudd_RecursiveDerefZdd(manager,list1);
							temp2 = Cudd_zddDivide(manager,temp1,list1);
							Cudd_Ref(temp2);
							Cudd_RecursiveDerefZdd(manager,temp1);
							Cudd_RecursiveDerefZdd(manager,list1);
						}
						graph[i].fault = Cudd_zddChange(manager,temp2,2*i+1);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp2);
						list1 = NULL;
						list2 = NULL;
					} else if(graph[i].Cval == 0){
						fanin = graph[i].Fin;
						temp1 = graph[fanin->id].fault;
						//Cudd_Ref(temp1);
						fanin = fanin->next;
						while(fanin!=NULL){
								temp2 = Cudd_zddProduct(manager,graph[fanin->id].fault,temp1);
								Cudd_Ref(temp2);
								Cudd_RecursiveDerefZdd(manager,temp1);
								Cudd_RecursiveDerefZdd(manager,graph[fanin->id].fault);
							temp1 = temp2;
							//Cudd_Ref(temp1);
							//Cudd_RecursiveDerefZdd(manager,temp2);
							fanin = fanin->next;
						}
						graph[i].fault = Cudd_zddChange(manager,temp1,2*i);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp1);
					}			
				break;
			    case OR    :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(1,graph[fanin->id].Cval,previous);
						fanin = fanin->next;
					}
					graph[i].Cval = previous;
				}

				//ZDD construction
					if(graph[i].Cval == 1){
						fanin = graph[i].Fin;
						while(fanin!=NULL){
							if(graph[fanin->id].Cval == 1){
								if(list1 == NULL){
									list1 = graph[fanin->id].fault;
									Cudd_Ref(list1);
								}else{
									temp1 = Cudd_zddProduct(manager,list1,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									temp1 = Cudd_zddDivide(manager,list1,temp2);
									list1 = temp1;
									Cudd_RecursiveDerefZdd(manager,temp1);
								}
							}else if(graph[fanin->id].Cval == 0){
								if(list2 == NULL){
									list2 = graph[fanin->id].fault;
									Cudd_Ref(list2);
								}else{
									temp1 = Cudd_zddProduct(manager,list2,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									temp1 = Cudd_zddDivide(manager,list2,temp2);
									list2 = temp1;
									Cudd_RecursiveDerefZdd(manager,temp1);
								}
							}
						fanin = fanin->next;
						}
						if(list1 == NULL){
							temp2 = list2;
							Cudd_RecursiveDerefZdd(manager,list2);
						}else if(list2 == NULL){
							temp2 = list1;
							Cudd_RecursiveDerefZdd(manager,list1);
						} else { // find the difference
							temp1 = Cudd_zddProduct(manager,list1,list2);
							Cudd_Ref(temp1);
							temp2 = Cudd_zddDivide(manager,temp1,list2);
								Cudd_RecursiveDerefZdd(manager,temp1);
								Cudd_RecursiveDerefZdd(manager,list1);
								Cudd_RecursiveDerefZdd(manager,list2);
						}
						Cudd_Ref(temp2);
						graph[i].fault = Cudd_zddChange(manager,temp2,2*i+1);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp2);
						list1 = NULL;//regular 
						list2 = NULL;//bar
					} else if(graph[i].Cval == 0){
						fanin = graph[i].Fin;
						temp1 = graph[fanin->id].fault;
						Cudd_Ref(temp1);
						fanin = fanin->next;
						while(fanin!=NULL){
								temp2 = Cudd_zddProduct(manager,graph[fanin->id].fault,temp1);
								Cudd_Ref(temp2);
								Cudd_RecursiveDerefZdd(manager,temp1);
							temp1 = temp2;
							Cudd_Ref(temp1);
							Cudd_RecursiveDerefZdd(manager,temp2);
							fanin = fanin->next;
						}
						graph[i].fault = Cudd_zddChange(manager,temp1,2*i);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp1);
					}
				break;

			    case NOR   :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(1,graph[fanin->id].Cval,previous);
						fanin = fanin->next;
					}
					graph[i].Cval = NOT_OUT(previous);
				}

				//ZDD construction
					if(graph[i].Cval == 0){
						fanin = graph[i].Fin;
						while(fanin!=NULL){
							if(graph[fanin->id].Cval == 1){
								if(list1 == NULL){
									list1 = graph[fanin->id].fault;
									Cudd_Ref(list1);
								}else{
									temp1 = Cudd_zddProduct(manager,list1,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									temp1 = Cudd_zddDivide(manager,list1,temp2);
									list1 = temp1;
									Cudd_RecursiveDerefZdd(manager,temp1);
								}
							}else if(graph[fanin->id].Cval == 0){
								if(list2 == NULL){
									list2 = graph[fanin->id].fault;
									Cudd_Ref(list2);
								}else{
									temp1 = Cudd_zddProduct(manager,list2,graph[fanin->id].fault);
									Cudd_Ref(temp1);
									temp2 = Cudd_zddDivide(manager,temp1,graph[fanin->id].fault);
									Cudd_Ref(temp2);
									temp1 = Cudd_zddDivide(manager,list2,temp2);
									list2 = temp1;
									Cudd_RecursiveDerefZdd(manager,temp1);
								}
							}
						fanin = fanin->next;
						}
						if(list1 == NULL){
							temp2 = list2;
							Cudd_RecursiveDerefZdd(manager,list2);
						}else if(list2 == NULL){
							temp2 = list1;
							Cudd_RecursiveDerefZdd(manager,list1);
						} else { // find the difference
							temp1 = Cudd_zddProduct(manager,list1,list2);
							Cudd_Ref(temp1);
							temp2 = Cudd_zddDivide(manager,temp1,list2);
								Cudd_RecursiveDerefZdd(manager,temp1);
								Cudd_RecursiveDerefZdd(manager,list1);
								Cudd_RecursiveDerefZdd(manager,list2);
						}
						Cudd_Ref(temp2);
						graph[i].fault = Cudd_zddChange(manager,temp2,2*i);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp2);
						list1 = NULL;//regular 
						list2 = NULL;//bar
					} else if(graph[i].Cval == 1){
						fanin = graph[i].Fin;
						temp1 = graph[fanin->id].fault;
						Cudd_Ref(temp1);
						fanin = fanin->next;
						while(fanin!=NULL){
								temp2 = Cudd_zddProduct(manager,graph[fanin->id].fault,temp1);
								Cudd_Ref(temp2);
								Cudd_RecursiveDerefZdd(manager,temp1);
							temp1 = temp2;
							Cudd_Ref(temp1);
							Cudd_RecursiveDerefZdd(manager,temp2);
							fanin = fanin->next;
						}
						graph[i].fault = Cudd_zddChange(manager,temp1,2*i+1);
						Cudd_Ref(graph[i].fault);
						Cudd_RecursiveDerefZdd(manager,temp1);
					}
				break;

			    case XOR   :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(0,Gates(1,NOT_OUT(graph[fanin->id].Cval),previous),Gates(1,graph[fanin->id].Cval,NOT_OUT(previous)));
						fanin = fanin->next;
					}
					graph[i].Cval = previous;
				}
				break;

			    case XNOR  :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(0,Gates(1,NOT_OUT(graph[fanin->id].Cval),previous),Gates(1,graph[fanin->id].Cval,NOT_OUT(previous)));
						fanin = fanin->next;
					}
					graph[i].Cval = NOT_OUT(previous);
				}
				break;

			    case BUFF  :
			    case FROM  :
					fanin = graph[i].Fin;
					graph[i].Cval = graph[fanin->id].Cval;
							//ZDD Construction
							if(graph[i].Cval == 0){
								graph[i].fault = Cudd_zddChange(manager,graph[fanin->id].fault,2*i);
								Cudd_Ref(graph[i].fault);
								Cudd_RecursiveDerefZdd(manager,graph[fanin->id].fault);
							} else if(graph[i].Cval == 1){
								graph[i].fault = Cudd_zddChange(manager,graph[fanin->id].fault,2*i+1);
								Cudd_Ref(graph[i].fault);
								Cudd_RecursiveDerefZdd(manager,graph[fanin->id].fault);
							}

				break;

			    case NOT   :
					fanin = graph[i].Fin;
					graph[i].Cval = NOT_OUT(graph[fanin->id].Cval);
							//ZDD Construction
							if(graph[i].Cval == 0){
								graph[i].fault = Cudd_zddChange(manager,graph[fanin->id].fault,2*i);
								Cudd_Ref(graph[i].fault);
							} else if(graph[i].Cval == 1){
								graph[i].fault = Cudd_zddChange(manager,graph[fanin->id].fault,2*i+1);
								Cudd_Ref(graph[i].fault);
							}
				break;

			    } //end case

		}
/*
if(graph[i].Type != 0){
printf("\n At node %d i have the minterms\n",i);
Cudd_zddPrintMinterm(manager,graph[i].fault);

}
*/
		}

/*
	 fp1 = fopen("bdddump1","w");
	  Cudd_zddDumpDot(manager,1,&graph[17].fault,NULL,NULL,fp1);
	  fclose(fp1);
*/

for(j=0;j<=Max;j++)
	if(graph[j].Po>0){
	list = my_Cudd_zddPrintMinterm(manager,graph[j].fault);
		for(i=0;i<manager->sizeZ;i++){
			if(list[i]==1){
				fault_grades[i] += 1.0/graph[j].Po;
			}

		}
}




/*
for(i=0;i<=Max;i++){
	if(graph[i].fault != NULL)
		Cudd_RecursiveDerefZdd(manager,graph[i].fault);
}
*/


}

for(i=0;i<manager->sizeZ;i++){
	printf("\nFor fault %d i have the grade %f",i,fault_grades[i]);
}
//printf("\nNo of Unreferenced Zdds inside: %d\n",my_Cudd_Check_Ref(manager));
}


int * my_Cudd_zddPrintMinterm(DdManager * zdd,DdNode * node)
 {
     int         i, size;
     int         *list,*value;
 
     size = (int)zdd->sizeZ;
     list = ALLOC(int, size);
     value = ALLOC(int, size);
     if (list == NULL) {
         zdd->errorCode = CUDD_MEMORY_OUT;
         return(0);
     }
     for (i = 0; i < size; i++) list[i] = 3; /* bogus value should disappear */
     my_zdd_print_minterm_aux(zdd, node, 0, list,value);

	free(list);
     return value;
 
} /* end of Cudd_zddPrintMinterm */


static void my_zdd_print_minterm_aux (DdManager * zdd,DdNode * 	node,int level,int * list,int *value)
{
     DdNode      *Nv, *Nnv;
     int         i, v;
     DdNode      *base = DD_ONE(zdd);


     if (Cudd_IsConstant(node)) {
         if (node == base) {
             /* Check for missing variable. */
             if (level != zdd->sizeZ) {
                 list[zdd->invpermZ[level]] = 0;
                 my_zdd_print_minterm_aux(zdd, node, level + 1, list,value);
                 return;
             }
		for (i = 0; i < zdd->sizeZ; i++) {
				 value[i] = list[i];
		}
         }
     } else {
         /* Check for missing variable. */
         if (level != cuddIZ(zdd,node->index)) {
             list[zdd->invpermZ[level]] = 0;
             my_zdd_print_minterm_aux(zdd, node, level + 1, list,value);
             return;
         }
 
         Nnv = cuddE(node);
         Nv = cuddT(node);
         if (Nv == Nnv) {
             list[node->index] = 2;
             my_zdd_print_minterm_aux(zdd, Nnv, level + 1, list,value);
             return;
         }
 
         list[node->index] = 1;
         my_zdd_print_minterm_aux(zdd, Nv, level + 1, list,value);
         list[node->index] = 0;
         my_zdd_print_minterm_aux(zdd, Nnv, level + 1, list,value);
     }
     return;
 
} /* end of zdd_print_minterm_aux */

int Gates (int select, int a, int b)
{
int output;
int LUT [2][3][3]= {		//AND
			{	{0,0,0},
				{0,1,2},
				{0,2,2}
			},	//OR
			{	{0,1,2},
				{1,1,1},
				{2,1,2}
			},	
		};
output = LUT[select][a][b];
return (output);
}
//Not gate
int NOT_OUT (int a)
{
	int LUT[3] = {1,0,2}; 


	return (LUT[a]);
}


int my_Cudd_Check_Ref(DdManager * manager) 	
 {
     int size;
     int i, j;
     int remain; /* the expected number of remaining references to one */
     DdNodePtr *nodelist;
     DdNode *node;
     DdNode *sentinel = &(manager->sentinel);
     DdSubtable *subtable;
     int count = 0;
     int index;
 
 #ifndef DD_NO_DEATH_ROW
     cuddClearDeathRow(manager);
 #endif
 
     /* First look at the BDD/ADD subtables. */
     remain = 1; /* reference from the manager */
     size = manager->size;
     remain += 2 * size; /* reference from the BDD projection functions */
 
     for (i = 0; i < size; i++) {
         subtable = &(manager->subtables[i]);
         nodelist = subtable->nodelist;
         for (j = 0; (unsigned) j < subtable->slots; j++) {
             node = nodelist[j];
             while (node != sentinel) {
                 if (node->ref != 0 && node->ref != DD_MAXREF) {
                     index = (int) node->index;
                     if (node != manager->vars[index]) {
                         count++; //Cudd_RecursiveDeref(manager,node);
                     } else {
                         if (node->ref != 1) {
                             count++; //Cudd_RecursiveDeref(manager,node);
                         }
                     }
                 }
                 node = node->next;
             }
         }
     }
 
     /* Then look at the ZDD subtables. */
     size = manager->sizeZ;
     if (size) /* references from ZDD universe */
         remain += 2;
 
     for (i = 0; i < size; i++) {
         subtable = &(manager->subtableZ[i]);
         nodelist = subtable->nodelist;
         for (j = 0; (unsigned) j < subtable->slots; j++) {
             node = nodelist[j];
             while (node != NULL) {
                 if (node->ref != 0 && node->ref != DD_MAXREF) {
                     index = (int) node->index;
                     if (node == manager->univ[manager->permZ[index]]) {
                         if (node->ref > 2) {
                             count++; //Cudd_RecursiveDerefZdd(manager,node);
                         }
                     } else {
                         count++; //Cudd_RecursiveDerefZdd(manager,node);
                     }
                 }
                 node = node->next;
             }
         }
     }
 
     /* Now examine the constant table. Plusinfinity, minusinfinity, and
     ** zero are referenced by the manager. One is referenced by the
     ** manager, by the ZDD universe, and by all projection functions.
     ** All other nodes should have no references.
     */
     nodelist = manager->constants.nodelist;
     for (j = 0; (unsigned) j < manager->constants.slots; j++) {
         node = nodelist[j];
         while (node != NULL) {
             if (node->ref != 0 && node->ref != DD_MAXREF) {
                 if (node == manager->one) {
                     if ((int) node->ref != remain) {
                         count++; //Cudd_RecursiveDeref(manager,node);
                     }
                 } else if (node == manager->zero ||
                 node == manager->plusinfinity ||
                 node == manager->minusinfinity) {
                     if (node->ref != 1) {
                         count++; //Cudd_RecursiveDeref(manager,node);
                     }
                 } else {
                     count++; //Cudd_RecursiveDeref(manager,node);
                 }
             }
             node = node->next;
         }
     }
     return(count);
 
 } /* end of Cudd_CheckZeroRef */
