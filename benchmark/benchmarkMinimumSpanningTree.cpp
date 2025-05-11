#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>

#include <networkit/graph/GraphTools.hpp>

#include <io/G6GraphReader.hpp>
#include <io/DimacsGraphReader.hpp>
#include <mst/MinimumSpanningTree.hpp>

template <typename T>
NetworKit::edgeweight run_algorithm(NetworKit::Graph &G) {
    auto algorithm = T(G);
    algorithm.run();
    auto &spanning_tree = algorithm.getForest();
    std::cout << spanning_tree.totalEdgeWeight() << " " << std::flush;
    algorithm.check();
    return spanning_tree.totalEdgeWeight();
}

template<>
NetworKit::edgeweight run_algorithm<Koala::ChazelleRubinfeldTrevisanMinimumSpanningTree>(NetworKit::Graph &G) {
    auto algorithm = Koala::ChazelleRubinfeldTrevisanMinimumSpanningTree(G);
    float eps = 0.45;

    int max_w = 0;
    G.forEdges([&max_w](NetworKit::node, NetworKit::node, NetworKit::edgeweight ew, NetworKit::edgeid) {
        max_w = std::max(max_w, static_cast<int>(ew));
    });

    algorithm.run(max_w, eps);
    std::cout << algorithm.getTreeWeight() << " " << std::flush;
    return algorithm.getTreeWeight();
}

std::map<std::string, int> ALGORITHM = {
    { "exact", 0 },
    { "Kruskal", 1 }, { "Prim", 2 }, { "Boruvka", 3 }, { "KKT", 4 }, { "CRT", 5 }, { "Partition", 6 }
};

void run_g6_tests(const std::string &path, const std::string &algorithm) {
    std::fstream file(path, std::fstream::in);
    std::map<int, int> classification;
    while (true) {
        std::string line;
        file >> line;
        if (!file.good()) {
            break;
        }
        auto G_directed = Koala::G6GraphReader().readline(line);
        auto G = NetworKit::Graph(G_directed.numberOfNodes(), true, false);
        G_directed.forEdges([&](NetworKit::node u, NetworKit::node v) {
            if (!G.hasEdge(u, v) && !G.hasEdge(v, u)) {
                G.addEdge(u, v);
            }
        });
        std::set<NetworKit::edgeweight> T;
        std::cout << line << " " << std::flush;
        switch (ALGORITHM[algorithm]) {
        case 0:
            T.insert(run_algorithm<Koala::KruskalMinimumSpanningTree>(G));
            T.insert(run_algorithm<Koala::PrimMinimumSpanningTree>(G));
            T.insert(run_algorithm<Koala::BoruvkaMinimumSpanningTree>(G));
            for (int i = 0; i < 5; i++) {
                T.insert(run_algorithm<Koala::KargerKleinTarjanMinimumSpanningTree>(G));
            }
            assert(T.size() == 1);
            break;
        case 1:
            run_algorithm<Koala::KruskalMinimumSpanningTree>(G);
            break;
        case 2:
            run_algorithm<Koala::PrimMinimumSpanningTree>(G);
            break;
        case 3:
            run_algorithm<Koala::BoruvkaMinimumSpanningTree>(G);
            break;
        case 5:
            run_algorithm<Koala::ChazelleRubinfeldTrevisanMinimumSpanningTree>(G);
            break;
        case 6:
            run_algorithm<Koala::PartitionMinimumSpanningTree>(G);
            break;
        }
        std::cout << std::endl;
    }
}

void run_dimacs_tests(const std::string &path, const std::string &algorithm) {
    auto G_directed = Koala::DimacsGraphReader().read(path);
    auto G = NetworKit::Graph(G_directed.numberOfNodes(), true, false);
    int max_edge = 0;
    G_directed.forEdges([&](NetworKit::node u, NetworKit::node v, NetworKit::edgeweight w) {
        if (!G.hasEdge(u, v) && !G.hasEdge(v, u) && w > 0) {
            G.addEdge(u, v, w);
        }
        max_edge = std::max(max_edge, (int) w);
    });
    std::cout << "MAX EDGE " << max_edge << std::endl;
    std::cout << path << " " << std::endl;
    std::set<NetworKit::edgeweight> T;
    // std::cout << "Kruskal" << std::endl;
    // T.insert(run_algorithm<Koala::KruskalMinimumSpanningTree>(G));
    // std::cout << "Prim" << std::endl;
    // T.insert(run_algorithm<Koala::PrimMinimumSpanningTree>(G));
    // std::cout << "Boruvka" << std::endl;
    // T.insert(run_algorithm<Koala::BoruvkaMinimumSpanningTree>(G));
    // for (int i = 0; i < 5; i++) {
    //     std::cout << "KKT" << std::endl;
    //     T.insert(run_algorithm<Koala::KargerKleinTarjanMinimumSpanningTree>(G));
    // }
    for (int i = 0; i < 5; i++) {
        std::cout << "Chazelle" << std::endl;
        T.insert(run_algorithm<Koala::ChazelleRubinfeldTrevisanMinimumSpanningTree>(G));
    }
    // std::cout << "Partition" << std::endl;
    // T.insert(run_algorithm<Koala::PartitionMinimumSpanningTree>(G));
    std::cout << "ALL DONE" << std::endl;
    assert(T.size() == 1);
    std::cout << std::endl;
    return;
}

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <algorithm> <file>" << std::endl;
        return 1;
    }
    std::string path(argv[2]);
    auto position = path.find_last_of(".");
    if (path.substr(position + 1) == "g6") {
        run_g6_tests(path, std::string(argv[1]));
    } else if (path.substr(position + 1) == "gr") {
        run_dimacs_tests(path, std::string(argv[1]));
    } else {
        std::cerr << "File type not supported: " << path << std::endl;
    }
    return 0;
}
