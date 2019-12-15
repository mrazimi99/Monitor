#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <thread>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <semaphore.h>

using namespace std;

class Edge
{
public:
	inline Edge(string _name, int _hardness);
	inline ~Edge();

	inline static double get_total_emission();
	inline static void increase_total_emission(unsigned int value);
	inline static void acquire_total_emission_semaphore();
	inline static void release_total_emission_semaphore();
	inline static void destroy_total_emission_semaphore();
	inline int get_hardness() const;
	inline string get_name() const;
	inline void acquire();
	inline void release();

private:
	static double total_emission;
	static sem_t total_emission_semaphore;
	string name;
	int hardness;
	sem_t semaphore;
};

double Edge::total_emission = 0;

sem_t Edge::total_emission_semaphore = []()
{
	sem_t semaphore;
	sem_init(&semaphore, 0, 1);
	return semaphore;
}();

Edge::Edge(string _name, int _hardness)
: name(_name)
, hardness(_hardness)
{
	sem_init(&semaphore, 0, 1);
}

Edge::~Edge()
{
	sem_destroy(&semaphore);
}

void Edge::acquire_total_emission_semaphore()
{
	sem_wait(&total_emission_semaphore);
}

void Edge::release_total_emission_semaphore()
{
	sem_post(&total_emission_semaphore);
}

void Edge::destroy_total_emission_semaphore()
{
	sem_destroy(&total_emission_semaphore);
}

double Edge::get_total_emission()
{
	return total_emission;
}

void Edge::increase_total_emission(unsigned int value)
{
	total_emission += value;
}

int Edge::get_hardness() const
{
	return hardness;
}

string Edge::get_name() const
{
	return name;
}

void Edge::acquire()
{
	sem_wait(&semaphore);
}

void Edge::release()
{
	sem_post(&semaphore);
}

void parse_input(string file_name, vector<Edge>& edges, map<string, int>& pathes);
void go(int p, vector<Edge*> edges, int path_number, int car_number);

void parse_input(string file_name, vector<Edge>& edges, map<string, int>& pathes)
{
	ifstream input(file_name);
	string line;

	while (true)
	{
		getline(input, line);

		if (line == "#")
			break;

		string number(line.begin() + 4, line.end());
		string name = string(1, line[0]) + string(1, line[2]);
		edges.push_back({name, stoi(number)});
	}

	while (getline(input, line))
	{
		line.erase(std::remove(line.begin(), line.end(), '-'), line.end());
		string value;
		getline(input, value);
		pathes[line] = stoi(value);
	}
}

void go(int p, vector<Edge*> edges, int path_number, int car_number)
{
	double population;
	string file_name = to_string(path_number) + "-" + to_string(car_number);
	ofstream output(file_name);

	for (auto& edge : edges)
	{
		edge->acquire();
		chrono::milliseconds enter = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
		population = 0;

		for (size_t i = 0; i <= 10e7; i++)
			population += i / (10e6 * p * edge->get_hardness());

		Edge::acquire_total_emission_semaphore();
		Edge::increase_total_emission(population);
		Edge::release_total_emission_semaphore();

		chrono::milliseconds exit = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch());
		edge->release();

		output << edge->get_name()[0] << "," << enter.count() << "," << edge->get_name()[1] << "," << exit.count() << "," << population << "," << Edge::get_total_emission() << endl;
	}
}