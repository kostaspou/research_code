#include "graph.h"

/***************************************************************************************************************************
 Main Function(Serial and Parallel Fault Simulation)
****************************************************************************************************************************/
void main(int argc,char **argv)
{
FILE *fisc,*ffau,*fres;             //file pointers used for .isc file, .vec file, .faults file and resultfile
int i,Max,Opt,Npi,Npo,Total,Tfs,no_inputs;              //maxnode id,option,tot no of PIs,tot no of Pos,Tot no of input patterns& faults in.vec in.faults

static NODE graph[Mnod];                         //structure used to store the ckt information in .isc file
static NODE new_graph[Mnod];                         //structure used to store the ckt information in .isc file
INFO netlist;
int *vector;

//Read the .isc file and store the information in graph structure
fisc=fopen(argv[1],"r");                           //file pointer to open .isc file 
//fres=fopen(argv[4],"w"); 			//file pointer to open output file
//ffau=fopen(argv[3],"r");                           //file pointer to open faults file 
Max=0; 
printf("\nRead the circuit. . .");
Max = node_table(fisc,graph);
//PrintCircuit(graph,Max);
//printf("\n#nodes = %d",Max);
netlist = Levelize(graph,new_graph,Max);
Max = netlist.max;
no_inputs = netlist.no_inputs;

algorithm(new_graph,Max,no_inputs);

/*vector = Simulate(new_graph,Max,25,0,no_inputs);
printf("\n");
for(i=0;i<no_inputs;i++)
	printf("%d",vector[i]);*/
PrintCircuit(new_graph,Max);
fclose(fisc);                            //close file pointer for .isc file



//fres=fopen(argv[2],"w");                           //file pointer to open .out file for printing results

//Perform Logic Simulation for each Input vector and print the Pos .val in output file   

//printf("\nNo of Unreferenced Zdds: %d\n", Cudd_CheckZeroRef(manager)); //Checking any unreferenced bdds in manager


//fclose(fisc);                                                  //close file pointer for .out file
ClearCircuit(graph,Mnod);                                      //clear memeory for all members of graph
return;
}//end of main
/****************************************************************************************************************************/

