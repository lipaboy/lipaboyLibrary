#pragma once

#include <tuple>

#include "extenders.h"
#include "stream_paired.h"
#include "stream_extended.h"
#include "stream_base.h"

namespace lipaboy_lib::stream_space {

    namespace operators {

        class paired {
        public:
            static constexpr bool isTerminated = false;

            template <class TFirst, class TSecond>
            using RetType = std::pair<TFirst, TSecond>; // return the same type

        public:
            template <class FirstStreamT, class SecondStreamT>
            auto nextElem(FirstStreamT & firstStream, SecondStreamT & secondStream)
                -> RetType<typename FirstStreamT::ResultValueType,
                           typename SecondStreamT::ResultValueType>
            {
                return std::make_pair(firstStream.nextElem(), secondStream.nextElem());
            }

            template <class FirstStreamT, class SecondStreamT>
            void incrementElem(FirstStreamT & firstStream, SecondStreamT & secondStream)
            {
                firstStream.incrementElem();
                secondStream.incrementElem();
            }

            template <class FirstStreamT, class SecondStreamT>
            bool hasNext(FirstStreamT & firstStream, SecondStreamT & secondStream)
            {
                return firstStream.hasNext() && secondStream.hasNext();
            }
        };

    }

    namespace shortening {

        template <class TOperator, class FirstStreamT, class SecondStreamT>
        using PairedStreamType =
                shortening::StreamPairedTypeExtender_t<FirstStreamT, SecondStreamT, TOperator>;

        template <class TOperator, class PairedStreamT>
        using PairedStreamTerminatedType =
            typename shortening::TerminatedOperatorTypeApply_t<PairedStreamT, TOperator>
                ::template RetType<typename PairedStreamT::ResultValueType>;
    }

    template <class ...ArgsFirst, class ...ArgsSecond>
    auto to_pair(StreamBase<ArgsFirst...>& first, StreamBase<ArgsSecond...>& second)
            -> shortening::PairedStreamType<operators::paired, StreamBase<ArgsFirst...>, StreamBase<ArgsSecond...> >
    {
        using TempType = shortening::PairedStreamType<operators::paired,
            StreamBase<ArgsFirst...>, StreamBase<ArgsSecond...> >;
        return TempType(operators::paired(), first, second);
    }

    template <class TOperator, class... Args>
    auto operator| (StreamPairedBase<Args...> stream, TOperator operation)
        -> shortening::PairedStreamTerminatedType<TOperator, StreamPairedBase<Args...> >
    {
        using StreamType = StreamPairedBase<Args...>;

        if constexpr (TOperator::isTerminated == true) {
            stream.template assertOnInfinite<StreamType>();
            return shortening::TerminatedOperatorTypeApply_t<StreamType, TOperator>
                (operation).apply(stream);
        }
//        else {
//            return shortening::StreamTypeExtender_t<StreamType, TOperator>
//                (operation, stream);
//        }
    }

}

