// import visualization libraries {
#include "algorithm-visualizer.h"
// }
#include <vector>
#include <string>
#include <limits>
using json = nlohmann::json;
// Tracers: graph, array1d (S) and log console
GraphTracer tracer = GraphTracer().directed(false).weighted().layoutCircle();
Array1DTracer tracerS = Array1DTracer("Distances");
LogTracer logger = LogTracer("Console");
int main()
{
	const int N = 5;
	// generate random graph like JS example: Randomize.Graph({ N: 5, ratio: 1, directed: false, weighted: true })
	std::vector<int> Gmat(N * N);
	Randomize::Integer randWeight(1, 9);
	Randomize::Graph<int> Ggen(N, 1.0, randWeight);
	// do NOT chain these methods: they return by value, which would modify a temporary
	// (so Ggen would keep its defaults). Call them separately to modify the original.
	Ggen.directed(false);
	Ggen.weighted(true);
	Ggen.fill(Gmat.data());
	// convert to 2D vector for tracer.set
	std::vector<std::vector<int>> G2(N, std::vector<int>(N));
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			G2[i][j] = Gmat[i * N + j];
		}
	}
	// initialize S (distance array) with INF
	const int INF = std::numeric_limits<int>::max();
	std::vector<int> S(N, INF);
	// layout and initial set
	Layout::setRoot(VerticalLayout({tracer, tracerS, logger}));
	tracer.log(logger);
	tracer.set(G2);
	// prepare display array for tracerS: use strings for INF, numbers otherwise
	std::vector<json> Sdisplay(N);
	for (int i = 0; i < N; ++i)
		Sdisplay[i] = "INF";
	tracerS.set(Sdisplay);
	Tracer::delay();
	// pick random start and end (s != e)
	Randomize::Integer randNode(0, N - 1);
	int s = randNode.create();
	int e;
	do
	{
		e = randNode.create();
	} while (e == s);
	logger.println(std::string("finding the shortest path from ") +
				   std::to_string(s) + " to " + std::to_string(e));
	Tracer::delay();
	// Dijkstra
	std::vector<bool> D(N, false);
	S[s] = 0;
	tracerS.patch(s, 0);
	Tracer::delay();
	tracerS.depatch(s);
	tracerS.select(s);
	int k = N;
	while (k--)
	{
		int minIndex = -1;
		int minDistance = INF;
		for (int i = 0; i < N; ++i)
		{
			if (!D[i] && S[i] < minDistance)
			{
				minDistance = S[i];
				minIndex = i;
			}
		}
		if (minDistance == INF)
			break;
		D[minIndex] = true;
		tracerS.select(minIndex);
		tracer.visit(minIndex);
		Tracer::delay();
		for (int i = 0; i < N; ++i)
		{
			if (G2[minIndex][i] && S[i] > S[minIndex] + G2[minIndex][i])
			{
				S[i] = S[minIndex] + G2[minIndex][i];
				tracerS.patch(i, S[i]);
				tracer.visit(i, minIndex, S[i]);
				Tracer::delay();
				tracerS.depatch(i);
				tracer.leave(i, minIndex);
				Tracer::delay();
			}
		}
		tracer.leave(minIndex);
		Tracer::delay();
	}
	if (S[e] == INF)
	{
		logger.println(std::string("there is no path from ") + std::to_string(s) + " to " + std::to_string(e));
	}
	else
	{
		logger.println(std::string("the shortest path from ") + std::to_string(s) + " to " + std::to_string(e) + " is " + std::to_string(S[e]));
	}
	Tracer::delay();
	return 0;
}