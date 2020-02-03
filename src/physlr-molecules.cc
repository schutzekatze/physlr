#include <fstream>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <utility>
#include <functional>

#include <stdint.h>
#include <chrono>
#include <tgmath.h>
#include <stdexcept>
#include <algorithm>

//#include <boost/config.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/biconnected_components.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>

#if _OPENMP
#include <omp.h>
#endif

#define PROGRAM "physlr-molecules"
#define PACKAGE_NAME "physlr"
#define GIT_REVISION "pre-autotools"

static uint64_t

// typedef boost::adjacency_matrix< undirectedS > MatrixGraph

memory_usage()
{
	int mem = 0;
	std::ifstream proc("/proc/self/status");
	for (std::string s; std::getline(proc, s);) {
		if (s.substr(0, 6) == "VmSize") {
			std::stringstream convert(s.substr(s.find_last_of('\t'), s.find_last_of('k') - 1));
			if (!(convert >> mem)) {
				return 0;
			}
			return mem;
		}
	}
	return mem;
}

struct vertexProperties
{
	std::string name = "";
	int weight = 0;
	size_t indexOriginal = 0;
};

struct edgeProperties
{
	int weight = 0;
};

struct edgeComponent_t
{
	enum
	{
		num = INT_MAX
	};
	using kind = boost::edge_property_tag;
} edgeComponent;


using namespace std;
using namespace std::chrono;

// this definition assumes there is no redundant edge in the undirected graph
using graph_t = boost::subgraph<boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::undirectedS,
    vertexProperties,
    boost::property<
        boost::edge_index_t,
        int,
        boost::property<edgeComponent_t, std::size_t, edgeProperties>>>>;
using vertex_t = graph_t::vertex_descriptor;
using edge_t = graph_t::edge_descriptor;
using barcodeToIndex_t = std::unordered_map<std::string, vertex_t>;
using indexToBarcode_t = std::unordered_map<vertex_t, std::string>;
using vertexSet_t = std::unordered_set<vertex_t>;
using componentToVertexSet_t = std::vector<vertexSet_t>;
using vertexToComponent_t = std::unordered_map<vertex_t, size_t>;
using vecVertexToComponent_t = std::vector<vertexToComponent_t>;
using vertexToIndex_t = std::unordered_map<vertex_t, size_t>; // wanna improve this? checkout boost::bimap
using indexToVertex_t = std::unordered_map<size_t, vertex_t>; // wanna improve this? checkout boost::bimap
using adjacencyMatrix_t = std::vector<std::vector<uint_fast32_t>>;
using adjacencyVector_t = std::vector<uint_fast32_t>;
using Clique_type = std::unordered_map<vertex_t, size_t>;

#define timeNow() std::chrono::high_resolution_clock::now()

struct cliques_visitor {
    // This is the visitor that will process each clique found by bron_kerbosch algorithm
    // Clique_type: std::set<Vertex> or std::unordered_map<Vertex> or etc (must support .insert(), prefer unordered)

    cliques_visitor(vector<Clique_type>& cliquesVec)
        : cliquesVec(cliquesVec) {
        // You may reserve some memory to avoid reallocation
    }

    // this is called each time a clique is found
    template <typename Clique, typename Graph>
    void clique(const Clique& clique, const Graph& g)
    {
        Clique_type current_clique;
        // Clique_type current_clique(clique);
        // Clique_type current_clique(clique.begin(), clique.end());
        if (clique.size() > 2)
        {
            for(auto it = clique.begin(); it != clique.end(); ++it)
            {
                // May need changes
                current_clique[*it] = 1;
            }
            cliquesVec.push_back(current_clique);
        }
    }

    std::vector<Clique_type>& cliquesVec;
};


static void
printVersion()
{
	const char VERSION_MESSAGE[] =
	    PROGRAM " (" PACKAGE_NAME ") " GIT_REVISION "\n"
	            "Written by Johnathan Wong and Amirhossein Afshinfard.\n"
	            "\n"
	            "Copyright 2019 Canada's Michael Smith Genome Science Centre\n";
	std::cerr << VERSION_MESSAGE << std::endl;
	exit(EXIT_SUCCESS);
}

static void
printErrorMsg(const std::string& progname, const std::string& msg)
{
	std::cerr << progname << ": " << msg
	          << "\nTry 'physlr-molecules --help' for more information.\n";
}

static void
printUsage(const std::string& progname)
{
	std::cout << "Usage:  " << progname
	          << "  [-s SEPARATION-STRATEGY] [-v] FILE...\n\n"
	             "  -v         enable verbose output\n"
	             "  -s --separation-strategy   \n"
	             "  SEPARATION-STRATEGY      `+` separated list of molecule separation strategies "
	             "[bc]\n"
	             "  --help     display this help and exit\n";
}

void
printGraph(const graph_t& g)
{
	std::cout << "U\tm" << std::endl;
	auto vertexItRange = boost::vertices(g);
	for (auto vertexIt = vertexItRange.first; vertexIt != vertexItRange.second; ++vertexIt) {
		auto& node1 = g[*vertexIt].name;
		auto& weight = g[*vertexIt].weight;
		std::cout << node1 << "\t" << weight << "\n";
	}
	std::cout << "\nU\tV\tm" << std::endl;
	auto edgeItRange = boost::edges(g);
	for (auto edgeIt = edgeItRange.first; edgeIt != edgeItRange.second; ++edgeIt) {
		auto& weight = g[*edgeIt].weight;
		auto& node1 = g[boost::source(*edgeIt, g)].name;
		auto& node2 = g[boost::target(*edgeIt, g)].name;
		std::cout << node1 << "\t" << node2 << "\t" << weight << "\n";
	}
}

