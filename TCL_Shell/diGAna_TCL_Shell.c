#include <iostream>
#include <stdio.h>
#include <partition.hxx>
#include <distributor.hxx>
#include <kernel.hxx>
#include <tcl.h>

#define LENGTH_OF(array) sizeof(array)/sizeof(array[0])

void create_graph_usage (Tcl_Interp * interp) {
	char const * usage_string = "Command usage : create_graph -name <name of the graph> -type <type of the graph 'directed' or 'undirected'> \
					 Mandatory : 'name' 											\
					 Optional : 'type' (default value : 'undirected')"; 			       
	Tcl_SetObjResult(interp,Tcl_NewStringObj(usage_string,-1));
}

void create_node_usage (Tcl_Interp * interp) {
	char const * usage_string = "Command usage : create_node -name <name of the node> -graph <name of the graph>                             \
					 Mandatory : 'graph' 											\
					 Optional : 'name' (default value : 'def_node')"; 			       
	Tcl_SetObjResult(interp,Tcl_NewStringObj(usage_string,-1));
}

void create_edge_usage (Tcl_Interp * interp) {
	char const * usage_string = "Command usage : create_edge -source_id <Name of source node/id of source node> -sink_id <Name of sink node/id of sink node> -graph <name of the graph>															\
					 Mandatory : 'source_id'									        	 \
					      	    'sink_id' 										      		 \
						     	    'graph'";

	Tcl_SetObjResult(interp,Tcl_NewStringObj(usage_string,-1));
}

int search(string search_space[], string value, int size) {

	int i;
	for ( i = 0 ; i < size ; i++ ) {
		if(strcmp(search_space[i].c_str(),value.c_str())==0) {
			return 1;
		}
	}
	return 0;
}


int create_graph_cmd(ClientData cd, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
		
	int i,size;
	string graph_name,graph_type;
	diganaGraphType digana_graph_type;

	if((objc==1) || (objc%2==0)) {
		create_graph_usage(interp);
		return TCL_ERROR;
	}

	string options [] = {Tcl_GetStringFromObj(objv[1],NULL) , Tcl_GetStringFromObj(objv[3],NULL)};
	string values [] = {Tcl_GetStringFromObj(objv[2],NULL) , Tcl_GetStringFromObj(objv[4],NULL)};

	if(!search(options , "-name",LENGTH_OF(options))) {
		create_graph_usage(interp);
		return TCL_ERROR;
	}

	size = objc/2;
	for ( i = 0 ; i < size ; i++ ) {
		if(options[i]=="-name") {
			graph_name = values[i];
		}
		else if(options[i]=="-type") {
			graph_type = values[i];
		}
		else {
			create_graph_usage(interp);
			return TCL_ERROR;
		}
	}
	if (!search(options, "-type",LENGTH_OF(options))) {
		digana_graph_type =  diganaUndirectedGraphS;
	}
	else if(graph_type == "undirected") {
		digana_graph_type = diganaUndirectedGraphS;
	}
	else if(graph_type == "directed") {
		digana_graph_type = diganaDirectedGraphS;
	}
	else {
		create_graph_usage(interp);
		return TCL_ERROR;
	}
	diganaGraphObjectIdentifier graph_identifier ( Null_Identifier_ID , graph_name );
	diganaKernelMgr::diganaGetKernel().create_graph ( graph_identifier , digana_graph_type );

	return TCL_OK;	

}

int create_node_cmd(ClientData cd, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
		
	int i,size,returned_id,id;
	char buffer[100];
	string graph_name,node_name;

	if((objc==1) || (objc%2==0)) {
		create_node_usage(interp);
		return TCL_ERROR;
	}
	
	string options[] = {Tcl_GetStringFromObj(objv[1],NULL),Tcl_GetStringFromObj(objv[3],NULL)};
	string values[] = {Tcl_GetStringFromObj(objv[2],NULL),Tcl_GetStringFromObj(objv[4],NULL)};

	if(!search(options,"-graph",LENGTH_OF(options))) {
		create_node_usage(interp);
	}
	
	size = objc/2;
	for ( i = 0 ; i < size ; i++ ) {
		if(options[i]=="-name") {
			node_name = values[i];
		}
		else if(options[i]=="-graph") {
			graph_name = values[i];
		}
		else {
			create_node_usage(interp);
			return TCL_ERROR;
		}
	}
	if (!search(options, "-name",LENGTH_OF(options))) {
		node_name =  "def_node";
	}
	
	id = diganaGraphMgr::getGraphMgr().getId(graph_name);	
	diganaGraphObjectIdentifier graph_Id (id , graph_name);
	diganaGraphObjectIdentifier node_Id ( Null_Identifier_ID , node_name);
	returned_id = diganaKernelMgr::diganaGetKernel().add_vertex ( graph_Id , node_Id );
	sprintf(buffer,"Id provided to node is %d",returned_id);
	Tcl_SetObjResult(interp,Tcl_NewStringObj(buffer,-1));
	return TCL_OK;	

}

int create_edge_cmd(ClientData cd, Tcl_Interp * interp, int objc, Tcl_Obj * const objv[]) {
		
	int i,size,id;
	int source_node_id,sink_node_id;
	string graph_name;

	if((objc==1) || (objc%2==0)) {
		create_edge_usage(interp);
		return TCL_ERROR;
	}
	
	string options[] = {Tcl_GetStringFromObj(objv[1],NULL),Tcl_GetStringFromObj(objv[3],NULL)};
	string values[] = {Tcl_GetStringFromObj(objv[2],NULL),Tcl_GetStringFromObj(objv[4],NULL)};

	if (!search(options,"-graph",LENGTH_OF(options)) ||
	   !search(options,"-source_id",LENGTH_OF(options)) ||
	   !search(options,"-sink_id",LENGTH_OF(options))){

		create_edge_usage(interp);
		return TCL_ERROR;
	}
	
	size = objc/2;
	for ( i = 0 ; i < size ; i++ ) {
		if(options[i]=="-source_id") {
			source_node_id = atoi(values[i].c_str());
		}
		else if(options[i]=="-sink_id") {
			sink_node_id = atoi(values[i].c_str());
		}
		else if(options[i]=="-graph"){
			graph_name = values[i];
		}
		else {
			create_edge_usage(interp);
			return TCL_ERROR;
		}
	}
	
	id = diganaGraphMgr::getGraphMgr().getId(graph_name);	
	diganaGraphObjectIdentifier graph_Id (id , graph_name);
	diganaKernelMgr::diganaGetKernel().add_edge (graph_Id , source_node_id , sink_node_id);
	
	return TCL_OK;	

}

int AppInit(Tcl_Interp *interp) {
	if (Tcl_Init(interp) == TCL_ERROR) return TCL_ERROR;
	Tcl_SetVar(interp,"tcl_rcFileName","~/.wishrc",TCL_GLOBAL_ONLY);
	Tcl_SetVar(interp,"tcl_prompt1","echo -n meow@user>> ",TCL_GLOBAL_ONLY);
	Tcl_CreateObjCommand (interp, "create_graph", create_graph_cmd, NULL, NULL);
	Tcl_CreateObjCommand (interp, "create_node" , create_node_cmd, NULL, NULL);
	Tcl_CreateObjCommand (interp, "create_edge" , create_edge_cmd, NULL, NULL);

	return TCL_OK;
}

int main(int argc, char *argv[]) {
	Tcl_Main(argc, argv, AppInit);
	return 0;
}
