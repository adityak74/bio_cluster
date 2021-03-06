/***********************************************************
/
/   Implementation of network data structure
/
/   Sharlee Climer 
/   June 2007
/
/   Breadth-first search added October/November 2007
/   Print out component size summary (PRINTCOMPSIZES)
/    added January 2011
/
/**********************************************************/



#include "network.h"

using namespace std;


Edge::Edge(int endpt, double wt) // create a new edge with default weight of 1
{  
  // if (weight < 0-TOL) fatal("Negative edge weight");
  if (endpt < 0)  fatal("Invalid endpoint for edge");

  target = endpt; // assign endpoint
  weight = wt; // assign weight
}

int Edge::getTarget() // get other edgepoint  
{
  return target;
}
  
double Edge::getWeight() // get weight of edge 
{
  return weight;  
}

Vertex::Vertex() // create a new vertex
{
  //if (indxNum < 0)  fatal("Vertex indices must be non-negative");
  //index = indxNum; // assign index of node
  //ID = GML; // assign GML ID number
  //label = lbl; // assign GML label
  degree = 0; // no incident edges yet
  firstEdge.next = 0; // first edge is pointed to by firstEdge.next
}

Vertex::~Vertex() // destructor
{
  /* delete edges; 
  Edge *edgePtr, *followPtr; // pointers to move through linked list of edges
  edgePtr = &firstEdge; // point to first edge
  followPtr = edgePtr;  

  while (edgePtr->next != 0) {// follow until find last edge
    edgePtr = edgePtr->next; // pointer points at next edge in list
    delete followPtr;
    followPtr = edgePtr;
  }
  Need to fix!!!!!!!!!!!!!!!!!!!!!!!!
  */
}

//int Vertex::getID() // get GML ID number of vertex

//note: firstEdge is a placeholder that points to true first edge
int Vertex::addEdge(int endpt, double wt) // add an edge to vertex, return 1 if successful
{
  if (endpt < 0) fatal("invalid endpoint for edge");
  if (wt < 0-TOL) fatal("invalid weight for edge");

  Edge *edgePtr; // pointer to move through linked list of edges
  edgePtr = &firstEdge; // point to first edge

  while (edgePtr->next != 0) // follow until find last edge
    edgePtr = edgePtr->next; // pointer points at next edge in list

  Edge *newEdge; // newEdge created
  if((newEdge = new Edge) == NULL)
    fatal("memory not allocated for edge");

  edgePtr->next = newEdge; // add new edge to list
  newEdge->target = endpt; // assign properties to new edge
  newEdge->weight = wt;
  newEdge->next = 0; // mark new edge as last on list

  return 1;
}

double Vertex::getWeight(int endpoint) // return weight of edge
{
  Edge *edgePtr; // pointer to move through linked list of edges
  edgePtr = &firstEdge; // point to first edge

  for (int i = 0; i < endpoint; i++)
    edgePtr = edgePtr->next; // pointer points at next edge in list

  return edgePtr->weight;
}

void Vertex::changeWeight(int endpoint, double newWeight) // change weight of edge
{
  Edge *edgePtr; // pointer to move through linked list of edges
  edgePtr = &firstEdge; // point to first edge

  for (int i = 0; i < endpoint; i++)
	edgePtr = edgePtr->next; // pointer points at next edge in list

  edgePtr->weight = newWeight;
}

int Vertex::haveEdge(int endpoint) // return 1 if edge exists, 0 if not
{
  Edge *edgePtr; // pointer to move through linked list of edges
  edgePtr = &firstEdge; // point to first edge

  if (edgePtr->next == 0) // no edges for this vertex
    return 0;

  while (edgePtr->next != 0) { // follow until find last edge
    edgePtr = edgePtr->next; // pointer points at next edge in list
    if (edgePtr->target == endpoint)
      return 1;
  }

  return 0;
}

int Vertex::getDegree() // get out-degree (full degree for undirected)
{
  return degree;
}

//char* Vertex::getLabel() // get GML label of vertex (NULL if none specified)

Edge* Vertex::getEdges() // get copy of edges eminating from vertex
{
  Edge* edge1; // FIX
  return edge1;
}

void Vertex::printEdges(int node, char *outputFile) // print edges eminating from vertex
{
  Edge *edgePtr; // pointer to move through linked list of edges
  edgePtr = &firstEdge; // point to first edge

  if (edgePtr->next == 0) return; // no edges from this node

  FILE *output; 
  if ((output = fopen(outputFile, "a")) == NULL)
      fatal ("Unable to open output file");

  while (edgePtr->next != 0) { // follow until find last edge
    edgePtr = edgePtr->next; // pointer points at next edge in list

    if(!RETAINSYMMETRIC || (node < edgePtr->target)) // print only once
      fprintf(output,"\n\tedge\n\t[\n\tsource %d\n\ttarget %d\n\t]", node+1, edgePtr->target+1);
  }

  fclose(output);
}

Network::Network(int num, int dir) // create a network with num vertices
{
  if (num < 1)  fatal("Network requires at least 1 vertex");
  numVertices = num;
  directed = dir; // 0 if undirected, 1 if directed network
  if((vertices = new Vertex[numVertices]) == NULL)
    fatal("memory not allocated"); // allocate memory

  numEdges = 0; // no edges added yet
}


Network::~Network() // destructor
{
  delete [] vertices; 
}

int Network::getNvertices() // get number of vertices in network
{
  return numVertices;
}

int Network::getNumEdges() // get number of vertices in network
{
  return numEdges;
}

int Network::getDegree(int vertex) // get degree of node
{
  int degree = vertices[vertex].getDegree();
  return degree;
}

int Network::isDirected() // return 0 if undirected, 1 if directed network
{
  return directed;
}

int Network::addEdge(int v1, int v2, double weight) // return 1 if successfully add edge
{
  if ((v1 > numVertices-1) || (v2 > numVertices-1))
    fatal("Attempt to add edge to non-existent node");
  if ((v1 < 0) || (v2 < 0))
    fatal("Attempt to add edge to negative numbered node");

  if (!DIRECTED) {
    if (v1 > v2) { // order v1 and v2
      int temp = v1;
      v1 = v2;
      v2 = temp;
    }
  }

  //std::cout << "v1 = " << v1 << ", v2 = " << v2 << std::endl;

  if(vertices[v1].haveEdge(v2)) { // edge already exist
	//	if (weight > vertices[v1].getWeight(v2))
	//  vertices[v1].changeWeight(v2, weight); // use highest weight for edge
    return 0; // signal no new edge added
  }

  vertices[v1].addEdge(v2, weight); // add edge

  if(RETAINSYMMETRIC) { // retain both (i,j) and (j,i)
    if(vertices[v2].haveEdge(v1)) // edge already exists, return failure
      return 0;
    vertices[v2].addEdge(v1, weight);
  }

  numEdges++; // update number of edges
  vertices[v1].degree++; // update degree of vertices
  if (!DIRECTED)
    vertices[v2].degree++; // increase degree for both vertices if undirected

  return 1;
}