void
readTSV(graph_t& g, const std::vector<std::string>& infiles, bool verbose)
{
	auto progname = "physlr-molecules";
	std::cerr << "Loading graph" << std::endl;
#if _OPENMP
	double sTime = omp_get_wtime();
#endif
	barcodeToIndex_t barcodeToIndex;
	indexToBarcode_t indexToBarcode;
	for (auto& infile : infiles) {
		infile == "-" ? "/dev/stdin" : infile;
		std::ifstream infileStream(infile);
		for (std::string line; std::getline(infileStream, line);) {
			if (line == "U\tm") {
				continue;
			}
			if (line.empty()) {
				break;
			}
			std::string node1;
			int weight;
			std::istringstream ss(line);
			if (ss >> node1 >> weight) {
				auto u = boost::add_vertex(g);
				g[u].name = node1;
				g[u].weight = weight;
				g[u].indexOriginal = u;
				barcodeToIndex[node1] = u;
				indexToBarcode[u] = node1;
			} else {
				printErrorMsg(progname, "unknown graph format");
				exit(EXIT_FAILURE);
			}
		}

		if (verbose) {
			std::cerr << "Loaded vertices to graph ";
#if _OPENMP
			std::cerr << "in sec: " << omp_get_wtime() - sTime << std::endl;
			sTime = omp_get_wtime();
#endif
		}
		for (std::string line; std::getline(infileStream, line);) {
			if (line == "U\tV\tm") {
				continue;
			}
			if (line.empty()) {
				printErrorMsg(progname, "unknown graph format");
				exit(EXIT_FAILURE);
			}
			std::string node1, node2;
			int weight;
			std::istringstream ss(line);
			if (ss >> node1 >> node2 >> weight) {
				auto E = boost::add_edge(barcodeToIndex[node1], barcodeToIndex[node2], g).first;
				g[E].weight = weight;
			} else {
				printErrorMsg(progname, "unknown graph format");
				exit(EXIT_FAILURE);
			}
		}

		if (verbose) {
			std::cerr << "Loaded edges to graph ";
		} else {
			std::cerr << "Loaded graph ";
		}
#if _OPENMP
		std::cerr << "in sec: " << omp_get_wtime() - sTime << std::endl;
		sTime = omp_get_wtime();
#endif
		std::cerr << "Memory usage: " << double(memory_usage()) / double(1048576) << "GB"
		          << std::endl;
	}
}

/* Generate a molecule separated graph (molSepG) using component/community information from
molecule separation (vecVertexToComponent). The input graph (inG) is the barcode overlap graph
or a molecule separated graph from the previous round of molecule separation.*/
void
componentsToNewGraph(
    const graph_t& inG,
    graph_t& molSepG,
    vecVertexToComponent_t& vecVertexToComponent)
{
	barcodeToIndex_t molSepGBarcodeToIndex;
#if _OPENMP
	double sTime = omp_get_wtime();
#endif
	for (size_t i = 0; i < vecVertexToComponent.size(); ++i) {

		size_t maxVal = 0;
		if (!vecVertexToComponent[i].empty()) {
			maxVal =
			    std::max_element(
			        vecVertexToComponent[i].begin(),
			        vecVertexToComponent[i].end(),
			        [](const vertexToComponent_t::value_type& p1,
			           const vertexToComponent_t::value_type& p2) { return p1.second < p2.second; })
			        ->second;
		}

		for (size_t j = 0; j < maxVal + 1; ++j) {
			auto u = boost::add_vertex(molSepG);
			molSepG[u].name = inG[i].name + "_" + std::to_string(j);
			molSepG[u].weight = inG[i].weight;
			molSepG[u].indexOriginal = u;
			molSepGBarcodeToIndex[molSepG[u].name] = u;
		}
	}

	auto edgeItRange = boost::edges(inG);
	for (auto edgeIt = edgeItRange.first; edgeIt != edgeItRange.second; ++edgeIt) {
		auto& u = inG[boost::source(*edgeIt, inG)].indexOriginal;
		auto& v = inG[boost::target(*edgeIt, inG)].indexOriginal;

		if (vecVertexToComponent[u].find(v) == vecVertexToComponent[u].end() ||
		    vecVertexToComponent[v].find(u) == vecVertexToComponent[v].end()) {
			continue;
		}

		auto& uMolecule = vecVertexToComponent[u][v];
		auto& vMolecule = vecVertexToComponent[v][u];
		auto uName = inG[u].name + "_" + std::to_string(uMolecule);
		auto vName = inG[v].name + "_" + std::to_string(vMolecule);
		auto e =
		    boost::add_edge(molSepGBarcodeToIndex[uName], molSepGBarcodeToIndex[vName], molSepG)
		        .first;
		molSepG[e].weight = inG[*edgeIt].weight;
	}

	std::cerr << "Generated new graph ";
#if _OPENMP
	std::cerr << "in sec: " << omp_get_wtime() - sTime << std::endl;
	sTime = omp_get_wtime();
#endif

	std::cerr << "Memory usage: " << double(memory_usage()) / double(1048576) << "GB" << std::endl;
}

