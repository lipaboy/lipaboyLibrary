#pragma once

#include "tools.h"

#include <ostream>
#include <string>

namespace lipaboy_lib::stream_space {

	namespace op {

		struct print_to : impl::TerminatedOperator
		{
		public:
			template <class T>
			using RetType = std::ostream&;

		public:
			print_to(std::ostream& o, std::string delimiter = "") : ostreamObj_(o), delimiter_(delimiter) {}

			template <class Stream_>
			std::ostream& apply(Stream_ & obj) {
				for (; obj.hasNext(); )
					ostream() << obj.nextElem() << delimiter();
				return ostream();
			}

			std::ostream& ostream() { return ostreamObj_; }
			std::string const & delimiter() const { return delimiter_; }
		private:
			std::ostream& ostreamObj_;
			std::string delimiter_;
		};

	}

}