void Network::printEdges(char *outputFile) // print all edges in network
{
  FILE *output;
  if ((output = fopen(outputFile, "w")) == NULL)
      fatal ("Unable to open output file");

  fprintf(output,"Graph with %d nodes and %d edges.\ngraph\n[", numVertices, numEdges);  

  for (int i = 0; i < numVertices; i++)
    fprintf(output,"\n  node\n  [\n    id %d\n  ]", i+1);

  fclose(output);

  for (int i = 0; i < numVertices; i++)
    vertices[i].printEdges(i, outputFile);

  if ((output = fopen(outputFile, "a")) == NULL)
      fatal ("Unable to open output file");

  fprintf(output,"\n]\n");  
  fclose(output);
}

double Network::getEdgeWeight(int v1, int v2) // get weight of edge between endpoints
{
  if ((v1 > numVertices-1) || (v2 > numVertices-1)) 
    fatal("Attempt to get weight of edge to non-existent node");
  if ((v1 < 0) || (v2 < 0))
    fatal("Attempt to get weight of edge to negative numbered node");
  
  if (!DIRECTED) {
    if (v1 > v2) { // order v1 and v2
      int temp = v1;
      v1 = v2;
      v2 = temp;
    }
  }

  if(!vertices[v1].haveEdge(v2)) // edge is not in graph
	fatal("Attempt to get weight of edge that is not in graph");

  double wt = vertices[v1].getWeight(v2); // get weight
  if (wt < 0-TOL)
	fatal ("Negative edge weight");

  return wt;
}

int Network::haveEdge(int v1, int v2) // return 1 if edge is in graph
{
  if ((v1 > numVertices-1) || (v2 > numVertices-1)) 
    fatal("Attempt to check edge to non-existent node");
  if ((v1 < 0) || (v2 < 0))
    fatal("Attempt to check edge to negative numbered node");
  
  if (!DIRECTED) {
    if (v1 > v2) { // order v1 and v2
      int temp = v1;
      v1 = v2;
      v2 = temp;
    }
  }

  if(vertices[v1].haveEdge(v2)) // edge is in graph
    return 1;
  else 
    return 0;
}

// Vertex::getOriginalDegree() returns int
// get out-degree (full degree for undirected)
int Vertex::getOriginalDegree()
{
  return original_degree;
}

// Network::getOriginalDegree() returns int
// get degree of node
int Network::getOriginalDegree(int vertex)
{
  int degree = vertices[vertex].getOriginalDegree();
  return degree;
}

void Network::setGlobalNetworkGE()
{
  originalNumEdges = numEdges;
  originalNumVertices = numVertices;

  // assign the degree of the original_degree of every node
  // this will be used for calculating modularity
  for (int i = 0; i < numVertices; i++)
  {
    vertices[i].original_degree = vertices[i].degree;
    if (0)
      std ::cout << "====Vertex #" << i + 1 << ", degree : " << vertices[i].degree << "\n";
  }
}

int Vertex::removeEdge(int end)
{
  Edge *edgePtr, *prevEdgePtr;
  int edgeFound = 0;

  // since this is initiliazed in stack no pointer reference.
  edgePtr = &firstEdge;  // fix for first and last element deletion and decerement the degrees
  prevEdgePtr = edgePtr; // prevEdge points to the first Edge for initial setup

  if (edgePtr->next == NULL) // no edges for this vertex
    return 0;

  while (edgePtr->next != NULL)
  { // follow until find last edge
    prevEdgePtr = edgePtr;
    edgePtr = edgePtr->next; // pointer points at next edge in list

    // std::cout << "DEBUG | at here :->" << edgePtr->target << " |" <<std::endl;
    if (edgePtr->target == end)
    {
      edgeFound = 1;
      break;
    }
  }
  if (edgeFound)
  {
    // std::cout << "DEBUG | edge found :->" << edgePtr->target << " |" <<std::endl;
    prevEdgePtr->next = edgePtr->next;
    if (edgePtr != NULL)
      delete edgePtr;
    return 1;
  }
  else
  {
    fatal("No such Edge in the List");
    return 0;
  }
}

// Network::removeEdge() returns int
// removes edge from v1 to v2, if success returns 1 else 0
int Network::removeEdge(int v1, int v2)
{

  int start = (v1 < v2) ? v1 : v2;
  int end = (v1 < v2) ? v2 : v1;

  if ((start > numVertices - 1) || (end > numVertices - 1))
    fatal("Attempt to add edge to non-existent node");
  if ((start < 0) || (end < 0))
    fatal("Attempt to add edge to negative numbered node");

  if (!vertices[v1].haveEdge(v2))
  {
    return 0; // no such edge in the list
  }

  int ret = vertices[start].removeEdge(end);
  if (RETAINSYMMETRIC)
  {
    if (vertices[end].haveEdge(start))
    {
      // std::cout << "--- Removing Symmetric Edge : (" << end << "," << start << ")" << std::endl;
      vertices[end].removeEdge(start);
    }
  }

  if (ret)
  {
    vertices[start].degree--; // update degree of vertices
    numEdges--;               // update number of edges
    if (!DIRECTED)
      vertices[end].degree--; // increase degree for both vertices if undirected
    return 1;
  }
}

