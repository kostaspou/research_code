

#include "graph.h"
//function to add from gates
void construct_stem(NODE *graph,int Max)
{
LIST *fanout,*temp1,*temp2;
int i,counter;

for(i=0;i<=Max;i++){
	//there is a non from gate which drives more than one gates so replace with from gates
	if(graph[i].Nfo>1){
		counter = 1;
		fanout = graph[i].Fot;
		FreeList(&temp1);
		CopyList(&graph[i].Fot,&temp1);	
		while(fanout!=NULL){
if(strlen(graph[i+counter].Name)!=0) printf("-------------WE HAVE A PROBLEM----------------i=%d\n",i);
			sprintf(graph[i+counter].Name,"from%d",i+counter);
			graph[i+counter].Nfi = 1;
			graph[i+counter].Nfo = 1;
			graph[i+counter].Type = 10;
			Delete(&temp1,fanout->id);
			InsertList(&temp1,i+counter);
			Delete(&graph[fanout->id].Fin,i);
			InsertList(&graph[fanout->id].Fin,i+counter);
 			InsertList(&graph[i+counter].Fin,i);
                  	InsertList(&graph[i+counter].Fot,fanout->id);
			counter++;
		fanout = fanout->next;
		}
		FreeList(&graph[i].Fot);
		CopyList(&temp1,&graph[i].Fot);
	}

}

}

//main function to levelize the circuit
//takes as input the old graph and returns a new levelized graph
INFO Levelize(NODE *graph,NODE *new_graph,int Max)
{
static LUT old_table[Htable];
static LUT new_table[Htable];
int i,flag,zero,j,max,level,node,id,counter,index,iter;
char name[Mlin];
LIST *fanin,*fanout;
int old_position,new_position,weight,npi;
INFO netlist;

clock_t begin,end;
begin = clock();

old_position = new_position = 0;
npi = 0;
	for(i=0;i<=Max;i++){
		if(graph[i].Type != 0) insert(old_table,graph[i].Name,&old_position);
	}

printf("\nSplitting the DFFs. . .");
//split the DFF
//replace it with one sudo input and one buffer as output
//also update the corresponding fanin and fanout lists
id = 0;
weight = 2;
for(i=0;i<=Max;i++){
graph[i].dff = 0;
graph[i].order=0;
	if(graph[i].Type == DFF){
		graph[i].Type = INPT;
		fanin = graph[i].Fin;
		graph[i].Nfi = 0;
		graph[i].dff = 1;
		graph[i].order=weight;
		node = fanin->id;
		FreeList(&graph[i].Fin);
		bzero(name,strlen(name));
		sprintf(name,"%s_next",graph[i].Name);
		id = insert(old_table,name,&old_position);
		strcpy(graph[id].Name,name);
		graph[id].Nfo=0;
		graph[id].Nfi=1;
		graph[id].dff = 1;
		graph[id].Po=1;
		graph[id].order=weight;
		graph[id].Type = BUFF;
		InsertList(&graph[id].Fin,node);
		Delete(&graph[node].Fot,i);
		InsertList(&graph[node].Fot,id);
		weight++;
	}
}
//case there are no DFFs
if(id==0) id = Max;
//if it is input assign level 0
	for(i=0;i<=id;i++){
		//if(graph[i].Type != 0) insert(old_table,graph[i].Name);
		graph[i].Mark = 0;
		graph[i].level = -1;
		if(graph[i].Type == INPT ||graph[i].Type == DFF){
			graph[i].Mark = 1;
			graph[i].level = 0;

		}
	}

printf("\nAssign levels to the gates. . .");
//continue with the rest gates
level = -1;
iter = 1;
while(iter==1){
iter = 0;
	for(j=0;j<=id+2;j++)
		if(graph[j].Mark == 0){
		max = -2;
		switch (graph[j].Type)  {
			case INPT  :
			case DFF   :
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
				if(iter == 0) iter = check_if_marked(graph,j);
				if(check_if_marked(graph,j) == 0){
					while(fanin!=NULL){
						max = MAX(graph[fanin->id].level,max);
						fanin = fanin->next;
					}
					graph[j].level = max+1;
					graph[j].Mark = 1;
					level = MAX(graph[j].level,level);
				}				
				break;
			case FROM  :
				fanin = graph[j].Fin;
				if(graph[fanin->id].Mark == 1){
				graph[j].level = graph[fanin->id].level;
				graph[j].Mark = 1;
				level = MAX(graph[j].level,level);
				}
				break;

		} //end case
	}
printf("\ncurrent level is %d",level);
}


//fill a second LUT that points the old ids with the new and create the new graph 
for(j=0;j<=level;j++){
	for(i=0;i<=id;i++){
		if(graph[i].level == j){
			index = insert(old_table,graph[i].Name,&old_position);
			bzero(name,strlen(name));
			sprintf(name,"%d",index);
			index = insert(new_table,name,&new_position);
			copy_node(graph,new_graph,i,index);
		}

	}
}
id++;

printf("\nUpdating the Graph. . .");
//update the lists with the new index taken from the second LUT
for(i=0;i<=id;i++){
fanin=NULL;
fanout=NULL;
	CopyList(&new_graph[i].Fin,&fanin);
	FreeList(&new_graph[i].Fin);
	while(fanin!=NULL){
		bzero(name,strlen(name));
		sprintf(name,"%d",fanin->id);
		index = insert(new_table,name,&new_position);
		InsertList(&new_graph[i].Fin,index);
		fanin = fanin->next;
	}
	CopyList(&new_graph[i].Fot,&fanout);
	FreeList(&new_graph[i].Fot);
	while(fanout!=NULL){
		bzero(name,strlen(name));
		sprintf(name,"%d",fanout->id);
		index = insert(new_table,name,&new_position);
		InsertList(&new_graph[i].Fot,index);
		fanout = fanout->next;
	}
}
end = clock();
printf("\nTIME SPENT TO LEVELIZE THE CIRCUIT %f",(double)(end-begin)/CLOCKS_PER_SEC);

printf("\nChecking if the graph is in topological order. . .\n");
for(i=0;i<=id;i++){
	if(new_graph[i].Nfi==0) {new_graph[i].Mark=1;}
	if(new_graph[i].Type == 1) {npi++;}
	else if(check_if_marked(new_graph,i) == 0){
		new_graph[i].Mark=1;
	}else printf("\n------------ERROR-----------i=%d",i);
}
netlist.no_inputs = npi;
netlist.max = id;
return netlist;
}


