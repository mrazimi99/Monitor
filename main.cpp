#include "commons.h"

int main()
{
	vector<Edge> edges;
	map<string, int> pathes;
	vector<thread> cars;
	srand(time(NULL));
	parse_input("input.in", edges, pathes);
	int car_number = 0;
	int path_number = 0;

	for (auto const& path : pathes)
	{
		vector<Edge*> this_edges;

		for (int index = 1; index < path.first.length(); ++index)
		{
			string name = string(1, path.first[index - 1]) + string(1, path.first[index]);

			for (auto& edge : edges)
			{
				if (name == edge.get_name())
					this_edges.push_back(&edge);
			}
		}

		for (int i = 0; i < path.second; ++i)
		{
			cars.push_back(thread(go, (rand() % 10) + 1, this_edges, path_number, car_number));
			++car_number;
		}

		++path_number;
	}

	for (auto& car : cars)
		car.join();

	Edge::release_total_emission_semaphore();
}