#include "graph.h"


void fault_grading(NODE *graph,int Max,DdManager *manager)
{
LIST *fanin;
int previous,i;
 DdNode *temp1,*temp2;
struct timespec seconds;

		for(i=0;i<=Max;i++){
// Switch statement to manipulate every node according to their type
			switch (graph[i].Type)  {
			    case INPT  :
				//generate random input vector
				gettimeofday(&seconds,NULL);
				srand(seconds.tv_nsec);
				graph[i].Cval = rand()%2;

				if(graph[i].Cval == 0){//printf("\ni am at rising input %d",i);
					graph[i].fault = Cudd_zddChange(manager,onez,2*i);
					Cudd_Ref(graph[i].fault);
				} else if (graph[i].Cval == 1){//printf("\ni am at falling input %d",i);
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
				break;

			    case NAND  :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(0,graph[fanin->id].Cval,previous);
						fanin = fanin->next;
					}
					graph[i].Cval = NOT_OUT(previous);
				}

							if(graph[i].Cval == 0){
								graph[i].fault = Cudd_zddChange(manager,onez,2*i);
								Cudd_Ref(graph[i].fault);
								//Cudd_RecursiveDerefZdd(manager,graph[fanin->Id].Rpath);
							} else if(graph[i].Cval == 1){
								graph[i].fault = Cudd_zddChange(manager,onez,2*i+1);
								Cudd_Ref(graph[i].fault);
								//Cudd_RecursiveDerefZdd(manager,graph[fanin->Id].Fpath);
							}
					fanin = graph[i].Fin;
					if(graph[fanin->id].Cval == 0){
						temp1 = graph[fanin->id].fault;
						Cudd_Ref(temp1);
					} else if(graph[fanin->id].Cval == 1){
						temp1 = Cudd_zddComplement(manager,graph[fanin->id].fault);
						Cudd_Ref(temp1);
					}
					fanin = fanin->next;
					while(fanin!=NULL){
						if(graph[fanin->id].Cval == 0){
							temp2 = Cudd_zddIntersect(manager,graph[fanin->id].fault,temp1);
							Cudd_Ref(temp2);
							Cudd_RecursiveDerefZdd(manager,temp1);
						} else if(graph[fanin->id].Cval == 1){
							temp2 = Cudd_zddIntersect(manager,Cudd_zddComplement(manager,graph[fanin->id].fault),temp1);
							Cudd_Ref(temp2);
							Cudd_RecursiveDerefZdd(manager,temp1);
						}
						temp1 = temp2;
						Cudd_Ref(temp1);
						Cudd_RecursiveDerefZdd(manager,temp2);
						fanin = fanin->next;
					}
					temp2 = Cudd_zddUnion(manager,graph[i].fault,temp1);
					Cudd_Ref(temp2);
					Cudd_RecursiveDerefZdd(manager,temp1);
					graph[i].fault = temp2;					
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

							if(graph[i].Cval == 0){
								graph[i].fault = Cudd_zddChange(manager,onez,2*i);
								Cudd_Ref(graph[i].fault);
								//Cudd_RecursiveDerefZdd(manager,graph[fanin->Id].Rpath);
							} else if(graph[i].Cval == 1){
								graph[i].fault = Cudd_zddChange(manager,onez,2*i+1);
								Cudd_Ref(graph[i].fault);
								//Cudd_RecursiveDerefZdd(manager,graph[fanin->Id].Fpath);
							}

					temp2 = Cudd_zddUnion(manager,graph[i].fault,graph[fanin->id].fault);
					Cudd_Ref(temp2);
					graph[i].fault = temp2;

				break;

			    case NOT   :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					graph[i].Cval = NOT_OUT(graph[fanin->id].Cval);
				}
				break;

			    } //end case
if(graph[i].Type != 0)
printf("\n At node %d i have the minterms %d ",i,Cudd_zddPrintMinterm(manager,graph[i].fault));
		}

}



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