//function to copy the two graphs
void copy_node(NODE *old_graph,NODE *new_graph,int old_i,int new_i)
{
strcpy(new_graph[new_i].Name,old_graph[old_i].Name);
new_graph[new_i].Type=old_graph[old_i].Type;
new_graph[new_i].Nfi=old_graph[old_i].Nfi;
new_graph[new_i].Nfo=old_graph[old_i].Nfo;
new_graph[new_i].Po=old_graph[old_i].Po;
new_graph[new_i].dff=old_graph[old_i].dff;
new_graph[new_i].order=old_graph[old_i].order;
new_graph[new_i].Mark=0;  
new_graph[new_i].Cval=-1;
new_graph[new_i].Fval=-1; 
CopyList(&old_graph[old_i].Fin,&new_graph[new_i].Fin);
CopyList(&old_graph[old_i].Fot,&new_graph[new_i].Fot);
new_graph[new_i].level=old_graph[old_i].level;   
return;
}

//run through the fanin list of the node and check if on of the inputs is not marked
int check_if_marked(NODE *graph,int node)
{
LIST *temp;
int flag=0;
temp = graph[node].Fin;
while(temp!=NULL){
	if(graph[temp->id].Mark == 0) flag = 1;
temp = temp->next;
}
return flag;
}

void PrintINFO(NODE *graph,int Max)
{
LIST *temp;
int  i;
printf("\nID\tNAME\tTypeE\tPO\tOrder\tDff\tWeight\tChain\tFANIN\tFANOUT\n");
for(i=0;i<=Max;i++){
  if(graph[i].Type!=0){
    printf("%d\t%s\t%d\t%d\t%d\t%d\t",i,graph[i].Name,graph[i].Type,graph[i].Po,graph[i].order,graph[i].dff);
    printf("%d\t%d\t",graph[i].weight,graph[i].chain);
    temp=NULL;  temp=graph[i].Fin;   if(temp!=NULL){  PrintList(temp); } printf("\t");
    temp=NULL;  temp=graph[i].Fot;   if(temp!=NULL){  PrintList(temp); }
    printf("\n"); } }
return;
}

