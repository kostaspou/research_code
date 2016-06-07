#include "graph.h"



int * Simulate(NODE *graph,int Max,int fault_node,int type_of_fault,int npi)
{
LIST *fanin,*temp;
LIST *frontier=NULL;
LIST *event = NULL;
LIST *improve = NULL;
char line[Mlin],str1[Mlin];
int i,j,length,output_number ,k,index;
int inp_value,inp_node;
int node_id,value,input_value,line_number,next_value;
int test,check,obj,eof_flag;
int f_node,f_value,detected,new_input,error,flag;
//error reports
int coverage,event_null,nodes_number;
coverage = event_null = nodes_number = 0;

	//init_node(graph , Max);
	depth(graph,Max);
controllability(graph,Max);

clock_t begin,end;
begin = clock();

int * vector = (int *) malloc(npi*sizeof(int));

nodes_number++;
	detected = 0;
	error = 0;
	f_node = node_id =fault_node;
	f_value = value = type_of_fault;
	while(detected != 1 && error != 1){
	new_input = 0;
	controllability(graph,Max);
	obj = objective(graph,Max,&node_id,&value,f_node,f_value,&frontier);
	//printf("\nobjective returned with value %d and new node is %d with value %d and f_node %d",obj,node_id,value,f_node);
	backtrace(graph,node_id,value,&inp_value,&inp_node,&new_input);
	//printf("\nBacktrace returned node %d and value %d\n",inp_node,inp_value);
	push (&event,inp_node);
	detected = Evaluation(graph,Max,inp_node,inp_value,f_node,f_value);
	d_frontier(graph,Max,&frontier);
	//printf("\nfrontier list\n");
	//PrintList(frontier);
		if(detected == 1){
			// printf("\n------------------------DETECTED-----------------\n");
			for(j=0;j<=Max;j++){
				if(graph[j].Cval == 3)
					graph[j].Fval = 1;
				else if(graph[j].Cval == 4)
					graph[j].Fval = 0;
				else 
					graph[j].Fval = graph[j].Cval;
			}
			coverage++;
		} else {
			if(obj == 1 && frontier == NULL){
				 if(detected != 1)
					//printf("\n----------NOT DETECTED--------------(path blocked)");
			error = 1;
			}
			if(obj == 3) {
				error = 1;
				//printf("\n----------NOT DETECTED--------------(fault masked)");
			} 
			if(new_input ==0 ) {
				error = 1;
				//printf("\n----------NOT NEW INPUT -----------");
			}
			while(error == 1){
				//printf("\n------------trying to backtrack-------------event list ---->");PrintList(event);
				inp_node = pop(&event);
				inp_value = invert(inp_value);
				frontier=NULL;
				detected = Evaluation(graph,Max,inp_node,inp_value,f_node,f_value);
				d_frontier(graph,Max,&frontier); 
				if(event == NULL){
					error = 0;
					detected = 1;
					//printf("\ncase event null");
					event_null++;
					push (&improve,f_node); 
				}else if(detected != 1 && frontier != NULL) {
					error = 0;
					//printf("\ncase continue");
				}else if(detected == 1){
					error = 0;
					detected = 1;
					for(j=0;j<=Max;j++){
						if(graph[j].Cval == 3)
							graph[j].Fval = 1;
						else if(graph[j].Cval == 4)
							graph[j].Fval = 0;
						else 
							graph[j].Fval = graph[j].Cval;
					}
					//printf("\n------------------------DETECTED-----------------\n");
					coverage++;
				}else if(frontier == NULL) {
					error = 1;
					//printf("\ncase next value from event list");
				}
			}
		}
	}
	//init_node(graph , Max);
	if(frontier!=NULL) frontier=NULL; event = NULL;

/*
//try to improve the coverage
//printf("\n-----TRY to improve the coverage");
temp = improve;
while(temp!=NULL){
detected = 0;
error = 0;
f_node = node_id =temp->id;
f_value = value = type_of_fault;
	while(detected != 1 && error != 1){
	new_input = 0;
	controllability(graph,Max);
	obj = objective(graph,Max,&node_id,&value,f_node,f_value,&frontier);
	backtrace(graph,node_id,value,&inp_value,&inp_node,&new_input);
	push (&event,inp_node);
	detected = Evaluation(graph,Max,inp_node,inp_value,f_node,f_value);
	d_frontier(graph,Max,&frontier);
	if(detected == 1){
		//printf("\n------------------------DETECTED AT FIRST BACKTRACK-----------------\n");
			for(j=0;j<=Max;j++){
				if(graph[j].Cval == 3)
					graph[j].Fval = 1;
				else if(graph[j].Cval == 4)
					graph[j].Fval = 0;
				else 
					graph[j].Fval = graph[j].Cval;
			}
		coverage++;
	} else {
		if(obj == 1 && frontier == NULL){
			 if(detected != 1) {
				//printf("\n----------NOT DETECTED--------------(path blocked)");
			} 
		error = 1;
		}
		if(obj == 3) {
			error = 1;
			//printf("\n----------NOT DETECTED--------------(fault masked)");
			} 
		if(new_input ==0 ) {
			error = 1;
			//printf("\n----------NOT NEW INPUT -----------");
			}
		if(error == 1){
			//printf("\n------------trying to backtrack-----------------");
			inp_node = pop(&event);
			graph[inp_node].Cval = 2;
			inp_node = pop(&event);
			inp_value = invert(inp_value);
			frontier=NULL;
			detected = Evaluation(graph,Max,inp_node,inp_value,f_node,f_value);
			d_frontier(graph,Max,&frontier); 
			if(event == NULL){
				error = 0;
				detected = 1;
				//printf("\ncase event null"); 
			}else if(detected != 1 && frontier != NULL) {
				error = 0;
				//printf("\ncase continue");
			}else if(detected == 1){
				error = 0;
				detected = 1;
				//printf("\n------------------------DETECTED AT FIRST BACKTRACK-----------------\n");
				for(j=0;j<=Max;j++){
					if(graph[j].Cval == 3)
						graph[j].Fval = 1;
					else if(graph[j].Cval == 4)
						graph[j].Fval = 0;
					else 
						graph[j].Fval = graph[j].Cval;
				}
				coverage++;
			}else if(frontier == NULL) {
				error = 0;
			}
		}
	}
	}
	//init_node(graph , Max);
	if(frontier!=NULL) frontier=NULL; event = NULL;
temp = temp->next;
}*/

					for(j=0;j<=Max;j++){
						if(graph[i].Fval != 2) graph[i].Mark = 1;
						if(graph[j].Type == INPT){
							vector[j-1] = graph[j].Fval;
							//fprintf(fres,"%d",input_value);	
						}
					}

/*
printf("\nTotal number of nodes %d and faults detected %d and event list is empty %d and not detected %d \n",nodes_number,coverage,event_null,nodes_number-coverage);
printf("------------------------RESULTS---------------------------\n\tTOTAl FAULTS->%d\n\tDETECTED->%d\n\tPERCENTAGE->%g\r\n",nodes_number,coverage,(((double)coverage)/nodes_number*100));
end = clock();
printf("\nTIME SPENT TO GENERATE THE PATTERNS %f\n",(double)(end-begin)/CLOCKS_PER_SEC);
*/
return vector;
}//end of Simulate function

