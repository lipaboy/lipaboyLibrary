#pragma once

#include <tuple>

namespace lipaboy_lib::stream_space {

    namespace operators {

        template <class TStream, class... Rest>
        class tupled : public tupled<Rest...>
        {
        public:
            using CurrentStreamType = TStream;
            using SubType = tupled<Rest...>;
            using SubTypePtr = SubType*;

            template <class TBaseResultValue>
            using RetType = std::tuple<TBaseResultValue,
                typename CurrentStreamType::ResultValueType, 
                typename Rest::ResultValueType...>;

        public:
            tupled() = default;

            template <class _TStream_, class... _Rest_>
            tupled(_TStream_ stream, _Rest_... rest)
                : stream_(stream), SubType(rest...)
            {}

            template <class BaseStreamT>
            auto nextElem(BaseStreamT& base)
                -> RetType<typename BaseStreamT::ResultValueType>
            {
                // DANGER place
                return std::make_tuple(
                        base.nextElem(), 
                        stream_.nextElem(),
                        ((tupled<Rest> *)this)->stream().nextElem()...
                    );
            }

            template <class BaseStreamT>
            void incrementSlider(BaseStreamT& base)
            {
                base.incrementSlider();
                stream().incrementSlider();
                (((tupled<Rest>*)this)->stream().incrementSlider(), ...);
            }

            template <class BaseStreamT>
            bool hasNext(BaseStreamT& base)
            {
                return base.hasNext() && stream().hasNext() && 
                    (((tupled<Rest>*)this)->stream().hasNext() && ...);
            }

        public:
            CurrentStreamType& stream() { return stream_; }

        protected:
            CurrentStreamType stream_;
        };

        //-----------------------Base Tupled-------------------------//

        template <class SecondStreamT>
        class tupled<SecondStreamT> {
        public:
            using SecondSubType = SecondStreamT;
            using SecondValueType = typename SecondStreamT::ResultValueType;

            template <class TBaseResultValue>
            using RetType = std::tuple<TBaseResultValue, 
                typename SecondStreamT::ResultValueType>;

        public:
            template <class _SecondStreamT_>
            tupled(_SecondStreamT_ stream)
                : stream_(stream)
            {}

            template <class BaseStreamT>
            auto nextElem(BaseStreamT& base)
                -> RetType<typename BaseStreamT::ResultValueType>
            {
                return std::make_tuple(base.nextElem(), stream_.nextElem());
            }

            template <class BaseStreamT>
            void incrementSlider(BaseStreamT& base)
            {
                base.incrementSlider();
                stream_.incrementSlider();
            }

            template <class BaseStreamT>
            bool hasNext(BaseStreamT& base)
            {
                return base.hasNext() && stream_.hasNext();
            }

        public:
            SecondStreamT& stream() { return stream_; }

        protected:
            SecondStreamT stream_;
        };

    }

}