/***************************************************************************************************************************
Copy List A to List B
****************************************************************************************************************************/
void CopyList(LIST **a,LIST **b)
{
LIST *temp;

temp = *a;	
while(temp!=NULL){
	InsertList(b,temp->id);
	temp = temp->next;
}
return;
}//end of InsertList
/***************************************************************************************************************************
Insert an element "x" at end of LIST "l", if "x" is not already in "l". 
****************************************************************************************************************************/
void InsertList(LIST **l,int x)
{
LIST *p,*tl;

if ((p=(LIST *) malloc(sizeof(LIST)))==NULL){ printf("LIST: Out of memory\n");  exit(1);  }   
else{
  p->id=x;  p->next=NULL;
  if(*l==NULL){  *l=p; return;  }
  tl=NULL; tl=*l; 
  while(tl!=NULL){
    if(tl->id==x){        break;   }
    if(tl->next==NULL){  tl->next=p; } 
    tl = tl->next; }  }
return;
}//end of InsertList
/***************************************************************************************************
Count the total number  of elements in LIST "Cur"          
***************************************************************************************************/
int CountList(LIST *Cur)
{
LIST *tmp=Cur;
int size=0;

while(tmp!=NULL){   
  size++;
  tmp = tmp->next; } 
return size;
}//end of PrintList
/***************************************************************************************************
Delete an element "X" from LIST "Cur",
***************************************************************************************************/	
void Delete(LIST **Cur,int X)
{
LIST *fir = (*Cur);
LIST *lst = NULL;

if (fir==NULL) return;  
while((fir->id!=X)&&(fir!= NULL)){
  lst=fir;
  fir=fir->next; }   
if(fir!=NULL){
   if(lst==NULL){  (*Cur)=(*Cur)->next;       }
   else{           lst->next = fir->next; }
   free(fir); }
else{ 
   return; }
return;
}//end of DeleteEle
/***************************************************************************************************************************
Print the elements in LIST "l"          
***************************************************************************************************************************/
void PrintList(LIST *l)
{
LIST *temp=NULL;

temp=l;
while(temp!=NULL){   
  printf("%d ", temp->id);
  temp = temp->next; } 
return;
}//end of PrintList
/***************************************************************************************************************************
Free all elements in  LIST "l"  
****************************************************************************************************************************/
void FreeList(LIST **l)
{
LIST *temp=NULL;

if(*l==NULL){   return;  }
temp=(*l);
while((*l) != NULL){
  temp=temp->next;
  free(*l); (*l)=temp; }   
(*l)=NULL;
return;
}//end of FreeList 	


static unsigned long hash_function(unsigned char * str)
{
unsigned long hash = 0;
int c;
while(c = *str++)
	hash = c + (hash << 6) - hash;
return hash;
}


//function to access the LUT
//if there is no entry similar to string s creates one and returns the id
//if there is then returns the id
int insert(LUT *table,char *s,int *value){

int i,output,pos;

pos = hash_function(s)%Htable;
if(strlen(table[pos].name)==0){
	bzero(table[pos].name,Mnam);
	strcpy(table[pos].name,s);
	*value = *value + 1;
	table[pos].value = *value;
	return table[pos].value;
} else {
	if(strcmp(table[pos].name,s)==0){
		return table[pos].value;
	} else if (strcmp(table[pos].name,s)!=0){
		i=0;
		while(strlen(table[i].name)!=0){
			if(strcmp(table[i].name,s)==0){
				return table[i].value;
			}		
		i++;
		}
		bzero(table[i].name,Mnam);
		strcpy(table[i].name,s);
		*value = *value + 1;
		table[i].value = *value;
		return table[i].value;
	}
}
}