//Functions that determine the output or the new value of each node


void x_path(NODE *graph,int node_id,int *check)
{
LIST *fanout;
	fanout = graph[node_id].Fot;
	while(fanout!=NULL){
		if(graph[fanout->id].Cval == 2){
			x_path(graph,fanout->id,check);
			if(*check == 1) break;
		}
	fanout = fanout->next;
	}

	if(graph[node_id].Po > 0){
		*check = 1;
	}

}


void backtrace(NODE *graph,int node_id,int value,int *result,int *inp_node,int *reach)
{
int in_node;
LIST * fanin;
if(value == 3) value = 1;
else if(value == 4) value =0;


			if(graph[node_id].Type == INPT){
				*reach = 1;
				//graph[node_id].Mark = 1;
				*inp_node = node_id;
				*result = value;
			}else if(graph[node_id].Type == AND || graph[node_id].Type == OR || graph[node_id].Type == BUFF || graph[node_id].Type == FROM){
				//IN_NODE(node_id)
in_node = graph[node_id].high;
/*
if((graph[node_id].Type == AND && value == 1)||(graph[node_id].Type == OR && value == 0)){
in_node = graph[node_id].low;
} else {
in_node = graph[node_id].high;
}*/
				backtrace(graph,in_node,value,result,inp_node,reach);
			}else if(graph[node_id].Type == NAND || graph[node_id].Type == NOR || graph[node_id].Type == NOT){
				//IN_NODE(node_id)
in_node = graph[node_id].high;
/*if((graph[node_id].Type == NAND && value == 1)||(graph[node_id].Type == NOR && value == 0)){
in_node = graph[node_id].high;
} else {
in_node = graph[node_id].low;
}*/
				backtrace(graph,in_node,~value+2,result,inp_node,reach);
			}else if(graph[node_id].Type == XOR || graph[node_id].Type == XNOR){				
				//IN_NODE(node_id)
in_node = graph[node_id].high;
				backtrace(graph,in_node,value,result,inp_node,reach);
			}

}

