#include "graph.h"

void update_graph(NODE *graph,int Max)
{
LIST *fanin;
int previous,i,res;



		for(i=0;i<=Max;i++){
			graph[i].Mark = 0;
// Switch statement to manipulate every node according to their type
			switch (graph[i].Type)  {
			    case INPT  :
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
				break;

			    case OR    :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					previous = graph[fanin->id].Cval;
					fanin = fanin->next;
					while(fanin!=NULL){
						previous = Gates(2,graph[fanin->id].Cval,previous);
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
						previous = Gates(2,graph[fanin->id].Cval,previous);
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
						previous = Gates(0,Gates(2,NOT_OUT(graph[fanin->id].Cval),previous),Gates(2,graph[fanin->id].Cval,NOT_OUT(previous)));
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
						previous = Gates(0,Gates(2,NOT_OUT(graph[fanin->id].Cval),previous),Gates(2,graph[fanin->id].Cval,NOT_OUT(previous)));
						fanin = fanin->next;
					}
					graph[i].Cval = NOT_OUT(previous);
				}
				break;

			    case BUFF  :
				if (graph[i].Nfi != 0) {
					fanin= NULL;
					fanin = graph[i].Fin;
					graph[i].Cval = FROM_OUT(graph[fanin->id].Cval);
				}
				break;

			    case NOT   :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					graph[i].Cval = NOT_OUT(graph[fanin->id].Cval);
				}
				break;

			    case FROM  :
				if (graph[i].Nfi != 0) {
					fanin = graph[i].Fin;
					graph[i].Cval = FROM_OUT(graph[fanin->id].Cval);
				}
				break;

			    } //end case
	}

}//end of function
