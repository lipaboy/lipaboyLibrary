#pragma once

#include <tuple>
#include "stream/stream_base.h"

namespace lipaboy_lib::stream_space {

    namespace operators {

        template <class SecondStreamT>
        class paired {
        public:
            using SecondSubType = SecondStreamT;

            template <class TFirst>
            using RetType = std::pair<TFirst, typename SecondStreamT::ResultValueType>;

        public:
            template <class... ArgsSecond>
            paired(StreamBase<ArgsSecond...> const & second)
                : second_(second) 
            {}
            template <class... ArgsSecond>
            paired(StreamBase<ArgsSecond...>&& second)
                : second_(std::move(second))
            {}

            template <class FirstStreamT>
            auto nextElem(FirstStreamT& first)
                -> RetType<typename FirstStreamT::ResultValueType>
            {
                return std::make_pair(first.nextElem(), second_.nextElem());
            }

            template <class FirstStreamT>
            void incrementSlider(FirstStreamT& first)
            {
                first.incrementSlider();
                second_.incrementSlider();
            }

            template <class FirstStreamT>
            bool hasNext(FirstStreamT& first)
            {
                return first.hasNext() && second_.hasNext();
            }

        private:
            SecondStreamT second_;
        };

    }

}