int objective(NODE *graph,int Max,int *node_id,int *value,int f_node, int f_value, LIST **frontier)
{

LIST *temp,*fanin;
int bin_value,node;

if(f_value == 1) bin_value = 3;
else if(f_value == 0) bin_value = 4;

	if(graph[f_node].Cval == bin_value){
		if(*frontier == NULL){
			return 3;
		}else {
			temp = *frontier;
			node = temp->id;
			if(graph[node].Type == AND || graph[node].Type == NAND){
				fanin = graph[node].Fin;
				while(fanin!=NULL){
					if(graph[fanin->id].Cval == 2){
						*node_id = fanin->id;
						*value = 1;
						break;
						}
				fanin = fanin->next;
				}
			} else if (graph[node].Type == OR || graph[node].Type == NOR){
				fanin = graph[node].Fin;
				while(fanin!=NULL){
					if(graph[fanin->id].Cval == 2){
						*node_id = fanin->id;
						*value = 0;
						break;
						}
				fanin = fanin->next;
				}
			}
			*frontier = NULL;
			return 1;
		}
	} else {
		return 0;
	}


}



void d_frontier(NODE *graph,int Max,LIST **frontier)
{
LIST *fanin;
int flag,i,check;


	for(i=0;i<=Max;i++){
	flag = 0;
	check = 0;
		if(graph[i].Type!=0)
			if(graph[i].Cval == 2){
				fanin = graph[i].Fin;
				while(fanin!=NULL){
					if(graph[fanin->id].Cval == 3 || graph[fanin->id].Cval == 4 ){
						flag = 1;
					}
				fanin = fanin->next;
				}
				if(flag == 1){
					x_path(graph,i,&check);
					if(check == 1){
						InsertList(&(*frontier),i);
						break;
					}
				}
			}
	}	
}

void depth(NODE *graph,int Max)
{
LIST *fanin;
int j,max;
	for(j=0;j<=Max;j++){
		max = -2;
		switch (graph[j].Type)  {
			case INPT  :
				break;
			case AND   :
			case NAND  :
			case OR    :
			case NOR   :
			case XOR   :
			case XNOR  :
			case BUFF  :
			case NOT   ://calculating the depth
				fanin = graph[j].Fin;
				while(fanin!=NULL){
					if(graph[fanin->id].level > max){
						 max = graph[fanin->id].level;
					}
					fanin = fanin->next;
				}
				graph[j].level = max+1;					
				break;
			case FROM  :
				fanin = graph[j].Fin;
				graph[j].level = graph[fanin->id].level;// i made a change here
				break;

		} //end case
	graph[j].Cval = graph[j].Fval;
	}

}