void
biconnectedComponents(graph_t& subgraph, vertexToComponent_t& vertexToComponent)
{
	// Find biconnected components
	boost::property_map<graph_t, edgeComponent_t>::type component =
	    boost::get(edgeComponent, subgraph);

	std::vector<vertex_t> artPointsVec;
	boost::biconnected_components(subgraph, component, std::back_inserter(artPointsVec));

	vertexSet_t artPoints(artPointsVec.begin(), artPointsVec.end());

	// Remove articulation points from biconnected components
	boost::graph_traits<graph_t>::edge_iterator ei, ei_end;
	componentToVertexSet_t componentToVertexSet;

	for (boost::tie(ei, ei_end) = boost::edges(subgraph); ei != ei_end; ++ei) {
		size_t componentNum = component[*ei];
		if (componentNum + 1 > componentToVertexSet.size()) {
			componentToVertexSet.resize(componentNum + 1);
		}

		auto node1 = source(*ei, subgraph);
		auto node2 = target(*ei, subgraph);

		if (artPoints.find(node1) == artPoints.end()) {
			componentToVertexSet[componentNum].insert(subgraph[node1].indexOriginal);
		}
		if (artPoints.find(node2) == artPoints.end()) {
			componentToVertexSet[componentNum].insert(subgraph[node2].indexOriginal);
		}
	}

	size_t moleculeNum = 0;

	// Remove components with size less than 2
	for (auto&& vertexSet : componentToVertexSet) {
		if (vertexSet.size() <= 1) {
			continue;
		}
		for (auto&& vertex : vertexSet) {
			vertexToComponent[vertex] = moleculeNum;
		}
		++moleculeNum;
	}
}

// Tools for cos_sim and k_cliques

void
square_adjacency_list(){
    // with no adjacency matrix:
    // starting with an adjacency list, connect each vertex into its 2nd order neighbors (only)
    // equivalent to transforming into adjacency matrix, squaring and then converting back.
}

adjacencyMatrix_t
convert_adj_list_adj_mat(graph_t& subgraph, vertexToIndex_t& vertexToIndex)
{
    // Inputs:
    // - subgraph: adjacency list to convert to adjacency list
    // - vertexToIndex: (empty, to be filled in)
    //      Dictionary of (vertex name) -> (index in temporary adjacency matrix)
    // Ouput(s):
    // - adj_mat: the adjacency matrix for subgraph
    // - vertexToIndex (referenced input)

    int N = boost::num_vertices(subgraph);
    cout<<"  inner subgraph size:"<<N<<endl;
    adjacencyVector_t tempVector(N, 0);
    adjacencyMatrix_t adj_mat(N, tempVector);

    // CHANGE &:
    //boost::property_map<graph_t, edge_weight_t>::type weight =
    //    boost::get(edge_weight, subgraph);

    // typedef graph_traits<Graph>::edge_iterator edge_iterator;
    typedef boost::graph_traits<graph_t>::edge_iterator edge_iterator;

    pair<edge_iterator, edge_iterator> ei = edges(subgraph);

    vertexToIndex_t::iterator got_a;
    vertexToIndex_t::iterator got_b;
    size_t adj_mat_index = 0;
    for (edge_iterator edge_iter = ei.first; edge_iter != ei.second; ++edge_iter)
    {
        vertex_t a = source(*edge_iter, subgraph);
        vertex_t b = target(*edge_iter, subgraph);
        // if not visited a or b
        //      add to dictionary
        // Could be more efficient by adding a "visited" property to vertices of the graph
        // Now we implement by hash table lookup:
        // std::unordered_map<std::string,double>::const_iterator got_a = vertexToIndex.find(a)
        got_a = vertexToIndex.find(a);
        size_t index_a;
        if ( got_a == vertexToIndex.end() )
        {
            vertexToIndex.insert (std::pair<vertex_t, size_t>(a, adj_mat_index));
            index_a = adj_mat_index++;
        }
        else
        {
            index_a = got_a -> second;
        }
        // std::unordered_map<std::string,double>::const_iterator got_b = vertexToIndex.find(b)
        got_b = vertexToIndex.find(b);
        size_t index_b;
        if ( got_b == vertexToIndex.end() )
        {
            vertexToIndex.insert (std::pair<vertex_t, size_t>(b, adj_mat_index));
            index_b = adj_mat_index++;
        }
        else
        {
            index_b = got_b -> second;
        }
        // CHANGE &:
        //adj_mat[index_a][index_b] = boost::get(weight, *edge_iter);
        //adj_mat[index_b][index_a] = boost::get(weight, *edge_iter);
        adj_mat[index_a][index_b] = (int)subgraph[*edge_iter].weight;
        adj_mat[index_b][index_a] = adj_mat[index_a][index_b];
    }
    return adj_mat; // Cannot return two, add one as input that's being altered
    // and care how you use this function elsewhere!
}

// // Functions related to cosine similarity:
// squaring matrix, algorithms: ijk and ikj and boost

adjacencyMatrix_t
square_matrix_ijk(
    adjacencyMatrix_t M,
    bool symmetric_output=true)
{
    // Compute M.M^T, not M.M
    int n = M.size();

    adjacencyVector_t tempVector(n, 0); // Fast initialization
    adjacencyMatrix_t M2(n, tempVector);

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if ( j < i && symmetric_output )
            {
                M2[i][j] = M2[j][i];
                continue;
            }
            for (int k = 0; k < n; k++)
            {
                // second matrix is transposed implicitly
                M2[i][j] += M[i][k] * M[j][k];
            }
        }
    }
    return M2;
}