// breadth-first search, outputs connected components
void Network::bfs(char *outputFile) 
{
  FILE *output;

  int testFile = system("test ! -f comp1.gml");
  if(testFile != 0)
	fatal("Component files already exist in this directory");

  if ((output = fopen(outputFile, "w")) == NULL)
      fatal ("Unable to open output file");

  int ptr; // point to current index of component array
  int startPtr, endPtr; // pointers for queue
  int *visited, *component, *queue, *clusterNum;
  int numSingle = 0; // count nodes with degree zero
  int numTwo = 0; // count number of components with 2 vertices
  int k = 0; // current cluster number
  int totalEdges = 0; // total edges in graph
  int numComp3orMore = 0; // number of components with 3 or more vertices
  FILE *outputGML; // output components to .gml files
  FILE *outputWG2; // output components to .wg2 files
  FILE *outputCompnn; // output node numbering for components
  int randomVal; // random value to place at end of comp file names
  int numNonSingle = 0; // number of non-singleton components
  int printNum = 0; // number of cluster printed out
  int maxSizeComp = 0; // max size of component printed to gml file
  int maxCompAll = 0; // max size of component overall
  int num[10000]; // hold number of components with each size
  int numNotCliques = 0; // hold number of components that aren't cliques
  
  for (int i = 0; i < 10000; i++)
	num[i] = 0;

  int compSize[MAX_NUM_COMPS]; // hold non-singleton component sizes
  double compDensity[MAX_NUM_COMPS]; // hold non-singleton component densities
  int compPtr = 0; // pointer for these two arrays

  if(BFS_WG2 && !RETAINSYMMETRIC) 
    fatal("Need to retain symmetric edges for .wg2 output");

  srand(time(NULL)); // assign random value between 1 - 1000
  randomVal = rand() % 1000 + 1;

  if(((visited = new int[numVertices]) == NULL)||((component = new int[numVertices]) == NULL))
    fatal("memory not allocated"); // allocate memory
  if(((queue = new int[numVertices]) == NULL)||((clusterNum = new int[numVertices]) == NULL))
    fatal("memory not allocated"); // allocate memory
 
  for (int i = 0; i < numVertices; i++) 
    visited[i] = 0; // initialize values to not visited

  for (int i = 0; i < numVertices; i++) {
    int degree = getDegree(i);

    if (degree == 0) {
      numSingle++;
      clusterNum[i] = -1; // designate node as singleton
      continue;
    }

    if (visited[i] == 0) { // unvisited vertex
      visited[i] = 1;
      numNonSingle++; // another nonsingleton cluster
      ptr = 0; // start a new component
      startPtr = 0; // start a new queue
      queue[0] = i; // first vertex in queue
      endPtr = 1;
	  
      while (startPtr != endPtr) { // while queue is not empty
		int node = queue[startPtr++]; // remove first vertex on queue
		component[ptr++] = node; // this vertex is in component
		visited[node] = 1; // mark as visited
		
		Edge *edgePtr; // pointer to move through linked list of edges
		edgePtr = &(vertices[node].firstEdge); // point to first edge
		
		while (edgePtr->next != 0) { // follow until find last edge
		  edgePtr = edgePtr->next; // pointer points at next edge in list
		  int endpt = edgePtr->target; // find adjacent vertex 
		  
		  if (!visited[endpt]) {
			visited[endpt] = 1; 
			queue[endPtr++] = endpt; // add to queue 
		  }
		}
		
      }
	  
      long int numberEdges = 0; // count edges in component
      for (int j = 0; j < ptr; j++)
		numberEdges += getDegree(component[j]);
      numberEdges /= 2; // count (i,j) and (j,i) only once
      //totalEdges += numberEdges; // total edges in graph
	  
      long double complete = (double)numberEdges; // compute completeness of cluster
      complete /= (long double)ptr * (long double)(ptr-1);
	  complete *= (double)(2.0); 
	  if (complete < 0 - TOL)
		fatal("Negative density of component computed");
	  
      for (int j = 0; j < ptr; j++) // assign cluster number
		clusterNum[component[j]] = k;
      k++; // move to next cluster number
	  
	  if (compPtr == MAX_NUM_COMPS)
		fatal("MAX_NUM_COMPS exceeded.  Change to larger value in network.h");

	  compSize[compPtr] = ptr; // record component size
	  compDensity[compPtr++] = complete; // record density

      if (ptr == 2) // only 2 vertices in component
		numTwo++;
	  
      else {
		numComp3orMore++;  // one more with 3 or more vertices

		if(complete < 0.99999) // not a clique
		  numNotCliques++;

		for (int k = 0; k < 10000; k++)
		  if (k == ptr) {
			num[k]++;
			break;
		  }
		
		if (1) {
		  std::cout << numNonSingle << ": " << ptr << " vertices, " << numberEdges << " edges (";
		  std::cout << complete << " complete)" << std::endl;
		}
		
		if (0) 
		  std::cout << ptr << "  " << complete << std::endl;
		
		if (DESCRIPTIVE_OUTPUT) {
		  fprintf(output, "%d vertices, %d edges (%.4f complete)\n", ptr, numberEdges, complete); 
		  for (int j = 0; j < ptr; j++) {
			std::cout << component[j] << " ";
			fprintf(output, "%d ", component[j]);
			if ((j+1) % 10 == 0) // 10 vertices per line
			  fprintf(output, "\n"); 
		  }
		  
		  std::cout << std::endl;
		  fprintf(output, "\n");
		}
      }
	  
      if (ptr > maxCompAll)
	maxCompAll = ptr; // keep track of largest component overall

      if ((ptr>2) && (BFS_GML || BFS_WG2)) // print out nontrivial components
		if (complete < MIN_COMPLETE+TOL) { // print only if not too complete
		  const char base[] = "comp"; // make up new filename for current comp.
	  const char suffix1[] = ".gml"; 
	  const char suffix2[] = ".nn";
	  const char suffix3[] = ".wg2";
	  char filename [50]; // .gml file
	  char filenn [50];  // node numbering file
	  char fileWG2 [50]; //.wg2 file
	  printNum++; // increase to next cluster printed
	  //sprintf(filename, "%s%d_%d%s", base, randomVal, printNum, suffix1);
	  //sprintf(filenn, "%s%d_%d%s", base, randomVal, printNum, suffix2);
	  //sprintf(fileWG2, "%s%d_%d%s", base, randomVal, printNum, suffix3);

	  sprintf(filename, "%s%d%s", base, printNum, suffix1);
	  sprintf(filenn, "%s%d%s", base, printNum, suffix2);
	  sprintf(fileWG2, "%s%d%s", base, printNum, suffix3);
	  
	  if(BFS_GML)
	    if ((outputGML = fopen(filename, "w")) == NULL)
	      fatal("Component output file could not be opened");
	  
	  if ((outputCompnn = fopen(filenn, "w")) == NULL)
	    fatal("Component node numbering file could not be opened");

	  if(BFS_WG2)
	    if ((outputWG2 = fopen(fileWG2, "w")) == NULL)
	      fatal("Component output file could not be opened");
	  
	  fprintf(outputCompnn, "%d nodes\n",ptr);

	  for (int j = 0 ; j < ptr; j++) // output node numbering 
	    fprintf(outputCompnn, "%d ", component[j]);
	  fprintf(outputCompnn, "\n");
	  fclose(outputCompnn);
	  
	  if(BFS_GML) {
	    fprintf(outputGML,"Graph with %d nodes and %d edges.\ngraph\n[", ptr, numberEdges);  
	    
	    for (int j = 0; j < ptr; j++)
	      fprintf(outputGML,"\n  node\n  [\n    id %d\n  ]", j+1);
	    fclose(outputGML);
	    
	    if ((outputGML = fopen(filename, "a")) == NULL)
	      fatal ("Unable to open component output file");
	  }

	  if (ptr > maxSizeComp)
	    maxSizeComp = ptr; // keep track of largest component printed

	  int *nn; // array to hold node numbering
	  if((nn = new int[numVertices]) == NULL)
	    fatal("memory not allocated"); // allocate memory
	  
	  for (int j = 0; j < ptr; j++)
	    nn[j] = -1; // initialize
	  for (int j = 0; j < ptr; j++)
	    nn[component[j]] = j; // insert new node numbers
	  
	  Edge *edgePtr; // pointer to move through linked list of edges
	  
	  for (int j = 0; j < ptr; j++) { // find edges in component
	    int node = component[j];
	    edgePtr = &(vertices[node].firstEdge); // point first edge
	    
	    if (nn[node] < 0) fatal("Node numbering array error");
	    if(BFS_WG2)
	      fprintf(outputWG2,"%d %d, X %d [ ",j, j, vertices[node].degree);  	    

	    while (edgePtr->next != 0) { // follow until find last edge
	      edgePtr = edgePtr->next; // pointer points at next edge in list
	      if (nn[edgePtr->target] < 0) fatal("node numbering array error");
	      
	      if (BFS_GML)
		if(!RETAINSYMMETRIC || (node < edgePtr->target)) // print once
		  fprintf(outputGML,"\n\tedge\n\t[\n\tsource %d\n\ttarget %d\n\tweight %f\n\t]", nn[node]+1, nn[edgePtr->target]+1, edgePtr->weight);

	      if(BFS_WG2)
		fprintf(outputWG2,"(%d, 1.0) ",nn[edgePtr->target]);
	    }

	    if(BFS_WG2)
	      fprintf(outputWG2,"] \n");
	  }
	  
	  //for (int j = 0; j < ptr; j++)
	  //vertices[component[j]].printEdges(component[j], filename);
	  
	  if (BFS_GML) {
	    fprintf(outputGML,"\n]\n");  
	    fclose(outputGML);
	  }

	  if(BFS_WG2)
	    fclose(outputWG2);
	}
    }
  }
  
    //for (int j = 0; j < numVertices; j++)
    //std::cout << visited[j] << std::endl;

  //if (totalEdges != numEdges) fatal("Error in number of edges found in bfs");

  if (!DESCRIPTIVE_OUTPUT) {
    fprintf(output, "%d nodes %d clusters %d edges\n", numVertices, k, numEdges);
    for (int i = 0; i < numVertices; i++)
      fprintf(output, "%d ", clusterNum[i]);
    fprintf(output, "\n\n");
  }
  
  std::cout << std::endl;
  std::cout << numSingle << " vertices with degree zero" << std::endl;
  std::cout << numTwo << " components with only 2 vertices" << std::endl;
  std::cout << numComp3orMore << " components with 3 or more vertices" << std::endl;

  fprintf(output, "%d singletons, %d components with only 2 vertices,\n%d components with 3 or more vertices\n", numSingle, numTwo, numComp3orMore);

  // compute statistics about nonsingleton component sizes and densities
  float avgdata = 0.0; // first compute for component sizes
  float sdata = 0.0;
  float confUpData, confLowData;
  float tempData;
  float median;
  int size = compPtr;
 
  if (size > 1) {
    for (int i = 0; i < size; i++)   // sort data
      for (int j = size-1; j > i; j--)
		if (compSize[j] < compSize[j-1]) {
		  tempData = compSize[j];
		  compSize[j] = compSize[j-1];
		  compSize[j-1] = (int)tempData;
		}
    
    if ((size % 2) == 0)  // an even number of data
      median = 0.5 * (compSize[size/2] + compSize[size/2 - 1]);
    
    else        // odd number of data
      median = compSize[size/2];
	
    for (int i = 0; i < size; i++)     // find mean of data
      avgdata += compSize[i];
    avgdata /= (float)size;
	
    for (int i = 0; i < size; i++)  // find 95% confidence interval
      sdata += (compSize[i] - avgdata) * (compSize[i] - avgdata);
    sdata /= (float)((size - 1) * size);
    sdata = sqrt(sdata);
  }
  
  else {
    median = avgdata = compSize[0];
    sdata = 0;
  }
  
  confUpData = avgdata + (2.0 * sdata);
  confLowData = avgdata - (2.0 * sdata);
  
  std::cout << "\nSizes of non-singleton components:" << std::endl;
  std::cout << "\tmax = " << compSize[size-1];
  std::cout << ", min = " << compSize[0];
  std::cout << ", median = " << median;
  std::cout << ", average = " << avgdata << std::endl;
  std::cout << "\t95% confidence interval: " << confUpData << ", " << confLowData << std::endl;

  float meanSize = avgdata; // save in case PRINTCOMPSIZES
  
  avgdata = 0.0;  // repeat for component densities
  sdata = 0.0;
  if (size > 1) {
    for (int i = 0; i < size; i++)   // sort data
      for (int j = size-1; j > i; j--)
		if (compDensity[j] < compDensity[j-1]) {
		  tempData = compDensity[j];
		  compDensity[j] = compDensity[j-1];
		  compDensity[j-1] = tempData;
		}
    
    if ((size % 2) == 0)  // an even number of data
      median = 0.5 * (compDensity[size/2] + compDensity[size/2 - 1]);
    
    else        // odd number of data
      median = compDensity[size/2];
	
    for (int i = 0; i < size; i++)     // find mean of data
      avgdata += compDensity[i];
    avgdata /= (float)size;
	
    for (int i = 0; i < size; i++)  // find 95% confidence interval
      sdata += (compDensity[i] - avgdata) * (compDensity[i] - avgdata);
    sdata /= (float)((size - 1) * size);
    sdata = sqrt(sdata);
  }
  
  else {
    median = avgdata = compDensity[0];
    sdata = 0;
  }
  
  confUpData = avgdata + (2.0 * sdata);
  confLowData = avgdata - (2.0 * sdata);
  
  std::cout << "\nDensities of non-singleton components:" << std::endl;
  std::cout << "\tmax = " << compDensity[size-1];
  std::cout << ", min = " << compDensity[0];
  std::cout << ", median = " << median;
  std::cout << ", average = " << avgdata << std::endl;
  std::cout << "\t95% confidence interval: " << confUpData << ", " << confLowData << std::endl;
  std::cout << std::endl;

  if(1) {
	num[1] = numSingle;
	num[2] = numTwo;
	for (int i = 1; i < 10000; i++)
	  if (num[i] > 0)
		//std::cout << num[i] << std::endl;
	    std::cout << "\t" << num[i] << " components with " << i << " vertices" << std::endl; 
    //std::cout << num3 << " " << num4 << " " << num5 << " " << num6 << " " << num7;
    //std::cout << " " << num8 << " " << num9 << " " << num10 << std::endl;

	if (0)
	  for (int i = 1; i < 64; i++)
		std::cout << num[i] << std::endl;
  }

  if(BFS_GML || BFS_WG2) {
    fprintf(output, "%d vertices in largest component printed to separate file\n",maxSizeComp);
    std::cout << "\n" << maxSizeComp << " vertices in largest component printed to separate file" << std::endl;
  }

  fprintf(output, "%d vertices in largest component overall\n",maxCompAll);
  std::cout << maxCompAll << " vertices in largest component overall" << std::endl;

  fprintf(output, "%d components are not cliques\n",numNotCliques);
  std::cout << numNotCliques << " components are not cliques" << std::endl;
  
  fclose(output);

  if(PRINTCOMPSIZES) { // print out summary of component sizes to "saveCompSizes.txt"
    FILE *compsize;

    // check if new file, if so print out header
    if ((compsize = fopen("saveCompSizes.txt", "r")) == NULL) { 
      if ((compsize = fopen("saveCompSizes.txt", "w")) == NULL)
	fatal("'saveCompSizes.txt' file could not be opened.\n");

      fprintf(compsize,"NumSingle NumDouble Num3orMore NumNotCliques MaxSize AvgSize\n"); 
      fclose(compsize);
    }

  if ((compsize = fopen("saveCompSizes.txt", "a")) == NULL)
    fatal("'saveCompSizes.txt' file could not be opened.\n");

  fprintf(compsize,"%d %d %d %d %d %.2f\n",numSingle, numTwo, numComp3orMore, numNotCliques, maxCompAll, meanSize);
  fclose(compsize);
  }

  delete [] visited;
  delete [] component;
  delete [] queue;
  delete [] clusterNum;
}

