#pragma once

#include "tools.h"

#include <ostream>
#include <string>

namespace lipaboy_lib {

	namespace stream_space {

		namespace operators_space {

			using std::ostream;
			using std::string;

			struct print_to {
			public:
				template <class T>
				using RetType = std::ostream&;

			public:
				print_to(std::ostream& o, string delimiter = "") : ostreamObj_(o), delimiter_(delimiter) {}
				static constexpr OperatorMetaTypeEnum metaInfo = PRINT_TO;
				static constexpr bool isTerminated = true;

				template <class Stream_>
				std::ostream& apply(Stream_ & obj) {
					for (; obj.hasNext(); )
						ostream() << obj.nextElem() << delimiter();
					return ostream();
				}

				std::ostream& ostream() { return ostreamObj_; }
				string const & delimiter() const { return delimiter_; }
			private:
				std::ostream& ostreamObj_;
				string delimiter_;
			};

		}

	}

}