//main function to read ISCAS89 .bench files
//same as read_bench function but
//it uses a LUT to keep record of the entries in order to correlate name and id
int node_table(FILE *fisc,NODE *graph){

static LUT table[Htable];

char str1[Mlin],str2[Mlin],line[Mlin];
int  i,j,id,fid;
int max = 0;
LIST *temp;
LIST *fanout,*temp1;
int index,counter,position;
temp1=NULL;
position = 0;

clock_t begin,end;
begin = clock();


// intialize all nodes in graph structure
for(i=0;i<Mnod;i++){ InitializeCircuit(graph,i); } 
//skip the comment lines 
do
fgets(line,Mlin,fisc);
while(line[0] == '#');

// read line by line
while(!feof(fisc)){
  //initialize temporary strings 
  
id = 0;

i=j=0;
bzero(str1,strlen(str1));

//skips the empty lines
if(strlen(line)>1){
		while(line[i]!='(' && line[i]!=' '){
			str1[j]=line[i];
			i++;
			j++;
		}
str1[j] = '\0';
if(strcmp(str1,"INPUT")==0){ //read the input
	j=0;
	i++;
	bzero(str1,strlen(str1));
	while(line[i]!=')'){
		str1[j]=line[i];
		i++;
		j++;
	}
	str1[j] = '\0';
	id = insert(table,str1,&position);
//printf("\nName of input: %sand id %d",str1,id);
	graph[id].Type = 1;
	strcpy(graph[id].Name,str1);
	graph[id].Nfi=0;
}else if(strcmp(str1,"OUTPUT")==0){ //reads the output
	j=0;
	i++;
	bzero(str1,strlen(str1));
	while(line[i]!=')'){
		str1[j]=line[i];
		i++;
		j++;
	}
	str1[j] = '\0';
	id = insert(table,str1,&position);
	strcpy(graph[id].Name,str1);
	//graph[id].Nfo=0;
	graph[id].Po=1;
//printf("\ni have an output of %sand id %d",str1,id);
}else{					//reads the rest gates
	id = insert(table,str1,&position);
	strcpy(graph[id].Name,str1);
//printf("\ni have a new gate %s with id %d",str1,id);
	i = i + 3;
	j=0;
  	bzero(str1,strlen(str1));
	while(line[i]!='('){
		str1[j]=line[i];
		i++;
		j++;
	}
	graph[id].Type=AssignTypee(str1);

	j=0;
	i++;
  	bzero(str1,strlen(str1));
	while(line[i]!=')'){
		if(line[i]!=' '){
			str1[j]=line[i];
			j++;
		}
		i++;
	}
	str1[j] = ' ';
	str1[j+1] = ')';
	str1[j+2] = '\0';
//printf(" and inputs %s",str1);
	i=0;
	while(str1[i]!=')'){
		j=0;
  		bzero(str2,strlen(str2));
		while(str1[i]!=','&&str1[i]!=' '){
			str2[j]=str1[i];
			j++;
			i++;
		}
	str2[j] = '\0';
	fid = insert(table,str2,&position);
	InsertList(&graph[id].Fin,fid);  
	InsertList(&graph[fid].Fot,id);

		i++;
	}
}

if(id>=max) max = id;

}
  bzero(line,strlen(line)); 
  fgets(line,Mlin,fisc);  
} // end while



//max++;

for(i=0;i<=max;i++){
temp = graph[i].Fin;
graph[i].Nfi = CountList(temp);
temp = graph[i].Fot;
graph[i].Nfo = CountList(temp);
}

//construct_stems
printf("\nConstructing the stems. . .");
counter = 1;
for(i=0;i<=max;i++){
	//there is a gate which drives more than one gates so replace with from gates
	if(graph[i].Nfo>1){
		fanout = graph[i].Fot;
		FreeList(&temp1);
		CopyList(&graph[i].Fot,&temp1);	
		while(fanout!=NULL){
			sprintf(str1,"from%d",i+counter);
			index = insert(table,str1,&position);
//if the new node exists it arises an error
if(strlen(graph[index].Name)!=0) printf("-------------WE HAVE PROBLEM----------------i=%d and index=%d and name %s and counter is %d\n",i,index,graph[index].Name,counter);
			sprintf(graph[index].Name,"from%d",i+counter);
			graph[index].Nfi = 1;
			graph[index].Nfo = 1;
			graph[index].Type = 10;
			Delete(&temp1,fanout->id);
			InsertList(&temp1,index);
			Delete(&graph[fanout->id].Fin,i);
			InsertList(&graph[fanout->id].Fin,index);
 			InsertList(&graph[index].Fin,i);
                  	InsertList(&graph[index].Fot,fanout->id);
			counter++;
		fanout = fanout->next;
		}
		FreeList(&graph[i].Fot);
		CopyList(&temp1,&graph[i].Fot);
	}

}
end = clock();
printf("\nTIME SPENT TO READ THE CIRCUIT %f",(double)(end-begin)/CLOCKS_PER_SEC);
//change max value

return index+1;

}