// breadth-first search and Q_Value calculation, outputs connected components
// and respected Q_Value of communities and total q_value
double Network::modularity(char *outputFile)
{
  double modularity_value = 0.0f;
  FILE *output;

  int testFile = system("test ! -f comp1.gml");
  if (testFile != 0)
    fatal("Component files already exist in this directory");

  if ((output = fopen(outputFile, "w")) == NULL)
    fatal("Unable to open output file");

  int ptr;              // point to current index of component array
  int startPtr, endPtr; // pointers for queue
  int *visited, *component, *queue, *clusterNum;
  int numSingle = 0; // count nodes with degree zero
  int numTwo = 0;    // count number of components with 2 vertices
  int k = 0;         // current cluster number
  // long int totalEdges = 0; // total edges in graph
  int numComp3orMore = 0; // number of components with 3 or more vertices
  FILE *outputGML;        // output components to .gml files
  FILE *outputWG2;        // output components to .wg2 files
  FILE *outputCompnn;     // output node numbering for components
  int randomVal;          // random value to place at end of comp file names
  int numNonSingle = 0;   // number of non-singleton components
  int printNum = 0;       // number of cluster printed out
  int maxSizeComp = 0;    // max size of component printed to gml file
  int maxCompAll = 0;     // max size of component overall
  int num[10000];         // hold number of components with each size
  int numNotCliques = 0;  // hold number of components that aren't cliques

  for (int i = 0; i < 10000; i++)
    num[i] = 0;

  int compSize[MAX_NUM_COMPS];       // hold non-singleton component sizes
  double compDensity[MAX_NUM_COMPS]; // hold non-singleton component densities
  int compPtr = 0;                   // pointer for these two arrays

  if (BFS_WG2 && !RETAINSYMMETRIC)
    fatal("Need to retain symmetric edges for .wg2 output");

  // srand(time(NULL)); // assign random value between 1 - 1000
  randomVal = rand() % 1000 + 1;

  if (((visited = new int[numVertices]) == NULL) || ((component = new int[numVertices]) == NULL))
    fatal("memory not allocated"); // allocate memory
  if (((queue = new int[numVertices]) == NULL) || ((clusterNum = new int[numVertices]) == NULL))
    fatal("memory not allocated"); // allocate memory

  for (int i = 0; i < numVertices; i++)
    visited[i] = 0; // initialize values to not visited

  for (int i = 0; i < numVertices; i++)
  {
    int degree = getDegree(i);

    if (degree == 0)
    {
      numSingle++;
      clusterNum[i] = -1; // designate node as singleton
      continue;
    }

    if (visited[i] == 0)
    { // unvisited vertex
      visited[i] = 1;
      numNonSingle++; // another nonsingleton cluster
      ptr = 0;        // start a new component
      startPtr = 0;   // start a new queue
      queue[0] = i;   // first vertex in queue
      endPtr = 1;

      while (startPtr != endPtr)
      {                               // while queue is not empty
        int node = queue[startPtr++]; // remove first vertex on queue
        component[ptr++] = node;      // this vertex is in component
        visited[node] = 1;            // mark as visited

        Edge *edgePtr;                         // pointer to move through linked list of edges
        edgePtr = &(vertices[node].firstEdge); // point to first edge

        while (edgePtr->next != NULL)
        {                              // follow until find last edge
          edgePtr = edgePtr->next;     // pointer points at next edge in list
          int endpt = edgePtr->target; // find adjacent vertex

          if (!visited[endpt])
          {
            visited[endpt] = 1;
            queue[endPtr++] = endpt; // add to queue
          }
        }
      }

      long int numberEdges = 0; // count edges in component
      for (int j = 0; j < ptr; j++)
        numberEdges += getDegree(component[j]);
      numberEdges /= 2; // count (i,j) and (j,i) only once
      // totalEdges += numberEdges; // total edges in graph

      long double complete = (double)numberEdges; // compute completeness of cluster
      complete /= (long double)ptr * (long double)(ptr - 1);
      complete *= (double)(2.0);
      if (complete < 0 - TOL)
        fatal("Negative density of component computed");

      for (int j = 0; j < ptr; j++) // assign cluster number
        clusterNum[component[j]] = k;
      k++; // move to next cluster number

      if (compPtr == MAX_NUM_COMPS)
        fatal("MAX_NUM_COMPS exceeded.  Change to larger value in network.h");

      compSize[compPtr] = ptr;           // record component size
      compDensity[compPtr++] = complete; // record density

      double sum_degree = 0.0f;
      for (int j = 0; j < ptr; j++)
        sum_degree += getOriginalDegree(component[j]);

      float first = (float)numberEdges / originalNumEdges;
      float second = pow((float)sum_degree / (2 * originalNumEdges), 2);

      if (0)
      {
        std ::cout << "--------------" << numberEdges << "," << originalNumEdges << "," << ptr << "," << originalNumVertices << std ::endl;
        for (int j = 0; j < ptr; j++) // loop through the components in cluster
          std ::cout << component[j] << ",";
        std ::cout << "-=-" << sum_degree;
        std ::cout << "\nci = " << first << " , ri = " << second << std ::endl;
        std ::cout << std ::endl;
      }

      modularity_value += first - second;

      if (1)
      {
        std::cout << numNonSingle << ": " << ptr << " vertices, " << numberEdges << " edges (";
        std::cout << complete << " complete), modularity_value : " << first - second << std::endl;
      }

      if (ptr == 2)
      { // only 2 vertices in component
        numTwo++;
      }
      else
      {
        numComp3orMore++; // one more with 3 or more vertices

        if (complete < 0.99999) // not a clique
          numNotCliques++;

        for (int k = 0; k < 10000; k++)
          if (k == ptr)
          {
            num[k]++;
            break;
          }

        // At this point we have the vertices and edges in a community.
        // Calcluating the q_value her

        if (0)
          std::cout << ptr << "  " << complete << std::endl;

        if (DESCRIPTIVE_OUTPUT)
        {
          fprintf(output, "%d vertices, %d edges (%.4f complete)\n", ptr, numberEdges, complete);
          for (int j = 0; j < ptr; j++)
          {
            std::cout << component[j] << " ";
            fprintf(output, "%d ", component[j]);
            if ((j + 1) % 10 == 0) // 10 vertices per line
              fprintf(output, "\n");
          }

          std::cout << std::endl;
          fprintf(output, "\n");
        }
      }

      if (ptr > maxCompAll)
        maxCompAll = ptr; // keep track of largest component overall

      if ((ptr > 2) && (BFS_GML || BFS_WG2)) // print out nontrivial components
        if (complete < MIN_COMPLETE + TOL)
        {                             // print only if not too complete
          const char base[] = "comp"; // make up new filename for current comp.
          const char suffix1[] = ".gml";
          const char suffix2[] = ".nn";
          const char suffix3[] = ".wg2";
          char filename[50]; // .gml file
          char filenn[50];   // node numbering file
          char fileWG2[50];  //.wg2 file
          printNum++;        // increase to next cluster printed
          //sprintf(filename, "%s%d_%d%s", base, randomVal, printNum, suffix1);
          //sprintf(filenn, "%s%d_%d%s", base, randomVal, printNum, suffix2);
          //sprintf(fileWG2, "%s%d_%d%s", base, randomVal, printNum, suffix3);

          sprintf(filename, "%s%d%s", base, printNum, suffix1);
          sprintf(filenn, "%s%d%s", base, printNum, suffix2);
          sprintf(fileWG2, "%s%d%s", base, printNum, suffix3);

          if (BFS_GML)
            if ((outputGML = fopen(filename, "w")) == NULL)
              fatal("Component output file could not be opened");

          if ((outputCompnn = fopen(filenn, "w")) == NULL)
            fatal("Component node numbering file could not be opened");

          if (BFS_WG2)
            if ((outputWG2 = fopen(fileWG2, "w")) == NULL)
              fatal("Component output file could not be opened");

          fprintf(outputCompnn, "%d nodes\n", ptr);

          for (int j = 0; j < ptr; j++) // output node numbering
            fprintf(outputCompnn, "%d ", component[j]);
          fprintf(outputCompnn, "\n");
          fclose(outputCompnn);

          if (BFS_GML)
          {
            fprintf(outputGML, "Graph with %d nodes and %d edges.\ngraph\n[", ptr, numberEdges);

            for (int j = 0; j < ptr; j++)
              fprintf(outputGML, "\n  node\n  [\n    id %d\n  ]", j + 1);
            fclose(outputGML);

            if ((outputGML = fopen(filename, "a")) == NULL)
              fatal("Unable to open component output file");
          }

          if (ptr > maxSizeComp)
            maxSizeComp = ptr; // keep track of largest component printed

          int *nn; // array to hold node numbering
          if ((nn = new int[numVertices]) == NULL)
            fatal("memory not allocated"); // allocate memory

          for (int j = 0; j < ptr; j++)
            nn[j] = -1; // initialize
          for (int j = 0; j < ptr; j++)
            nn[component[j]] = j; // insert new node numbers

          Edge *edgePtr; // pointer to move through linked list of edges

          for (int j = 0; j < ptr; j++)
          { // find edges in component
            int node = component[j];
            edgePtr = &(vertices[node].firstEdge); // point first edge

            if (nn[node] < 0)
              fatal("Node numbering array error");
            if (BFS_WG2)
              fprintf(outputWG2, "%d %d, X %d [ ", j, j, vertices[node].degree);

            while (edgePtr->next != NULL)
            {                          // follow until find last edge
              edgePtr = edgePtr->next; // pointer points at next edge in list
              if (nn[edgePtr->target] < 0)
                fatal("node numbering array error");

              if (BFS_GML)
                if (!RETAINSYMMETRIC || (node < edgePtr->target)) // print once
                  fprintf(outputGML, "\n\tedge\n\t[\n\tsource %d\n\ttarget %d\n\tweight %f\n\t]", nn[node] + 1, nn[edgePtr->target] + 1, edgePtr->weight);

              if (BFS_WG2)
                fprintf(outputWG2, "(%d, 1.0) ", nn[edgePtr->target]);
            }

            if (BFS_WG2)
              fprintf(outputWG2, "] \n");
          }

          //for (int j = 0; j < ptr; j++)
          //vertices[component[j]].printEdges(component[j], filename);

          if (BFS_GML)
          {
            fprintf(outputGML, "\n]\n");
            fclose(outputGML);
          }

          if (BFS_WG2)
            fclose(outputWG2);
        }
    }
  }

  //for (int j = 0; j < numVertices; j++)
  //std::cout << visited[j] << std::endl;

  //if (totalEdges != numEdges) fatal("Error in number of edges found in bfs");

  if (!DESCRIPTIVE_OUTPUT)
  {
    fprintf(output, "%d nodes %d clusters %d edges\n", numVertices, k, numEdges);
    for (int i = 0; i < numVertices; i++)
      fprintf(output, "%d ", clusterNum[i]);
    fprintf(output, "\n\n");
  }

  std::cout << std::endl;
  std::cout << numSingle << " vertices with degree zero" << std::endl;
  std::cout << numTwo << " components with only 2 vertices" << std::endl;
  std::cout << numComp3orMore << " components with 3 or more vertices" << std::endl;

  std::cout << "Total Modularity value for the given network : " << modularity_value << std::endl;
  std::cout << "Total communities in the network : " << k << std::endl;

  fprintf(output, "%d singletons, %d components with only 2 vertices,\n%d components with 3 or more vertices\n", numSingle, numTwo, numComp3orMore);

  // compute statistics about nonsingleton component sizes and densities
  float avgdata = 0.0; // first compute for component sizes
  float sdata = 0.0;
  float confUpData, confLowData;
  float tempData;
  float median;
  int size = compPtr;

  if (size > 1)
  {
    for (int i = 0; i < size; i++) // sort data
      for (int j = size - 1; j > i; j--)
        if (compSize[j] < compSize[j - 1])
        {
          tempData = compSize[j];
          compSize[j] = compSize[j - 1];
          compSize[j - 1] = (int)tempData;
        }

    if ((size % 2) == 0) // an even number of data
      median = 0.5 * (compSize[size / 2] + compSize[size / 2 - 1]);

    else // odd number of data
      median = compSize[size / 2];

    for (int i = 0; i < size; i++) // find mean of data
      avgdata += compSize[i];
    avgdata /= (float)size;

    for (int i = 0; i < size; i++) // find 95% confidence interval
      sdata += (compSize[i] - avgdata) * (compSize[i] - avgdata);
    sdata /= (float)((size - 1) * size);
    sdata = sqrt(sdata);
  }

  else
  {
    median = avgdata = compSize[0];
    sdata = 0;
  }

  confUpData = avgdata + (2.0 * sdata);
  confLowData = avgdata - (2.0 * sdata);

  std::cout << "\nSizes of non-singleton components:" << std::endl;
  std::cout << "\tmax = " << compSize[size - 1];
  std::cout << ", min = " << compSize[0];
  std::cout << ", median = " << median;
  std::cout << ", average = " << avgdata << std::endl;
  std::cout << "\t95% confidence interval: " << confUpData << ", " << confLowData << std::endl;

  float meanSize = avgdata; // save in case PRINTCOMPSIZES

  avgdata = 0.0; // repeat for component densities
  sdata = 0.0;
  if (size > 1)
  {
    for (int i = 0; i < size; i++) // sort data
      for (int j = size - 1; j > i; j--)
        if (compDensity[j] < compDensity[j - 1])
        {
          tempData = compDensity[j];
          compDensity[j] = compDensity[j - 1];
          compDensity[j - 1] = tempData;
        }

    if ((size % 2) == 0) // an even number of data
      median = 0.5 * (compDensity[size / 2] + compDensity[size / 2 - 1]);

    else // odd number of data
      median = compDensity[size / 2];

    for (int i = 0; i < size; i++) // find mean of data
      avgdata += compDensity[i];
    avgdata /= (float)size;

    for (int i = 0; i < size; i++) // find 95% confidence interval
      sdata += (compDensity[i] - avgdata) * (compDensity[i] - avgdata);
    sdata /= (float)((size - 1) * size);
    sdata = sqrt(sdata);
  }

  else
  {
    median = avgdata = compDensity[0];
    sdata = 0;
  }

  confUpData = avgdata + (2.0 * sdata);
  confLowData = avgdata - (2.0 * sdata);

  std::cout << "\nDensities of non-singleton components:" << std::endl;
  std::cout << "\tmax = " << compDensity[size - 1];
  std::cout << ", min = " << compDensity[0];
  std::cout << ", median = " << median;
  std::cout << ", average = " << avgdata << std::endl;
  std::cout << "\t95% confidence interval: " << confUpData << ", " << confLowData << std::endl;
  std::cout << std::endl;

  if (1)
  {
    num[1] = numSingle;
    num[2] = numTwo;
    for (int i = 1; i < 10000; i++)
      if (num[i] > 0)
        //std::cout << num[i] << std::endl;
        std::cout << "\t" << num[i] << " components with " << i << " vertices" << std::endl;
    //std::cout << num3 << " " << num4 << " " << num5 << " " << num6 << " " << num7;
    //std::cout << " " << num8 << " " << num9 << " " << num10 << std::endl;

    if (0)
      for (int i = 1; i < 64; i++)
        std::cout << num[i] << std::endl;
  }

  if (BFS_GML || BFS_WG2)
  {
    fprintf(output, "%d vertices in largest component printed to separate file\n", maxSizeComp);
    std::cout << "\n"
              << maxSizeComp << " vertices in largest component printed to separate file" << std::endl;
  }

  fprintf(output, "%d vertices in largest component overall\n", maxCompAll);
  std::cout << maxCompAll << " vertices in largest component overall" << std::endl;

  fprintf(output, "%d components are not cliques\n", numNotCliques);
  std::cout << numNotCliques << " components are not cliques" << std::endl;

  fclose(output);

  if (PRINTCOMPSIZES)
  { // print out summary of component sizes to "saveCompSizes.txt"
    FILE *compsize;

    // check if new file, if so print out header
    if ((compsize = fopen("saveCompSizes.txt", "r")) == NULL)
    {
      if ((compsize = fopen("saveCompSizes.txt", "w")) == NULL)
        fatal("'saveCompSizes.txt' file could not be opened.\n");

      fprintf(compsize, "NumSingle NumDouble Num3orMore NumNotCliques MaxSize AvgSize\n");
      fclose(compsize);
    }

    if ((compsize = fopen("saveCompSizes.txt", "a")) == NULL)
      fatal("'saveCompSizes.txt' file could not be opened.\n");

    fprintf(compsize, "%d %d %d %d %d %.2f\n", numSingle, numTwo, numComp3orMore, numNotCliques, maxCompAll, meanSize);
    fclose(compsize);
  }

  delete[] visited;
  delete[] component;
  delete[] queue;
  delete[] clusterNum;

  return modularity_value;
}

