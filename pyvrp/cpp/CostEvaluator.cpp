#include "CostEvaluator.h"

#include <stdexcept>
#include <utility>

using pyvrp::CostEvaluator;
using pyvrp::DepotContext;

CostEvaluator::CostEvaluator(std::vector<double> loadPenalties,
                             double twPenalty,
                             double distPenalty)
    : loadPenalties_(std::move(loadPenalties)),
      twPenalty_(twPenalty),
      distPenalty_(distPenalty)
{
    for (auto const penalty : loadPenalties_)
        if (penalty < 0)
            throw std::invalid_argument("load_penalties must be >= 0.");

    if (twPenalty_ < 0)
        throw std::invalid_argument("tw_penalty must be >= 0.");

    if (distPenalty_ < 0)
        throw std::invalid_argument("dist_penalty must be >= 0.");
}

CostEvaluator CostEvaluator::withDepotContext(DepotContext context) const
{
    auto copy = *this;
    copy.depotCtx_ = context;
    return copy;
}
