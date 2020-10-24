#pragma once

#include <tuple>

namespace lipaboy_lib::stream_space {

    namespace operators {

        template <class SecondStreamT>
        class paired {
        public:
            using SecondSubType = SecondStreamT;

            template <class TFirst>
            using RetType = std::pair<TFirst, typename SecondStreamT::ResultValueType>; // return the same type

        public:
            template <class _SecondStreamT_>
            paired(_SecondStreamT_ second)
                : second_(std::forward<_SecondStreamT_>(second)) 
            {}

            template <class StreamType>
            auto nextElem(StreamType& stream)
                -> RetType<typename StreamType::ResultValueType>
            {
                return std::make_pair(stream.nextElem(), second_.nextElem());
            }

            template <class StreamType>
            void incrementElem(StreamType& stream)
            {
                stream.incrementElem();
                second_.nextElem();
            }

            template <class StreamType>
            bool hasNext(StreamType& stream)
            {
                return stream.hasNext() && second_.hasNext();
            }

        private:
            SecondStreamT second_;
        };

    }

}

