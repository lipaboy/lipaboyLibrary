#pragma once

#include <optional>

namespace lipaboy_lib::stream_space {

    namespace op {

        template <class ResultType>
        class or_else {
        public:
            or_else(ResultType const & orValue) : orValue_(orValue) {}
            or_else(ResultType && orValue) : orValue_(std::move(orValue)) {}

            ResultType value() const { return orValue_; }

        private:
            ResultType orValue_;
        };

    }

    template <class ResultType>
    ResultType operator| (std::optional<ResultType> resultOpt, op::or_else<ResultType> op)
    {
        return resultOpt.value_or(op.value());
    }

}