//Singleton and Doubleton Removal
void Network::removeSingle_Double()
{
	int prev = 0, j;
	Edge* fE;
	Edge* jPtr;
	int *markedArr = new int[numVertices];
	for(int i = 0; i < numVertices; i++)
	{
		markedArr[i] = 0;
	}
	for(int i = 0; i < numVertices; i++)
	{
		fE = &(vertices[i].firstEdge);
		if(getDegree(i) == 1 && markedArr[i] == 0)
		{
			markedArr[i] = 1;
			j = fE->next->target;
			std :: cout << "\t---first node : " << i << " ,next node : " << j << "\n\n";
			if(getDegree(j) == 2 && markedArr[j] == 0)
			{
				while(getDegree(j) == 2 && markedArr[j] == 0)
				{
					markedArr[j] = 1;
					std ::cout << "\t While loop degree two: prev " << prev << " ,next node : " << j << "\n\n";
					prev = j;
					jPtr = (&(vertices[j].firstEdge));
					jPtr = jPtr->next;
					if( markedArr[jPtr->target] ) 
					{
						jPtr = jPtr->next;
						j = jPtr->target;
						std ::cout << "\t Moving to next node: prev " << prev << " ,next node : " << j << "\n\n";
					}
					j = jPtr->target;
					std ::cout << "\t Moved j and prev: prev " << prev << " ,j : " << j << "\n\n";
				}

				removeEdge(j,prev);
				std ::cout << "\t Cutting edge of Multi Degree : ( " << j << ", " << prev << ")\n";
				markedArr[j] = 1; 
				markedArr[prev] = 1;
			}
			else
			{
				removeEdge(i,j);
				markedArr[j] = 1;
				std ::cout << "\t--- Single Cutting edge : ( " << i << ", " << j << ")\n";
			}
		}
		
	}
	
    delete [] markedArr;
}

