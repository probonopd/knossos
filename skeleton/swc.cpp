#include "swc.h"

#include "dataset.h"
#include "skeletonizer.h"

#include <QDebug>

#include <boost/spirit/home/x3.hpp>

void parseSWC(QIODevice && file) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("*.swc open failed");
    }
    const auto stream = file.readAll();
    auto first = std::cbegin(stream);

    auto treeID = Skeletonizer::singleton().addTree().treeID;
    const auto node = [treeID](auto & ctx){
        using namespace boost::fusion;
        const auto & attr = _attr(ctx);
        const auto pos = floatCoordinate(at_c<2>(attr), at_c<3>(attr), at_c<4>(attr)) * 1000.0 / Dataset::current().scale;
        const auto radius = at_c<5>(attr) * 1000.0 / Dataset::current().scale.x;
        auto src = at_c<0>(attr);
        auto trg = at_c<6>(attr);
        if (src < trg) {
            std::swap(src, trg);
        }
        try {
            auto node = Skeletonizer::singleton().addNode(src, radius, treeID, pos, VIEWPORT_UNDEFINED, 1, boost::none, false);
            if (const auto connectedNode = Skeletonizer::singleton().findNodeByNodeID(trg)) {
                Skeletonizer::singleton().addSegment(*connectedNode, node.get());
            }
        } catch(...) {}
    };
    using namespace boost::spirit::x3;
    const auto comment_ = lexeme['#' >> *(char_ - eol) >> (eol|eoi)];
    const auto node_ = (int_ >> int_ >> double_ >> double_ >> double_ >> double_ >> int_)[node];
    const bool res = boost::spirit::x3::phrase_parse(first, std::end(stream), *(node_ | comment_), boost::spirit::x3::space);

    if (!res || first != std::end(stream)) {
        Skeletonizer::singleton().clearSkeleton();
        throw std::runtime_error("parseSWC parsing failed");
    }
}
