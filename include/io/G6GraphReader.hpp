/*
 * G6GraphReader.hpp
 *
 *  Created on: 06.10.2021
 *      Author: Krzysztof Turowski (krzysztof.szymon.turowski@gmail.com)
 */

#pragma once

#include <string>

#include <networkit/io/GraphReader.hpp>

namespace Koala {

/**
 * @ingroup io
 * A reader for graph6 graph format. Each line contains a single graph.
 * Full definition: https://users.cecs.anu.edu.au/~bdm/data/formats.txt
 *
 */
class G6GraphReader final : public NetworKit::GraphReader {
 public:
    G6GraphReader() = default;

    /**
     * Given the path of an input file, read the graph from its first line.
     *
     * @param[in]  path  input file path
     * @param[out]  the graph read from file
     */
    NetworKit::Graph read(std::string_view path) override;

    /**
     * Given an input string, read the graph from it.
     *
     * @param[in]  path  input string
     * @param[out]  the graph read from string
     */
    NetworKit::Graph readline(const std::string &line);
};

} /* namespace Koala */
