#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>

using namespace std;

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    This is the graph class, it contains all the
//                  nodes.
// ----------------------------------------------------------------


template<class NodeType, class ArcType>
class Graph {
private:

	// typedef the classes to make our lives easier.
	typedef GraphArc<NodeType, ArcType> Arc;
	typedef GraphNode<NodeType, ArcType> Node;

	// ----------------------------------------------------------------
	//  Description:    An array of all the nodes in the graph.
	// ----------------------------------------------------------------
	Node** m_pNodes;

	// ----------------------------------------------------------------
	//  Description:    The maximum number of nodes in the graph.
	// ----------------------------------------------------------------
	int m_maxNodes;


	// ----------------------------------------------------------------
	//  Description:    The actual number of nodes in the graph.
	// ----------------------------------------------------------------
	int m_count;

	//used to check if the start and goal have been selected
	bool start;
	bool end;
	bool search;

	string startNode;
	string goalNode;
	int pathCost;

	sf::Texture nodeInfoTexture;
	sf::Sprite nodeInfo;
	sf::Text gn;
	sf::Text hn;
	sf::Font font;

public:           
	// Constructor and destructor functions
	Graph( int size );
	~Graph();

	// Accessors
	Node** nodeArray() const {
		return m_pNodes;
	}

	int size() const {
		return m_count;
	}

	bool startSelected() {
		return start;
	}

	bool startSearch() {
		return search;
	}

	string StartNode() {
		return startNode;
	}

	string GoalNode() {
		return goalNode;
	}

	int PathCost() {
		return pathCost;
	}

	void resetSelectedNodes() {
		start = false;
		end = false;
		for (int i = 0; i < m_maxNodes; i++) {
			m_pNodes[i]->setHeuristic(0);
		}
		resetNodes();
	}

	// Public member functions.
	bool addNode( NodeType data, int index, std::pair<int, int> pos);
	void removeNode( int index );
	bool addArc( int from, int to, ArcType weight );
	void removeArc( int from, int to );
	Arc* getArc( int from, int to );        
	void clearMarks();
	void depthFirst( Node* pNode, void (*pProcess)(Node*) );
	void breadthFirst( Node* pNode, void (*pProcess)(Node*) );
	void breadthFirstSearch( Node* pNode, void (*pProcess)(Node*),  NodeType data );
	void UCS( Node* pNode, Node* goal);
	void AStar(Node* start, Node* goal, std::vector<Node*> &path );
	void resetNodes();
	void drawNodes(sf::RenderWindow window);
	void checkMousePos(sf::RenderWindow &window);
	void selectNodes(sf::RenderWindow &window);
	void drawNodeInfo(sf::RenderWindow &window);
};

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    Constructor, this constructs an empty graph
//  Arguments:      The maximum number of nodes.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::Graph( int size ) : m_maxNodes( size ) {
	int i;
	m_pNodes = new Node * [m_maxNodes];
	// go through every index and clear it to null (0)
	for( i = 0; i < m_maxNodes; i++ ) {
		m_pNodes[i] = 0;
	}

	start = false;
	end = false;
	search = false;

	startNode = "";
	goalNode = "";
	pathCost = 0;

	nodeInfoTexture;
	if (!nodeInfoTexture.loadFromFile("nodeInfo.png")) {
		cout << "No image with that name found";
	}
	nodeInfoTexture.setSmooth(true);
	nodeInfo.setTexture(nodeInfoTexture);
	nodeInfo.setPosition(-1000, -1000);

	font.loadFromFile("C:\\Windows\\Fonts\\GARA.TTF");

	// set the node count to 0.
	m_count = 0;
}

// ----------------------------------------------------------------
//  Name:           ~Graph
//  Description:    destructor, This deletes every node
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::~Graph() {
	int index;
	for( index = 0; index < m_maxNodes; index++ ) {
		if( m_pNodes[index] != 0 ) {
			delete m_pNodes[index];
		}
	}
	// Delete the actual array
	delete m_pNodes;
}

// ----------------------------------------------------------------
//  Name:           addNode
//  Description:    This adds a node at a given index in the graph.
//  Arguments:      The first parameter is the data to store in the node.
//                  The second parameter is the index to store the node.
//  Return Value:   true if successful
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addNode( NodeType data, int index, std::pair<int, int> pos) {
	bool nodeNotPresent = false;
	// find out if a node does not exist at that index.
	if ( m_pNodes[index] == 0) {
		nodeNotPresent = true;
		// create a new node, put the data in it, and unmark it.
		m_pNodes[index] = new Node(); // <------ ADDED brackets to all the default constructor
		m_pNodes[index]->setData(data);
		m_pNodes[index]->setMarked(false);
		m_pNodes[index]->setPosition(pos);

		// increase the count and return success.
		m_count++;
	}

	return nodeNotPresent;
}