void controllability(NODE *graph,int Max)
{

LIST *fanin;

int j,max,min;

	for(j=0;j<=Max;j++){
		graph[j].high = 0;
		graph[j].low = 0;
		max = -2;
		switch (graph[j].Type)  {
			case INPT  :
				//graph[j].level = 0;
				break;
			case AND   :
			case NAND  :
			case OR    :
			case NOR   :
			case XOR   :
			case XNOR  :
			case BUFF  :
			case NOT   :
				fanin = graph[j].Fin;
				while(fanin!=NULL){
				if(graph[fanin->id].Cval==2)
					if(graph[fanin->id].level > max){
						 max = graph[fanin->id].level;
						graph[j].low = fanin->id;
					}
					fanin = fanin->next;
				}
				min = max;
				fanin = graph[j].Fin;
				while(fanin!=NULL){
				if(graph[fanin->id].Cval==2)
					if(graph[fanin->id].level <= min){
						min = graph[fanin->id].level;
						graph[j].high = fanin->id;
					}
					fanin = fanin->next;
				}				
				break;
			case FROM  :
			fanin = graph[j].Fin;
			if(graph[fanin->id].Cval==2 ){
				graph[j].low = fanin->id;
				graph[j].high = fanin->id;
			}
				break;

		} //end case
	}

}


		

int Evaluation(NODE *graph,int Max,int node_id,int value,int f_node,int f_value)
{
LIST *fanin;
int previous,i,res;



		for(i=0;i<=Max;i++){
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

				//inject the input
				if (i == node_id && graph[i].Mark == 0){
					graph[i].Cval = value;
					//printf("i am here\n");
				}
					if(i == f_node && graph[i].Mark == 0){
						if(graph[i].Cval == f_value){
							if(f_value == 1) graph[i].Cval = 3;
							else if(f_value == 0) graph[i].Cval = 4;
							}
					}
		}

	for(i=0;i<=Max;i++){
		if(graph[i].Po > 0)
			if(graph[i].Cval==3 || graph[i].Cval==4){
				return 1;
			}
	}
return 0;
}//end of evaluation function


int Gates (int select, int a, int b)
{
int output;
int LUT [6][5][5]= {		//AND
			{	{0,0,0,0,0},
				{0,1,2,3,4},
				{0,2,2,2,2},
				{0,3,2,3,0},
				{0,4,2,0,4}
			},	//NAND
			{	{1,1,1,1,3},
		    		{1,0,2,4,3},
	   	    		{1,2,2,2,3},
				{1,4,2,4,3},
				{3,3,3,3,3}
			},	//OR
			{	{0,1,2,3,4},
				{1,1,1,1,1},
				{2,1,2,2,2},
				{3,1,2,3,1},
				{4,1,2,1,4}
			},	//NOR
			{	{1,0,2,4,3},
				{0,0,0,4,0},
				{2,0,2,4,2},
				{4,4,4,4,4},
				{3,0,2,4,3}
			},	//XOR
			{	{0,1,2,0,0},
				{1,0,2,0,0},
				{2,2,2,0,0},
				{0,0,0,0,0},
				{0,0,0,0,0}
			},	//XNOR
			{	{1,0,2,0,0},
				{0,1,2,0,0},
				{2,2,2,0,0},
				{0,0,0,0,0},
				{0,0,0,0,0}
			},	
		};
output = LUT[select][a][b];
return (output);
}

//Not gate
int NOT_OUT (int a)
{
	int LUT[5] = {1,0,2,4,3}; 


	return (LUT[a]);
}

// wire
int FROM_OUT (int a)
{
	int LUT[5] = {0,1,2,3,4}; 


	return (LUT[a]);
}


void init_node(NODE *graph , int Max)
{
	int k;
	for(k=0;k<=Max;k++)
	{
		graph[k].Cval = 2;
		graph[k].Fval = 2;
		graph[k].Mark = 0;
		//graph[k].level = 0;
	}
}

void push (LIST ** top, int data)
{

LIST *temp;
	if(top == NULL)
	{
		*top = (LIST *)malloc(sizeof(LIST));
		(*top)->next = NULL;
		(*top)->id = data;
	} else 
	{
		temp = (LIST *)malloc(sizeof(LIST));
		temp->next = *top;
		temp->id = data;
		*top = temp;
	}
}

int pop(LIST ** top)
{
LIST *temp;
int value;
	temp = *top;
	if(temp == NULL)
	{
		printf("ERROR: Trying to pop from empty stack");
		value = -1;
	} else {
		temp = temp->next;
		value = (*top)->id;
		free(*top);
		*top = temp;
	}
return value;
}