adjacencyMatrix_t
square_matrix_ijk2(
    adjacencyMatrix_t M,
    bool symmetric=true)
{   // NOT FINISHED YET!
    int n = M.size();

    adjacencyVector_t tempVector(n, 0); // Fast initialization
    adjacencyMatrix_t M2(n, tempVector);
    adjacencyMatrix_t::iterator M_iter = M.begin();
    adjacencyMatrix_t::iterator M2_iter = M2.begin();

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if ( j < i && symmetric )
            {
                M2[i][j] = M2[j][i];
                continue;
            }
            for (int k = 0; k < n; k++) {
                // second matrix is transposed implicitly
                M2[i][j] += M[i][k] * M[j][k];
            }
        }
    }
    return M2;
}

vector<vector<double> >
square_matrix_ijk(
    vector<vector<double> > M,
    bool symmetric_output=true)
{
    // Compute M.M^T (not M.M)
    int n = M.size();

    vector<double> tempVector(n, 0.0); // Fast initialization
    vector<vector<double> > M2(n, tempVector);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if ( j < i && symmetric_output )
            {
                M2[i][j] = M2[j][i];
                continue;
             }
            for (int k = 0; k < n; k++)
            {
                // second argument is transposed implicitly
                M2[i][j] += M[i][k] * M[j][k];
            }
        }
    }
    return M2;
}

adjacencyMatrix_t
square_matrix_ikj( // Might be faster than ijk, benchmark it
    adjacencyMatrix_t M,
    bool symmetric=true)
{
    // Square the input matrix iterating i, k, then j

    // Fast initialization:
    int n = M.size();
    adjacencyVector_t tempVector(n, 0);
    adjacencyMatrix_t M2(n, tempVector);
    // Multiplication
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
//            if ( !M[i][k] )
//                continue;
            for (int j = 0; j < n; j++) {
                if ( j < i && symmetric ) {
                    M2[i][j] = M2[j][i];
                    continue;
                }
                M2[i][j] += M[i][k] * M[k][j];
            }
        }
    }
    return M2;
}

vector<vector<double> >
square_matrix_ikj( // Might be faster than ijk, benchmark it
    vector<vector<double> > M,
    bool symmetric=true)
{
    // Square the input matrix iterating i, k, then j

    // Fast initialization:
    int n = M.size();
    vector<double> tempVector(n, 0.0); // Fast initialization
    vector<vector<double> > M2(n, tempVector);
    // Multiplication
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            if ( !M[i][k] )
                // one side of multip. is zero, so skip
                continue;
            for (int j = 0; j < n; j++) {
                if ( j < i && symmetric ) {
                    M2[i][j] = M2[j][i];
                    continue;
                }
                M2[i][j] += M[i][k] * M[k][j];
            }
        }
    }
    return M2;
}


//boost::numeric::ublas::matrix<int>
//square_matrix_boost(
//    adjacencyMatrix_t M)
//{
//    return boost::numeric::ublas::prod(M, M);
//}

inline double
cosine_similarity_vectors(
    adjacencyMatrix_t::iterator& row_i,
    adjacencyMatrix_t::iterator& row_j){
    // Input: 2 vectors (1D) as rows and columns of a Matrix
    // Output: Cosine similarity of the two vectors
    // (Cosine Similarity between 2 corresponding vertices)

    float mul = 0.0; // also test double
    float d_i = 0.0;
    float d_j = 0.0;

    if (row_i->size() != row_j->size())
    {
        throw std::logic_error("Vector A and Vector B are not the same size");
    }

    // Prevent Division by zero
    if (row_i->size() < 1)
    {
        throw std::logic_error("Input vectors for multiplication are empty");
    }

    adjacencyVector_t::iterator i_iter = row_i->begin();
    adjacencyVector_t::iterator j_iter = row_j->begin();
    for( ; i_iter != row_i->end(); i_iter++ , j_iter++ )
    {
        mul += *i_iter * *j_iter;
        d_i += *i_iter * *i_iter;
        d_j += *j_iter * *j_iter;
        // cout<<"\nDebug - mul:"<<mul<<" - d_i:"<<d_i<<" - d_j:"<<d_j<<endl;
    }
    if (mul == 0.0f)
    {
        return 0;
    }
    if (d_i == 0.0f || d_j == 0.0f)
    {
        return 0;
//        throw std::logic_error(
//                "cosine similarity is not defined whenever one or both "
//                "input vectors are zero-vectors.");
    }
    //return mul / (sqrt(d_a) * sqrt(d_b));
    return mul / sqrt(d_i * d_j);
}

inline
void
calculate_cosine_similarity_2d(
    adjacencyMatrix_t adj_mat, // CHANGE: to a reference!
    vector<vector<double>>& cosimilarity)
{
    // calculate the cosine similarity of the input 2d-matrix with itself
    // Strategy: row-normalize then square the matrix.

    int n = adj_mat.size();
    vector<double> temp(n, 0.0);
    vector<vector<double> > normalized(n, temp);
    double row_sum = 0;
    uint_fast32_t init = 0;

    adjacencyMatrix_t::iterator row_i;
    vector<vector<double> >::iterator normalized_row_i = normalized.begin();
    for (row_i = adj_mat.begin(); row_i != adj_mat.end(); ++row_i, ++normalized_row_i)
    {
        row_sum = 0;
        vector<uint_fast32_t>::iterator first = row_i->begin();
        vector<uint_fast32_t>::iterator last = row_i->end();
        while(first!=last){
            row_sum += *first * *first;
            row_sum += *first * *first;
            ++first;
        }

        first = row_i->begin();
        vector<double>::iterator first_normalized = normalized_row_i->begin();
        vector<double>::iterator last_normalized = normalized_row_i->end();
        while(first!=last){
            *first_normalized = *first / sqrt(1.0 * row_sum);
            ++first;
            ++first_normalized;
        }
    }
//    cosimilarity = square_matrix_ijk(normalized);
    cosimilarity = square_matrix_ikj(normalized);
}