bool chromosome_sorter(const chromosome_map &a, const chromosome_map &b)
{
  return a.fitness > b.fitness;
}

void Network::cluster_ga()
{
	//Gets new number of Vertices in the Cluster after singleton and doubleton removal
	int mapVertices = 0;
	for(int i = 0; i < numVertices; i++)
	{
		if(getDegree(i) > 1)
		{
			mapVertices++;
		}
	}
	
	int *mapArr = new int[mapVertices];
	int mapArrPos = 0;
	std::cout << "Mapped Array" << endl;
	for(int i = 0; i < numVertices; i++)
	{
		if(getDegree(i) > 1)
		{
			mapArr[mapArrPos++] = i;
		}
	}
	
	for(int i = 0; i < mapVertices; i++)
	{
		std::cout << mapArr[i] << endl;
	}
	
	//Used to Calculate number of clusters to use
	int **clusterArr = new int*[mapVertices / CLUSTERBOUND - 2];
	double *modularity_Arr = new double[mapVertices];
	int *edge_Arr = new int[numEdges];
	
	for(int i = 0; i < mapVertices / CLUSTERBOUND - 2; i++)
	{
		clusterArr[i] = new int[mapVertices];
	}
	
	
	//Used for genetic algorithm
	int **gaArr = new int*[POPULATIONSIZE];
	double *modularity_gaArr = new double[mapVertices];
	int *edge_gaArr = new int[numEdges];
	
	for(int i = 0; i < POPULATIONSIZE; i++)
	{
		gaArr[i] =  new int[numVertices];
	}
	
	//Variables
	double highest_val = -1.0;
	double second_highest_val = -1.0;
	double lowest_val = 1.0;
	double second_lowest_val = 1.0;
	int highest_node = 0;
	int second_highest_node = 0;
	int lowest_node = 0;
	int second_lowest_node = 0;
	int lower_bound = 0;
	int cluster_size = 0;
	int* ptr;
	
	//Initializes clustering Array
	std::cout << "Initial Cluster Array" << endl;
	for(int i = 0; i < mapVertices / CLUSTERBOUND - 2; i++)
	{
		for(int j = 0; j < mapVertices; j++)
		{
			clusterArr[i][j] = rand() % (i + 2);
			std::cout << clusterArr[i][j] << " ";
		}
		std::cout << endl;
	}
	
	//Gets modularity of each new cluster
	for(int i = 0; i < mapVertices / CLUSTERBOUND - 2; i++)
	{
		//Gets modularity of the new cluster
		modularity_Arr[i] = getModularity( *clusterArr[i] , mapVertices);
  }
  // breakpoint
  

  //Finds highest modularity
	std::cout << "Modularity" << endl;
	for(int i = 0; i < mapVertices / CLUSTERBOUND - 2; i++)
	{
		if(modularity_Arr[i] > highest_val)
		{
			highest_val = modularity_Arr[i];
			highest_node = i + 2;
		}
		std::cout << modularity_Arr[i] << endl;
	}
	std::cout << "Highest Modularity" << endl << highest_val << endl << "Number of Clusters" << endl << highest_node << endl;
  
  //Sets the lower bound for clusters
	lower_bound = highest_node - NEIGHBORSIZE;
	
	//Initializes genetic algorithm array
	std::cout << "Genetic Algorithm Array" << endl;
	for(int i = 0; i < POPULATIONSIZE; i++)
	{
		cluster_size = (rand() % (NEIGHBORSIZE * 2)) + lower_bound;
		for(int j = 0; j < mapVertices; j++)
		{
			gaArr[i][j] = rand() % cluster_size;
			std::cout << gaArr[i][j] << " " ;
		}
		std::cout << endl;
	}
  
  //Loops through the gaArray
	std::cout << "Genetic Algorithm Modularity" << endl;
	for(int i = 0; i < POPULATIONSIZE; i++)
	{
		//Gets modularity of the new cluster
		modularity_gaArr[i] = getModularity( *gaArr[i], mapVertices);
		std::cout << modularity_gaArr[i] << endl;
	}
  
  //Crossover and Mutation
	int *chosenArr = new int[SELECTIONSIZE];
	int random = 0;
	
  chromosome_map global_max;
  global_max.chromosome_index = 0;
  global_max.fitness = -1.0;

	//Loops through the Generations
	for(int i = 0; i < GENERATIONS; i++)
	{
    

		//Loops until Total Population has been changed
		for(int i = 0; i < POPULATIONSIZE / 2; i++)
		{
      chromosome_map selection_cmap[SELECTIONSIZE];
      chromosome_map cmap[POPULATIONSIZE];
      for (int i = 0; i < POPULATIONSIZE; i++)
      {
        cmap[i].chromosome_index = i;
        cmap[i].fitness = modularity_gaArr[i];
      }
      std::sort(cmap, cmap + POPULATIONSIZE, &chromosome_sorter);


      //Selecting Parents
			for(int j = 0; j < SELECTIONSIZE; j++)
			{
				random = rand() % POPULATIONSIZE;
				for(int k = 0; k < SELECTIONSIZE; k++)
				{
					if(chosenArr[k] == 0 && random != chosenArr[k])
					{
						chosenArr[j] = random;
						break;
					}
					else
					{
            random = rand() % POPULATIONSIZE;
            chosenArr[j] = random;
						break;
					}
				}
        std :: cout << "Selected : " << chosenArr[j] << "\n";
        selection_cmap[j].chromosome_index = chosenArr[j];
        selection_cmap[j].fitness = modularity_gaArr[chosenArr[j]];
      }

      std::sort(selection_cmap, selection_cmap + SELECTIONSIZE, &chromosome_sorter);

      for(int i = 0; i < SELECTIONSIZE; i++) {
        std ::cout << selection_cmap[i].chromosome_index << " , " << selection_cmap[i].fitness << std ::endl;
      }
      
      highest_node = selection_cmap[0].chromosome_index;
      second_highest_node = selection_cmap[1].chromosome_index;

      lowest_node = cmap[POPULATIONSIZE - 1].chromosome_index;
      second_lowest_node = cmap[POPULATIONSIZE - 2].chromosome_index;

      std::cout << "Highest : " << highest_node << " , Second High : " << second_highest_node << " , Lowest : " << lowest_node << " , Second Lowest : " << second_lowest_node << std :: endl;
      
      //Crossover
			random = rand() % mapVertices;
      std::cout << "Crossover point : " << random << std :: endl;
			for(int j = 0; j < mapVertices; j++)
			{
        if ( j < random ) {
          gaArr[lowest_node][j] = gaArr[highest_node][j];
          gaArr[second_lowest_node][j] = gaArr[second_highest_node][j];
        } else {
          gaArr[lowest_node][j] = gaArr[second_highest_node][j];
          gaArr[second_lowest_node][j] = gaArr[highest_node][j];
        }
			}
      
      //Mutation of Worst
			for(int j = 0; j < mapVertices; j++)
			{
				cluster_size = (rand() % (NEIGHBORSIZE * 2)) + lower_bound;
				random = rand() % 100;
				if(random < MUTATIONRATE * 100)
				{
					gaArr[lowest_node][j] = rand() % cluster_size;
				}
			}
      
      //Mutation of Second Worst
			for(int j = 0; j < mapVertices; j++)
			{
				cluster_size = (rand() % (NEIGHBORSIZE * 2)) + lower_bound;
				random = rand() % 100;
				if(random < MUTATIONRATE * 100)
				{
					gaArr[second_lowest_node][j] = rand() % cluster_size;
				}
			}
      
      int max_fitness_chromosome = 0;
      //Finds modularity for new Array
			for(int i = 0; i < POPULATIONSIZE; i++)
			{
        //Gets modularity of the new cluster
        modularity_gaArr[i] = getModularity(*gaArr[i], mapVertices);
        std::cout << modularity_gaArr[i] << endl;
        if ( global_max.fitness < modularity_gaArr[i] ) {
          global_max.fitness = modularity_gaArr[i];
          global_max.chromosome_index = i;
        }
			}
      
    }
	}

  std :: cout << "Max :" << global_max.chromosome_index << " , Fitness : " << global_max.fitness << std :: endl;

  for (int j = 0; j < mapVertices; j++)
  {
    for (int k = j; k < mapVertices; k++)
    {
      if (gaArr[j] != gaArr[k])
      {
        if (removeEdge(mapArr[j], mapArr[k]))
        {
          for (int l = 0; l < numEdges; l++)
          {
            if (edge_Arr[l] == 0)
            {
              edge_Arr[l] = mapVertices * mapArr[j] + mapArr[k];
              break;
            }
          }
        }
      }
    }
  }

  bfs("final_clustering.out");

  delete[] mapArr;
	delete[] edge_Arr;
	delete[] edge_gaArr;
	delete[] modularity_Arr;
	delete[] modularity_gaArr;
	delete[] gaArr;
	delete[] clusterArr;
	delete[] chosenArr;
}