// ----------------------------------------------------------------
//  Name:           removeNode
//  Description:    This removes a node from the graph
//  Arguments:      The index of the node to return.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeNode( int index ) {
	// Only proceed if node does exist.
	if( m_pNodes[index] != 0 ) {
		// now find every arc that points to the node that
		// is being removed and remove it.
		int node;
		Arc* arc;

		// loop through every node
		for( node = 0; node < m_maxNodes; node++ ) {
			// if the node is valid...
			if( m_pNodes[node] != 0 ) {
				// see if the node has an arc pointing to the current node.
				arc = m_pNodes[node]->getArc( m_pNodes[index] );
			}
			// if it has an arc pointing to the current node, then
			// remove the arc.
			if( arc != 0 ) {
				removeArc( node, index );
			}
		}


		// now that every arc pointing to the current node has been removed,
		// the node can be deleted.
		delete m_pNodes[index];
		m_pNodes[index] = 0;
		m_count--;
	}
}

// ----------------------------------------------------------------
//  Name:           addArd
//  Description:    Adds an arc from the first index to the 
//                  second index with the specified weight.
//  Arguments:      The first argument is the originating node index
//                  The second argument is the ending node index
//                  The third argument is the weight of the arc
//  Return Value:   true on success.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addArc( int from, int to, ArcType weight ) {
	bool proceed = true; 
	// make sure both nodes exist.
	if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
		proceed = false;
	}

	// if an arc already exists we should not proceed
	if( m_pNodes[from]->getArc( m_pNodes[to] ) != 0 ) {
		proceed = false;
	}

	if (proceed == true) {
		// add the arc to the "from" node.
		m_pNodes[from]->addArc( m_pNodes[to], weight );
	}

	return proceed;
}

// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This removes the arc from the first index to the second index
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeArc( int from, int to ) {
	// Make sure that the node exists before trying to remove
	// an arc from it.
	bool nodeExists = true;
	if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
		nodeExists = false;
	}

	if (nodeExists == true) {
		// remove the arc.
		m_pNodes[from]->removeArc( m_pNodes[to] );
	}
}


// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    Gets a pointer to an arc from the first index
//                  to the second index.
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   pointer to the arc, or 0 if it doesn't exist.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
// Dev-CPP doesn't like Arc* as the (typedef'd) return type?
GraphArc<NodeType, ArcType>* Graph<NodeType, ArcType>::getArc( int from, int to ) {
	Arc* pArc = 0;
	// make sure the to and from nodes exist
	if( m_pNodes[from] != 0 && m_pNodes[to] != 0 ) {
		pArc = m_pNodes[from]->getArc( m_pNodes[to] );
	}

	return pArc;
}


// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearMarks() {
	int index;
	for( index = 0; index < m_maxNodes; index++ ) {
		if( m_pNodes[index] != 0 ) {
			m_pNodes[index]->setMarked(false);
		}
	}
}


// ----------------------------------------------------------------
//  Name:           depthFirst
//  Description:    Performs a depth-first traversal on the specified 
//                  node.
//  Arguments:      The first argument is the starting node
//                  The second argument is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::depthFirst( Node* pNode, void (*pProcess)(Node*) ) {
	if( pNode != 0 ) {
		// process the current node and mark it
		pProcess( pNode );
		pNode->setMarked(true);

		// go through each connecting node
		list<Arc>::iterator iter = pNode->arcList().begin();
		list<Arc>::iterator endIter = pNode->arcList().end();

		for( ; iter != endIter; ++iter) {
			// process the linked node if it isn't already marked.
			if ( (*iter).node()->marked() == false ) {
				depthFirst( (*iter).node(), pProcess);
			}            
		}
	}
}