inline
void
calculate_cosine_similarity_2d_v2(
    adjacencyMatrix_t& adj_mat,
    vector<vector<double> >& cosimilarity)
{
    // Assumptions: the input matrix is symmetric and cubic
    // This function calculate the 2-dimensional cosine similarity of the input matrix
    // to itself, that is the similarity between vertices of the corresponding graph
    // for the input matrix (as adj matrix)

    adjacencyMatrix_t::iterator row_i;
    adjacencyMatrix_t::iterator row_j;
    vector<vector<double> >::iterator out_row = cosimilarity.begin();
    vector<double>::iterator out_col = out_row->begin();
    if (adj_mat.size() != cosimilarity.size())
    {
       cout<<" Error: Input Matrices are not of the same size.";
       exit (EXIT_FAILURE);
    }

    int i = 0;
    int j = 0;

    for (row_i = adj_mat.begin(); row_i != adj_mat.end(); ++row_i)
    {
        j = 0;
        for (row_j = adj_mat.begin(); row_j != adj_mat.end(); ++row_j)
        {
            if (j < i)
            {
                cosimilarity[i][j] = cosimilarity[j][i];
            }
            else
            {
                cosimilarity[i][j] = cosine_similarity_vectors(row_i, row_j);
            }
            j += 1;
        }
        i += 1;
    }
}

template<typename K, typename V>
std::unordered_map<V,K> inverse_map(std::unordered_map<K,V> &map)
{
	std::unordered_map<V,K> inverse;
	for (const auto &p: map) {
		inverse.insert(std::make_pair(p.second, p.first));
	}
	return inverse;
}

void
community_detection_cosine_similarity(
    graph_t& subgraph, vertexToComponent_t& vertexToComponent,
    bool squaring = true, double threshold=0.1)
{
    // Detect communities using cosine similarity of vertices

    // 1- Calculate the cosine similarity:
//    vertexToIndex_t vertexToIndex(num_vertices(subgraph));
    vertexToIndex_t vertexToIndex;
    vertexToIndex.reserve(boost::num_vertices(subgraph));
    cout<<"size of vertexToInex: "<<vertexToIndex.size()<<endl;

    adjacencyMatrix_t adj_mat(convert_adj_list_adj_mat(subgraph, vertexToIndex));
    indexToVertex_t indexToVertex = inverse_map(vertexToIndex);
    cout<<" size of subgraph:"<<num_vertices(subgraph)<<endl;
    cout<<" size of adj_mat:"<<adj_mat.size();
    cout<<" size of vertexToInex: "<<vertexToIndex.size()<<"\n size of indexToVer: "<<indexToVertex.size()<<endl;

    int size_adj_mat = adj_mat.size();
    vector<double> tempVector(size_adj_mat, 0);
    vector<vector<double>> cosSimilarity2d(size_adj_mat, tempVector);

    if (squaring)
        calculate_cosine_similarity_2d(square_matrix_ikj(adj_mat, true),
                                        // may need some change
                                        //square_matrix_ijk(adj_mat, true),
                                        //square_matrix_boost(adj_mat),
                                        cosSimilarity2d);
    else
        calculate_cosine_similarity_2d(adj_mat, cosSimilarity2d);
    // 2- Determine the threshold:
    // not implemented yet; so use a predefined universal threshold.
    threshold = threshold;

    // 3- Filter out edges:
    for (int i = 0; i < adj_mat.size() ; i++)
    {
        for (int j = i+1; j < adj_mat.size() ; j++)
            {
                if (cosSimilarity2d[i][j] < threshold)
                {
                    adj_mat[i][j] = 0;
                    adj_mat[j][i] = 0;
                }
            }
    }
    // 4- Detect Communities (find connected components - DFS)
    //      Alternative implementation: convert to adjacency list and use boost to find cc

    // / use .reserve to set the capacity of the below 2d vector instead of initialization
//    const int max_communities = 100;
//    vector<vector<uint_fast32_t>> communities(max_communities,vector<uint_fast32_t>(adj_mat.size(),-1));

    size_t community_id = 0;
    stack<size_t> toCheck;
    //unordered_map<int, int>;
    vector<int> zeros(adj_mat.size(),0);
    vector<int> isDetected(adj_mat.size(),0);
    //vector<int> isVisited(zeros);
    bool isSingleton = false;
    //cout<<"NEW TRACE: "<<adj_mat.size()<<"\n";
    for (size_t i = 0 ; i < adj_mat.size(); i++)
    {
        //cout<<"i : "<<i<<endl;
        // DFS traversal
        if (isDetected[i])
            continue; // this node is included in a community already.
        toCheck.push(i);
        isDetected[i] = 1;
        isSingleton = true;
        //cout<<"entered"<<endl;
        size_t ii;

        while(!toCheck.empty()){

            ii = toCheck.top();
            toCheck.pop();
            // /communities[community_id].push_back(ii);

            auto vt = indexToVertex.find(ii);
            //cout<<"ii : "<<ii<<":"<<vt->second<<" | ";
            //vertexToComponent.insert (std::pair<vertex_t, size_t>(vt->second, community_id));
            if (vt != indexToVertex.end())
                vertexToComponent[vt->second] = community_id;
                //componentToVertexSet[componentNum].insert(subgraph[node1].indexOriginal);
            else
            {
                cout<<"BIG BUG on "<<ii<<" when processing "<<i<<"| Size of dict: "<<indexToVertex.size()<<endl;

            }
//            if (vt != indexToVertex.end())
//                vertexToComponent.insert (std::pair<vertex_t, size_t>(vt->second, community_id));

            for (size_t j = 0 ; j < adj_mat.size(); j++)
            {
                if (isDetected[j])
                    continue; // this node is included in a community already.
                if (adj_mat[ii][j] > 0){
                    toCheck.push(j);
                    isDetected[j]=1;
                    isSingleton = false;
                }
            }
        }
        //cout<<"\n";
        if (isSingleton)
        {
            // /communities[community_id].pop_back();
            auto vt = indexToVertex.find(i);
            if (vt != indexToVertex.end())
                cout<<"were supposed to erase something"<<endl;
                vertexToComponent.erase ( indexToVertex.find(i) );
            else{
                //
                cout<<"\nCould not find this one in the dict:"<<i<<endl;
                continue;
            }
//            ++community_id;
        }
        else
            ++community_id;
//        ++community_id;
    }
}