/****************************************************************************************************************************
Initialize the paricular memeber of graph structure
****************************************************************************************************************************/           
void InitializeCircuit(NODE *graph,int num)
{
bzero(graph[num].Name,Mnam);
graph[num].Type=graph[num].Nfi=graph[num].Nfo=graph[num].Po=graph[num].Mark=0;  
graph[num].Cval=graph[num].Fval=-1; 
graph[num].Fin=graph[num].Fot=NULL;   
return;
}//end of InitializeCircuit 
/****************************************************************************************************************************
Convert (char *) Typee read to (int)     
****************************************************************************************************************************/
int AssignTypee(char *Type)
{
if      ((strcmp(Type,"inpt")==0) || (strcmp(Type,"INPT")==0))       return 1;
else if ((strcmp(Type,"and")==0)  || (strcmp(Type,"AND")==0))        return 2;
else if ((strcmp(Type,"nand")==0) || (strcmp(Type,"NAND")==0))       return 3;
else if ((strcmp(Type,"or")==0)   || (strcmp(Type,"OR")==0))         return 4;
else if ((strcmp(Type,"nor")==0)  || (strcmp(Type,"NOR")==0))        return 5;
else if ((strcmp(Type,"xor")==0)  || (strcmp(Type,"XOR")==0))        return 6;
else if ((strcmp(Type,"xnor")==0) || (strcmp(Type,"XNOR")==0))       return 7;
else if ((strcmp(Type,"buff")==0) || (strcmp(Type,"BUFF")==0))       return 8;
else if ((strcmp(Type,"not")==0)  || (strcmp(Type,"NOT")==0))        return 9;
else if ((strcmp(Type,"from")==0) || (strcmp(Type,"FROM")==0))       return 10;
else if ((strcmp(Type,"dff")==0) || (strcmp(Type,"DFF")==0))       return 11;
else                          			                   return 0;
}//end of AssignTypee
/****************************************************************************************************************************
Print all members of graph structure(except Type=0) after reading the bench file
*****************************************************************************************************************************/
void PrintCircuit(NODE *graph,int Max)
{
LIST *temp;
int  i;
printf("\nID\tNAME\tTypeE\tPO\tFin\tFot\tCVAL\tFVAL\tMark\tLevel\tFANIN\tFANOUT\n");
for(i=0;i<=Max;i++){
  if(graph[i].Type!=0){
    printf("%d\t%s\t%d\t%d\t%d\t%d\t",i,graph[i].Name,graph[i].Type,graph[i].Po,graph[i].Nfi,graph[i].Nfo);
    printf("%d\t%d\t%d\t%d\t",graph[i].Cval,graph[i].Fval,graph[i].Mark,graph[i].level);
    temp=NULL;  temp=graph[i].Fin;   if(temp!=NULL){  PrintList(temp); } printf("\t");
    temp=NULL;  temp=graph[i].Fot;   if(temp!=NULL){  PrintList(temp); }
    printf("\n"); } }
return;
}//end of PrintCircuit
/****************************************************************************************************************************
Free the memory of all member of graph structure
*****************************************************************************************************************************/
void ClearCircuit(NODE *graph,int i)
{
int num=0;
for(num=0;num<i;num++){
  graph[num].Type=graph[num].Nfi=graph[num].Nfo=graph[num].Po=0;
  graph[num].Mark=graph[num].Cval=graph[num].Fval=0; 
  if(graph[num].Type!=0){  bzero(graph[num].Name,Mnam);      
    if(graph[num].Fin!=NULL){   FreeList(&graph[num].Fin);  graph[num].Fin = NULL;     } 
    if(graph[num].Fot!=NULL){   FreeList(&graph[num].Fot);  graph[num].Fot = NULL;     } } }
return;
}//end of ClearCircuit
/*****************************************************************************************************************************
 Routine to read the .vec files
*****************************************************************************************************************************/
int ReadVec(FILE *fvec,PATTERN *vector)
{
int a,b,c,d;      //random variables 
char str[Mpi];        //char array

a=b=c=d=0;               //intializing the temporary variables 
while(!feof(fvec)){    
  bzero(str,Mpi);                  //initialing the string   
  fgets(str,Mpi,fvec);             //reading a single line
  if(*str!='\0'){ 
    bzero(vector[a].piv,Mpi);                           //initialing the string 
    strcpy(vector[a].piv,str);                          //copy the string into a piv in vector structure  
    a++; } } 
return a;
}//end of readvec
/****************************************************************************************************************************/