double Network::getModularity(int& ptr, int mapVertices)
{
  int *p = &ptr;
  int *edge_Arr = new int[numEdges];

  for(int i = 0; i < numEdges; i++ )
    edge_Arr[i] = 0;

  int nmapVertices = 0;
  for (int i = 0; i < numVertices; i++)
  {
    if (getDegree(i) > 1)
    {
      nmapVertices++;
    }
  }

  int *mapArr = new int[nmapVertices];
  int mapArrPos = 0;
  std::cout << "Mapped Array" << endl;
  for (int i = 0; i < numVertices; i++)
  {
    if (getDegree(i) > 1)
    {
      mapArr[mapArrPos++] = i;
    }
  }

  // for (int i = 0; i < nmapVertices; i++)
  // {
  //   std::cout << mapArr[i] << endl;
  // }

  for (int j = 0; j < nmapVertices; j++)
  {
    for (int k = j; k < nmapVertices; k++)
    {
      if (p[j] != p[k])
      {
        if (removeEdge(mapArr[j], mapArr[k]))
        {
          for (int l = 0; l < numEdges; l++)
          {
            if (edge_Arr[l] == 0)
            {
              edge_Arr[l] = nmapVertices * mapArr[j] + mapArr[k];
              break;
            }
          }
        }
      }
    }
  }

  double modularity_value = modularity("temp.mod");

  std::cout << "Adding Edges from Mapped Array" << endl;
  for (int j = 0; j < numEdges; j++)
  {
    if (edge_Arr[j] != 0) {
      addEdge(edge_Arr[j] / nmapVertices, edge_Arr[j] % nmapVertices);
    }
  }

  // delete[] edge_Arr;
  // delete [] mapArr;
  // delete p;

  return modularity_value;
}