int
inline share_edges(Clique_type& a, Clique_type& b)
{
    // whether two cliques share at least an edge.
    // two cliques (Clique_type) share edges if they share at least 2 vertices.
    unsigned int count = 0;
    for (Clique_type::iterator it = a.begin(); it != a.end(); ++it)
    {
//        cout<<decltype(it->first);
        if (b.count(it->first))
        {
            count++;
//            cout<<"connection -"<<endl;
        }
        if (count > 1)
        {
//            cout<<"count > 1"<<endl;
            return 1;
        }
    }
    // count < 2
    return 0;
}

void
Community_detection_k3_cliques(
    graph_t& subgraph, vertexToComponent_t& vertexToComponent,
    int k = 3)
{
    // k-cliques community detection in case of k=3
    // based on matrix multiplication
    if (k != 3)
    {
        cout<<" This implementation of k-cliques does not support any k other than 3.";
        exit (EXIT_FAILURE);
    }

    /// TEST WHICH IS FASTER:
    /// 1- MATRIX MULTIPLICATION TO FIND TRIANGLES?
//    vertexToIndex_t vertexToIndex(num_vertices(subgraph));
//    adjacencyMatrix_t adj_mat(convert_adj_list_adj_mat(subgraph, vertexToIndex));
//    indexToVertex_t indexToVertex = inverse_map(vertexToIndex);
//
//    size_t size_adj_mat = adj_mat.size();
//    typedef vector< tuple<int, int, int> > triangleVector_t;
//
//    adjacencyMatrix_t squared_adj_mat(square_matrix_ijk(adj_mat));
//    const int adj_mat_size = adj_mat.size();
//    triangleVector_t triangleVector;
//    for (int i = 0; i < adj_mat_size; i++)
//    {
//        for (int j = i+1; j < adj_mat_size; j++)
//        {
//            if ( adj_mat[i][j] > 0 && squared_adj_mat[i][j] > 0 )
//            {
//                // There are triangles comprises of vertices i, j; find the 3rd vertices.
//                for (int k = 0; k < adj_mat_size; k++)
//                {
//                    if ( adj_mat[k][i] > 0 && adj_mat[k][j] > 0 )
//                    {
//                        triangleVector.push_back(tuple<int, int, int>(i, j, k));
//                    }
//                }
//
//            }
//        }
//    }
//    // NEW IDEA: WHEN YOU FIND TRIANGLES LAYING ON AN EDGE, CONNECT ALL AS ADJACENT in a MATRIX. THEN DFS ITERATE the MATRIX
//    // Now go over these triangles to mix them.
//    // ...

    /// 2- GRAPH TRAVERSAL TO FIND TRIANGLES (without squaring)

    /// 3- MATRIX TO VECTOR CONVERSION + BITWISE AND ON INTEGERS (compacted vectors)?

    /// 4- NORMAL K-CLIQUE DETECTION using boost
    // vector<vector<int> > vecs(3,vector<int>(5));
    // std::vector<Clique_type> allCliquesVec(100, Clique_type(1000));
    std::vector<Clique_type> allCliquesVec;
//    allCliquesVec.resize(100);                              // resize the columns
//    for (auto &row : allCliquesVec) { row.reserve(1000); }   // resize the rows
    cliques_visitor visitor(allCliquesVec);
    // - use the Bron-Kerbosch algorithm to find all cliques
    boost::bron_kerbosch_all_cliques(subgraph, visitor);

    // - find adjacent cliques sharing 2 vertices at least (one edge at least)
    size_t cliquesCount = allCliquesVec.size();
    //cout<<"size of allCliqueVec: "<<cliquesCount<<endl;
    vector<vector<int>> connections(cliquesCount,vector<int>(cliquesCount,0));
    for (size_t i = 0; i < cliquesCount; i++)
    {
        //cout<<"\t - size of allCliqueVec[i]: "<<allCliquesVec[i].size()<<endl;
        if (allCliquesVec[i].size() < 3)
            continue; // not a 3-clique
        for (size_t j = i; j < cliquesCount; j++)
        {
            if (connections[i][j] > 0)
                continue; // already connected
            // if indirectly_connected(allCliquesVec[i], allCliquesVec[j]), continue; (may be more efficient)
            if (allCliquesVec[j].size() < 3)
                continue; // not a k3-clique
            if (share_edges(allCliquesVec[i], allCliquesVec[j]))
            {
                connections[i][j] = 1;
            }
        }
    }

    // - percolate over (DFS) adjacent cliques and mix: k3-cliques
    //std::vector<Clique_type> k3CliquesVec(100, Clique_type(1000));
    std::vector<Clique_type> k3CliquesVec;
    k3CliquesVec.resize(100);                              // resize the columns
    for (auto &row : k3CliquesVec) { row.reserve(1000); }   // resize the rows

    size_t community_id = 0;
    stack<size_t> toCheck;
    vector<size_t> isDetected(cliquesCount,0);
    //int max_communities = 100;
    // vector<vector<size_t>> communities(max_communities,vector<uint_fast32_t>(cliquesCount(),-1));
    for (size_t i = 0 ; i < cliquesCount; i++)
    {
        // DFS traversal
        if (isDetected[i])
            continue; // this clique set is added to a community already.
        toCheck.push(i);
        isDetected[i] = 1;
        size_t ii = -1;

        while(!toCheck.empty()){

            ii = toCheck.top();
            toCheck.pop();
            //communities[community_id].push_back(ii);
            //k3CliquesVec[community_id].insert(allCliquesVec[ii].begin(), allCliquesVec[ii].end());
            //for (size_t jj = 0; jj < allCliquesVec[ii].size(); jj++ ){
            //    cout<<"Community ID being assigned to: "<<ii<<" - "<<jj<<"- id: "<<community_id<<endl;
            //    vertexToComponent[allCliquesVec[ii][jj]] = community_id;
            //}
            for (auto& x:allCliquesVec[ii])
            {
                //cout<<"Community ID being assigned to: "<<ii<<" - "<<"- id: "<<community_id<<endl;
                vertexToComponent[x.first] = community_id;
            }

            for (size_t j = 0 ; j < cliquesCount; j++)
            {
                if (isDetected[j])
                    continue; // this clique is included in this community already.
                if (connections[ii][j] > 0){
                    toCheck.push(j);
                    isDetected[j]=1;
                }
            }
        }
        community_id++;
    }
    // in case you use k3CliquesVec: iterate over communities and add them to vertexToComponent
//    for (int i = 0; i < k3CliquesVec.size(); i++)
//    {
//        for (int j = 0; j < k3CliquesVec[i].size(); j++)
//            {   // inexact, CHANGE: if exist vertexToComponent[k3CliquesVec[i][j]] then add i.
//                vertexToComponent[k3CliquesVec[i][j]] = i;
//            }
//    }
    /// 5- K-CLIQUE DETECTION using Google OR-Tools

    /// 6- K-CLIQUE DETECTION using Cliquer
}

