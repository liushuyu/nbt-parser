//
// Created by cth451 on 2020/05/10.
//

#include <istream>
#include "tags/DoubleTag.h"
#include "Conversion.h"
#include "Logging.h"

namespace NBTP {
	DoubleTag::EndianConv DoubleTag::toH = Conversion::conv_d_beh;
	DoubleTag::EndianConv DoubleTag::toJ = Conversion::conv_d_hbe;

	std::ostream &DoubleTag::nbtOutput(std::ostream &ostream) const {
		// Perform host to java big endian conversion
		double big = toJ(this->payload);
		ostream.write(reinterpret_cast<const char *>(&big), sizeof(double));
		return ostream;
	}

	DoubleTag::DoubleTag(std::istream &input, ssize_t &counter, IOFormat format) {
		switch (format) {
			case BIN:
				V buffer;
				input.read(reinterpret_cast<char *>(&buffer), sizeof(V));
				// Perform java big-endian to host conversion
				buffer = toH(buffer);
				if (input.fail()) {
					Logging::error(fmt::format(IO_UNEXPECTED_EOF, sizeof(V)), counter);
				}
				this->payload = buffer;
				counter += sizeof(V);
				break;
			case PRETTY_PRINT:
				Logging::error(PARSE_PRETTY, counter);
				break;
		}
	}
}