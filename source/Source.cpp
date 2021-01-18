#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <algorithm>

using namespace std;

struct Vec2 {
	int x = 0, y = 0;
	Vec2(int xPos, int yPos) { x = xPos, y = yPos; }
	bool operator==(Vec2 vec) {
		if (x == vec.x && y == vec.y) { return true; }
		else { return false; }
	}
};
struct Chromosome {
	vector <Vec2> pointv;
	double fitness = 0;
	double number = 0;
	double generation = 0;
};
struct Generation {
	vector <Chromosome> population;
	double avgFitness = 0.0;
};

double distance(Vec2 p1, Vec2 p2);
void reproduce(Generation population, vector <Generation> & populationv);
double findMax(vector <Chromosome> &population);
vector <Chromosome> cycle(Chromosome c1, Chromosome c2);
vector <unsigned> select(Generation &population);
double sigma(Generation &vec, int index);
int searchValue(vector <Vec2> vec, Vec2 value);
void mutate(Generation &population, double pMutate);

int main() {
	srand(time(NULL));

	sf::RenderWindow window(sf::VideoMode(900, 600), "Travelling Salesman");

	vector <Vec2> pointv;
	int xMax = window.getSize().x;
	int yMax = window.getSize().y;
	int chromosomeCt = 1000;

	sf::Font sansation;
	sansation.loadFromFile("sansation.ttf");

	sf::Text genInformation;

	int currGen = 0;
	int currChromosome = 0;

	int pointCount = 20;
	for (int i = 0; i < pointCount; i++) pointv.push_back(Vec2(rand() % xMax, rand() % yMax));

	vector <Generation> populationv;

	Generation initPop;
	for (int i = 0; i < chromosomeCt; i++) {
		Chromosome chromosome;
		vector <Vec2> temp = pointv;
		std::random_shuffle(temp.begin(), temp.end());
		chromosome.pointv = temp;

		double totalDistance = 0;
		for (unsigned j = 0; j < chromosome.pointv.size() - 1; j++) {
			totalDistance += distance(chromosome.pointv[j], chromosome.pointv[j + 1]);
		}
		totalDistance += distance(chromosome.pointv[0], chromosome.pointv.back());
		chromosome.fitness = totalDistance;

		initPop.population.push_back(chromosome);

		double avgFitness = 0.0;
		for (unsigned i = 0; i < initPop.population.size(); i++) {
			avgFitness += initPop.population[i].fitness;
		}
		avgFitness /= initPop.population.size();
		initPop.avgFitness = avgFitness;
	}
	populationv.push_back(initPop);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::N) {
					cout << "generating..." << endl;
					for (unsigned i = 0; i < 1; i++) {
						reproduce(populationv.back(), populationv);
						currGen = populationv.size() - 1;
						mutate(populationv[currGen], 0.125);
					}
					cout << "done!" << endl;
				}
				if (event.key.code == sf::Keyboard::M) cout << populationv[currGen].population[currChromosome].fitness << endl;
				if (event.key.code == sf::Keyboard::Period) {
					currChromosome++;
					if (currChromosome == populationv[currGen].population.size()) { currChromosome = 0; }
				}
				if (event.key.code == sf::Keyboard::Comma) {
					currChromosome--;
					if (currChromosome < 0) { currChromosome = populationv[currGen].population.size() - 1; }
				}
				if (event.key.code == sf::Keyboard::Equal) {
					currGen++;
					if (currGen == populationv.size()) { currGen = 0; }
				}
				if (event.key.code == sf::Keyboard::Dash) {
					currGen--;
					if (currGen < 0) { currGen = populationv.size() - 1; }
				}
			}
		}

		window.clear();

		for (unsigned i = 0; i < pointv.size(); i++) {
			sf::CircleShape point(3.0f);
			point.setOrigin(1.5f, 1.5f);
			point.setPosition((float)pointv[i].x, (float)pointv[i].y);
			window.draw(point);
		}

		if (populationv.size() > 0) {
			if (populationv[currGen].population.size() > 0) {
				for (unsigned i = 0; i < populationv[currGen].population[currChromosome].pointv.size() - 1; i++) {
					sf::Vertex line[] = {
						sf::Vertex(sf::Vector2f(populationv[currGen].population[currChromosome].pointv[i].x, populationv[currGen].population[currChromosome].pointv[i].y)),
						sf::Vertex(sf::Vector2f(populationv[currGen].population[currChromosome].pointv[i + 1].x, populationv[currGen].population[currChromosome].pointv[i + 1].y))
					};
					window.draw(line, 2, sf::Lines);
				}
				sf::Vertex line[] = {
					sf::Vertex(sf::Vector2f(populationv[currGen].population[currChromosome].pointv[0].x, populationv[currGen].population[currChromosome].pointv[0].y)),
					sf::Vertex(sf::Vector2f(populationv[currGen].population[currChromosome].pointv.back().x, populationv[currGen].population[currChromosome].pointv.back().y))
				};
				window.draw(line, 2, sf::Lines);
			}
		}

		genInformation.setString("Generation " + std::to_string(currGen + 1) + 
								 "\nOrganism " + std::to_string(currChromosome + 1) + 
								 "\nFitness: " + std::to_string(populationv[currGen].population[currChromosome].fitness) +
								 "\nAverage Distance: " + std::to_string(populationv[currGen].avgFitness)
								 );
		genInformation.setCharacterSize(16);
		genInformation.setFont(sansation);
		
		window.draw(genInformation);

		window.display();
	}

	return 0;
}