//template <class Neighbours_Type>
//void
//bin_neighbours(Neighbours_Type neighbours, vector<Neighbours_Type>& binned_neighbours, size_t = 50)
//{
//    // vector<vector<int> > vecs(3,vector<int>(5));
//    // #reserve #reserve() reserve memory
//
//    // bin_neighbours(neighbours, binned_neighbours);
//    size_t neighborhood_size = 0;
//	for (auto neigh_it = neighbours.first; neigh_it < neighbours.second; ++neigh_it){
//	    neighborhood_size++;
//	}
//	binned_neighbours.resize(neighborhood_size);
//
//}

int
main(int argc, char* argv[])
{
	auto progname = "physlr-molecules";
	int optindex = 0;
	static int help = 0;
	std::string separationStrategy = "bc";
	bool verbose = false;
	bool failed = false;
	static const struct option longopts[] = {
		{ "help", no_argument, &help, 1 },
		{ "separation-strategy", required_argument, nullptr, 's' },
		{ nullptr, 0, nullptr, 0 }
	};
	for (int c; (c = getopt_long(argc, argv, "s:v", longopts, &optindex)) != -1;) {
		switch (c) {
		case 0:
			break;
		case 's':
			separationStrategy.assign(optarg);
			break;
		case 'v':
			verbose = true;
			break;
		default:
			exit(EXIT_FAILURE);
		}
	}
	std::vector<std::string> infiles(&argv[optind], &argv[argc]);
	if (argc < 1) {
		failed = true;
	}
	if (help != 0) {
		printVersion();
		printUsage(progname);
		exit(EXIT_SUCCESS);
	} else if (infiles.empty()) {
		printErrorMsg(progname, "missing file operand");
		failed = true;
	}
	if (separationStrategy != "bc") {
		printErrorMsg(progname, "unsupported molecule separation strategy");
		failed = true;
	}
	if (failed) {
		printUsage(progname);
		exit(EXIT_FAILURE);
	}

	graph_t g;
	readTSV(g, infiles, verbose);

	barcodeToIndex_t barcodeToIndex;
	indexToBarcode_t indexToBarcode;
	std::string node1;

//	node1 = "A";
//	auto u = boost::add_vertex(g);
//    g[u].name = node1;
//	g[u].weight = 10;
//	g[u].indexOriginal = u;
//	barcodeToIndex[node1] = u;
//	indexToBarcode[u] = node1;
//
//    node1 = "B";
//	u = boost::add_vertex(g);
//    g[u].name = node1;
//	g[u].weight = 10;
//	g[u].indexOriginal = u;
//	barcodeToIndex[node1] = u;
//	indexToBarcode[u] = node1;
//
//    node1 = "C";
//	u = boost::add_vertex(g);
//    g[u].name = node1;
//	g[u].weight = 10;
//	g[u].indexOriginal = u;
//	barcodeToIndex[node1] = u;
//	indexToBarcode[u] = node1;
//
//    node1 = "D";
//	u = boost::add_vertex(g);
//    g[u].name = node1;
//	g[u].weight = 10;
//	g[u].indexOriginal = u;
//	barcodeToIndex[node1] = u;
//	indexToBarcode[u] = node1;
//
//    node1 = "E";
//	u = boost::add_vertex(g);
//    g[u].name = node1;
//	g[u].weight = 10;
//	g[u].indexOriginal = u;
//	barcodeToIndex[node1] = u;
//	indexToBarcode[u] = node1;
//
//    vector<vector<int>> aaaa;
//    aaaa.resize(100);
//    for (auto &row : aaaa) { row.reserve(1000); }
//    //cout<<"size:"<<aaaa[1].size()<<endl;
////    cout<<aaaa[1]<<endl;
//
//	auto ab = boost::add_edge(barcodeToIndex["A"], barcodeToIndex["B"], g).first;
//	g[ab].weight = 10;
//
//	auto ac = boost::add_edge(barcodeToIndex["A"], barcodeToIndex["C"], g).first;
//	g[ac].weight = 10;
//
//	auto ad = boost::add_edge(barcodeToIndex["A"], barcodeToIndex["D"], g).first;
//	g[ad].weight = 10;
//
//	auto ae = boost::add_edge(barcodeToIndex["A"], barcodeToIndex["E"], g).first;
//	g[ae].weight = 10;
//
//	auto bc = boost::add_edge(barcodeToIndex["B"], barcodeToIndex["C"], g).first;
//	g[bc].weight = 10;
//
//	auto bd = boost::add_edge(barcodeToIndex["B"], barcodeToIndex["D"], g).first;
//	g[bd].weight = 10;
//
//	auto be = boost::add_edge(barcodeToIndex["B"], barcodeToIndex["E"], g).first;
//	g[be].weight = 10;
//
//	auto cd = boost::add_edge(barcodeToIndex["C"], barcodeToIndex["D"], g).first;
//	g[cd].weight = 10;
//
//	auto ce = boost::add_edge(barcodeToIndex["C"], barcodeToIndex["E"], g).first;
//	g[ce].weight = 10;
//
//	auto de = boost::add_edge(barcodeToIndex["D"], barcodeToIndex["E"], g).first;
//	g[de].weight = 10;

    printGraph(g);
    cout<<"\n\n\n";
	vecVertexToComponent_t vecVertexToComponent;
	vecVertexToComponent.resize(boost::num_vertices(g));

#if _OPENMP
	double sTime = omp_get_wtime();
#endif
	auto vertexItRange = vertices(g);
	size_t vertexCount = 0;
	cout<<"Total number of subgraphs: "<<boost::num_vertices(g)<<endl;
	auto start = timeNow();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>(stop - start);
	for (auto vertexIt = vertexItRange.first; vertexIt != vertexItRange.second; ++vertexIt) {
		// Find neighbour of vertex and generate neighbour induced subgraph
		auto neighbours = boost::adjacent_vertices(*vertexIt, g);
        vertexCount++;

		if (vertexCount % 100 == 0){
		    cout<<"processing "<<vertexCount<<"th subgraph of "<<boost::num_vertices(g)<<endl;
		    stop = high_resolution_clock::now();
            duration = duration_cast<microseconds>(stop - start);
            cout << "\ttime since last report (micro-s): "<<duration.count() << endl;
		    start = timeNow();
		}
//		if (vertexCount == 1000){
//		    break;
//		}
//		vector<decltype(neighbours)> binned_neighbours;
//		bin_neighbours(neighbours, binned_neighbours);
//		size_t neighborhood_size = 0;
//		for (auto neigh_it = neighbours.first; neigh_it < neighbours.second; ++neigh_it){
//		    neighborhood_size++;
//		}
//		cout<<"Size: "<<neighborhood_size++<<endl;
		graph_t& subgraph = g.create_subgraph(neighbours.first, neighbours.second);
        cout<<" ||| Processing subgraph "<<vertexCount<<" - node count: "<<num_vertices(subgraph)<<endl;
		vertexToComponent_t vertexToComponent;
		if (vertexToComponent.size() > 0 ){
		    cout<<"\n BIG BUG\n ";
		}
		//biconnectedComponents(subgraph, vertexToComponent);
        community_detection_cosine_similarity(subgraph, vertexToComponent, false);
        //Community_detection_k3_cliques(subgraph, vertexToComponent);

		// Delete subgraph to keep memory in control
		for (auto& i : g.m_children) {
			// NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
			delete i;
		}
		g.m_children.clear();

		vecVertexToComponent[*vertexIt] = vertexToComponent;
	}

	std::cerr << "Finished molecule separation ";
#if _OPENMP
	std::cerr << "in sec: " << omp_get_wtime() - sTime << std::endl;
	sTime = omp_get_wtime();
#endif
	std::cerr << "Memory usage: " << double(memory_usage()) / double(1048576) << "GB" << std::endl;

	std::cerr << "Generating molecule overlap graph" << std::endl;

	graph_t molSepG;
	componentsToNewGraph(g, molSepG, vecVertexToComponent);
	printGraph(molSepG);
	if (verbose) {
		std::cerr << "Printed graph" << std::endl;
#if _OPENMP
		std::cerr << "in sec: " << omp_get_wtime() - sTime << std::endl;
#endif
	}
}