// ----------------------------------------------------------------
//  Name:           breadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirst( Node* pNode, void (*pProcess)(Node*) ) {
	if( pNode != 0 ) {
		queue<Node*> nodeQueue;        
		// place the first node on the queue, and mark it.
		nodeQueue.push( pNode );
		pNode->setMarked(true);

		// loop through the queue while there are nodes in it.
		while( nodeQueue.size() != 0 ) {
			// process the node at the front of the queue.
			pProcess( nodeQueue.front() );

			// add all of the child nodes that have not been 
			// marked into the queue
			list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();

			for( ; iter != endIter; iter++ ) {
				if ( (*iter).node()->marked() == false) {
					// mark the node and add it to the queue.
					(*iter).node()->setMarked(true);
					(*iter).node()->setPrevious(nodeQueue.front());
					nodeQueue.push( (*iter).node() );
				}
			}

			// dequeue the current node.
			nodeQueue.pop();
		}
	}  
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirstSearch( Node* pNode, void (*pProcess)(Node*), NodeType data ) {
	bool foundNode = false;
	GraphNode<NodeType, ArcType>* theNode = NULL;
	if( pNode != 0 ) {
		queue<Node*> nodeQueue;        
		// place the first node on the queue, and mark it.
		nodeQueue.push( pNode );
		pNode->setMarked(true);

		// loop through the queue while there are nodes in it.
		while( nodeQueue.size() != 0 && foundNode == false) {
			// add all of the child nodes that have not been 
			// marked into the queue
			list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();

			for( ; iter != endIter; iter++ ) {
				if ( (*iter).node()->marked() == false) {
					// mark the node and add it to the queue.
					(*iter).node()->setMarked(true);
					(*iter).node()->setPrevious(nodeQueue.front());
					nodeQueue.push( (*iter).node() );
				}
				//if the target node has been found
				if ((*iter).node()->data() == data) {
					theNode = (*iter).node();
					foundNode = true;
					break;
				}
			}

			// dequeue the current node.
			nodeQueue.pop();
		}
		cout << "This is the shortest path to " << data << ":" << endl;
		GraphNode<NodeType, ArcType>* tempNode = theNode;
		while(tempNode->getPrevious() != NULL) {
			pProcess (tempNode->getPrevious());
			tempNode = tempNode->getPrevious();
		}

	}  
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::UCS( Node* pNode, Node* goal)
{
	GraphNode<NodeType, ArcType>* foundNode;
	if( pNode != 0 ) {
		priority_queue<Node*, vector<Node *>, NodeSearchCostComparer> nodeQueue;     
		// place the first node on the queue, and mark it.
		nodeQueue.push( pNode );
		nodeQueue.top()->data().second = 0;
		pNode->setMarked(true);

		// loop through the queue while there are nodes in it.
		while( nodeQueue.size() != 0 && nodeQueue.top() != goal) {
			// add all of the child nodes that have not been 
			// marked into the queue
			list<Arc>::const_iterator iter = nodeQueue.top()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.top()->arcList().end();

			for( ; iter != endIter; iter++ ) {
				GraphNode<NodeType, ArcType>* temp = nodeQueue.top()->getPrevious();
				if ((*iter).node() != temp) {
					int distanceChild = (*iter).weight() + nodeQueue.top()->data().second;
					// mark the node and add it to the queue.
					if (distanceChild < (*iter).node()->data().second) {
						(*iter).node()->data().second = distanceChild;
						(*iter).node()->setPrevious(nodeQueue.top());
					}
					if ( (*iter).node()->marked() == false) {
						(*iter).node()->setMarked(true);
						nodeQueue.push( (*iter).node() );
					}
				}
			}	  

			// dequeue the current node.
			nodeQueue.pop();
		}  

		foundNode = nodeQueue.top();
		//setting the heuristic value of the start node to the goal node
		pNode->setHeuristic((foundNode->data().second * 90) / 100);
	}
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::AStar(Node* start, Node* goal, std::vector<Node*> &path ) {

	GraphNode<NodeType, ArcType>* foundNode;
	priority_queue<Node*, vector<Node *>, NodeSearchCostComparer> nodeQueue;

	for (int i = 0; i < m_count; i++) {
		if (m_pNodes[i] != goal) {
			sf::Vector2f startPoint = sf::Vector2f(m_pNodes[i]->getX(), m_pNodes[i]->getY());
			sf::Vector2f end = sf::Vector2f(goal->getX(), goal->getY());
			int realWeight = (int)(sqrt((pow(end.x - startPoint.x, 2)) + (pow(end.y - startPoint.y, 2))));
			m_pNodes[i]->setHeuristic(realWeight);
			//UCS(m_pNodes[i], goal);
		}
		resetNodes();
	}

	start->data().second = 0;
	nodeQueue.push( start );
	nodeQueue.top()->setMarked(true);

	while (nodeQueue.size() != 0 && nodeQueue.top() != goal) {
		list<Arc>::const_iterator iter = nodeQueue.top()->arcList().begin();
		list<Arc>::const_iterator endIter = nodeQueue.top()->arcList().end();

		for( ; iter != endIter; iter++ ) {
			if ((*iter).node() != nodeQueue.top()->getPrevious() && (*iter).node()->marked() != true) {
				int distanceChild = nodeQueue.top()->data().second + (*iter).weight();// + (*iter).node()->getHeuristic();
				int estTotalCost = (*iter).node()->data().second;
				(*iter).node()->setColor(2);
				estTotalCost = abs(estTotalCost);
				if (distanceChild < estTotalCost) {
					(*iter).node()->data().second = distanceChild;
					(*iter).node()->setPrevious(nodeQueue.top());
				}
				if ( (*iter).node()->marked() == false) {
					nodeQueue.push( (*iter).node() );
					(*iter).node()->setMarked(true);
				}
			}

		}
		nodeQueue.pop();
	}
	if (nodeQueue.size() != 0) {
		search = false;
		foundNode = nodeQueue.top();
		foundNode->setColor(1);
		pathCost = foundNode->data().second;
		cout << "The fastest route to: " << foundNode->data().first << endl;
		path.push_back(foundNode); //final node, the node that the path is to.
		while (foundNode->getPrevious() != NULL) {
			path.push_back(foundNode->getPrevious());
			cout << "-> " << foundNode->getPrevious()->data().first << endl;
			foundNode = foundNode->getPrevious();
			if (foundNode->getColor() != 3)
				foundNode->setColor(1);
		}

		/*for (int i = path.size() - 1; i >= 0; i--) {

		}*/
	}
	else
		cout << "There is no path from node " << start->data().first << " to " << goal->data().first;
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::resetNodes() {
	for(int i = 0; i < m_count; i++) {
		m_pNodes[i]->setMarked(false);
		m_pNodes[i]->data().second = INT_MAX;
		m_pNodes[i]->setPrevious(NULL);
		m_pNodes[i]->setColor(0);
	}
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::drawNodes(sf::RenderWindow window) {
	for (int i = 0; i < m_count; i++) {
		window.draw(m_pNodes[i]->getCircle());
	}
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::checkMousePos(sf::RenderWindow &window) {
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	for (int i = 0; i < m_count; i++) {		
		int tempColor = m_pNodes[i]->getColor();
		sf::Vector2f nodePos = sf::Vector2f(m_pNodes[i]->getX() + 12.5f, m_pNodes[i]->getY() + 12.5f);
		int distance = (int)(sqrt((pow(mousePos.x - nodePos.x, 2)) + (pow(mousePos.y - nodePos.y, 2))));
		if (distance < 25) {
			if (m_pNodes[i]->getColor() == 0)
				m_pNodes[i]->setColor(3);			

			nodeInfo.setPosition(mousePos.x, mousePos.y);

			if (m_pNodes[i]->data().second < 1000000)
				gn = sf::Text(to_string(m_pNodes[i]->data().second), font, 16);
			else
				gn = sf::Text("NA", font, 16);
			gn.setPosition(mousePos.x + 50, mousePos.y + 12);

			hn = sf::Text(to_string(m_pNodes[i]->getHeuristic()), font, 16);
			hn.setPosition(mousePos.x + 50, mousePos.y + 38);

			hn.setStyle(sf::Text::Bold);
			gn.setStyle(sf::Text::Bold);

			hn.setColor(sf::Color(0,0,0));
			gn.setColor(sf::Color(0,0,0));
			break;
		}
		else {
			if (tempColor == 0 || tempColor == 3)
				m_pNodes[i]->setColor(0);

			nodeInfo.setPosition(-1000, -1000);
			gn.setPosition(-1000, -1000);
			hn.setPosition(-1000, -1000);
		}


	}
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::selectNodes(sf::RenderWindow &window) {
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			for (int i = 0; i < m_count; i++) {
				sf::Vector2f nodePos = sf::Vector2f(m_pNodes[i]->getX() + 12.5f, m_pNodes[i]->getY() + 12.5f);
				int distance = (int)(sqrt((pow(mousePos.x - nodePos.x, 2)) + (pow(mousePos.y - nodePos.y, 2))));
				if (distance < 25) {
					if (!start) {
						m_pNodes[i]->setColor(4);
						start = true; 
						startNode = m_pNodes[i]->data().first;
					}
					else {
						m_pNodes[i]->setColor(5);
						end = true;
						goalNode  = m_pNodes[i]->data().first;
						search = true;
					}
					break;
				}
			}
			if (mousePos.x > 1020 && mousePos.x < 1120 && mousePos.y > 200 && mousePos.y < 270) {
				resetSelectedNodes();
				resetNodes();
				startNode = "";
				goalNode = "";
				pathCost = 0;
			}
		}
	}
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::drawNodeInfo(sf::RenderWindow &window) {
	window.draw(nodeInfo);
	window.draw(gn);
	window.draw(hn);
}

#include "GraphNode.h"
#include "GraphArc.h"


#endif
