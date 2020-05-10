//
// Created by cth451 on 2020/05/09.
//

#include "IntTag.h"
#include "conv.h"
#include "constants.h"
#include <boost/format.hpp>
#include <istream>

namespace NBTP {
	IntTag::EndianConv IntTag::toH = Conversion::conv_32_beh;
	IntTag::EndianConv IntTag::toJ = Conversion::conv_32_hbe;

	TagType IntTag::typeCode() noexcept {
		return TagType::INT;
	}

	std::ostream &IntTag::output(std::ostream &ostream, IOFormat format) {
		switch (format) {
			case TEXT:
				textOutput(ostream, 0);
				break;
			case BIN:
				// Perform host to java big endian conversion
				V big = toJ(this->payload);
				ostream.write(reinterpret_cast<const char *>(&big), sizeof(V));
		}
		return ostream;
	}

	std::ostream &IntTag::textOutput(std::ostream &ostream, unsigned int indent) {
		ostream << boost::format("(%s) %d") % TypeNames[this->typeCode()] % (int64_t) this->payload;
		return ostream;
	}

	IntTag::IntTag(V value) {
		this->payload = value;
	}

	IntTag::IntTag(std::istream &input) {
		V buffer;
		input.read(reinterpret_cast<char *>(&buffer), sizeof(V));
		// Perform java big-endian to host conversion
		buffer = toH(buffer);
		if (input.fail()) {
			throw std::ios_base::failure(IO_UNEXPECTED_EOF);
		}
		this->payload = buffer;
	}
}