void mutate(Generation &population, double pMutate) {
	for (unsigned i = 0; i < population.population.size(); i++) {
		double random = (double)rand() / RAND_MAX;
		if (random < pMutate) {
			unsigned randIndex1 = rand() % population.population[i].pointv.size();
			unsigned randIndex2 = rand() % population.population[i].pointv.size();
			Vec2 temp = population.population[i].pointv[randIndex1];
			population.population[i].pointv[randIndex1] = population.population[i].pointv[randIndex2];
			population.population[i].pointv[randIndex2] = temp;
			double totalDistance = 0;
			for (unsigned j = 0; j < population.population[i].pointv.size() - 1; j++) {
				totalDistance += distance(population.population[i].pointv[j], population.population[i].pointv[j + 1]);
			}
			totalDistance += distance(population.population[i].pointv[0], population.population[i].pointv.back());
			population.population[i].fitness = totalDistance;
		}
	}
}

void reproduce(Generation population, vector <Generation> &populationv) {
	Generation gen;

	for (unsigned i = 0; i < population.population.size(); i++) {
		//population.population[i].fitness = findMax(population.population) * 1.125 - population.population[i].fitness;
		population.population[i].fitness = findMax(population.population) - population.population[i].fitness;
	}

	for (unsigned i = 0; i < population.population.size() / 2; i++) {
		gen.population.push_back(cycle(population.population[select(population)[0]], population.population[select(population)[1]])[0]);
		gen.population.push_back(cycle(population.population[select(population)[0]], population.population[select(population)[1]])[1]);
	}

	for (unsigned i = 0; i < gen.population.size(); i++) {
		double totalDistance = 0;
		for (unsigned j = 0; j < gen.population[i].pointv.size() - 1; j++) {
			totalDistance += distance(gen.population[i].pointv[j], gen.population[i].pointv[j + 1]);
		}
		totalDistance += distance(gen.population[i].pointv[0], gen.population[i].pointv.back());
		gen.population[i].fitness = totalDistance;
	}

	double avgFitness = 0.0;
	for (unsigned i = 0; i < gen.population.size(); i++) {
		avgFitness += gen.population[i].fitness;
	}
	avgFitness /= gen.population.size();
	gen.avgFitness = avgFitness;

	populationv.push_back(gen);
}

vector <Chromosome> cycle(Chromosome c1, Chromosome c2) {
	Chromosome child1 = c1;
	Chromosome child2 = c2;

	vector <vector <int>> cyclev;
	vector <int> indexv;
	for (unsigned i = 0; i < c1.pointv.size(); i++) {
		indexv.push_back(i);
	}

	for (unsigned i = 0; i < indexv.size(); i++) {
		if (indexv[i] != -1) {
			int index = i;
			vector <int> cycle1;
			while (1) {
				cycle1.push_back(index);
				index = searchValue(c1.pointv, c2.pointv[index]);

				if (cycle1.front() == index) break;

				indexv[index] = -1;
			}
			cyclev.push_back(cycle1);
		}
	}

	for (unsigned i = 0; i < cyclev.size(); i+=2) {
		for (unsigned j = 0; j < cyclev[i].size(); j++) {
			Vec2 temp = child1.pointv[cyclev[i][j]];
			child1.pointv[cyclev[i][j]] = child2.pointv[cyclev[i][j]];
			child2.pointv[cyclev[i][j]] = temp;
		}
	}

	vector <Chromosome> offspring;
	offspring.push_back(child1);
	offspring.push_back(child2);

	return offspring;
}

int searchValue(vector <Vec2> vec, Vec2 value) {
	for (unsigned i = 0; i < vec.size(); i++) {
		if (vec[i] == value) {
			return i;
		}
	}
	return -1;
}
vector <unsigned> select(Generation &population) {
	double totalFitness = 0.0;
	for (unsigned i = 0; i < population.population.size(); i++) { totalFitness += population.population[i].fitness; }

	vector <unsigned> indexv;
	for (unsigned a = 0; a < 2; a++) {
		double random = (double)rand() / (RAND_MAX);
		for (unsigned i = 0; i < population.population.size(); i++) {
			if (sigma(population, i) / totalFitness > random) {
				indexv.push_back(i);
				break;
			}
		}
	}
	while (indexv.size() != 2) {
		indexv.push_back(0);
	}

	return indexv;
}
double sigma(Generation &vec, int index) {
	if (index >= 0) { return (sigma(vec, index - 1) + vec.population[index].fitness); }
	else { return 0.0; }
}
double distance(Vec2 p1, Vec2 p2) { return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2)); }
double findMax(vector <Chromosome> &population) {
	double max = 0.0;
	if (population.size() < 1) return 0.0;
	for (unsigned i = 0; i < population.size(); i++) {
		if (population[i].fitness > max) max = population[i].fitness;
	}
	return max;
}