#include "graph.h"


int ** create_fault_list(NODE *graph,int Max,int *vector,int option)
{
LIST *fanin;
int previous,i,k,j,index;
 DdNode *temp1,*temp2;
 DdNode *list1,*list2;
struct timespec seconds;
FILE *fp1;

int *list,**full_list;
float *fault_grades;
//char *buffer;
list1 = NULL;
list2 = NULL;

manager=Cudd_Init(0,0,CUDD_UNIQUE_SLOTS,CUDD_CACHE_SLOTS,0);          //Intializing CUDD package manger
onez = Cudd_ReadZddOne(manager, ( (2 * Mnod ) + 5 ));
Cudd_DisableGarbageCollection(manager);

	fault_grades = ALLOC(float, 3*Mnod);
for(i=0;i<manager->sizeZ;i++){
	fault_grades[i]=0;
}
index = 0;

		for(i=0;i<=Max;i++){
// Switch statement to manipulate every node according to their type
if(graph[i].Type != 0){
printf("\ni am at NODE =  %d",i);
			switch (graph[i].Type)  {
			    case INPT  :
				if(option == 0){
				//generate random input vector
					gettimeofday(&seconds,NULL);
					srand(seconds.tv_nsec);
					graph[i].Cval = rand()%2;
//if(i == 5 || i ==6 || i == 7) graph[i].Cval = 2;
//if(i==1 || i==4 || i==6) graph[i].Cval = 1; else graph[i].Cval = 0;
					vector[index] = graph[i].Cval;
				//else apply the input vector
				} else if (option == 1){
					graph[i].Cval = vector[index];
				} else if(option == 2){
					if(graph[i].dff != 1){
					gettimeofday(&seconds,NULL);
					srand(seconds.tv_nsec);
					graph[i].Cval = rand()%2;
					}
					vector[index] = graph[i].Cval;
				}
				index++;
//if(i==5) graph[i].Cval = 0;
//else graph[i].Cval = 1;
//graph[i].Cval = 0;

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
						previous = Two_Gates(0,graph[fanin->id].Cval,previous);
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
						previous = Two_Gates(0,graph[fanin->id].Cval,previous);
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
						previous = Two_Gates(1,graph[fanin->id].Cval,previous);
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
						previous = Two_Gates(1,graph[fanin->id].Cval,previous);
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
						previous = Two_Gates(0,Two_Gates(1,NOT_OUT(graph[fanin->id].Cval),previous),Two_Gates(1,graph[fanin->id].Cval,NOT_OUT(previous)));
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
						previous = Two_Gates(0,Two_Gates(1,NOT_OUT(graph[fanin->id].Cval),previous),Two_Gates(1,graph[fanin->id].Cval,NOT_OUT(previous)));
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

full_list =(int**) malloc(sizeof(int*));
k=1;
for(j=0;j<=Max;j++){
	if(graph[j].Po>0 && graph[j].Cval != 2){
	list = my_Cudd_zddPrintMinterm(manager,graph[j].fault);
full_list =(int**) realloc(full_list,k*sizeof(int*));
full_list[k-1]=list;
k++;
	}
}
Cudd_Quit(manager);

return full_list;
}


preprocess_info fault_grading(NODE *graph,int Max,int npi)
{

int **fault_list;
int *list;
int k,j,i,n,no_vectors,max,top;
no_vectors = 40;
float *fault_grades = (float *)malloc(2*Max*sizeof(float));
int * vector,*swap;
float temp;
int detected,undetected;

preprocess_info preprocess;
//vector_info vector_and_grade[no_vectors];
vector_info * vector_and_grade = (vector_info *)malloc(no_vectors*sizeof(vector_info));


for(i=0;i<2*Max;i++){
	fault_grades[i] = 0;
}
for(n=0;n<no_vectors;n++){
	vector = (int *) malloc(npi*sizeof(int));
	fault_list = create_fault_list(graph,Max,vector,0);
	vector_and_grade[n].vector = vector;
	k=0;
	for(j=0;j<=Max;j++)
		if(graph[j].Po>0 && graph[j].Cval != 2){
			list = fault_list[k];
			i=0;
			while(list[i]!=10){
				if(list[i]==1){
					fault_grades[i] += 1.0/pow(2,graph[j].order);
				}
			//printf("\nelement %dth in list is %d",i,list[i]);
			i++;
			}
	k++;
	}
vector = NULL;
free(vector);
}
max = 0;
for(j=0;j<=Max;j++){
	if(graph[j].order > max) max = graph[j].order;
}
top =max;
for(j=0;j<=Max;j++){
	if(graph[j].dff == 1 && graph[j].Type == 1) {
		graph[j].weight = max;
		graph[graph[j].chain].weight = max;
		max--;
	} else if(graph[j].dff != 1) graph[j].weight = top;
}


for(n=0;n<no_vectors;n++){
vector_and_grade[n].grade = 0.0;
	fault_list = create_fault_list(graph,Max,vector_and_grade[n].vector,1);
	k=0;
	for(j=0;j<=Max;j++){
			if(graph[j].Po>0 && graph[j].Cval != 2){
				list = fault_list[k];
				i=0;
				while(list[i]!=10){
					if(list[i]==1){
						vector_and_grade[n].grade += (float)graph[j].weight/fault_grades[i];
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

detected = undetected = 0;
for(i=2;i<2*Max;i++){
	if(fault_grades[i] != 0.0 ){ detected++;
	}else {undetected++;}
}
float result;
result = (float)detected/(detected+undetected)*100;
printf("\n fault coverage :%f%",result);


preprocess.vector_data = vector_and_grade;
preprocess.fault_grades = fault_grades;

return preprocess;
}

void algorithm(NODE *graph,int Max,int npi){

int i,j,k;
preprocess_info preprocess;
int * vector,*list,**faults;
float *fault_grades;
vector_info *vector_and_grade;
fault_list_info fault_list[2*Max],swap;
int *temp_vector = (int *)malloc(npi*sizeof(int));

preprocess = fault_grading(graph,Max,npi);

vector_and_grade = preprocess.vector_data;
fault_grades = preprocess.fault_grades;

for(i=2;i<2*Max;i++){
	fault_list[i].value = fault_grades[i];
	fault_list[i].id = i;
	fault_list[i].detected = 0;
}
//update fault list
//vector = vector_and_grade[0].vector;
//memcpy(temp_vector,vector,npi*sizeof(int));
	faults = create_fault_list(graph,Max,vector_and_grade[0].vector,1);
	k=0;
	for(j=0;j<=Max;j++){
			if(graph[j].Po>0 && graph[j].Cval != 2){
				list = faults[k];
				i=0;
				while(list[i]!=10){
					if(list[i]==1){
						fault_list[i].detected = 1;
					}
				//printf("\nelement %dth in list is %d",i,list[i]);
				i++;
				}
		k++;
		}
	}



/*
for(i=0;i<39;i++){
	printf("\n%dth vector at new is :",i);
	vector = vector_and_grade[i].vector;
	for(j=0;j<npi;j++){
		printf("%d",vector[j]);
	}
printf(" with grade %f",vector_and_grade[i].grade);
}
*/
/*
printf("\nGrade of vector:");
for(i=0;i<npi;i++){
	printf("%d",vector_and_grade[0].vector[i]);
}
printf("is %f",vector_and_grade[0].grade);
vector = vector_and_grade[0].vector;
memcpy(temp_vector,vector,npi*sizeof(int));
shift(graph,Max,npi,temp_vector,fault_list,1);
printf("\nof vector:");
for(i=0;i<npi;i++){
	printf("%d",vector[i]);
}
*/



  for(i=0;i<(2*Max-1);i++)
  {
    for(j=0;j<2*Max-i-1;j++)
    {
      if (fault_list[j].value > fault_list[j+1].value)
      { 
        swap = fault_list[j];
        fault_list[j]   = fault_list[j+1];
        fault_list[j+1] = swap;

      }
    }
  }

vector = vector_and_grade[0].vector;
memcpy(temp_vector,vector,npi*sizeof(int));
printf("Before apply");
for(i=0;i<npi;i++){
	printf("%d",vector_and_grade[0].vector[i]);
}
apply(graph,Max,npi,temp_vector,fault_list);
/*
memcpy(temp_vector,vector,npi*sizeof(int));
printf("Before shift");
for(i=0;i<npi;i++){
	printf("%d",vector_and_grade[0].vector[i]);
}
shift(graph,Max,npi,temp_vector,fault_list,0);
*/
for(i=2;i<2*Max;i++){
	printf("\nNew function for fault %d i have the grade %f and detected %d",fault_list[i].id,fault_list[i].value,fault_list[i].detected );
}


}

void apply(NODE *graph,int Max,int npi,int *in_vector,fault_list_info *fault_info)
{
int i,j,k,last_dff,flag;
int **fault_list;
int *list,*vector;
float grade;
/*
	for(i=0;i<npi-1;i++){
		if(graph[i+1].dff == 1){
			in_vector[i] = in_vector[i+1];
		}

	}
	in_vector[i] = value;
*/
//update input
	//run this to get values, maybe i can do it before calling this function
	fault_list = create_fault_list(graph,Max,in_vector,1);
for(i=0;i<Max;i++){
	if(graph[i].Po == 1 && graph[i].dff == 1) graph[graph[i].chain].Cval = graph[i].Cval;
	else graph[i].Cval = 2;
}

update_graph(graph,Max);


/*
//generate random input fro PI not dff
fault_list = create_fault_list(graph,Max,in_vector,2);
printf("\nVector 1:");
for(j=0;j<npi;j++) printf("%d",in_vector[j]);
*/

//call PODEM for the first not detected fault in the list
for(i=2;i<8;i++){
	if(fault_info[i].detected == 0){
		if(fault_info[i].id%2==0){
			vector = Simulate(graph,Max,fault_info[i].id/2,0,npi);

		}if(fault_info[i].id%2==1){
			vector = Simulate(graph,Max,(fault_info[i].id-1)/2,1,npi);

		}
PrintCircuit(graph,Max);
	}

	flag = 0;
				for(j=0;j<npi;j++) printf("%d",vector[j]);
	for(j=0;j<npi;j++)
		if(vector[j]==2 || graph[0].Cval == 3)
			flag = 1;
//if(flag == 0){printf("the vector is full at i %d",i); break;}
}

/*
for(i=0;i<=Max;i++){
	if(graph[i].dff == 1 && graph[i].Po == 1 && graph[i].order ==2) last_dff = i;
}

	k=0;
	grade = 0.0;
	for(j=0;j<=Max;j++)
		if(graph[j].Po ==1 && graph[j].Cval != 2){
			if(graph[j].dff !=1){
				list = fault_list[k];
				i=0;
				while(list[i]!=10){
					if(list[i]==1 && fault_info[i].detected == 0){
						grade += (float)graph[j].weight/fault_info[i].value;//i have fault with 0
					}
				//printf("\nelement %dth in list is %d",i,list[i]);
				i++;
				}
			}
		k++;
	}
printf("\nThe grade after apply is %f",grade);
*/
}

void shift(NODE *graph,int Max,int npi,int *in_vector,fault_list_info *fault_info,int value)
{
int i,j,k,last_dff;
int **fault_list;
int *list;
float grade;
/*
	for(i=0;i<npi-1;i++){
		if(graph[i+1].dff == 1){
			in_vector[i] = in_vector[i+1];
		}

	}
	in_vector[i] = value;
*/
for(i=0;i<=Max;i++){
	if(graph[i].dff == 1 && graph[i].Po == 1 && graph[i].order ==2) last_dff = i;
}
	fault_list = create_fault_list(graph,Max,in_vector,1);
	k=0;
	grade = 0.0;
	for(j=0;j<=Max;j++)
		if(graph[j].Po ==1 && graph[j].Cval != 2){
			if(last_dff == j){
				list = fault_list[k];
				i=0;
				while(list[i]!=10){
					if(list[i]==1 && fault_info[i].detected == 0){
						grade += (float)(graph[j].weight+1)/fault_info[i].value;//i have fault with 0
					}
				//printf("\nelement %dth in list is %d",i,list[i]);
				i++;
				}
			}
		k++;
	}
printf("\nThe grade after shift is %f",grade);
printf("\nVector inside shift:");
for(i=0;i<npi;i++){
	printf("%d",in_vector[i]);
}
//PrintINFO(graph,Max);
}

int * my_Cudd_zddPrintMinterm(DdManager * zdd,DdNode * node)
 {
     int         i, size;
     int         *list,*value;
 
     size = (int)zdd->sizeZ;
     list = ALLOC(int, size);
     value = ALLOC(int, size+1);
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
		value[i] = 10;
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

int Two_Gates (int select, int a, int b)
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
/*
//Not gate
int NOT_OUT (int a)
{
	int LUT[3] = {1,0,2}; 


	return (LUT[a]);
}*/
