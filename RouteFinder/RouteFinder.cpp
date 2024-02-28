#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

/*
Route Problem:
inputs:
• list of landmarks and their associated distances as a text file
• desired route distance
• number of closest results to show

show the number of desired results of the closest routes to the desired distance
*/

//reads landmark.txt input
ifstream landmark_list_read;

class Route
{
private:
	double startDistance;
	double endDistance;
	std::string startLandmark;
	std::string endLandmark;

public:
	Route();

	Route(double startDistance, double endDistance, std::string startLandmark, std::string endLandmark)
	{
		this->startDistance = startDistance;
		this->endDistance = endDistance;
		this->startLandmark = startLandmark;
		this->endLandmark = endLandmark;
	}

	//if routes start and end in same place, the routes are equal
	bool operator==(const Route& otherRoute)
	{
		bool startDistanceMatch = (this->startDistance == otherRoute.startDistance);
		bool endDistanceMatch = ((this->endDistance == otherRoute.endDistance));
		if (startDistanceMatch && endDistanceMatch)
			return true;
		return false;
	}

	void operator=(const Route& otherRoute)
	{
		this->startDistance = otherRoute.startDistance;
		this->endDistance = otherRoute.endDistance;

		this->startLandmark = otherRoute.startLandmark;
		this->endLandmark = otherRoute.endLandmark;
	}
	
	double getRouteDistance()
	{
		return this->endDistance - this->startDistance;
	}

	std::string getStartLandmark()
	{
		return this->startLandmark;
	}

	std::string getEndLandmark()
	{
		return this->endLandmark;
	}
};

double getLandmarkDistance(std::string lineRead)
{
	for (int i = 0; i < lineRead.length(); i++)
	{
		if (std::isalpha(lineRead.at(i)))
		{
			std::string distance = lineRead.substr(0, i - 1);
			return std::stod(distance);
		}
	}
	return 0.0;
}

std::string getLandmark(std::string lineRead)
{
	for (int i = 0; i < lineRead.length(); i++)
	{
		if (std::isalpha(lineRead.at(i)))
		{
			std::string landmark = lineRead.substr(i, lineRead.length() - 1);
			return landmark;
		}
	}
	return "no landmark provided";
}

//returns vector of landmarks and their associated distance
std::vector<std::pair<double, std::string>> getLandmarks()
{
	landmark_list_read.open("landmarks.txt");
	if (!landmark_list_read.is_open())
	{
		std::cout << "landmarks.txt was not found";
		return {};
	}

	std::vector<pair<double, std::string>> landmarkList;
	std::string lineRead;
	while (getline(landmark_list_read, lineRead))
	{
		double distance = getLandmarkDistance(lineRead);
		std::string landmark = getLandmark(lineRead);
		landmarkList.push_back({ distance, landmark });
	}
	return landmarkList;
}

//finds the route with the greatest difference in distance from the desired distance and returns the difference
double getWorstRouteCloseness(std::vector<Route>* routes, double desiredDistance)
{
	//each route has starting landmark and distance and ending route and distance		
	double worstRouteCloseness = abs(desiredDistance - ((*routes)[0].getRouteDistance()));
	for (int i = 0; i < (*routes).size(); i++)
	{
		double routeCloseness = abs(desiredDistance - ((*routes))[i].getRouteDistance());
		if (routeCloseness > worstRouteCloseness)
			worstRouteCloseness = routeCloseness;
	}
	//std::cout << "worstCloseness was " << worstRouteCloseness << "\n";
	return worstRouteCloseness;
}

Route* getWorstRoute(std::vector<Route>* routes, double desiredDistance)
{
	Route* worstRoute = &(*routes)[0];
	//initially set to first route closeness
	double worstRouteCloseness = abs(desiredDistance - ((*routes)[0].getRouteDistance()));

	for (int i = 0; i < (*routes).size(); i++)
	{
		double routeCloseness = abs(desiredDistance - ((*routes)[i].getRouteDistance()));
		if (routeCloseness > worstRouteCloseness)
		{
			worstRouteCloseness = routeCloseness;
			worstRoute = &(*routes)[i];
		}
	}
	return worstRoute;
}

