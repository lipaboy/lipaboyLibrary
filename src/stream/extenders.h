#pragma once

#include "extra_tools/extra_tools.h"
#include "stream_paired.h"

namespace lipaboy_lib::stream_space {

    namespace shortening {

        //---------------StreamTypeExtender---------------//

        template <class TStream, class TOperator>
        struct StreamTypeExtender {
                using type = typename std::remove_reference_t<TStream>::
                        template ExtendedStreamType<std::remove_reference_t<TOperator> >;
        };

        /*template <class TIterator>
        struct StreamTypeExtender<void, TIterator> {
        using type = Stream<TIterator>;
        };*/

        template <class TStream, class TOperator>
            using StreamTypeExtender_t = typename StreamTypeExtender<TStream, TOperator>::type;

        // Paired Extender

        template <class FirstStreamT, class SecondStreamT, class TOperator>
        struct StreamPairedTypeExtender {
            using type = StreamPairedBase<TOperator, FirstStreamT, SecondStreamT>;
        };

        template <class FirstStreamT, class SecondStreamT, class TOperator>
        using StreamPairedTypeExtender_t
            = typename StreamPairedTypeExtender<FirstStreamT, SecondStreamT, TOperator>::type;

        template <class TStream, class TOperator>
        struct TerminatedOperatorTypeApply {
            using type = std::remove_reference_t<TOperator>;
        };

        template <class TStream, class TOperator>
        using TerminatedOperatorTypeApply_t =
            typename TerminatedOperatorTypeApply<TStream, TOperator>::type;
    }

}

