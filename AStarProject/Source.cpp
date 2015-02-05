
#ifdef _DEBUG
#pragma comment(lib,"sfml-graphics-d.lib")
#pragma comment(lib,"sfml-audio-d.lib")
#pragma comment(lib,"sfml-system-d.lib")
#pragma comment(lib,"sfml-window-d.lib")
#pragma comment(lib,"sfml-network-d.lib")
#else
#pragma comment(lib,"sfml-graphics.lib")
#pragma comment(lib,"sfml-audio.lib")
#pragma comment(lib,"sfml-system.lib")
#pragma comment(lib,"sfml-window.lib")
#pragma comment(lib,"sfml-network.lib")
#endif
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <map>

#include "Graph.h"


using namespace std;

typedef GraphArc<pair<string, int>, int> Arc1;
typedef GraphNode<pair<string, int>, int> Node;

int main(int argc, char *argv[]) {

	sf::Texture resetButtonTexture;
	if (!resetButtonTexture.loadFromFile("button.png")) {
		cout << "No image with that name found";
	}
	resetButtonTexture.setSmooth(true);
	sf::Sprite resetButton;
	resetButton.setTexture(resetButtonTexture);
	resetButton.setPosition(1020, 200);

	sf::Texture titleTexture;
	if (!titleTexture.loadFromFile("title.png")) {
		cout << "No image with that name found";
	}
	titleTexture.setSmooth(true);
	sf::Sprite title;
	title.setTexture(titleTexture);
	title.setPosition(945, 10);


	sf::Texture infoTexture;
	if (!infoTexture.loadFromFile("infoSheet.png")) {
		cout << "No image with that name found";
	}
	infoTexture.setSmooth(true);
	sf::Sprite info;
	info.setTexture(infoTexture);
	info.setPosition(970, 290);

	sf::RenderWindow window(sf::VideoMode(1280, 720, 32), "A* PathFinding");
	window.setFramerateLimit(60);	

	int const graphSize = 30;
	Graph<pair<string, int>, int> graph(graphSize);

	string stringArray[graphSize];
	std::pair<int, int> nodePositions[graphSize];

	string tempString;
	std::vector<Node*> path;
	path.reserve(20);

	pair<string, int> temp;
	temp.second = INT_MAX; //setting the value on the node to 'infinite'

	ifstream myfile;
	myfile.open("Nodes.txt");
	
	int index = 0;

	int x, y;
	while (myfile >> tempString >> x >> y) {
		//getline(myfile, tempString);
		stringArray[index] = tempString;
		nodePositions[index] = make_pair(x * 2 / 1.1, y * 2 / 1.1);
		index++;
	}

	index = 0;
	//add nodes with values from 0 -> 6
	while(index < graphSize) {
		temp.first = stringArray[index];
		graph.addNode(temp, index, nodePositions[index]);
		index++;
	}
	myfile.close();

	myfile.open("Arcs.txt");

	int from, to, weight;
	while (myfile >> from >> to >> weight) {
		sf::Vector2f start = sf::Vector2f(graph.nodeArray()[from]->getX(), graph.nodeArray()[from]->getY());
		sf::Vector2f end = sf::Vector2f(graph.nodeArray()[to]->getX(), graph.nodeArray()[to]->getY());
		int realWeight = (int)(sqrt((pow(end.x - start.x, 2)) + (pow(end.y - start.y, 2))));
		graph.addArc(from, to, realWeight);
	}
	myfile.close();

	//setting up the circles for the nodes
	int size = 25;
	sf::CircleShape circles[graphSize];
	for (int i = 0; i < graphSize; i++) {
		sf::CircleShape circle(size);
		circle.setFillColor(sf::Color(0,0,155,255));
		circle.setOrigin(size / 2, size / 2);
		circle.setPosition(graph.nodeArray()[i]->getX(), graph.nodeArray()[i]->getY());
		circles[i] = circle;
	}
	//setting up font for the drawing of info	
	//load a font
	sf::Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\GARA.TTF");

	while (window.isOpen())
	{
		window.clear();
		//drawing the arcs between the nodes
		for (int i = 0; i < graphSize; i++) { 
			list<Arc1>::const_iterator iter = graph.nodeArray()[i]->arcList().begin();
			list<Arc1>::const_iterator endIter = graph.nodeArray()[i]->arcList().end();
			for( ; iter != endIter; iter++ ) {
				sf::Vector2f start = sf::Vector2f(graph.nodeArray()[i]->getX() + size / 2, graph.nodeArray()[i]->getY() + size / 2);
				sf::Vector2f end = sf::Vector2f((*iter).node()->getX() + size / 2, (*iter).node()->getY() + size / 2);
				sf::Vertex line[] =
				{
					sf::Vertex(start),
					sf::Vertex(end)
				};
				window.draw(line, 2, sf::Lines);

				sf::Text arcWeight(to_string((*iter).weight()), font, 20);
				arcWeight.setColor(sf::Color::Magenta);
				arcWeight.setPosition(start + sf::Vector2f((end - start).x / 2 - 20, (end - start).y / 2));
				window.draw(arcWeight);
			}
		}

		graph.checkMousePos(window);
		graph.selectNodes(window);

		//graph.drawNodes(window);
		//COLOR 0 = NORMAL, 1 = YELLOW, 2 = RED, 3 = HIGHLIGHTED, 4 = SELECTED, 5= GOAL SELECTED
		for (int i = 0; i < graphSize; i++) {
			if (graph.nodeArray()[i]->getColor() == 0) {
				circles[i].setFillColor(sf::Color(0,255,255));
				circles[i].setOutlineThickness(4);
				circles[i].setOutlineColor(sf::Color(0, 102, 0));
			}
			else if (graph.nodeArray()[i]->getColor() == 1) {
				circles[i].setFillColor(sf::Color(240,230,140));
				circles[i].setOutlineThickness(4);
				circles[i].setOutlineColor(sf::Color(255, 0, 0));
			}
			else if (graph.nodeArray()[i]->getColor() == 2) {
				circles[i].setFillColor(sf::Color(68, 243, 267));
				circles[i].setOutlineThickness(4);
				circles[i].setOutlineColor(sf::Color(0, 102, 0));
			}
			else if (graph.nodeArray()[i]->getColor() == 3) {
				circles[i].setFillColor(sf::Color(240,230,140));
				circles[i].setOutlineThickness(4);
				circles[i].setOutlineColor(sf::Color(255, 0, 0));
			}
			else if (graph.nodeArray()[i]->getColor() == 4) {
				circles[i].setFillColor(sf::Color(240,0,0));
				circles[i].setOutlineThickness(4);
				circles[i].setOutlineColor(sf::Color(240,230,140));
			}
			else if (graph.nodeArray()[i]->getColor() == 5) {
				circles[i].setFillColor(sf::Color(240,230,140));
				circles[i].setOutlineThickness(4);
				circles[i].setOutlineColor(sf::Color(255, 0, 0));
			}

			window.draw(circles[i]);

			//drawing the names of the nodes
			sf::Text nodeName(graph.nodeArray()[i]->data().first, font, 30);
			nodeName.setColor(sf::Color(0,0,0));
			nodeName.setStyle(sf::Text::Bold);
			nodeName.setPosition(graph.nodeArray()[i]->getX() + 4, graph.nodeArray()[i]->getY()- 10);
			window.draw(nodeName);
		}

			//used to specify which nodes you are using as start and end
		if (graph.startSelected() == false)
			graph.selectNodes(window);
		else if (graph.startSearch() == false)
			graph.selectNodes(window);

		int startIndex = 0;
		int goalIndex = 0;
		
		if (graph.startSearch()) {

			for (; startIndex < graph.size(); startIndex++) {
				if (graph.StartNode() == graph.nodeArray()[startIndex]->data().first) {
					break;
				}
			}
			for (; goalIndex < graph.size(); goalIndex++) {
				if (graph.GoalNode() == graph.nodeArray()[goalIndex]->data().first) {
					break;
				}
			}
			graph.AStar(graph.nodeArray()[startIndex], graph.nodeArray()[goalIndex], path);	
		}

		graph.drawNodeInfo(window);

		window.draw(resetButton);
		window.draw(title);
		window.draw(info);

		sf::Text startNode(graph.StartNode(), font, 30);
		startNode.setColor(sf::Color(0,0,0));
		startNode.setStyle(sf::Text::Bold);
		startNode.setPosition(1060, 330);
		window.draw(startNode);

		sf::Text goalNode(graph.GoalNode(), font, 30);
		goalNode.setColor(sf::Color(0,0,0));
		goalNode.setStyle(sf::Text::Bold);
		goalNode.setPosition(1060, 420);
		window.draw(goalNode);

		sf::Text pathCost(to_string(graph.PathCost()), font, 30);
		pathCost.setColor(sf::Color(0,0,0));
		pathCost.setStyle(sf::Text::Bold);
		pathCost.setPosition(1050, 510);
		window.draw(pathCost);

		window.display();
	}


	system("PAUSE");
}

class NodeSearchCostComparer {
public:
	bool operator()(Node * n1, Node * n2) {
		pair<string, int> p1 = n1->data();
		pair<string, int> p2 = n2->data();
		return p1.second > p2.second; 
	}
};
