#include "graph.h"


void test_podem(NODE *graph,int Max,int npi){

int i,j,k;
int * vector,*list,**faults;

for(j=0;j<=Max;j++){
	graph[j].Cval = graph[j].Fval = 2;
	graph[j].Mark =0;
}

graph[1].Cval = 1;
graph[5].Cval = 0;

update_graph(graph,Max);

vector = Simulate(graph,Max,25,0,npi);
PrintINFO(graph,Max);
printf("\nReturned Vector");
for(i=0;i<npi;i++){
//if(vector[i] == 2) vector[i]=1;
	printf("%d",vector[i]);
}
//update fault list
	faults = create_fault_list(graph,Max,vector,1);
	k=0;
	for(j=0;j<=Max;j++){
			if(graph[j].Po == 1 && graph[j].Cval != 2){
				list = faults[k];
				i=0;
				while(list[i]!=10){
					if(list[i]==1){
						printf("\n I have detected the %d fault",i);
					}
				//printf("\nelement %dth in list is %d",i,list[i]);
				i++;
				}
		k++;
		}
	}


}

int * fetch_vector(NODE *graph,int Max,vector_info * vector_and_grade,fault_list_info *fault_grades ,int no_vectors)
{
int n,i,j,k;
int **fault_list,*list,*swap;
float temp;

for(n=0;n<no_vectors;n++){
vector_and_grade[n].grade = 0.0;
	fault_list = create_fault_list(graph,Max,vector_and_grade[n].vector,1);
	k=0;
	for(j=0;j<=Max;j++){
			if(graph[j].Po>0 && graph[j].Cval != 2){
				list = fault_list[k];
				i=0;
				while(list[i]!=10){
					if(list[i]==1 && fault_grades[i].detected == 0){
						vector_and_grade[n].grade += (float)graph[j].weight/fault_grades[i].value;
					}
				//printf("\nelement %dth in list is %d",i,list[i]);
				i++;
				}
		k++;
		}
	}
}


//order the vectors
  for(i=0;i<(no_vectors-1);i++)
  {
    for(j=0;j<no_vectors-i-1;j++)
    {
      if (vector_and_grade[j].grade < vector_and_grade[j+1].grade) /* For decreasing order use < */
      { 
        swap = vector_and_grade[j].vector;
	temp = vector_and_grade[j].grade;
        vector_and_grade[j].vector   = vector_and_grade[j+1].vector;
	vector_and_grade[j].grade = vector_and_grade[j+1].grade;
        vector_and_grade[j+1].vector = swap;
	vector_and_grade[j+1].grade = temp;

      }
    }
  }
printf("\nNext random vector has grade %f and is :\n",vector_and_grade[0].grade);
for(i=0;i<7;i++){
	printf("%d",vector_and_grade[0].vector[i]);
}
return vector_and_grade[0].vector;
}

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
		if(graph[i].Cval != 2) graph[i].Mark = 1;
		graph[i].Fval = graph[i].Cval;
	}

}//end of function