void removeRoute(Route routeToRemove, std::vector<Route>* routes)
{
	for (int i = 0; i < (*routes).size(); i++)
	{
		if ((*routes)[i] == routeToRemove)
		{
			(*routes).erase((*routes).begin() + i);
		}
	}
}

//central function for filling the route results
std::vector<Route> getClosestRoutes(std::vector<std::pair<double, std::string>> landmarks, int numRoutes, double distance)
{
	std::vector<Route> closestRoutes;

	//checks for numRoutes <= 0 case
	bool canFillMoreRoutes = (closestRoutes.size() < numRoutes);

	for (int i = 0; i < landmarks.size(); i++)
	{
		double startDistance = landmarks[i].first;
		std::string startLandmark = landmarks[i].second;

		for (int j = i + 1; j < landmarks.size(); j++)
		{
			double endDistance = landmarks[j].first;
			std::string endLandmark = landmarks[j].second;

			bool isRoutesFilled = (closestRoutes.size() >= numRoutes);
			if (!isRoutesFilled)
			{
				Route newRoute(startDistance, endDistance, startLandmark, endLandmark);
				closestRoutes.push_back(newRoute);
				continue;
			}

			//checks if numRoutes is greater than landmark list size
			if (!canFillMoreRoutes)
				break;

			double newRouteCloseness = abs(distance - (endDistance - startDistance));
			double worstRouteCloseness = getWorstRouteCloseness(&closestRoutes, distance);
			//less closeness means it is closer to the desired value							
			bool betterRouteFound = (newRouteCloseness < worstRouteCloseness);

			if (betterRouteFound)
			{
				removeRoute((*getWorstRoute(&closestRoutes, distance)), &closestRoutes);
				Route newRoute(startDistance, endDistance, startLandmark, endLandmark);
				closestRoutes.push_back(newRoute);
			}
			
			canFillMoreRoutes = (closestRoutes.size() < landmarks.size());
		}
	}
	return closestRoutes;
}

std::vector<Route> sortByCloseness(std::vector<Route>* routes, double distance)
{
	//selection sort	
	for (int i = 0; i < (*routes).size(); i++)
	{
		double closeness1 = abs(distance - (*routes)[i].getRouteDistance());
		for (int j = i + 1; j < (*routes).size(); j++)
		{
			double closeness2 = abs(distance - (*routes)[j].getRouteDistance());
			if (closeness2 < closeness1)
			{
				//swap elements
				Route tempRoute = (*routes)[i];
				(*routes)[i] = (*routes)[j];
				(*routes)[j] = tempRoute;
				//update closeness1 when new element swapped to it
				closeness1 = abs(distance - (*routes)[i].getRouteDistance());
			}
		}
	}
	return (*routes);
}

void printRoutes(std::vector<Route> routes)
{
	for (int i = 0; i < routes.size(); i++)
	{
		std::cout << i + 1 << ". " << routes[i].getRouteDistance() << "mi";
		std::cout << ": " << routes[i].getStartLandmark() << " -> " << routes[i].getEndLandmark();
		std::cout << "\n";
	}
}

void main()
{
	int numRoutes = 0;
	double distance = 0.0;
	std::cout << "Enter the interval distance you want in miles: ";
	std::cin >> distance;
	std::cout << "Enter the number of routes you want to see: ";
	std::cin >> numRoutes;
	std::cout << "\n";

	std::vector<pair<double, std::string>> landmarks = getLandmarks();
	std::vector<Route> closestRoutes = getClosestRoutes(landmarks, numRoutes, distance);
	std::vector<Route> sortedRoutes = sortByCloseness(&closestRoutes, distance);
	printRoutes(closestRoutes);	
}